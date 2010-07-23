#pragma once

#include <Rendering/RenderingInfo.h>
#include <vector>
#include <set>
#include <map>
#include "Mesh.h"
#include "types.h"
#include <Utilities/Intersection/Intersection.h>
#include <MeshRepresentation/CGALRepresentation/CGALLaunchSite.h>
#include <MeshRepresentation/CGALRepresentation/CGALAdapter.h>
#include <MeshRepresentation/IMeshRepresentation.h>

// usings
using namespace MeshRepresentations::CGALRepresentation;
using namespace MeshRepresentations;


namespace SnowSim {

	namespace Model {
		template<class T> class ILaunchSite;
		class EdgeGroupFace;
		class World;
	}
}

// usings
using namespace SnowSim::Model;

// class declaration
namespace SnowSim {
	namespace Model {
	class EdgeGroup
		{
		public:

			#pragma region Public Structures

			/////////////////////
			/// Public Structures
			/////////////////////

			struct LaunchSiteInfo;
			struct Drop;

			/// LaunchSiteInfo
			struct LaunchSiteInfo {
				float	flake_flutter;
				float	launchsite_multiplicator;
				int		max_launchsites_per_face;
			};

			/// drop
			struct Drop {
				/// vertex at the beginning of the drop
				Mesh::vertex* origin;
				/// halfedge of the corresponding face
				Mesh::halfedge* halfedge;
				/// next drop
				Drop* next;
				/// previous drop
				Drop* prev;
				/// index number
				int index;
				/// a marker
				bool marked;
				/// beginning launchsite
				ILaunchSite<PrecisionType>* start_launchsite;
				/// beginning launchsite
				ILaunchSite<PrecisionType>* end_launchsite;

				Drop() :
					origin(NULL),
					halfedge(NULL),
					next(NULL),
					prev(NULL),
					index(-1),
					marked(false),
					start_launchsite(NULL),
					end_launchsite(NULL)
				{}
			};

			#pragma endregion

			#pragma region Public typedefs

			typedef MeshRepresentations::CGALRepresentation::Finite_vertices_iterator	CDT_VIterator;
			typedef MeshRepresentations::CGALRepresentation::Vertex_circulator			CDT_VCirculator;
			typedef MeshRepresentations::CGALRepresentation::Edge_iterator				CDT_EIterator;
			typedef MeshRepresentations::CGALRepresentation::Face_iterator				CDT_FIterator;
			typedef MeshRepresentations::CGALRepresentation::Face_circulator			CDT_FCirculator;

			typedef std::vector<Drop*>::iterator	Drop_Iterator;

			#pragma endregion

		private:

			#pragma region Private typedefs

			//////////////////////
			/// Private typedefs
			//////////////////////

			typedef std::vector<EdgeGroupFace*>::iterator								egf_iterator;
			typedef MeshRepresentations::CGALRepresentation::CGALAdapter				CDT_Mesh;
			typedef MeshRepresentations::CGALRepresentation::CGALLaunchSite<>			CDT_LaunchSite;
			typedef MeshRepresentations::CGALRepresentation::LaunchSite_Pair			LaunchSite_Pair;

			typedef SnowSim::Rendering::RenderingInfo			RenderingInfo;

			#pragma endregion

			struct EGF_RenderInfo {
				double height;
				EdgeGroupFace* face;
			};


			/////////////////////
			/// Private Members
			/////////////////////

			World*		m_world;

			IMeshRepresentation<PrecisionType, CDT_LaunchSite, CDT_VIterator,
				CDT_VCirculator, CDT_EIterator, CDT_FIterator, CDT_FCirculator>* m_cdt_mesh;

			int numberDrops;
			int numberLaunchSites;

			/// The number of optimizations for launch site creation
			unsigned int m_iLaunchSiteOptimizations;

			bool isVisible;
			bool doRenderDrops;
			bool doRenderLaunchSites;
			bool doRenderInitialLaunchSites;
			bool doRenderLaunchAreas;
			bool doRenderEdgeGroupFaces;
			bool doRenderNeighbourRelation;
			bool doRenderProjectedEdgeGroup;
			bool doRenderTriangulation;
			bool doRenderSnowNormals;
			bool m_shadeTriangulation;

			point3f	edgeGroupFaceColor;

			std::vector<Drop*>			m_drops;
			std::vector<Drop*>			drop_sources;
			point3f						drop_color;
			/// color of normals
			point3f normal_color;
			float						drop_lineWidth;

			point3f						launchsite_color;
			point3f						launchsite_highlight_color;
			point3f						initlaunchsite_color;

			std::vector<EdgeGroupFace*> m_EdgeGroupFaces;

			/// The boudning box of the current edge group
			/// plus the accumulated snow
			BBox3f m_boundingbox;

			/////////////////////
			/// Private Methods
			/////////////////////

			void checkFor_drops(Mesh::face* face, std::set<Mesh::face*>* faces);

			/// initial launch site generation with fearings optimizations
			inline void generate_initial_launchsites(EdgeGroupFace* face,
					LaunchSiteInfo& info, int optimizations);

			/// initial launch sites generation with segment voronoi diagrams
			inline void generate_initial_launchsites2(EdgeGroupFace* face, LaunchSiteInfo& info, int optimizations);

			void generate_edgeGroupFaces(Mesh::face* face);
			void connect_drops();
			void connect_edgeGroupFaces();
			void insert_drop_constrains();
			void CreateLaunchSiteResponsibilities();
			void CreateLaunchSiteResponsibilities_Voronoi();

			void renderDrops();
			void renderLaunchSites();
			void renderEdgeGroupFaces();
			void renderNeighbourRelations();
			void renderProjectedEdgeGroup();
			void renderTriangulation(RenderingInfo& info);
			void renderLaunchAreas();
			void renderSnowNormals();

			point3f caluclate_center(EdgeGroupFace* face);

			/// Sets up the light for snow rendering
			void SetLight();

			/// A method to check and possibily update the bounding
			/// box of the mesh pool.
			void CheckBoundingBox();

		public:
			/// TODO: This method should not be public
			/// A method to check and possibily update the bounding
			/// box of the mesh pool by comparing the current bounding box
			/// to the new-ls and maybe include it
			void CheckBoundingBox(ILaunchSite<PrecisionType>* new_ls);

		public:

			/////////////////////////////
			/// Constructor & Destructor
			/////////////////////////////

			EdgeGroup(World* world, std::set<Mesh::face*>* faces,
				unsigned int optimizations = 5);
			~EdgeGroup(void);

			/////////////////////
			/// Public Methods
			/////////////////////

			void Visible(bool visible) { isVisible = visible; }
			bool Visible() { return isVisible; }

			void DropsVisible(bool visible) { doRenderDrops = visible; }
			bool DropsVisible() { return doRenderDrops; }

			void LaunchSitesVisible(bool visible) { doRenderLaunchSites = visible; }
			bool LaunchSitesVisible() { return doRenderLaunchSites; }

			void InitialLaunchSitesVisible(bool visible) { doRenderInitialLaunchSites = visible; }
			bool InitialLaunchSitesVisible() { return doRenderInitialLaunchSites; }

			void LaunchAreasVisible(bool visible) { doRenderLaunchAreas = visible; }
			bool LaunchAreasVisible() { return doRenderLaunchAreas; }

			void EdgeGroupFacesVisible(bool visible) { doRenderEdgeGroupFaces = visible; }
			bool EdgeGroupFacesVisible() { return doRenderEdgeGroupFaces; }

			void ProjectedEdgeGroupVisible(bool visible) { doRenderProjectedEdgeGroup = visible; }
			bool ProjectedEdgeGroupVisible() { return doRenderProjectedEdgeGroup; }

			void TriangulationVisible(bool visible) { doRenderTriangulation = visible; }
			bool TriangulationVisible() { return doRenderTriangulation; }

			bool TriangulationShaded() { return m_shadeTriangulation; }
			void TriangulationShaded(bool shaded) { m_shadeTriangulation = shaded; }

			bool SnowNormalsVisible() { return doRenderSnowNormals; }
			void SnowNormalsVisible(bool visible) { doRenderSnowNormals = visible; }

			point3f GetEdgeGroupFaceColor() { return edgeGroupFaceColor; }
			void SetEdgeGroupFaceColor(point3f color) { edgeGroupFaceColor = color; }

			unsigned int GetLaunchSiteOptimizations() { return m_iLaunchSiteOptimizations; }
			void SetLaunchSiteOptimizations(unsigned int optimizations) { m_iLaunchSiteOptimizations = optimizations; }

			// render the edge group
			void render(RenderingInfo& info);

			//  generate the launch sites
			//  returns: total number of created launch sites
			unsigned int generateLaunchSites(LaunchSiteInfo& ls_info);

			// get the number of launch sites
			unsigned int GetNumberLaunchSites();

			// gets the begin of the launch site iterator
			CDT_VIterator LaunchSites_Begin();

			// gets the end of the launch site iterator
			CDT_VIterator LaunchSites_End();

			// checks if there are any constrained edges going into this vertex iterator.
			bool Are_there_incident_constraints(CDT_VIterator& site_iterator);

			// computes the responsibility area for every launch site
			void ComputeResponsibilityAreas();

			// Smoothes the normals of the snow mesh
			void SmoothNormals();

			// get the bounding box of all meshes
			BBox3f GetBoundingBox();

			// Gets the neighbors of the launch site in this edge group
			// (only if the launch site belongs to this edge group)
			CDT_VCirculator GetNeighbors(ILaunchSite<PrecisionType>* ls);

			// Tests for intersection with this edge group or the snow surface
			bool IsIntersecting(point3f& p1, point3f& p2, point3f& intersection);

			// Tests for intersection with this edge groups ground surface (the "non snow")
			bool IsIntersectingGround(point3f& p1, point3f& p2);

			// Tests for intersection with this edge groups snow surface
			bool IsIntersectingSnow(point3f& p1, point3f& p2, point3f& intersection);

			// Get the nearest launch site and the distance to it
			std::pair<ILaunchSite<PrecisionType>*, PrecisionType>
			GetNearestLaunchSite(point3f& position);

			/// Add a new launch sitei to the edge group
			ILaunchSite<PrecisionType>* AddNewLaunchSite(const point3f& position);

			/// Get the edge group face in which the (to the XZ plane) projected point lies
			/// -> if the point is not contained in any edge group NULL is returned
			EdgeGroupFace* GetSurroundingEdgeGroupFace(const point3f& position);

			/// Gets the begin iterator for drop sources
			Drop_Iterator DropSources_Begin();

			/// Gets the end iterator for drop sources
			Drop_Iterator DropSources_End();

			/// Tests if the the vector from v1 to v2 is an edge of the triangulation
			/// and writes the third point to out_v3 if so
			bool
			IsEdge(ILaunchSite<PrecisionType>* v1, ILaunchSite<PrecisionType>* v2, ILaunchSite<PrecisionType>** out_v3);

			/// Removes the constraining edge between ls1 and ls2. Returns true if this
			/// has been successfully done, false otherwise (also if there is no constraint
			/// at all).
			bool RemoveConstraint(ILaunchSite<PrecisionType>* ls1, ILaunchSite<PrecisionType>* ls2);

			/// Inserts a new constraint between ls1 and ls2
			bool InsertConstraint(ILaunchSite<PrecisionType>* ls1, ILaunchSite<PrecisionType>* ls2);

			// Inserts a new launch site in the mesh representation
			ILaunchSite<PrecisionType>*
			Insert(const point3f& position);

			/// Gets a string that represents the mesh with
			/// wavefront object code
			/// Returns the written amount if vertices and normals
			template<class OutputStream>
			std::pair<int, int> getObjRepresentation(OutputStream& os, std::string name, int v_count = 0, int n_count = 0);

			/// Gets a string that represents the mesh with
			/// sketch code
			template<class OutputStream>
			void getSketchRepresentation(OutputStream& os, std::string name);
		};

		struct Local_Face
		{
			int v1,v2,v3;

			Local_Face()
				: v1(0), v2(0), v3(0)
			{}

			Local_Face(int a, int b, int c)
				: v1(a), v2(b), v3(c)
			{}

		};
		/// Gets a string that represents the mesh with
		/// wavefront object code
		template<class OutputStream>
		std::pair<int, int> EdgeGroup::getObjRepresentation(OutputStream& os, std::string name, int v_count, int n_count)
		{
			// ---snow---
			//SnowModel* sm = this->GetSnowModel();
			CDT_FIterator end(m_cdt_mesh->Faces_End());

			point3f top(0.0f, 1.0f, 0.0f);
			typedef std::map< ILaunchSite<PrecisionType>*, int >	MapType;
			MapType			launch_sites_done;
			typedef std::vector< ILaunchSite<PrecisionType>* > 		VecType;
			VecType 		launch_sites;
			typedef std::vector<Local_Face>		FaceList_t;
			FaceList_t		faces;

			for (CDT_FIterator fit = m_cdt_mesh->Faces_Begin();
					fit != end; ++fit)
			{
				LaunchSite_Tripple sites
					= m_cdt_mesh->GetLaunchSitesFromFace(fit);

				point3f pos1 = sites.first->GetPosition();
				point3f pos2 = sites.second->GetPosition();
				point3f pos3 = sites.third->GetPosition();

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

				if ( sites.first->GetType() == LaunchSite::OnDrop )
				{
					drop_site = sites.first;
					other_site = sites.second;
					check_for_exclusion = true;
				}
				else if ( sites.second->GetType() == LaunchSite::OnDrop )
				{
					drop_site = sites.first;
					other_site = sites.second;
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
										( drop_end_pos.x() - drop_origin_pos.x() ) * ( other_site->z() - drop_origin_pos.z() )
									 -	( other_site->x()  - drop_origin_pos.x() ) * ( drop_end_pos.z() - drop_origin_pos.z() ));

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

				int index1, index2, index3;

				MapType::iterator m_it;
				// DOES YET NOT WORK
				m_it = launch_sites_done.find( sites.first );
				if ( m_it == launch_sites_done.end() ) {
					launch_sites.push_back( sites.first );
					index1 = launch_sites.size();
					launch_sites_done.insert( make_pair( sites.first, index1 ) );
				} else {
					index1 = (*m_it).second;
				}

				m_it = launch_sites_done.find( sites.second );
				if ( m_it == launch_sites_done.end() ) {
					launch_sites.push_back( sites.second );
					index2 = launch_sites.size();
					launch_sites_done.insert( make_pair( sites.second, index2 ) );
				} else {
					index2 = (*m_it).second;
				}

				m_it = launch_sites_done.find( sites.third );
				if ( m_it == launch_sites_done.end() ) {
					launch_sites.push_back( sites.third );
					index3 = launch_sites.size();
					launch_sites_done.insert( make_pair( sites.third, index3 ) );
				} else {
					index3 = (*m_it).second;
				}

				faces.push_back( Local_Face(index1, index2, index3) );
			}

			// now write out the collected vertices, normals and faces
			os << "# Edge group: " << name << std::endl;

			// write out vertices
			VecType::iterator	v_it( launch_sites.begin() );
			while ( v_it != launch_sites.end() )
			{
				point3f v = (*v_it)->GetPosition();
				os << "v " << v.x() << " " << v.y() << " " << v.z() << std::endl;
				++v_it;
			}

			// write out faces
			FaceList_t::iterator f_it( faces.begin() );
			while ( f_it != faces.end() )
			{
				Local_Face& f = (*f_it);
				os << "f " << f.v1  + v_count << " " << f.v2  + v_count << " " << f.v3  + v_count << std::endl;
				++f_it;
			}

			os << std::endl;

			return make_pair( launch_sites.size(), 0 );
		}

		/// Gets a string that represents the mesh with
		/// sketch code
		template<class OutputStream>
		void EdgeGroup::getSketchRepresentation(OutputStream& os, std::string name)
		{
			os << "def " << name << "{" << std::endl;

			// ---edge groups---

			// add a style
			os << "\tspecial|\\tikzstyle{" << name << "_eg_style} = [fill=orange!20,draw=orange]|[lay=under]" << std::endl;
			os << "\tspecial|\\tikzstyle{" << name << "_snow_style} = [fill=blue!17,fill opacity=0.7,draw=blue!90,style=thin]|[lay=under]" << std::endl;

			for (egf_iterator it = m_EdgeGroupFaces.begin(); it != m_EdgeGroupFaces.end(); ++it) {
				EdgeGroupFace* face = *it;

				// add the polygon
				os << "\tpolygon[fill style=" << name << "_eg_style,cull=false]";

				// the '-0.005f' is to prevent z fighting
				for (int i = 0; i < face->nVertices; ++i) {
					point3f pos = face->vertices[i]->position;
					os << "(" << pos.x() << "," << pos.y() - 0.005f << "," << pos.z() << ")";
				}

				os << std::endl;
			}

			// ---snow---
			//SnowModel* sm = this->GetSnowModel();
			CDT_FIterator end(m_cdt_mesh->Faces_End());

			point3f top(0.0f, 1.0f, 0.0f);

			for (CDT_FIterator fit = m_cdt_mesh->Faces_Begin();
					fit != end; ++fit)
			{
				LaunchSite_Tripple sites
					= m_cdt_mesh->GetLaunchSitesFromFace(fit);

				point3f pos1 = sites.first->GetPosition();
				point3f pos2 = sites.second->GetPosition();
				point3f pos3 = sites.third->GetPosition();

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

				if ( sites.first->GetType() == LaunchSite::OnDrop )
				{
					drop_site = sites.first;
					other_site = sites.second;
					check_for_exclusion = true;
				}
				else if ( sites.second->GetType() == LaunchSite::OnDrop )
				{
					drop_site = sites.first;
					other_site = sites.second;
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
										( drop_end_pos.x() - drop_origin_pos.x() ) * ( other_site->z() - drop_origin_pos.z() )
									 -	( other_site->x()  - drop_origin_pos.x() ) * ( drop_end_pos.z() - drop_origin_pos.z() ));

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

				// add the polygon
				os << "\tpolygon[fill style=" << name << "_snow_style,cull=false]";
				os << "(" << pos1.x() << "," << pos1.y() << "," << pos1.z() << ")";
				os << "(" << pos2.x() << "," << pos2.y() << "," << pos2.z() << ")";
				os << "(" << pos3.x() << "," << pos3.y() << "," << pos3.z() << ")";
				os << std::endl;
			}

			os << "}" << std::endl;
		}
	}
}
