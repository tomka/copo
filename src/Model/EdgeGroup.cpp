#include <Utilities/OpenGL.h>

#include "EdgeGroup.h"
#include "Exceptions/SnowModelException.h"
#include "Exceptions/NotImplementedException.h"
#include "Exceptions/NotSupportedException.h"
#include "types.h"
#include <time.h>
#include <utility>
#include <stdexcept>
#include "Model/World.h"
#include "utilities.h"
#include "EdgeGroupFace.h"
#include <vector>
#include <list>
#include <MeshRepresentation/CGALRepresentation/CGALAdapter.h>
#include <MeshRepresentation/CGALRepresentation/CGALLaunchSite.h>
#include <Utilities/Intersection/Intersection.h>
#include <utilities.h>
#include <Utilities/node_wrap.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Voronoi_diagram_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_traits_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_policies_2.h>
#include <CGAL/Triangulation_data_structure_2.h>
#include <CGAL/Triangulation_vertex_base_with_info_2.h>
#include <CGAL/Object.h>
#include <CGAL/Cartesian.h>
#include <CGAL/Boolean_set_operations_2.h>
#include <Utilities/Color.h>
#include <iostream>
#include <fstream>
#include <Utilities/Timer.h>
#include <utilities.h>


// class declaration
namespace SnowSim {
	namespace Model {

	EdgeGroup::EdgeGroup(World* world, std::set<Mesh::face*>* faces,
		unsigned int optimizations /* = 5 */)
		:	m_world(world),
			numberDrops(0),
			numberLaunchSites(0),
			doRenderDrops(true),
			doRenderLaunchSites(false),
			doRenderLaunchAreas(false),
			doRenderInitialLaunchSites(false),
			doRenderEdgeGroupFaces(false),
			doRenderNeighbourRelation(false),
			doRenderTriangulation(true),
			doRenderSnowNormals(false),
			m_shadeTriangulation(false),
			drop_color(red_color),
			launchsite_color(blue_color),
			launchsite_highlight_color(red_color),
			initlaunchsite_color(point3f(0.5f, 0.1f, 0.6f)),
			normal_color(red_color),
			drop_lineWidth(5.0f),
			m_iLaunchSiteOptimizations(optimizations),
			isVisible(true)
		{
			srand((unsigned)time(NULL));

			// generate a random color
			edgeGroupFaceColor = Utilities::Color::GetRandomColor();

			// init the constrained dynamic delaunay triangulation
			BBox3f world_bb = world->BoundingBox();

			m_cdt_mesh = new CGALAdapter();

			Mesh::face * face;
			for (std::set<Mesh::face*>::iterator it = faces->begin(); it != faces->end(); ++it) {
				face = (*it);

				// check for drops
				checkFor_drops(face, faces);

				// generate edge group faces
				generate_edgeGroupFaces(face);
			}

			if (m_drops.size() == 0)
				throw new SnowSim::Exceptions::SnowModelException("No Drops in EdgeGroup found!");

			/* connect drops and insert them as constraints into CDT */
			connect_drops();
			insert_drop_constrains();

			// connect edge group faces to make them one mesh
			connect_edgeGroupFaces();

			// create the initial bounding box
			CheckBoundingBox();
	}

	struct Responsibily
	{
		ILaunchSite<PrecisionType>* ls;
		PrecisionType tri_area;
		PrecisionType vor_area;

		Responsibily()
		{
			ls = NULL;
			tri_area = -1.0;
			vor_area = -1.0;
		}
		Responsibily(ILaunchSite<PrecisionType>* site)
		{
			ls = site;
			tri_area = -1.0;
			vor_area = -1.0;
		}
	};

	void EdgeGroup::CreateLaunchSiteResponsibilities()
	{
		bool use_triangle_responsibilities = true;
		bool benchmark_different_approaches = false;

		if ( use_triangle_responsibilities ) {
			m_cdt_mesh->CreateLaunchSiteResponsibilities();
		} else {
			CreateLaunchSiteResponsibilities_Voronoi();
		}

		// if benchmarking is not set up, ignore it
		if ( !benchmark_different_approaches )
			return;

		// test
		typedef std::map<ILaunchSite<PrecisionType>*, Responsibily > map_type;
		map_type resp_map;

		SnowSim::Utilities::Timer t;
		t.start();
		CreateLaunchSiteResponsibilities_Voronoi();
		t.stop();
		double t_vor = t.diffTimeInMSec();
		std::cout << std::endl << "\t\tNeeded time (Voronoi-Area): " << t_vor;

		CDT_VIterator l_it = m_cdt_mesh->LaunchSites_Begin();
		while ( l_it != m_cdt_mesh->LaunchSites_End() )
		{
			ILaunchSite<PrecisionType>* ls = &(*l_it);

			Responsibily r(ls);
			r.vor_area = ls->GetResponsibility();
			if ( resp_map.find( ls ) == resp_map.end() )
				resp_map.insert( make_pair(ls, r) );
			else
				resp_map[ls] = r;

			++l_it;
		}

		t.start();
		m_cdt_mesh->CreateLaunchSiteResponsibilities();
		t.stop();
		double t_tri = t.diffTimeInMSec();
		std::cout << std::endl << "\t\tNeeded time (Triangle-Area): " << t_tri;

		l_it = m_cdt_mesh->LaunchSites_Begin();
		while ( l_it != m_cdt_mesh->LaunchSites_End() )
		{
			ILaunchSite<PrecisionType>* ls = &(*l_it);

			Responsibily r(ls);
			r.vor_area = ls->GetResponsibility();
			if ( ! (resp_map.find( ls ) == resp_map.end()) )
			{
				resp_map[ls].tri_area = ls->GetResponsibility();
			}
			++l_it;
		}
		std::cout << std::endl << "\t\tTringle xTimes faster: " << t_vor / t_tri;

		map_type::iterator m_it = resp_map.begin();
		ofstream file;
		file.open("responsibilities.txt", std::ios::out);		//open a file
		file << "Voronoi(" <<  t_vor << ")   Triangle(" << t_tri << ")    % (tri/vor)" <<  std::endl;
		while (m_it != resp_map.end())
		{
			map_type::value_type element = (*m_it);
			map_type::key_type ls = element.first;
			map_type::mapped_type r = element.second;

			double percent = r.tri_area / r.vor_area;

			file << r.vor_area << "    " << r.tri_area << "    " << percent << "%" << std::endl;

			++m_it;
		}
		file.close();

	}

	void EdgeGroup::CreateLaunchSiteResponsibilities_Voronoi()
	{
		egf_iterator it = this->m_EdgeGroupFaces.begin();

		#pragma region Local typedefs

		// typedefs for defining the adaptor
		typedef CGAL::Exact_predicates_inexact_constructions_kernel                  K;
		typedef ILaunchSite<PrecisionType>*											 InfoType;
		typedef CGAL::Triangulation_vertex_base_with_info_2<InfoType,K>				 Vb;
		typedef CGAL::Triangulation_data_structure_2<Vb> 							 Tds;
		typedef CGAL::Delaunay_triangulation_2<K,Tds>	                             DT;
		typedef CGAL::Delaunay_triangulation_adaptation_traits_2<DT>                 AT;
		typedef CGAL::Delaunay_triangulation_caching_degeneracy_removal_policy_2<DT> AP;
		typedef CGAL::Voronoi_diagram_2<DT,AT,AP>                                    VD;

		// typedef for the result type of the point location
		typedef AT::Site_2                    	Site_2;
		typedef AT::Point_2                   	Point_2;
		typedef DT::Segment						Segment;
		typedef CGAL::Ray_2<K>					Ray;

		typedef VD::Locate_result             	Locate_result;
		typedef VD::Vertex_handle           	Vertex_handle;
		typedef VD::Face_handle               	Face_handle;
		typedef VD::Halfedge_handle          	Halfedge_handle;
		typedef VD::Ccb_halfedge_circulator 	Ccb_halfedge_circulator;
		typedef VD::Bounded_faces_iterator	 	Bounded_faces_iterator;
		typedef VD::Unbounded_faces_iterator  	Unbounded_faces_iterator;
		typedef std::map<EdgeGroupFace*, VD*>	Voronoi_map;
		typedef Voronoi_map::iterator			Voronoi_map_iterator;

		// instead of
		//typedef CGAL::Cartesian<Number_type>               K2;
		// workaround for VC++
		//struct K2 : public CGAL::Cartesian<Number_type> {};

		//typedef K2::Point_2                          K2_Point_2;
		typedef CGAL::Polygon_2<K>          	   	Polygon_2;
		typedef CGAL::General_polygon_2	<K>			General_polygon_2;
		typedef CGAL::Polygon_with_holes_2<K>	    Polygon_with_holes_2;


		#pragma endregion

		Voronoi_map egf_ls_map;

		// 1. iterate through all launch sites and save them in correct voronoi diagram
		for (CDT_VIterator vit = m_cdt_mesh->LaunchSites_Begin();
				vit != m_cdt_mesh->LaunchSites_End(); ++vit)
		{
			ILaunchSite<PrecisionType>* site = &(*vit);
			EdgeGroupFace* face = site->GetFace();

			// ignore vertices that are connected to constrains (e. g. are on a drop)
			if ( Are_there_incident_constraints(vit) )
				continue;

			if (egf_ls_map.find(face) == egf_ls_map.end())
				egf_ls_map.insert( make_pair( face, new VD() ) );

			// add launch site to voronoi diagram
			Face_handle handle = egf_ls_map[face]->insert( Site_2( site->x(), site->z() ) );
			VD::Delaunay_vertex_handle vertex = handle->dual();
			vertex->info() = site;
		}

		// 2. iterate through all the voronoi diagrams and calculate areas
		Voronoi_map_iterator m_it = egf_ls_map.begin();
		for ( ; m_it != egf_ls_map.end(); ++m_it)
		{
			EdgeGroupFace* egf = (*m_it).first;

			// create vector of vertives of the egf
			std::vector<point3f> vertices;
			point3f v1 = egf->vertices[0]->position;
			point3f v2 = egf->vertices[1]->position;
			point3f v3 = egf->vertices[2]->position;
			vertices.push_back(v1);
			vertices.push_back(v2);
			vertices.push_back(v3);
			vertices[0].y() = 0.0f;
			vertices[1].y() = 0.0f;
			vertices[2].y() = 0.0f;

			point3f egf_edge1 = vertices[1] - vertices[0];
			point3f egf_edge2 = vertices[2] - vertices[0];

			// Create the egf triangle
			Point_2 v0_cgal( v1.x(), v1.z() );
			Point_2 v1_cgal( v2.x(), v2.z() );
			Point_2 v2_cgal( v3.x(), v3.z() );

			Polygon_2 p_egf_tri;
			p_egf_tri.push_back( v0_cgal );
			p_egf_tri.push_back( v1_cgal );
			p_egf_tri.push_back( v2_cgal );


			if (p_egf_tri.orientation() == CGAL::CLOCKWISE)
				p_egf_tri.reverse_orientation();

			VD* vd = (*m_it).second;

			// 2.1. first, iterate over the bounded faces of the VD
			Bounded_faces_iterator b_it = vd->bounded_faces_begin();

			while ( b_it != vd->bounded_faces_end() )
			{
				VD::Face& cell = *b_it;
				InfoType current_site = cell.dual()->info();

				// iterate around the vertices of the voronoi cell
				// and clip it against triangle
				Ccb_halfedge_circulator he_it = cell.outer_ccb(), done(he_it);

				// build up the voronoi polygon
				Polygon_2 p_cell;
				do {
					VD::Halfedge& he = *he_it;
					p_cell.push_back( he.source()->point() );
					++he_it;
				} while ( he_it != done );

				if (p_cell.orientation() == CGAL::CLOCKWISE)
					p_cell.reverse_orientation();

				typedef std::list<Polygon_with_holes_2>            Pwh_list_2;

				// clip the cell polygon with the EGF triangle
				Pwh_list_2                  intR;
				Pwh_list_2::const_iterator  it;

				CGAL::intersection( p_egf_tri, p_cell, std::back_inserter(intR));

				for (it = intR.begin(); it != intR.end(); ++it)
				{
					// draw the polygon if there is an union
					Polygon_with_holes_2::General_polygon_2 clipped
						= (*it).outer_boundary();

					// save the area
					current_site->SetResponsibility( clipped.area() );
				}

				++b_it;
			}

			// 2.2. then, iterate over the unbounded faces of the VD
			// TODO: See rendering of launch areas
		}

		// 3. Clean up
		m_it = egf_ls_map.begin();
		for ( ; m_it != egf_ls_map.end(); ++m_it)
		{
			delete (*m_it).second;
		}
	}

	unsigned int EdgeGroup::generateLaunchSites(LaunchSiteInfo& ls_info)
	{
		EdgeGroupFace* egFace;

		//std::cout << "\tadding initial launch sites...";
		//Utilities::Timer t;
		//t.start();
		for (egf_iterator it = m_EdgeGroupFaces.begin(); it != m_EdgeGroupFaces.end(); ++it) {
				egFace = *it;
				// generate launch sites
					generate_initial_launchsites(egFace, ls_info, m_iLaunchSiteOptimizations);
		}
		//t.stop();

		//std::cout << "ready" << std::endl << "\t-> added " << numberLaunchSites
		//	<< " launch sites (" << m_iLaunchSiteOptimizations  << "x optimized) in " << t.diffTimeInMSec() << "msec" << std::endl;

		CreateLaunchSiteResponsibilities();

		return numberLaunchSites;
	}

	EdgeGroup::~EdgeGroup(void)
		{
		}

	unsigned int EdgeGroup::GetNumberLaunchSites()
	{
		return this->numberLaunchSites;
	}

	void EdgeGroup::insert_drop_constrains() {
		for (std::vector<Drop*>::iterator it = drop_sources.begin(); it != drop_sources.end(); ++it) {
			Drop *drop = (*it);
			Drop *start(drop);

			do {
				typedef std::pair<ILaunchSite<PrecisionType>*, ILaunchSite<PrecisionType>*> ls_pair_t;
				ls_pair_t new_launchsites = m_cdt_mesh->InsertConstraint(
						drop->origin->position,
						drop->next->origin->position );
				drop->start_launchsite = new_launchsites.first;
				drop->end_launchsite = new_launchsites.second;

				drop = drop->next;
			} while (drop != start);
		}
	}

	// Inserts a new launch site in the mesh representation
	ILaunchSite<PrecisionType>*
	EdgeGroup::Insert(const point3f& position)
	{
		return m_cdt_mesh->Insert( position );
	}


	/// Inserts a new constraint between ls1 and ls2
	bool EdgeGroup::InsertConstraint(ILaunchSite<PrecisionType>* ls1, ILaunchSite<PrecisionType>* ls2)
	{
		return m_cdt_mesh->InsertConstraint( ls1, ls2 );
	}

	void EdgeGroup::ComputeResponsibilityAreas()
	{
		m_cdt_mesh->CreateLaunchSiteResponsibilities();
	}

	/// Gets the begin iterator for drop sources
	EdgeGroup::Drop_Iterator EdgeGroup::DropSources_Begin()
	{
		return drop_sources.begin();
	}

	/// Gets the end iterator for drop sources
	EdgeGroup::Drop_Iterator EdgeGroup::DropSources_End()
	{
		return drop_sources.end();
	}

	/// Tests if the the vector from v1 to v2 is an edge of the triangulation
	/// and writes the third point to out_v3 if so
	bool
	EdgeGroup::IsEdge(ILaunchSite<PrecisionType>* v1, ILaunchSite<PrecisionType>* v2, ILaunchSite<PrecisionType>** out_v3)
	{
		return m_cdt_mesh->IsEdge(v1, v2, out_v3);
	}

	void EdgeGroup::generate_edgeGroupFaces(Mesh::face* face) {
		EdgeGroupFace * egf = new EdgeGroupFace(this, face);

		Mesh::halfedge* h = face->m_halfedge;

		egf->nNeighbors =0;
		egf->nVertices = 0;

		egf->normal.set(face->normal.x(), face->normal.y(), face->normal.z());
		egf->area = face->area;
		egf->slope_angle = face->slope_angle;
		egf->original_face = face;

		do { // go through all edges, collect all that are drops
			egf->nVertices++;
			egf->vertices.push_back(h->origin);
			egf->halfedges.push_back(h);
			h=h->next;
		} while (h!=face->m_halfedge);

		double vertices[6];

		for (int i = 0; i < egf->nVertices; ++i) {
			vertices[2*i] =egf->vertices[i]->position.x();
			vertices[2*i + 1] =egf->vertices[i]->position.z();
		}

		// old, we have currently only one single CDT in the EdgeGroup
		//egf->cdt_launchareas = new IncDelaunay(egf->nVertices, vertices);

		//egf->cdt_launchareas->Changed += event_handler(egf, &EdgeGroupFace::HandleMeshChange);

		this->m_EdgeGroupFaces.push_back(egf);
	}

	void EdgeGroup::connect_edgeGroupFaces() {
		for (egf_iterator it1 = m_EdgeGroupFaces.begin();
							it1 != m_EdgeGroupFaces.end(); ++it1) {
			for (egf_iterator it2 = m_EdgeGroupFaces.begin();
							it2 != m_EdgeGroupFaces.end(); ++it2) {
				EdgeGroupFace * egf1 = (*it1);
				EdgeGroupFace * egf2 = (*it2);

				for (int nr_vert1 = 0; nr_vert1 < egf1->nVertices; ++nr_vert1) {
					Mesh::halfedge* he1 = egf1->halfedges[nr_vert1]->inverse;
					if ( he1 == NULL ) continue; // the face can not have any neigbour on this side

					for (int nr_vert2 = 0; nr_vert2 < egf2->nVertices; ++nr_vert2) {
						Mesh::halfedge* he2 = egf2->halfedges[nr_vert2];
						if ( !he2->inverse ) continue; // the face can not have any neigbour on this side

						if (he1 == he2) { // this only works with half edge data structure

							// TODO: Should we connect from both sides right now?
							egf1->nNeighbors++;
							egf1->neighbors.push_back(egf2);
						}
					}
				}
			}
		}
	}

	/// initial launch sites generation with segment voronoi diagrams
	void EdgeGroup::generate_initial_launchsites2(EdgeGroupFace* face, LaunchSiteInfo& info, int optimizations) {

	}

	/// initial launch site generation with fearings optimizations
	void EdgeGroup::generate_initial_launchsites(EdgeGroupFace* face, LaunchSiteInfo& info, int optimizations) {
		int nLaunchSites;
		int nLaunchSiteCount = 0;

		// the number of retries in the exception case
		unsigned int max_retries = 3;
		unsigned int retries_done = 0;

		point3f position;

		point3f nearest_neighbour;
		float a, b, distMax = 0;

		point3f best_position;

		if (info.flake_flutter < 0.0001f)
			info.flake_flutter = 0.0001f;

		nLaunchSites = (int) ((face->area / info.flake_flutter) + 0.5);
		nLaunchSites = MIN(
			MAX(1, nLaunchSites),
			info.max_launchsites_per_face);

		nLaunchSites *= info.launchsite_multiplicator;

		// for each new launch site
		for (int i_ls = 0; i_ls < nLaunchSites; ++i_ls)
		{
			try {

				// optimize "optimizations"-times
				for (int j_opt = 0; j_opt <= optimizations; ++j_opt)
				{
						//// generate random numbers so that point = aA + bB + bC and a + b + c = 1
						//a = rand() / (float)RAND_MAX; // set a to random number: [0...1]
						//b = rand() / (float)RAND_MAX; // set b to random number: [0...1]
						//
						//if (a + b > 1.0)
						//{
						//	a = 1 - a;
						//	b = 1 - b;
						//}

						//// get random point in triangle
						//position =    a * face->vertices[0]->position +
						//		  	  b * face->vertices[1]->position +
						//	(1 - a - b) * face->vertices[2]->position;

						a = rand() / (float)RAND_MAX; // set a to random number: [0...1]
						b = ( 1.0f - a ) * ( rand() / (float)RAND_MAX ); // set a to random number: [0...1]

						position = a * face->vertices[0]->position
								 + b * face->vertices[1]->position
								 + (1-a-b) * face->vertices[2]->position;

						// get the nearest launch site to the current position
						std::pair<ILaunchSite<PrecisionType>*, PrecisionType>
							result = m_cdt_mesh->GetNearestLaunchSite(position);

						// get the distance
						PrecisionType dist = result.second;

						if (dist > distMax || j_opt == 0)
						{
							best_position = position;
							distMax = dist;
						}
				}

				// insert new launch site
				ILaunchSite<PrecisionType>* new_ls = m_cdt_mesh->Insert(best_position);
				new_ls->SetInitial(true); // make it an initial one
				new_ls->SetFace(face); // and set the face
				new_ls->SetBaseHeight(best_position.y());
				++nLaunchSiteCount;
				CheckBoundingBox(new_ls);
			} catch (std::logic_error& ex) {
				++retries_done;
				--i_ls;
				std::cerr << std::endl << "\t[Error] An error occurred while launch site creation:" << std::endl
					<< "\t" << ex.what() << std::endl;

				if (retries_done <= max_retries) {
					std::cout << "\t-> retrying (" << retries_done << "/" << max_retries << ")" << std::endl;
				} else {
					std::cout << "\t-> maximum retries reached" << std::endl;
					throw ex;
				}
			}
		}

		numberLaunchSites += nLaunchSiteCount;

		SmoothNormals();
	}

	point3f EdgeGroup::caluclate_center(EdgeGroupFace* face) {
		return point3f();
	}

	// Smoothes the normals of the snow mesh
	void EdgeGroup::SmoothNormals()
	{
		for (CDT_VIterator vit = m_cdt_mesh->LaunchSites_Begin();
				vit != m_cdt_mesh->LaunchSites_End();
				++vit)
		{
			ILaunchSite<PrecisionType>& site = *vit;

			CDT_FCirculator neighbor_faces = m_cdt_mesh->GetNeighborFaces(vit), done(neighbor_faces);

			if  (neighbor_faces != NULL)
			{
				point3f n = point3f(0.0f, 0.0f, 0.0f);
				do {

					LaunchSite_Tripple face = m_cdt_mesh->GetLaunchSitesFromFace(neighbor_faces);

					n += cross(	face.second->GetPosition() - face.first->GetPosition(),
								face.third->GetPosition() - face.first->GetPosition());

					++neighbor_faces;
				} while ( neighbor_faces != done );

				cgv::math::linalg::fix_col_vec<float, 3> neg_normal(-normalize(n));
				site.SetNormal(neg_normal);
			}
		}
	}

	#pragma region Rendering

	void EdgeGroup::renderNeighbourRelations() {
		point3f face_center, neighbour_center;

		glLineWidth(2.0f);

		glColor3f(blue_color.x(), blue_color.y(), blue_color.z());

		for (egf_iterator it = m_EdgeGroupFaces.begin(); it != m_EdgeGroupFaces.end(); ++it) {
			EdgeGroupFace* face = *it;

			face_center = face->GetOriginalFace()->compute_center();

			for (egf_iterator n_it = face->neighbors.begin(); n_it != face->neighbors.end(); ++n_it) {
				neighbour_center = (*n_it)->GetOriginalFace()->compute_center();

				glBegin(GL_LINES);
					glVertex3fv(face_center);
					glVertex3fv(neighbour_center);
				glEnd();
			}
		}
	}

	void EdgeGroup::renderProjectedEdgeGroup() {
		glDisable(GL_LIGHTING);
		glColor3f(1.0f, 0.0f, 0.0f);
		glLineWidth(1.0f);

		PrecisionType bb_y = this->m_world->BoundingBox().min.y();
		bb_y -= 2.0f; // put the projection a bit lower as the world bb, since we want to see it ;)

		for (egf_iterator it = m_EdgeGroupFaces.begin(); it != m_EdgeGroupFaces.end(); ++it) {
			EdgeGroupFace* face = *it;

			glBegin(GL_LINE_LOOP);
				for (int i = 0; i < face->nVertices; ++i) {
					point3f pos = face->vertices[i]->position;

					glVertex3f(pos.x(), bb_y, pos.z());
				}
			glEnd();
		}
	}

	void EdgeGroup::renderEdgeGroupFaces() {
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(0.5f, 0.0f);
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		glDisable(GL_LIGHTING);

		for (egf_iterator it = m_EdgeGroupFaces.begin(); it != m_EdgeGroupFaces.end(); ++it) {
			EdgeGroupFace* face = *it;

				//float material[] = {color.x(),color.y(),color.z(),1.0f};
				//glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material);
				glColor4f(edgeGroupFaceColor.x(), edgeGroupFaceColor.y(), edgeGroupFaceColor.z(), 1.0f);

				// Did not compile on linux:
				//glPushName((GLuint)face);

				glBegin(GL_POLYGON);
					glNormal3fv(face->normal);
					for (int i = 0; i < face->nVertices; ++i) {
						glVertex3fv(face->vertices[i]->position);
					}
				glEnd();

				//glPopName();
		}

		glEnable(GL_LIGHTING);
		glPolygonOffset(0.0, 0.0);
		glDisable(GL_POLYGON_OFFSET_FILL);
	}

	void EdgeGroup::renderDrops()
	{
		glLineWidth(drop_lineWidth);
		glDisable(GL_LIGHTING);

		for (std::vector<Drop *>::iterator it = drop_sources.begin(); it != drop_sources.end(); ++it) {
			Drop *drop = (*it);
			Drop *start = drop;

			do {
				glBegin(GL_LINES);
					glColor3f(drop_color.x(), drop_color.y(), drop_color.z());
					glVertex3fv(drop->halfedge->origin->position);
					glVertex3fv(drop->halfedge->next->origin->position);
				glEnd();

				drop = drop->next;
			} while (drop != start);
		}
	}

	void EdgeGroup::renderSnowNormals() {

		float point_size= 3.0f;

		glPointSize(point_size);

		glBegin(GL_LINES);

		for (CDT_VIterator vit = m_cdt_mesh->LaunchSites_Begin();
				vit != m_cdt_mesh->LaunchSites_End();
				++vit)
		{
			// ignore vertices that are connected to constrains (e. g. are on a drop)
			if ( Are_there_incident_constraints(vit) )
				continue;

			ILaunchSite<PrecisionType>* site = &(*vit);

			point3f n_end( site->GetPosition()+ site->GetNormal() );

			glColor3f(normal_color.x(), normal_color.y(), normal_color.z());

			glVertex3d( site->x(), site->y(), site->z() );
			glVertex3d( n_end.x(), n_end.y(), n_end.z() );
		}

		glEnd();
	}

	void EdgeGroup::renderLaunchSites()
	{
		glDisable(GL_LIGHTING);

		float point_size= 3.0f;

		glPointSize(point_size);

		glBegin(GL_POINTS);

		for (CDT_VIterator vit = m_cdt_mesh->LaunchSites_Begin();
				vit != m_cdt_mesh->LaunchSites_End();
				++vit)
		{
			ILaunchSite<PrecisionType>* site = &(*vit);

			// ignore vertices that are connected to constrains (e. g. are on a drop)
			if ( Are_there_incident_constraints(vit) )
				continue;

			if (site->IsHighlighted()) {
				glEnd();

				glPointSize(6.0f);
				glColor3f( launchsite_highlight_color.x(), launchsite_highlight_color.y(), launchsite_highlight_color.z() );

				glBegin(GL_POINTS);
					glVertex3d( site->x(), site->y(), site->z() );
				glEnd();

				glPointSize(point_size);
				glBegin(GL_POINTS);
			} else {
				if (site->IsInitial())
				{
					if (!doRenderInitialLaunchSites) continue;
					glColor3f( initlaunchsite_color.x(), initlaunchsite_color.y(), initlaunchsite_color.z() );
				}
				else
				{
					glColor3f( launchsite_color.x(), launchsite_color.y(), launchsite_color.z() );
				}
			}

			glVertex3d( site->x(), site->y(), site->z() );
		}

		glEnd();

		glEnable(GL_LIGHTING);
	}

	void EdgeGroup::SetLight()
	{
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT1);
		glDisable(GL_LIGHT0);
		GLfloat light_ambient[] = { 0.7f, 0.7f, 0.7f, 1.0f };
		GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		GLfloat light_position[] = { 1.0f, 1.0f, 1.0f, 0.0f };
		glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse);
		glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular);
		glLightfv(GL_LIGHT1, GL_POSITION, light_position);
	}

	//#pragma optimize("", off);
	void EdgeGroup::renderTriangulation(RenderingInfo& info)
	{
		if (m_shadeTriangulation) {
			glPolygonOffset(0.0f, 0.0f);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

			SnowModel* sm = m_world->GetSnowModel();
			bool use_shaders = ( sm->UseSnowShader() ) && ( glActiveTexture != NULL );
			Shading::Shader* shader = sm->GetSnowShader();

			PrecisionType snow_clipping_thsh = sm->GetSnowClippingThreshold();

			SetLight();

			//float material[] = {m_snowColor.x(),m_snowColor.y(),m_snowColor.z(),1.0f};
			//glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material);

			GLfloat front_diffuse[]  = {0.33f,0.33f,0.38f,1.0f};
			GLfloat front_ambient[]  = {0.75f,0.75f,0.95f,1.0f};
			GLfloat front_specular[] = {0.45f,0.45f,0.5f,1.0f};
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, front_specular);
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, front_ambient);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, front_diffuse);
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 20.0f);

			PrecisionType flakeDustTh = sm->GetFlakeDustingThreshold();
			bool doFlakeDusting = false;

			// should we use flake dusting?
			if ( sm->IsFlakeDustingEnabled() && glTexImage3D != NULL )
			{
				doFlakeDusting = true;
			}

			CDT_FIterator end(m_cdt_mesh->Faces_End());

			std::vector<LaunchSite_Tripple> flakeDustingFaces;

			point3f top(0.0f, 1.0f, 0.0f);

			// end shader use
			if ( use_shaders )
			{
				// shaders have access to texture unit 1
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, info.GetNoiseTexture().texname);
				glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

				shader->begin();
				shader->GiveUniform4fv("color0", front_diffuse);
				shader->GiveUniform4fv("color1", front_ambient);
				shader->GiveUniform4fv("color2", front_specular);

				point3f eye_pos = info.GetEyePosition();
				shader->GiveUniform3f("eyePos", eye_pos.x(), eye_pos.y(), eye_pos.z() );
				shader->GiveUniform3f("lightPos", 500.0, 500.0, 500.0 );
				shader->GiveUniform1i("tex", 1);
			}

			for (CDT_FIterator fit = m_cdt_mesh->Faces_Begin();
					fit != end; ++fit)
			{
				LaunchSite_Tripple sites = m_cdt_mesh->GetLaunchSitesFromFace(fit);

				point3f pos1 = sites.first->GetPosition();
				point3f pos2 = sites.second->GetPosition();
				point3f pos3 = sites.third->GetPosition();

				// check if the edge is in the edge group (e. g. on the right
				// side of the constrains)
				// The implication that one of the vertices must be on a drop
				// if the face is NOT in the edge group, is not quite true.
				// But since we do not add launch sites on the "outer" side
				// we can assume that it is true.
				// 16.08.2009: Removed Previous checking, it could
				// be stated much simpler:
				if (	sites.first->GetType() == LaunchSite::OnDrop
					&&	sites.second->GetType() == LaunchSite::OnDrop
					&&	sites.third->GetType() == LaunchSite::OnDrop)
				{
					// do not draw the face if it is not "inside" of the snow mesh
					continue;
				}

				PrecisionType h1 = sites.first->GetSnowHeight();
				PrecisionType h2 = sites.second->GetSnowHeight();
				PrecisionType h3 = sites.third->GetSnowHeight();

				if (	( h1 < snow_clipping_thsh )
						&&
						( h2 < snow_clipping_thsh )
						&&
						( h3 < snow_clipping_thsh ) )
				{
					/* do not render snow face if snow height is lower then clipping threshold*/
					continue;

//					if ( ( sites.first->GetType() != LaunchSite::OnDrop )
//						&&
//						 ( sites.second->GetType() != LaunchSite::OnDrop )
//						&&
//						 ( sites.third->GetType() != LaunchSite::OnDrop ))
//					{
//
//						/* do not render snow face if snow height is 0 and no vertex is on a drop*/
//						continue;
//					}
//					else
//					{
//						/* the snow face height is 0 and a drop is part of the face */
//
////						if ( doFlakeDusting && ( flakeDustTh > NearlyZero ) )
////						{
////							flakeDustingFaces.push_back(sites);
//							continue;
////						}
//					}
				} else
				{
					PrecisionType h1_proj = dot( point3f(0, h1, 0), sites.first->GetNormal());
					PrecisionType h2_proj = dot( point3f(0, h2, 0), sites.second->GetNormal());
					PrecisionType h3_proj = dot( point3f(0, h3, 0), sites.third->GetNormal());

					if ( doFlakeDusting &&
							( h1_proj < flakeDustTh )
							&&
							( h2_proj < flakeDustTh )
							&&
							( h3_proj < flakeDustTh ))
					{
						if (	((h1_proj < NearlyZero ) && (sites.first->GetType() != LaunchSite::OnDrop))
							|| 	((h2_proj < NearlyZero ) && (sites.second->GetType() != LaunchSite::OnDrop))
							||	((h3_proj < NearlyZero ) && (sites.third->GetType() != LaunchSite::OnDrop)) )
						{
							/* Put flake dusting on a face if the on the normal projected height is below the dusting threshold */
							flakeDustingFaces.push_back(sites);
							continue;
						} else
						{
							if ( 	( h1 < flakeDustTh )
								&&	( h2 < flakeDustTh )
								&&	( h3 < flakeDustTh ) )
							{
								flakeDustingFaces.push_back(sites);
								continue;
							}
						}
					}
				}

				// glPushName((GLuint)face);

				glBegin(GL_POLYGON);
					glNormal3fv(sites.first->GetNormal());
					glTexCoord2f(0.0, 0.0);
					//glMultiTexCoord2f(GL_TEXTURE1, 0.0, 0.0);
					glVertex3fv(pos1);
					glNormal3fv(sites.second->GetNormal());
					glTexCoord2f(1.0, 0.0);
					//glMultiTexCoord2f(GL_TEXTURE1, 1.0, 0.0);
					glVertex3fv(pos2);
					glNormal3fv(sites.third->GetNormal());
					glTexCoord2f(1.0, 1.0);
					//glMultiTexCoord2f(GL_TEXTURE1, 1.0, 1.0);
					glVertex3fv(pos3);
				glEnd();

				// glPopName();
			}

			// end shader use
			if ( use_shaders )
			{
				glDisable(GL_TEXTURE_2D);
				shader->end();
			}

			if ( doFlakeDusting && flakeDustingFaces.size() > 0 )
			{
				// flake dusting texture is in texture unit 0
				glActiveTexture(GL_TEXTURE0);

				/* only do flake dusting if is enabled and we are able to work with 3d textures */
				glEnable(GL_TEXTURE_3D);

				SnowModel::Texture tex = sm->GetFlakeDustingTexture();

				glBindTexture(GL_TEXTURE_3D, tex.texname);

				std::vector<LaunchSite_Tripple>::iterator fd_it = flakeDustingFaces.begin();

				while (fd_it != flakeDustingFaces.end() )
				{
					LaunchSite_Tripple sites = *fd_it;

					point3f pos1 = sites.first->GetPosition();
					point3f pos2 = sites.second->GetPosition();
					point3f pos3 = sites.third->GetPosition();

					float m = 0.7;
					point3f tex1 = m*pos1;
					point3f tex2 = m*pos2;
					point3f tex3 = m*pos3;

					glBegin(GL_POLYGON);
						glNormal3fv(sites.first->GetNormal());
						glTexCoord3fv(tex1);
						glVertex3fv(pos1);
						glNormal3fv(sites.second->GetNormal());
						glTexCoord3fv(tex2);
						glVertex3fv(pos2);
						glNormal3fv(sites.third->GetNormal());
						glTexCoord3fv(tex3);
						glVertex3fv(pos3);
					glEnd();

					++fd_it;
				}

				glDisable(GL_TEXTURE_3D);
			}

			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT1);
		} else {
			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT1);
			glLineWidth(1.0f);

			glBegin(GL_LINES);

			CDT_EIterator end(m_cdt_mesh->Edges_End());

			for (CDT_EIterator eit = m_cdt_mesh->Edges_Begin();
					eit != end; ++eit)
			{
				// draw drops (constrains) in a different color
				if (m_cdt_mesh->IsConstrained(eit))
					glColor3f(1,0,1);
				else
					glColor3f(1,1,1);

				LaunchSite_Pair sites = m_cdt_mesh->GetLaunchSitesFromEdge(eit);

				ILaunchSite<PrecisionType>* site1 = sites.first;
				ILaunchSite<PrecisionType>* site2 = sites.second;

				// check if the edge is in the edge group (e. g. on the right
				// side of the constains)
				// The implication that one of the vertices must be on a drop
				// if the face is NOT in the edge group, is not quite true.
				// But since we do not add launch sites on the "outer" side
				// we can assume that it is true.
				ILaunchSite<PrecisionType>* drop_site;
				ILaunchSite<PrecisionType>* other_site;
				bool check_for_exclusion = false;
				bool exclude_edge = false;

				if ( site1->GetType() == LaunchSite::OnDrop )
				{
					drop_site = site1;
					other_site = site2;
					check_for_exclusion = true;
				}
				else if ( site2->GetType() == LaunchSite::OnDrop )
				{
					drop_site = site2;
					other_site = site1;
					check_for_exclusion = true;
				}

				if ( check_for_exclusion )
				{
					/* go through all drops */
					for (int drop_source=0; drop_source < drop_sources.size(); ++drop_source)
					{
						Drop* drop = drop_sources[drop_source];

						do {
							// if a vertex is IN the edge group the vertex
							// has to be on the left side of the drop

							point3f drop_origin_pos = drop->halfedge->origin->position;
							point3f drop_end_pos = drop->halfedge->next->origin->position;

							/* find the drop which has drop_site as origin */
							if (	( abs(drop_origin_pos.x() - drop_site->x() ) < NearlyZero )
								&&	( abs(drop_origin_pos.z() - drop_site->z() ) < NearlyZero ) )
							{
								/* check if site2 (as another vertex of the edge)
								   is on the left side of the drop:
								   Return: >0 for P2 left of the line through P0 and P1
								           =0 for P2 on the line
								           <0 for P2 right of the line */
								int isLeft = (int)(
										( drop_end_pos.z() - drop_origin_pos.z() ) * ( other_site->x() - drop_origin_pos.x() )
									 -	( other_site->z()  - drop_origin_pos.z() ) * ( drop_end_pos.x() - drop_origin_pos.x() ));

								if ( isLeft > 0 )
								{
									exclude_edge = true;
									break;
								}
							}

							drop = drop->next;
						} while ( drop != drop_sources[drop_source] );

						if (exclude_edge)
						{
							break;
						}
					}
				}

				if (exclude_edge)
				{
					continue;
				}

				glVertex3d(site1->x(), site1->y(), site1->z());
				glVertex3d(site2->x(), site2->y(), site2->z());
			}

			glEnd();
		}
	}

	//#pragma optimize("", on);
	void EdgeGroup::renderLaunchAreas()
	{
		egf_iterator it = this->m_EdgeGroupFaces.begin();

		PrecisionType bb_y = this->m_world->BoundingBox().min.y();
		bb_y -= 2.0f; // only debug

		#pragma region Local typedefs

		// typedefs for defining the adaptor
		typedef CGAL::Exact_predicates_inexact_constructions_kernel                  K;
		typedef ILaunchSite<PrecisionType>*											 InfoType;
		typedef CGAL::Triangulation_vertex_base_with_info_2<InfoType,K>				 Vb;
		typedef CGAL::Triangulation_data_structure_2<Vb> 							 Tds;
		typedef CGAL::Delaunay_triangulation_2<K,Tds>	                             DT;
		typedef CGAL::Delaunay_triangulation_adaptation_traits_2<DT>                 AT;
		typedef CGAL::Delaunay_triangulation_caching_degeneracy_removal_policy_2<DT> AP;
		typedef CGAL::Voronoi_diagram_2<DT,AT,AP>                                    VD;

		// typedef for the result type of the point location
		typedef AT::Site_2                    	Site_2;
		typedef AT::Point_2                   	Point_2;
		typedef DT::Segment						Segment;
		typedef CGAL::Ray_2<K>					Ray;

		typedef VD::Locate_result             	Locate_result;
		typedef VD::Vertex_handle           	Vertex_handle;
		typedef VD::Face_handle               	Face_handle;
		typedef VD::Halfedge_handle          	Halfedge_handle;
		typedef VD::Ccb_halfedge_circulator 	Ccb_halfedge_circulator;
		typedef VD::Bounded_faces_iterator	 	Bounded_faces_iterator;
		typedef VD::Unbounded_faces_iterator  	Unbounded_faces_iterator;
		typedef std::map<EdgeGroupFace*, VD*>	Voronoi_map;
		typedef Voronoi_map::iterator			Voronoi_map_iterator;

		// instead of
		//typedef CGAL::Cartesian<Number_type>               K2;
		// workaround for VC++
		//struct K2 : public CGAL::Cartesian<Number_type> {};

		//typedef K2::Point_2                          K2_Point_2;
		typedef CGAL::Polygon_2<K>          	   	Polygon_2;
		typedef CGAL::General_polygon_2	<K>			General_polygon_2;
		typedef CGAL::Polygon_with_holes_2<K>	    Polygon_with_holes_2;


		#pragma endregion

		Voronoi_map egf_ls_map;

		// 1. iterate through all launch sites and save them in correct voronoi diagram
		for (CDT_VIterator vit = m_cdt_mesh->LaunchSites_Begin();
				vit != m_cdt_mesh->LaunchSites_End(); ++vit)
		{
			ILaunchSite<PrecisionType>* site = &(*vit);
			EdgeGroupFace* face = site->GetFace();

			// ignore vertices that are connected to constrains (e. g. are on a drop)
			if ( Are_there_incident_constraints(vit) )
				continue;

			if (egf_ls_map.find(face) == egf_ls_map.end())
				egf_ls_map.insert( make_pair( face, new VD() ) );

			// add launch site to voronoi diagram
			Face_handle handle = egf_ls_map[face]->insert( Site_2( site->x(), site->z() ) );
			VD::Delaunay_vertex_handle vertex = handle->dual();
			vertex->info() = site;
		}

		// 2. iterate through all the voronoi diagrams and calculate areas
		Voronoi_map_iterator m_it = egf_ls_map.begin();
		for ( ; m_it != egf_ls_map.end(); ++m_it)
		{
			EdgeGroupFace* egf = (*m_it).first;

			// create vector of vertives of the egf
			std::vector<point3f> vertices;
			point3f v1 = egf->vertices[0]->position;
			point3f v2 = egf->vertices[1]->position;
			point3f v3 = egf->vertices[2]->position;
			vertices.push_back(v1);
			vertices.push_back(v2);
			vertices.push_back(v3);
			vertices[0].y() = 0.0f;
			vertices[1].y() = 0.0f;
			vertices[2].y() = 0.0f;

			point3f egf_edge1 = vertices[1] - vertices[0];
			point3f egf_edge2 = vertices[2] - vertices[0];

			// Create the egf triangle
			Point_2 v0_cgal( v1.x(), v1.z() );
			Point_2 v1_cgal( v2.x(), v2.z() );
			Point_2 v2_cgal( v3.x(), v3.z() );

			Polygon_2 p_egf_tri;
			p_egf_tri.push_back( v0_cgal );
			p_egf_tri.push_back( v1_cgal );
			p_egf_tri.push_back( v2_cgal );


			if (p_egf_tri.orientation() == CGAL::CLOCKWISE)
				p_egf_tri.reverse_orientation();

			VD* vd = (*m_it).second;

			// 2.1. first, iterate over the bounded faces of the VD
			Bounded_faces_iterator b_it = vd->bounded_faces_begin();

			// for alternative LINES drawing
			//glBegin(GL_LINES);

				while ( b_it != vd->bounded_faces_end() )
				{
					VD::Face& cell = *b_it;
					InfoType current_site = cell.dual()->info();

					// iterate around the vertices of the voronoi cell
					// and clip it against triangle
					Ccb_halfedge_circulator he_it = cell.outer_ccb(), done(he_it);

					// build up the voronoi polygon
					Polygon_2 p_cell;
					do {
						VD::Halfedge& he = *he_it;
						p_cell.push_back( he.source()->point() );

						// alternative:
//						Point_2 source = he.source()->point();
//						Point_2 target = he.target()->point();
//						point3f p1( source.x(), 0.0, source.y() );
//						point3f p2( target.x(), 0.0, target.y() );
//
//						if ( clip_line_to_triangle(vertices, 3, p1, p2) )
//						{
//							glColor3f(1, 0, 0);
//							glVertex3f(p1.x(), bb_y, p1.z());
//							glVertex3f(p2.x(), bb_y, p2.z());
//						}

						++he_it;
					} while ( he_it != done );

					if (p_cell.orientation() == CGAL::CLOCKWISE)
						p_cell.reverse_orientation();

					typedef std::list<Polygon_with_holes_2>            Pwh_list_2;

					// clip the cell polygon with the EGF triangle
					Pwh_list_2                  intR;
					Pwh_list_2::const_iterator  it;

					CGAL::intersection( p_egf_tri, p_cell, std::back_inserter(intR));

					for (it = intR.begin(); it != intR.end(); ++it)
					{
						// draw the polygon if there is an union
						Polygon_with_holes_2::General_polygon_2 clipped
							= (*it).outer_boundary();

						Polygon_with_holes_2::General_polygon_2::Vertex_const_iterator
							cell_v_it = clipped.vertices_begin();

						glBegin(GL_LINE_LOOP);
							glColor3f(1, 0, 0);
							while ( cell_v_it != clipped.vertices_end() )
							{
								glVertex3f( (*cell_v_it).x(), bb_y, (*cell_v_it).y() );
								++cell_v_it;
							}
						glEnd();

						//current_site->SetResponsibility( clipped.area() );
					}

					++b_it;
				}

			// for alternative LINES drawing
			//glEnd();

			// 2.2. then, iterate over the unbounded faces of the VD
			Unbounded_faces_iterator ub_it = vd->unbounded_faces_begin();
			int fc = 0;
			while ( ub_it != vd->unbounded_faces_end() )
			{
				VD::Face& cell = *ub_it;

				InfoType current_site = cell.dual()->info();

				// iterate around the vertices of the voronoi cell
				// and clip it against triangle
				Ccb_halfedge_circulator he_it = cell.outer_ccb(), done(he_it);

				// get the bounding box of the triangle
				BBox3f bb_tri(vertices[0], vertices[0]);
				for (int tri_v=0; tri_v<3; ++tri_v)
				{
					PrecisionType x = vertices[tri_v].x();
					PrecisionType z = vertices[tri_v].z();

					if ( x > bb_tri.max.x() )
						bb_tri.max.x() = x;
					if ( x < bb_tri.min.x() )
						bb_tri.min.x() = x;
					if ( z > bb_tri.max.z() )
						bb_tri.max.z() = z;
					if ( z < bb_tri.min.z() )
						bb_tri.min.z() = z;
				}
				std::vector<point3f> v_bb_tri;
				v_bb_tri.push_back( point3f(bb_tri.min.x(), 0.0, bb_tri.min.z()) );
				v_bb_tri.push_back( point3f(bb_tri.min.x(), 0.0, bb_tri.max.z()) );
				v_bb_tri.push_back( point3f(bb_tri.max.x(), 0.0, bb_tri.max.z()) );
				v_bb_tri.push_back( point3f(bb_tri.max.x(), 0.0, bb_tri.min.z()) );

//				glBegin(GL_LINE_LOOP);
//					glColor3f(0,0,1);
//					glVertex3f(bb_tri.min.x(), bb_y, bb_tri.min.z());
//					glVertex3f(bb_tri.min.x(), bb_y, bb_tri.max.z());
//					glVertex3f(bb_tri.max.x(), bb_y, bb_tri.max.z());
//					glVertex3f(bb_tri.max.x(), bb_y, bb_tri.min.z());
//				glEnd();

				// build up the voronoi polygon
				Polygon_2 cell_points;
				int last_corner_hit = -1;
				bool hit_occured = false;

				do {
					VD::Halfedge& he = *(he_it->twin());

					Point_2 source(0.0,0.0), target(0.0, 0.0);
					point3f p1;
					point3f p2;

					if ( he.is_segment() )
					{
						source = he.source()->point();
						p1.set( source.x(), 0.0, source.y() );

						target = he.target()->point();
						p2.set( target.x(), 0.0, target.y() );

						bool source_inside = point_in_triangle_2d(vertices, p1);
						bool target_inside = point_in_triangle_2d(vertices, p2);

						if ( source_inside )
						{
							// add source, independent from target
							cell_points.push_back( source );

							if ( !target_inside ) {
								// source inside, target outside
								// -> clip to triangle
								int side_hit;
								if ( !clip_line_to_triangle(vertices, 3, p1, p2, side_hit) )
									throw logic_error("Unbounded Voronoi-Cell-Clipping: s inside, t outside - couldn't clip!");

								cell_points.push_back( Site_2( p2.x(), p2.z() ) );

								// check for previously occurred hits
								if ( hit_occured )
								{
									if (side_hit == 0) {
										if ( last_corner_hit == 1 ) {
											cell_points.push_back( v0_cgal );
										} else if ( last_corner_hit == 2 ) {
											cell_points.push_back( v0_cgal );
											cell_points.push_back( v1_cgal );
										}
									} else if ( side_hit == 1 ) {
										if ( last_corner_hit == 2 ) {
											cell_points.push_back( v1_cgal );
										} else if ( last_corner_hit == 0 ) {
											cell_points.push_back( v1_cgal );
											cell_points.push_back( v2_cgal );
										}
									} else { // side_hit == 2
										if ( last_corner_hit == 0 ) {
											cell_points.push_back( v2_cgal );
										} else if ( last_corner_hit == 1 ) {
											cell_points.push_back( v2_cgal );
											cell_points.push_back( v0_cgal );
										}
									}
									hit_occured = false;
								} else {
									hit_occured = true;
									last_corner_hit = side_hit;
								}
							}
						}
						else
						{
							if ( target_inside ) {

								// source is outside, target inside
								// -> clip to triangle and add hit
								int side_hit;
								if ( !clip_line_to_triangle(vertices, 3, p2, p1, side_hit) )
									throw logic_error("Unbounded Voronoi-Cell-Clipping: s outside, t inside - couldn't clip!");

								// check for previously occurred hits
								if ( hit_occured )
								{
									if (side_hit == 0) {
										if ( last_corner_hit == 2 ) {
											cell_points.push_back( v2_cgal );
										} else if ( last_corner_hit == 1 ) {
											cell_points.push_back( v1_cgal );
											cell_points.push_back( v2_cgal );
										}
									} else if ( side_hit == 1 ) {
										if ( last_corner_hit == 0 ) {
											cell_points.push_back( v0_cgal );
										} else if ( last_corner_hit == 2 ) {
											cell_points.push_back( v2_cgal );
											cell_points.push_back( v0_cgal );
										}
									} else {
										if ( last_corner_hit == 1 ) {
											cell_points.push_back( v1_cgal );
										} else if ( last_corner_hit == 0 ) {
											cell_points.push_back( v0_cgal );
											cell_points.push_back( v1_cgal );
										}
									}
									hit_occured = false;
								} else {
									hit_occured = true;
									last_corner_hit = side_hit;
								}

								cell_points.push_back( Site_2( p1.x(), p1.z() ) );
							} else {
								// important case: one line and maybe two intersections in triangle
								// FIXME: Does not work yet!

								// get intersection points
//								int side_hit;
//								point3f hit1_point = p2;
//								bool ray1_hit = intersect_ray_with_polygon( vertices, p1, hit1_point, side_hit );
//								int hit1_side= side_hit;
//
//								point3f hit2_point = p1;
//								bool ray2_hit = intersect_ray_with_polygon( vertices, p2, hit2_point, side_hit );
//								int hit2_side= side_hit;
//
//								if ( ray1_hit && ray2_hit )
//								{
//									cell_points.push_back ( Site_2( hit1_point.x(), hit1_point.z() ) );
//									cell_points.push_back ( Site_2( hit2_point.x(), hit2_point.z() ) );
//
//									// we have two intersections, check direction and add them
//									if ( hit1_side == 0 ) {
//										if ( hit2_side == 1 ) {
//											cell_points.push_back ( v1_cgal );
//											cell_points.push_back ( v2_cgal );
//										} else if ( hit2_side == 2 ) {
//											cell_points.push_back ( v2_cgal );
//										}
//									} else if ( hit1_side == 1 ) {
//										if ( hit2_side == 2 ) {
//											cell_points.push_back ( v2_cgal );
//											cell_points.push_back ( v0_cgal );
//										} else if ( hit2_side == 0 ) {
//											cell_points.push_back ( v0_cgal );
//										}
//									} else { // hit1_side == 2
//										if ( hit2_side == 0 ) {
//											cell_points.push_back ( v0_cgal );
//											cell_points.push_back ( v1_cgal );
//										} else if ( hit2_side == 1 ) {
//											cell_points.push_back ( v1_cgal );
//										}
//									}
//								}
							}
						}


					}
					else if ( he.is_ray() )
					{
						if ( he.has_source() )
						{
							source = he.source()->point();
							p1.set( source.x(), 0.0, source.y() );

							bool source_inside = point_in_triangle_2d(vertices, p1);

							if ( source_inside )
							{
								// construct out going direction
								VD::Delaunay_vertex_handle d_edge_l = he.down();
								VD::Delaunay_vertex_handle d_edge_r = he.up();

								Point_2 d_el_pos = (*d_edge_l).point();
								Point_2 d_er_pos = (*d_edge_r).point();

								point3f delaunay_dir( d_el_pos.x() - d_er_pos.x(), 0.0, d_el_pos.y() - d_er_pos.y() );
								point3f dir = normalize( cross(delaunay_dir, top_direction) );

								p2 = p1 + dir;

								// source inside, get hit point with triangle
								int side_hit;
								if ( !clip_ray_to_polygon(vertices, p1, p2, side_hit) )
									throw logic_error("Unbounded Voronoi-Cell-Clipping: s inside, t ray - couldn't clip!");

								// add source and hit point
								cell_points.push_back( source );
								cell_points.push_back( Site_2( p2.x(), p2.z() ) );

								// check for previously occurred hits
								if ( hit_occured )
								{
									if (side_hit == 0) {
										if ( last_corner_hit == 1 ) {
											cell_points.push_back( v0_cgal );
										} else if ( last_corner_hit == 2 ) {
											cell_points.push_back( v0_cgal );
											cell_points.push_back( v1_cgal );
										}
									} else if ( side_hit == 1 ) {
										if ( last_corner_hit == 2 ) {
											cell_points.push_back( v1_cgal );
										} else if ( last_corner_hit == 0 ) {
											cell_points.push_back( v1_cgal );
											cell_points.push_back( v2_cgal );
										}
									} else { // side_hit == 2
										if ( last_corner_hit == 0 ) {
											cell_points.push_back( v2_cgal );
										} else if ( last_corner_hit == 1 ) {
											cell_points.push_back( v2_cgal );
											cell_points.push_back( v0_cgal );
										}
									}
									hit_occured = false;
								} else {
									hit_occured = true;
									last_corner_hit = side_hit;
								}
							} else {
								// important case: one line and maybe two intersections in triangle
								// TODO: Check and fix
							}

						}
						else
						{
							// must have target
							target = he.target()->point();
							p2.set( target.x(), 0.0, target.y() );

							bool target_inside = point_in_triangle_2d(vertices, p2);

							if ( target_inside )
							{
								// construct in going direction
								VD::Delaunay_vertex_handle d_edge_l = he.up();
								VD::Delaunay_vertex_handle d_edge_r = he.down();

								Point_2 d_el_pos = (*d_edge_l).point();
								Point_2 d_er_pos = (*d_edge_r).point();

								point3f delaunay_dir(d_el_pos.x() - d_er_pos.x(), 0.0, d_el_pos.y() - d_er_pos.y());
								point3f dir = normalize(cross(down_direction, delaunay_dir));

								p1 = p2 + dir;

								// target inside, get hit point with triangle
								int side_hit;
								if ( !clip_ray_to_polygon(vertices, p2, p1, side_hit) )
									throw logic_error("Unbounded Voronoi-Cell-Clipping: s ray, t inside - couldn't clip!");

								// check for previously occurred hits
								if ( hit_occured )
								{
									if (side_hit == 0) {
										if ( last_corner_hit == 2 ) {
											cell_points.push_back( v2_cgal );
										} else if ( last_corner_hit == 1 ) {
											cell_points.push_back( v1_cgal );
											cell_points.push_back( v2_cgal );
										}
									} else if ( side_hit == 1 ) {
										if ( last_corner_hit == 0 ) {
											cell_points.push_back( v0_cgal );
										} else if ( last_corner_hit == 2 ) {
											cell_points.push_back( v2_cgal );
											cell_points.push_back( v0_cgal );
										}
									} else {
										if ( last_corner_hit == 1 ) {
											cell_points.push_back( v1_cgal );
										} else if ( last_corner_hit == 0 ) {
											cell_points.push_back( v0_cgal );
											cell_points.push_back( v1_cgal );
										}
									}
									hit_occured = false;
								} else {
									hit_occured = true;
									last_corner_hit = side_hit;
								}

								// add source and hit point
								cell_points.push_back( Site_2( p1.x(), p1.z() ) );
							}
							else
							{
								// important case: one line and maybe two intersections in triangle
								// TODO: Check and fix
							}
						}
					}
					else if ( he.is_unbounded() )
					{
						throw logic_error("Unbounded Voronoi-Cell-Clipping: s ray, t ray - not supported!");
					}
					else
					{
						throw logic_error("Unbounded Voronoi-Cell-Clipping: An error occurred. Halfedge no ray, not unbound and no segment!");
					}

					--he_it; // move to next halfedge
					// we have to take operator-- since the voronoi diagram is oriented the other way round
					// (since CGAL works with normal left hand system)
				} while ( he_it != done);

				++fc;

				glBegin(GL_LINE_STRIP);
					for (int k=0; k < cell_points.size(); ++k)
						glVertex3f(cell_points[k].x(), bb_y,  cell_points[k].y());
				glEnd();

				glBegin(GL_LINE_STRIP);
					glColor3f(0, 0, 1);
					for (int k=0; k < cell_points.size(); ++k)
						glVertex3f(cell_points[k].x(), bb_y - 0.8f * fc,  cell_points[k].y());
				glEnd();

				glBegin(GL_POINTS);
					glColor3f(0,1,0);
					glVertex3f(current_site->GetPosition().x(), bb_y - 0.8f * fc, current_site->GetPosition().z() );
				glEnd();

				if (cell_points.size() == 0)
				{
					glBegin(GL_POINTS);
						glColor3f(0,0,1);
						glVertex3f(current_site->GetPosition().x(), bb_y, current_site->GetPosition().z() );
					glEnd();
					++ub_it;

					std::cerr << "[Error] Degnerate voronoi clipping segment with size 0 (blue)!" << std::endl;

					continue;
				}

				try {
					// we have to reverse orientation since CGAL uses right handed system
					cell_points.reverse_orientation();

					typedef std::list<Polygon_with_holes_2>            Pwh_list_2;

					// clip the cell polygon with the EGF triangle
					Pwh_list_2                  intR;
					Pwh_list_2::const_iterator  it;

					CGAL::intersection( p_egf_tri, cell_points, std::back_inserter(intR) );

					for (it = intR.begin(); it != intR.end(); ++it)
					{
						// draw the polygon if there is an union
						Polygon_with_holes_2::General_polygon_2 clipped
							= (*it).outer_boundary();

						Polygon_with_holes_2::General_polygon_2::Vertex_const_iterator
							cell_v_it = clipped.vertices_begin();

						glBegin(GL_LINE_LOOP);
							glColor3f(1, 0, 0);
							while ( cell_v_it != clipped.vertices_end() )
							{
								//std::cout << " P: " <<  (*cell_v_it).x() << ", " << (*cell_v_it).y();
								glVertex3f( (*cell_v_it).x(), bb_y, (*cell_v_it).y() );
								++cell_v_it;
							}
						glEnd();

						glBegin(GL_POINTS);
							point3f sp = current_site->GetPosition();
							glVertex3f( sp.x(), bb_y, sp.z() );
						glEnd();

						//current_site->SetResponsibility( clipped.area() );
					}
				}
				catch (std::exception &e)
				{
					std::cerr << "[ERROR] " << e.what() << std::endl;
				}

				++ub_it;
			} // end while loop through unbound faces

		}

		// 3. Clean up
		m_it = egf_ls_map.begin();
		for ( ; m_it != egf_ls_map.end(); ++m_it)
		{
			delete (*m_it).second;
		}
	}

	void EdgeGroup::render(RenderingInfo& info)
	{
		if (this->isVisible) {
			try
			{
				if (doRenderEdgeGroupFaces) renderEdgeGroupFaces();
				if (doRenderDrops) renderDrops();
				if (doRenderLaunchSites) renderLaunchSites();
				if (doRenderNeighbourRelation) renderNeighbourRelations();
				if (doRenderProjectedEdgeGroup) renderProjectedEdgeGroup();
				if (doRenderTriangulation) renderTriangulation(info);
				if (doRenderLaunchAreas) renderLaunchAreas();
				if (doRenderSnowNormals) renderSnowNormals();

			} catch (exception& e) {
				std::cout << "[Edge Group] Rendering error: " << e.what() << std::endl;
			}
		}
	}

	#pragma endregion

	/**
	 * Finds the drops of a face and adds them
	 * to  the m_drops member container.
	 */
	void EdgeGroup::checkFor_drops(Mesh::face* face, std::set<Mesh::face*>* faces)
	{
		Mesh::halfedge* h = face->m_halfedge;

		do { // go through all edges, collect all that are drops
			if (	(h->inverse == NULL)
				||	(h->inverse->border == true)
				||	(h->inverse->m_face->slope_angle >= (PI_2 - NearlyZero) )
				||	faces->find(h->inverse->m_face) == faces->end() ) // check if the face is in this edge group
			{ // we found a drop
				Drop* d_new = new Drop();
				d_new->halfedge = h;
				d_new->index = numberDrops;
				d_new->origin = h->next->origin;
				m_drops.push_back(d_new);
				numberDrops++;
			}
			h=h->next;
		} while (h!=face->m_halfedge);
	}

	/**
	 * Connects all the drops, found in the m_drops container,
	 * that fit together.
	 */
	void EdgeGroup::connect_drops() {

		unsigned int dropCount = m_drops.size();

		// unmark the drops
		for (unsigned i = 0; i < dropCount; ++i) {
			m_drops[i]->marked = false;
		}

		int nConnections = 0;
		std::vector<Drop*> potential_next;

		for (unsigned i1 = 0; i1 < dropCount; ++i1) {

			Drop* current_drop = m_drops[i1];

			// collect potential next drops
			for (unsigned i2 = 0; i2 < dropCount; ++i2) {
				if (	!(current_drop->marked)
					&&	( m_drops[i2]->origin == current_drop->halfedge->origin ) ) {

					potential_next.push_back(m_drops[i2]);
				}
			}

			// get the best fitting next drop
			if (potential_next.size() == 1)
			{
				current_drop->next = potential_next.front();
				current_drop->next->prev = current_drop;
				current_drop->marked = true; // mark the drop as "having a next drop"
				++nConnections;
				potential_next.clear();
			}
			else if (potential_next.size() > 1)
			{
				// find the drop with the next half edge ccw
				// around the origin of the potentials
				std::vector<Drop*>::iterator it = potential_next.begin();
				int min_dist = INT_MAX;
				Drop* next_drop = NULL;

				while (it != potential_next.end())
				{
					Mesh::halfedge* he = current_drop->halfedge->prev;
					Drop* pot_drop = *it;
					int cur_dist = 0;
					while (he != current_drop->halfedge->inverse)
					{
						++cur_dist;
						if (he == pot_drop->halfedge)
						{
							if (cur_dist < min_dist)
							{
								min_dist = cur_dist;
								next_drop = pot_drop;
							}
						}

						he = he->inverse->prev;
					}

					++it;
				}

				if (next_drop != NULL) {
					current_drop->next = next_drop;
					next_drop->prev = current_drop;
					current_drop->marked = true; // mark the drop as "having a next drop"
					++nConnections;
					potential_next.clear();
				} else {
					throw Exceptions::SnowModelException("Could not connect all drops.");
				}
			}
		}

		//std::ofstream myfile;
		//myfile.open ("drops.txt");

		//for (int d = 0; d < m_drops.size(); ++d)
		//{
		//	myfile << m_drops[d]->origin->position.x() << " " << m_drops[d]->origin->position.z()/* << "#h: " << m_drops[d]->halfedge*/ << std::endl;
		//}
		//myfile.close();

		/* copy a working set of the drop candidates */
		std::set<Drop*> drop_candidates(m_drops.begin(), m_drops.end());

		/* find all sources */
		Drop* d;
		int nDrops = 0;

		//myfile.open ("drops2.txt");

		do {
			d = *(drop_candidates.begin());
			Drop* start(d);

			drop_sources.push_back(d);

			//myfile << "New Dropline" << std::endl << std::endl;

			do {
				drop_candidates.erase(d);
				d = d->next;
				nDrops++;

				//myfile << d->origin->position.x() << " " << d->origin->position.z()/* << "#h: " << d->halfedge*/ << std::endl;
			//} while ( (d != start) && (nDrops < dropCount) );
			} while ( d != start );
		} while (nDrops < dropCount);

		if (nConnections != dropCount)
			std::cout << "[Edge Group] Not all drops could have been connected (Drops: "
				<< m_drops.size() << " Connections: " << nConnections << ")" << std::endl;
	}

	EdgeGroup::CDT_VIterator EdgeGroup::LaunchSites_Begin()
	{
		return m_cdt_mesh->LaunchSites_Begin();
	}

	EdgeGroup::CDT_VIterator EdgeGroup::LaunchSites_End()
	{
		return m_cdt_mesh->LaunchSites_End();
	}

	bool EdgeGroup::Are_there_incident_constraints(CDT_VIterator& site_iterator)
	{
		return m_cdt_mesh->Are_there_incident_constraints(site_iterator);
	}

	// get the bounding box of all meshes
	BBox3f EdgeGroup::GetBoundingBox() {
		return m_boundingbox;
	}

	void EdgeGroup::CheckBoundingBox(ILaunchSite<PrecisionType>* new_ls)
	{
		// TODO: Handle case corretly when only one launch site
		// is the total amount of launch sites in this edge group

		point3f pos = new_ls->GetPosition();
		point3f min = m_boundingbox.min;
		point3f max = m_boundingbox.max;

		if ( pos.x() < min.x() ) min.x() = pos.x();
		if ( pos.x() > max.x() ) max.x() = pos.x();

		if ( pos.y() < min.y() ) min.y() = pos.y();
		if ( pos.y() > max.y() ) max.y() = pos.y();

		if ( pos.z() < min.z() ) min.z() = pos.z();
		if ( pos.z() > max.z() ) max.z() = pos.z();

		m_boundingbox = BBox3f(point3f(min.x(), min.y(), min.z()), point3f(max.x(), max.y(), max.z()));
	}

	void EdgeGroup::CheckBoundingBox() {

		// combin edge group faces bounding box
		// with (snow) mesh representation bounding box

		float minX = 0.0f, minY = 0.0f, minZ = 0.0f;
		float maxX = 0.0f, maxY = 0.0f, maxZ = 0.0f;

		if (m_EdgeGroupFaces.size() > 0) {
			std::vector<EdgeGroupFace*>::iterator it = m_EdgeGroupFaces.begin();

			BBox3f bb = (*it)->GetBoundingBox();
			point3f minBB = bb.min;
			minX = minBB.x(), minY = minBB.y(), minZ = minBB.z();
			point3f maxBB = bb.max;
			maxX = maxBB.x(), maxY = maxBB.y(), maxZ = maxBB.z();

			++it;

			while (it!=m_EdgeGroupFaces.end()) {
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

		for( CDT_VIterator vit = m_cdt_mesh->LaunchSites_Begin();
			vit != m_cdt_mesh->LaunchSites_End(); ++vit)
		{
			CheckBoundingBox(&(*vit));
		}
	}

	// Gets the neighbors of the launch site in this edge group
	// (only if the launch site belongs to this edge group)
	EdgeGroup::CDT_VCirculator EdgeGroup::GetNeighbors(ILaunchSite<PrecisionType>* ls)
	{
		return m_cdt_mesh->GetNeighbors(ls);
	}

	// Tests for intersection with this edge group or its snow
	bool EdgeGroup::IsIntersecting(point3f& p1, point3f& p2, point3f& intersection)
	{
		return ( IsIntersectingGround(p1, p2) || IsIntersectingSnow(p1, p2, intersection) );
	}

	// Tests for intersection with this edge groups ground surface (the "non snow")
	// if the segment p1 to p2 starts or ends on the edgegroup FALSE is returned
	bool EdgeGroup::IsIntersectingGround(point3f& p1, point3f& p2)
	{
		point3f intersection;

		for (egf_iterator it = m_EdgeGroupFaces.begin(); it != m_EdgeGroupFaces.end(); ++it) {
			EdgeGroupFace* face = *it;
			float t;

			if( SnowSim::Utilities::Intersect::intersect(
				face->vertices[0]->position,
				face->vertices[1]->position - face->vertices[0]->position,
				face->vertices[2]->position - face->vertices[0]->position,
				p1, p2, true, false, t) )
			{
				if (t > NearlyZero && t < (1.0 - NearlyZero) )
					return true;
			}
		}

		return false;
	}

	// Tests for intersection with this edge groups snow surface
	// Returns FALSE if the intersection is p1 or p2
	bool EdgeGroup::IsIntersectingSnow(point3f& p1, point3f& p2, point3f& intersection)
	{
		float t;
		if ( m_cdt_mesh->IsIntersecting(p1, p2, t) )
		{
			if (t > NearlyZero && t < (1.0 - NearlyZero) )
			{
				intersection = p1 + t * (p2 - p1);

				/*	Check if the intersection is on the correct constraint side
					e. g. let a line do not intersect by going through a hole */

				// TODO: Currently this works only for concave edge groups without holes

				// check if the edge is in the edge group (e. g. on the right
				// side of the constains)
				// The implication that one of the vertices must be on a drop
				// if the face is NOT in the edge group, is not quite true.
				// But since we do not add launch sites on the "outer" side
				// we can assume that it is true.

				/* go through all drops */
				for (unsigned int drop_source=0; drop_source < drop_sources.size(); ++drop_source)
				{
					Drop* drop = drop_sources[drop_source];

					do {
						// if a vertex is IN the edge group the vertex
						// has to be on the left side of the drop

						point3f drop_origin_pos = drop->halfedge->origin->position;
						point3f drop_end_pos = drop->halfedge->next->origin->position;

						/* check if site2 (as another vertex of the edge)
						   is on the left side of the drop:
						   Return: >0 for P2 left of the line through P0 and P1
								   =0 for P2 on the line
								   <0 for P2 right of the line */

						int isLeft = isLeft_right_handed_2d(drop_end_pos, drop_origin_pos, intersection);

						if ( isLeft > 0 )
						{
							return false;
						}

						drop = drop->next;
					} while ( drop != drop_sources[drop_source] );
				}


				return true;
			} else
				return false;
		} else
			return false;
	}

	// Get the nearest launch site and the distance to it
	std::pair<ILaunchSite<PrecisionType>*, PrecisionType>
	EdgeGroup::GetNearestLaunchSite(point3f& position)
	{
		return m_cdt_mesh->GetNearestLaunchSite(position);

	}

	/// Removes the constraining edge between ls1 and ls2. Returns true if this
	/// has been successfully done, false otherwise (also if there is no constraint
	/// at all).
	bool EdgeGroup::RemoveConstraint(ILaunchSite<PrecisionType>* ls1, ILaunchSite<PrecisionType>* ls2)
	{
		return m_cdt_mesh->RemoveConstraint(ls1, ls2);
	}


	/// Add a new launch sitei to the edge group
	ILaunchSite<PrecisionType>*
	EdgeGroup::AddNewLaunchSite(const point3f& position)
	{
		int nrLSs = m_cdt_mesh->NumberLaunchSites();

		ILaunchSite<PrecisionType>* new_ls
			= m_cdt_mesh->Insert(position);

		if ( nrLSs == m_cdt_mesh->NumberLaunchSites() )
			return new_ls;

		// recreate launch site responsibilites
		CreateLaunchSiteResponsibilities();

		// TODO: Find the correct face the new launch site is in
		// Use the neighbors as hint
		EdgeGroupFace* face = GetSurroundingEdgeGroupFace(position);

		if (face == NULL)
			throw SnowSim::Exceptions::SnowModelException("Cannot add new launch site since the point is not inside this edge group!");

		new_ls->SetFace(face); // and set the face

		new_ls->SetBaseHeight(position.y());
		++numberLaunchSites;
		CheckBoundingBox(new_ls);

		return new_ls;
	}

	/// Get the edge group face in which the (to the XZ plane) projected point lies
	/// -> if the point is not contained in any edge group NULL is returned
	EdgeGroupFace* EdgeGroup::GetSurroundingEdgeGroupFace(const point3f& position)
	{
		std::vector<EdgeGroupFace*>::iterator egf_it = m_EdgeGroupFaces.begin();
		std::vector<EdgeGroupFace*>::iterator egf_end = m_EdgeGroupFaces.end();
		ray3f ray(position, point3f(0.0, 1.0, 0.0));

		while (egf_it != egf_end)
		{
			EdgeGroupFace* egf = *egf_it;

			if ( SnowSim::Utilities::Intersect::intersect(egf->GetOriginalFace(), ray, true) )
				return egf;
		};

		return NULL;
	}
}
}
