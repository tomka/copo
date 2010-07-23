#include <Utilities/OpenGL.h>
#include "SnowModel.h"
#include "Mesh.h"
#include <Model/World.h>
#include "EdgeGroup.h"
//#include <Flake_Schema_Straight.h>
#include <Model/Flake_Schema_DynStraight.h>
#include <exception>
#include <Exceptions/NotSupportedException.h>
#include <Utilities/Intersection/Intersection.h>
#include <Utilities/Intersection/TriangleIntersection.h>
#include <cgv/scene/geometry/ray.h>
#include <iostream>
#include <fstream>
#include <deque>
#include <cstdio>
#include <Shading/GLSL_Shader.h>


namespace SnowSim {
	namespace Model {
		SnowModel::SnowModel(World* world, MeshPool* mesh_pool)
			:	m_snow_density(200),
				m_world(world),
				m_meshPool(mesh_pool),
				edgeGroupsVisible(true),
				launchSitesVisible(true),
				launchsite_resolution(1),
				max_launchsites_per_face(20),
				m_maxSnowHeight(1.0f),
				m_doFlakeDusting(false),
				m_flakeDustingThreshold(0.5),
				m_snowClippingThreshold(NearlyZero),
				m_texturesInitialized(false),
				m_EGGeneration_ExplicitOverlayCheck(true),
				m_iLaunchSiteOptimizations(5),
				m_rememberColors(true),
				m_use_snow_shader(false)
			{
				//flake_schema = new Flake_Schema_Straight<>(this, world);
				flake_schema = new FlakeSchema::Flake_Schema_DynStraight<>(this, world);

				this->m_snow_shader = new Shading::GLSL_Shader();

				//init();

				// load colors if possible
				if (m_rememberColors)
				{
					load_colors();

				}
			}

		void SnowModel::init()
		{

		}

		SnowModel::~SnowModel(void)
		{
			delete(flake_schema);
		}

		void
		SnowModel::save_colors()
		{
			std::ofstream color_file;
			try
			{
				color_file.open("egf_colors.clrs");

				if (color_file) {
					for (eg_it it = edge_groups.begin(); it != edge_groups.end(); ++it) {
						if (*it) {
							point3f color = (*it)->GetEdgeGroupFaceColor();
							color_file << color.x() << " " << color.y() << " " << color.z() << std::endl;
						}
					}
					color_file.close();
				}
			}
			catch (std::exception& e)
			{
				std::cerr << "[SnowModel] Error: " << e.what() << std::endl;
				if (color_file)
					color_file.close();
			}
			catch (...)
			{
				if (color_file)
					color_file.close();
			}
		}

		void
		SnowModel::load_colors()
		{
			std::fstream color_file;
			try
			{
				color_file.open("egf_colors.clrs", std::ios::in);

				if (color_file)
				{
					// delete previous colors
					m_vecRememberedColors.clear();

					char line[4000];
					while(true)
					{
						color_file.getline(&line[0],4000,'\n');
						if(color_file.eof()||color_file.fail())
							break;

						std::istringstream ins;
						ins.str(line);
						float r,g,b;
						ins >> r >> g >> b;

						m_vecRememberedColors.push_back(point3f(r,g,b));
					}
				}
			}
			catch (...)
			{
				if (color_file)
					color_file.close();
			}
		}

		unsigned int SnowModel::GetNumberEdgeGroups()
		{
			return this->edge_groups.size();
		}

		EdgeGroup* SnowModel::GetEdgeGroup(unsigned int index)
		{
			return this->edge_groups[index];
		}

		/**
		 * Generates edge groups over the mesh.
		 */
		void SnowModel::generateEdgeGroups()
		{
			std::cout << "[Snow Model] Generating edge groups" << std::endl;
			std::vector< std::set<Mesh::face*>* > candidate_meshes;

			::Utilities::Timer t;

			t.start();

			/* clear the current edge group container */
			RemoveEdgeGroups();

			/* go through every mesh */
			#pragma omp parallel for
			for (unsigned int i=0; i < m_meshPool->getNrMeshs(); ++i) {

				/* copy the whole mesh to work on it */
				Mesh* tmpMesh = m_meshPool->getMesh(i);
				Mesh::face* f = tmpMesh->mesh()->face_source;

				candidate_meshes.clear();

				/* init all marks to false */
				do {
					f->marked = false;
					f = f->next_face;
				} while (f != NULL);


				std::cout << "\t*Growing edge groups...";
				std::flush(std::cout);

				int percent_step = static_cast<int>(tmpMesh->mesh()->number_faces * 0.1f);
				int step = 10;

				if (percent_step < 1)
				{
					step = static_cast<int>(100.0f / tmpMesh->mesh()->number_faces);
					percent_step = 1;
				}
				unsigned int percent_count = 0;
				int percent = 0;
				int face_count = 0;

				/* go through all faces */
				f = tmpMesh->mesh()->face_source;
				unsigned int eg_count = 0;
				do {

					if (face_count > percent_count)
					{
						std::cout << " " << percent;
						std::flush(std::cout);
						percent_count += percent_step;
						percent += step;
					}

					/*	get only those faces that could potentially get
					 *	covered with snow (slope angle < 90� and facing outside) */
					if (!f->marked) {
						if (f->slope_angle < ( PI_2 - NearlyZero) )
						{
							++eg_count;
							std::set<Mesh::face*> * candidate_mesh = new std::set<Mesh::face*>();
							candidate_meshes.push_back(candidate_mesh);

							/* find the candidate faces that are reachable
							 * from the current face */
							find_candidates(f, candidate_mesh, tmpMesh);

							face_count += candidate_mesh->size();
						} else {
							++face_count;
						}
					}

					f = f->next_face;
				} while (f != NULL);

				std::cout << " ...ready" << std::endl << "\t*Initializating " << candidate_meshes.size() << " edge groups...";
				std::flush(std::cout);

				percent_step = static_cast<int>(candidate_meshes.size() * 0.1f);
				step = 10;

				if (percent_step < 1)
				{
					step = static_cast<int>(100.0f / candidate_meshes.size());
					percent_step = 1;
				}
				percent_count = 0;
				percent = 0;

				for (unsigned int i = 0; i < candidate_meshes.size(); ++i) {
					try {
						//std::cout << "\tFound new edge group (#" << (i + 1) << ")" << std::endl;
						// TODO: go through the found candidates and see if they overlap in Y-Order (top)
						EdgeGroup* eg = new EdgeGroup(m_world, candidate_meshes[i], m_iLaunchSiteOptimizations);
						if ( m_rememberColors && i < m_vecRememberedColors.size() ) {
							eg->SetEdgeGroupFaceColor(m_vecRememberedColors[i]);
						}
						edge_groups.push_back(eg);

						if (i == percent_count)
						{
							std::cout << " " << percent;
							std::flush(std::cout);
							percent_count += percent_step;
							percent += step;
						}
					} catch (exception &e) {
						std::cerr << "[Error] Error within the creation of EdgeGroup " << i + 1 << ": " << e.what() << std::endl;
					}
				}

				std::cout << " ...ready" << std::endl;

				if (m_rememberColors) {
					save_colors();
				}
			}

			t.stop();
			std::cout << "\tTime: " << t.diffTimeInMSec() << "msec" << std::endl;
		}

		/**
		 * Remove all edge groups and clear the container.
		 */
		void
		SnowModel::RemoveEdgeGroups()
		{
			/* Remember colors of the previous edge groups */
			if (m_rememberColors == false) {
				m_vecRememberedColors.clear();
				// clear edge group collection
				for (eg_it it = edge_groups.begin(); it != edge_groups.end(); ++it) {
					if (*it) delete(*it);
				}

				// load last saved colors
				load_colors();
			}
			else if (edge_groups.size() > 0){
				m_vecRememberedColors.clear();
				// clear edge group collection
				for (eg_it it = edge_groups.begin(); it != edge_groups.end(); ++it) {
					if (*it) {
						m_vecRememberedColors.push_back((*it)->GetEdgeGroupFaceColor());
						delete(*it);
					}
				}

				save_colors();
			} else {
				// if there are no edge groups, load the saved colors
				load_colors();
			}

			edge_groups.clear();
		}

		/**
		 * Checks if the face f overlays any face in the mesh container.
		 * Returns: True if there are overlays, false otherwise
		 */
		template<class InputIterator>
		bool
		SnowModel::Overlays(Mesh::face* f, InputIterator mesh_begin, InputIterator mesh_end)
		{
			point3f p1 = f->m_halfedge->origin->position;
			point3f p2 = f->m_halfedge->next->origin->position;
			point3f p3 = f->m_halfedge->prev->origin->position;

			while ( mesh_begin != mesh_end )
			{
				Mesh::face* cand_face = *mesh_begin;

				point3f q1 = cand_face->m_halfedge->origin->position;
				point3f q2 = cand_face->m_halfedge->next->origin->position;
				point3f q3 = cand_face->m_halfedge->prev->origin->position;

				if ( Utilities::Intersect::TriTriIntersection_2d(
						p1, p2, p3, q1, q2, q3 ) )
				{
					return true;
				}

				// auskommentiert am 26.07.09 da der Test auch true zurückgibt,
				// wenn Kanten übereinander liegen, daher passt er hier nicht
//				if ( Utilities::Intersect::tri_tri_overlap_test_3d(
//						&(p1[0]), &(p2[0]), &(p3[0]), &(q1[0]), &(q2[0]), &(q3[0]) ) )
//				{
//					return true;
//				}

				++mesh_begin;
			}

			return false;
		}

		/**
		 * Finds all the valid edge group faces that are reachable
		 * from "f" and adds them to the container "mesh"
		 */
		void
		SnowModel::find_candidates(Mesh::face* f, std::set<Mesh::face*>* eg_candidates, Mesh* mesh)
		{
			std::deque<Mesh::face*> candidates;
			candidates.push_back(f);
			f->marked = true;

			if (m_EGGeneration_ExplicitOverlayCheck) {
				while (candidates.size() > 0) {
					Mesh::face* cur_f = candidates.front();
					candidates.pop_front(); // delete first element in queue

					eg_candidates->insert(cur_f);

					Mesh::halfedge* he = cur_f->m_halfedge;

					do {
						/* make sure we are not on a border */
						if (he->inverse->border != true )
						{
							Mesh::face* adj_face = he->inverse->m_face;
							if (	(adj_face != NULL)	// is there a neighbor at all?
								&&	(!adj_face->marked)	// has the neighbor already been added somewhere?
								&&	(adj_face->slope_angle < (PI_2 - NearlyZero)) // is the slope angle below 90 degree
								&&	(!Overlays(adj_face, eg_candidates->begin(), eg_candidates->end()))  // Explicitly check for overlaps with other edge group faces
								&&  (!Overlays(adj_face, candidates.begin(), candidates.end())) ) // Explicitly check for overlaps with other candidates
							{
								candidates.push_back(adj_face);
								adj_face->marked = true;
							}
						}
						he = he->next;
					} while (he != cur_f->m_halfedge);
				}
			} else {
				while (candidates.size() > 0) {
					Mesh::face* cur_f = candidates.front();
					candidates.pop_front(); // delete element in queue

					eg_candidates->insert(cur_f);

					Mesh::halfedge* he = cur_f->m_halfedge;

					do {
						/* make sure we are not on a border */
						if (he->inverse->border != true )
						{
							Mesh::face* adj_face = he->inverse->m_face;
							if (	adj_face != NULL	// is there a neighbor at all?
								&&	!adj_face->marked	// has the neighbor already been added somewhere?
								&&	adj_face->slope_angle < (PI_2 - NearlyZero) ) // is the slope angle below 90�
							{
								candidates.push_back(adj_face);
								adj_face->marked = true;
							}
						}
						he = he->next;
					} while (he != cur_f->m_halfedge);
				}
			}
		}

		void SnowModel::generateLaunchSites() {
			std::cout << "[Snow Model] generating launch sites" << std::endl << "\t*Progress: ";

			std::flush(std::cout);

			int percent_step = static_cast<int>(edge_groups.size() * 0.1f);
			int step = 10;

			if (percent_step < 1)
			{
				step = static_cast<int>(100.0f / edge_groups.size());
				percent_step = 1;
			}
			unsigned int percent_count = 0;
			int percent = 0;
			unsigned int eg_count = 0;

			::Utilities::Timer t;

			t.start();

			// create new launch site information
			EdgeGroup::LaunchSiteInfo ls_info;
			ls_info.flake_flutter = flake_schema->getFlakeFlutter();
			ls_info.launchsite_multiplicator = launchsite_resolution;
			ls_info.max_launchsites_per_face = max_launchsites_per_face;

			unsigned int ls_count = 0;

			//#pragma omp for schedule(dynamic,10)
			#pragma omp parallel for
			for (int i=0; i < edge_groups.size(); ++i)
			{
				if (eg_count > percent_count)
				{
					#pragma omp critical(eg_count_ls_creation)
					{
						// to make sure that an parallel thread does not
						// does the increment, too - we check again.
						if ( eg_count > percent_count ) {
							std::cout << " " << percent;
							std::flush(std::cout);
							percent_count += percent_step;
							percent += step;
						}
					}
				}

				#pragma omp atomic
				++eg_count;

				try {
					ls_count += edge_groups[i]->generateLaunchSites(ls_info);
				} catch (std::logic_error& ex) {
					std::cerr << std::endl << "\t[Error] An error occurred while launch site creation on edge group #" << eg_count << ":" << std::endl
										<< "\t" << ex.what() << std::endl << "\t->Ignoring edge group" << std::endl;
				}
			}

			// OpenMP does not work with iterators, so the above replaces the following:
			//for (eg_it it = edge_groups.begin(); it != edge_groups.end(); ++it) {
			//	ls_count += (*it)->generateLaunchSites(ls_info);
			//}

			// update bounding box
			checkBoundingBox();

			t.stop();


			std::cout << "...ready" << std::endl << "\t*Total launch sites: " << ls_count << std::endl
				<< "\t*Time: " << t.diffTimeInMSec() << "msec" << std::endl;

		}

		void SnowModel::checkBoundingBox() {
			float minX = 0.0f, minY = 0.0f, minZ = 0.0f;
			float maxX = 0.0f, maxY = 0.0f, maxZ = 0.0f;

			if (edge_groups.size() > 0) {
				std::vector<EdgeGroup*>::iterator it = edge_groups.begin();

				BBox3f bb = (*it)->GetBoundingBox();
				point3f minBB = bb.min;
				minX = minBB.x(), minY = minBB.y(), minZ = minBB.z();
				point3f maxBB = bb.max;
				maxX = maxBB.x(), maxY = maxBB.y(), maxZ = maxBB.z();

				// increment the iterator
				// (we handled the first edge group seperately)
				++it;

				while (it != edge_groups.end()) {
					bb = (*it)->GetBoundingBox();
					minBB = bb.min;
					maxBB = bb.max;;

					if ( minBB.x() < minX ) minX = minBB.x();
					if ( minBB.y() < minY ) minY = minBB.y();
					if ( minBB.z() < minZ ) minZ = minBB.z();
					if ( maxBB.x() > maxX ) maxX = maxBB.x();
					if ( maxBB.y() > maxY ) maxY = maxBB.y();
					if ( maxBB.z() > maxZ ) maxZ = maxBB.z();

					++it;
				}
			}

			m_boundingbox = BBox3f(point3f(minX, minY, minZ), point3f(maxX, maxY, maxZ));
		}

		FlakeSchema::Flake_Schema<PrecisionType>* SnowModel::GetFlakeSchema()
		{
			return flake_schema;
		}

		void SnowModel::SetFlakeSchema(FlakeSchema::Flake_Schema<PrecisionType>* schema)
		{
			if (schema == NULL)
				throw Exceptions::ArgumentException("schema must not be null!");

			flake_schema = schema;
			OnPropertyChanged();

			std::cout << "[Snow Model] Set flake schema to \"" << schema->getName() << "\"" << std::endl;
		}

		/// Gets the bounding box of the snow
		BBox3f SnowModel::GetBoundingBox() const
		{
			return m_boundingbox;
		}

		/// Gets if there are some edge groups in the current model
		bool SnowModel::edgeGroupsGenerated() const
		{ return (edge_groups.size() > 0); }

		void SnowModel::render(Rendering::RenderingInfo& info) {

			if ( !m_texturesInitialized && m_doFlakeDusting )
			{
				try {
					SetFlakeDustingTexture(SnowSim::Shading::GetRGBATexture());
					m_texturesInitialized = true;
				} catch (Exceptions::NotSupportedException& ex) {
					m_doFlakeDusting = false;
					std::cerr << "[Error] " << ex.what() << std::endl << "\t->Feature will be disabled." << std::endl;
				}
			}

			if (edgeGroupsVisible) {
				int eg = 0;
				for (unsigned int i=0; i < edge_groups.size(); ++i) {
					//if (eg == 2)
						edge_groups[i]->render(info);
					++eg;
				}
			}
		}

		int SnowModel::GetLaunchSiteOptimizations()
		{
			int optimizations;
			bool asigned = false;

			for (std::vector<EdgeGroup*>::iterator it = edge_groups.begin(); it != edge_groups.end(); ++it) {
				if (!asigned) {
					optimizations = (*it)->GetLaunchSiteOptimizations();
					asigned = true;
				} else if ( (*it)->GetLaunchSiteOptimizations() != optimizations ) {
					return -1; // return -1 in case the optimization amounts differ among edge groups
				}
			}

			if (optimizations != m_iLaunchSiteOptimizations)
				m_iLaunchSiteOptimizations = optimizations;

			return optimizations;
		}

		void SnowModel::SetLaunchSiteOptimizations(unsigned int optimizations)
		{
			m_iLaunchSiteOptimizations = optimizations;

			for (std::vector<EdgeGroup*>::iterator it = edge_groups.begin(); it != edge_groups.end(); ++it) {
				(*it)->SetLaunchSiteOptimizations(optimizations);
			}

			OnPropertyChanged();
		}

		#pragma region Visibility Properties

		void SnowModel::DropsVisible(bool visible) {
			dropsVisible = visible;
			for (std::vector<EdgeGroup*>::iterator it = edge_groups.begin(); it != edge_groups.end(); ++it) {
				(*it)->DropsVisible(visible);
			}
			OnPropertyChanged();
		}

		void SnowModel::LaunchSitesVisible(bool visible) {
			launchSitesVisible = visible;
			for (std::vector<EdgeGroup*>::iterator it = edge_groups.begin(); it != edge_groups.end(); ++it) {
				(*it)->LaunchSitesVisible(visible);
			}
			OnPropertyChanged();
		}

		bool SnowModel::InitialLaunchSitesVisible() {
			bool initialLaunchSitesVisible = true;
			for (std::vector<EdgeGroup*>::iterator it = edge_groups.begin(); it != edge_groups.end(); ++it) {
				initialLaunchSitesVisible = initialLaunchSitesVisible && (*it)->InitialLaunchSitesVisible();
			}
			return initialLaunchSitesVisible;
		}
		void SnowModel::InitialLaunchSitesVisible(bool visible) {
			for (std::vector<EdgeGroup*>::iterator it = edge_groups.begin(); it != edge_groups.end(); ++it) {
				(*it)->InitialLaunchSitesVisible(visible);
			}
			OnPropertyChanged();
		}


		bool SnowModel::LaunchAreasVisible() {
			bool launchAreasVisible = true;
			for (std::vector<EdgeGroup*>::iterator it = edge_groups.begin(); it != edge_groups.end(); ++it) {
				launchAreasVisible = launchAreasVisible && (*it)->LaunchAreasVisible();
			}
			return launchAreasVisible;
		}

		void SnowModel::LaunchAreasVisible(bool visible) {
			for (std::vector<EdgeGroup*>::iterator it = edge_groups.begin(); it != edge_groups.end(); ++it) {
				(*it)->LaunchAreasVisible(visible);
			}
			OnPropertyChanged();
		}

		bool SnowModel::ProjectedEdgeGroupsVisible() {
			bool projectionVisible = true;
			for (std::vector<EdgeGroup*>::iterator it = edge_groups.begin(); it != edge_groups.end(); ++it) {
				projectionVisible = projectionVisible && (*it)->ProjectedEdgeGroupVisible();
			}
			return projectionVisible;
		}

		void SnowModel::ProjectedEdgeGroupsVisible(bool visible) {
			for (std::vector<EdgeGroup*>::iterator it = edge_groups.begin(); it != edge_groups.end(); ++it) {
				(*it)->ProjectedEdgeGroupVisible(visible);
			}
			OnPropertyChanged();
		}

		bool SnowModel::EdgeGoupFacesVisible() {
			bool edgeGroupFacesVisible = true;

			if (edge_groups.size() == 0)
				return false;

			for (std::vector<EdgeGroup*>::iterator it = edge_groups.begin(); it != edge_groups.end(); ++it) {
				edgeGroupFacesVisible = edgeGroupFacesVisible && (*it)->EdgeGroupFacesVisible();
			}
			return edgeGroupFacesVisible;
		}

		void SnowModel::EdgeGoupFacesVisible(bool visible) {
			for (std::vector<EdgeGroup*>::iterator it = edge_groups.begin(); it != edge_groups.end(); ++it) {
				(*it)->EdgeGroupFacesVisible(visible);
			}
			OnPropertyChanged();
		}

		bool SnowModel::SnowTriangulationVisible() {
			bool triangulationVisible = true;
			for (std::vector<EdgeGroup*>::iterator it = edge_groups.begin(); it != edge_groups.end(); ++it) {
				triangulationVisible = triangulationVisible && (*it)->TriangulationVisible();
			}
			return triangulationVisible;
		}

		void SnowModel::SnowTriangulationVisible(bool visible) {
			for (std::vector<EdgeGroup*>::iterator it = edge_groups.begin(); it != edge_groups.end(); ++it) {
				(*it)->TriangulationVisible(visible);
			}
			OnPropertyChanged();
		}

		bool SnowModel::SnowTriangulationShaded() {
			bool triangulationShaded = true;
			for (std::vector<EdgeGroup*>::iterator it = edge_groups.begin(); it != edge_groups.end(); ++it) {
				triangulationShaded = triangulationShaded && (*it)->TriangulationVisible();
			}
			return triangulationShaded;
		}

		void SnowModel::SnowTriangulationShaded(bool shaded) {
			for (std::vector<EdgeGroup*>::iterator it = edge_groups.begin(); it != edge_groups.end(); ++it) {
				(*it)->TriangulationShaded(shaded);
			}
			OnPropertyChanged();
		}

		#pragma endregion

		bool SnowModel::IsIntersectingModel(point3f& p1, point3f& p2, point3f& intersection) {
			for (std::vector<EdgeGroup*>::iterator it = edge_groups.begin(); it != edge_groups.end(); ++it) {
				if ((*it)->IsIntersecting(p1, p2, intersection))
					return true;
			}
			return false;
		}

		// Tests for intersection with edge groups ground surfaces (the "non snow")
		bool SnowModel::IsIntersectingGround(point3f& p1, point3f& p2)
		{
			for (std::vector<EdgeGroup*>::iterator it = edge_groups.begin(); it != edge_groups.end(); ++it) {
				if ((*it)->IsIntersectingGround(p1, p2))
					return true;
			}
			return false;
		}

		// Tests for intersection with edge groups snow surface
		bool SnowModel::IsIntersectingSnow(point3f& p1, point3f& p2, point3f& intersection, EdgeGroup** hit_eg)
		{
			for (std::vector<EdgeGroup*>::iterator it = edge_groups.begin(); it != edge_groups.end(); ++it) {
				if ((*it)->IsIntersectingSnow(p1, p2, intersection)) {
					(*hit_eg) = *it;
					return true;
				}
			}
			return false;
		}

		void SnowModel::SetRandomEGFColors()
		{
			for (std::vector<EdgeGroup*>::iterator it = edge_groups.begin();
				it != edge_groups.end(); ++it) {
					point3f clr = GetRandomColor();
					(*it)->SetEdgeGroupFaceColor( clr );
			}
		}

		bool SnowModel::SnowNormalsVisible()
		{
			// return false, if no edge groups available
			if (edge_groups.size() == 0)
				return false;

			bool snowNormalsVisible = true;
			for (std::vector<EdgeGroup*>::iterator it = edge_groups.begin(); it != edge_groups.end(); ++it) {
				snowNormalsVisible = snowNormalsVisible && (*it)->SnowNormalsVisible();
			}
			return snowNormalsVisible;
		}

		void SnowModel::SnowNormalsVisible(bool visible)
		{
			for (std::vector<EdgeGroup*>::iterator it = edge_groups.begin(); it != edge_groups.end(); ++it) {
				(*it)->SnowNormalsVisible(visible);
			}
		}

		// Smoothes the normals of every edge group (snow surface)
		void SnowModel::SmoothNormals()
		{
			for (std::vector<EdgeGroup*>::iterator it = edge_groups.begin(); it != edge_groups.end(); ++it) {
				(*it)->SmoothNormals();
			}
		}

		/* Gets the flake dusting texture. */
		SnowModel::Texture SnowModel::GetFlakeDustingTexture()
		{
			return m_flakeDustingTexture;
		}

		void SnowModel::UseSnowShader(bool use_shader)
		{
			if ( use_shader == true )
			{
				// check if hardware is capable of using
				// shaders and other required stuff
				if (	glActiveTexture == NULL 
					||	! ( m_snow_shader->IsSupportByHardware() ) )
					throw Exceptions::NotSupportedException("Shaders are not supported on this hardware.\nHence they will not be activated.");

			}

			// asign new value
			m_use_snow_shader = use_shader;

			// flag property change
			OnPropertyChanged();
		}

		void SnowModel::SetFlakeDustingTexture(Texture texture)
		{
			std::cout << "[SnowModel] Setting new flake dusting texture...";

			if ( glActiveTexture == NULL )
				throw Exceptions::NotSupportedException("glActiveTexture() is not supported on this hardware.");

			std::cout << std::endl << "\t*Using texture unit 0" << std::endl;
			glActiveTexture(GL_TEXTURE0);

			if (m_flakeDustingTexture.pixels != NULL)
				delete(m_flakeDustingTexture.pixels);

			if (m_flakeDustingTexture.texname != 0)
			{
				// delete old texture
				glDeleteTextures(1, &(m_flakeDustingTexture.texname) );
			}

			m_flakeDustingTexture = texture;

			if ( glTexImage3D == NULL || glTexEnvi == NULL )
				throw Exceptions::NotSupportedException("glTexImage3D is not supported on this hardware.");

			if ( !texture.bound )
			{
				// tell GL we want it to use this texture when it textures our polygons
				// (this function has multiple uses, see BuildTexture())
				glGenTextures(1, &(m_flakeDustingTexture.texname));
				glBindTexture(GL_TEXTURE_3D, m_flakeDustingTexture.texname);

				m_flakeDustingTexture.bound = true;
			}

			glTexImage3D(GL_TEXTURE_3D, m_flakeDustingTexture.level, m_flakeDustingTexture.internal_format,
					m_flakeDustingTexture.width, m_flakeDustingTexture.height, m_flakeDustingTexture.depth,
					m_flakeDustingTexture.border, m_flakeDustingTexture.format, m_flakeDustingTexture.type,
					m_flakeDustingTexture.pixels);

			// our texture colors will replace the untextured colors
			glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

			std::cout << "ready" << std::endl;
		}
	}
}
