#pragma once

#include <Utilities/Color.h>
#include <Shading/Texture.h>
#include "MeshPool.h"
#include "Flake_Schema.h"
#include "Flake_Schema_Normal.h"
#include "Flake_Schema_Small.h"
#include <vector>
#include <iostream>
#include <Utilities/MeshHelpers.h>
#include <Geometry/LineSegment.h>
#include <types.h>
#include <Exceptions/ArgumentException.h>
#include <Utilities/Timer.h>
#include <sstream>
#include <Model/EdgeGroup.h>
#include <Shading/Shader.h>
#include <Rendering/RenderingInfo.h>
#include <boost/signal.hpp>

using namespace SnowSim::Utilities::Color;

namespace SnowSim {
	namespace Model {

		//class EdgeGroup;
		class World;

		class SnowModel
			{
			public:
				typedef SnowSim::Shading::Texture	Texture;

				boost::signal<void (SnowModel*)> PropertyChanged;

			private:

				void init();

				World* m_world;

				typedef std::vector<EdgeGroup*>::iterator eg_it;

				bool edgeGroupsVisible;
				bool dropsVisible;
				bool launchSitesVisible;

				/// True, if color of previous launch sites should be
				/// reused, when edge groups get overridden.
				bool m_rememberColors;

				/// The collection for remembering the colors
				std::vector<point3f> m_vecRememberedColors;


				/// True if should be checked if potential
				/// edge group faces overlay while generation.
				/// If they overlay, do not add them.
				bool m_EGGeneration_ExplicitOverlayCheck;

				bool m_doFlakeDusting;

				bool m_texturesInitialized;


				/* The texture used for flake dusting */
				Texture m_flakeDustingTexture;

				/*	The maximum snow height to which the flake dusting
					should occur. Snow higher then this will be drawn
					normally. This unit is measured in [cm] */
				PrecisionType m_flakeDustingThreshold;

				/* The maximum snow height to which snow should not be
				 * drawn
				 */
				PrecisionType m_snowClippingThreshold;

				std::vector<EdgeGroup*> edge_groups;
				MeshPool* m_meshPool;

				FlakeSchema::Flake_Schema<>	*flake_schema;
				float			launchsite_resolution;

				/// The number of optimizations for launch site creation
				unsigned int m_iLaunchSiteOptimizations;

				/// The bounding box of the snow model.
				BBox3f m_boundingbox;

				// The snow density, measured in kg / m^3
				/* Densities, from P. Fearing / PhD, p. 12
					Wild snow			3
					Dry flakes			30-50
					Loose Dry Snow		50-90
					Wind packed snow	120-300
					Cornice Snow		400
					Wet snow			500-830
					Glacier ice			800
					Hard ice			917
					Water				1000 */
				PrecisionType	m_snow_density;

				int				max_launchsites_per_face;

				// max. snow height [cm]
				float m_maxSnowHeight;

				// the snow shader
				Shading::Shader* m_snow_shader;
				// indicates if the snow shader should be used
				bool m_use_snow_shader;

				// helpers

				// clears the list of edge groups and delets them
				void
				RemoveEdgeGroups();

				/**
				 * Checks if the face f overlays any face in the mesh container.
				 */
				template<class InputIterator>
				bool
				Overlays(Mesh::face* f, InputIterator mesh_begin, InputIterator mesh_end);

				/**
				 * Finds all the valid edge group faces that are reachable
				 * from "f" and adds them to the container "eg_candidates"
				 */
				void
				find_candidates(Mesh::face* f, std::set<Mesh::face*> *eg_candidates, Mesh* mesh);

				void
				save_colors();

				void
				load_colors();

				void OnPropertyChanged()
				{
					PropertyChanged(this);
				}



			public:
				/// A method to check and possibily update the bounding
				/// box of the snow model.
				void checkBoundingBox();

			public:
				SnowModel(World* world, MeshPool* mesh_pool);

				~SnowModel(void);

				/// Generate new edge groups of the current meshes,
				/// potentially replacing old ones
				void generateEdgeGroups();

				/// Generate new launch sites in the edge groups
				void generateLaunchSites();

				/// Gets if there are some edge groups in the current model
				bool edgeGroupsGenerated() const;

				/// Gets how many edge groups are currently in the model
				unsigned int GetNumberEdgeGroups();

				/// Gets the edge group with the specified index
				EdgeGroup* GetEdgeGroup(unsigned int index);

				/// Gets the currently set flake schema
				FlakeSchema::Flake_Schema<PrecisionType>* GetFlakeSchema();

				/// Gets the bounding box of the snow
				BBox3f GetBoundingBox() const;

				/// Sets the used flake schema
				void SetFlakeSchema(FlakeSchema::Flake_Schema<PrecisionType>* schema);

				bool EdgeGroupsVisible() {return edgeGroupsVisible; }
				void EdgeGroupsVisible(bool visible) { edgeGroupsVisible = visible; OnPropertyChanged(); }

				float LaunchSiteResoltution() { return launchsite_resolution; }
				void  LaunchSiteResoltution(float resolution) { launchsite_resolution = resolution; OnPropertyChanged(); }

				int	 MaxLaunchSitesPerFace() { return max_launchsites_per_face; }
				void MaxLaunchSitesPerFace(int amount) { max_launchsites_per_face = amount; OnPropertyChanged(); }

				bool DropsVisible() {return dropsVisible; }
				void DropsVisible(bool visible);

				bool ProjectedEdgeGroupsVisible();
				void ProjectedEdgeGroupsVisible(bool visible);

				bool LaunchSitesVisible() {return launchSitesVisible; }
				void LaunchSitesVisible(bool visible);

				bool InitialLaunchSitesVisible();
				void InitialLaunchSitesVisible(bool visible);

				bool LaunchAreasVisible();
				void LaunchAreasVisible(bool visible);

				bool EdgeGoupFacesVisible();
				void EdgeGoupFacesVisible(bool visible);

				bool SnowTriangulationVisible();
				void SnowTriangulationVisible(bool visible);

				bool SnowTriangulationShaded();
				void SnowTriangulationShaded(bool shaded);

				bool SnowNormalsVisible();
				void SnowNormalsVisible(bool visible);

				int GetLaunchSiteOptimizations();
				void SetLaunchSiteOptimizations(unsigned int optimizations);

				bool RememberEGFColors() { return m_rememberColors; }
				void RememberEGFColors(bool remember) {
					if (remember)
						save_colors();
					m_rememberColors = remember;
					OnPropertyChanged();
				}

				void render(Rendering::RenderingInfo& info);

				// Sets random colors to the edge group faces
				void SetRandomEGFColors();

				// Tests for intersection with this edge groups ground or snow surfaces
				bool IsIntersectingModel(point3f& p1, point3f& p2, point3f& intersection);

				// Tests for intersection with edge groups ground surfaces (the "non snow")
				bool IsIntersectingGround(point3f& p1, point3f& p2);

				// Tests for intersection with edge groups snow surface
				bool IsIntersectingSnow(point3f& p1, point3f& p2, point3f& intersection, EdgeGroup** hit_eg);

				float MaxSnowHeight() { return m_maxSnowHeight; }
				void  MaxSnowHeight(float max_height) { m_maxSnowHeight = max_height; OnPropertyChanged(); }

				// Get and set the snow density [kg / m^3]
				PrecisionType GetDensity() const { return m_snow_density; }
				void SetDensity(PrecisionType density) { m_snow_density = density; OnPropertyChanged(); }

				// Smoothes the normals of every edge group (snow surface)
				void SmoothNormals();

				// Gets if flake dusting is enabled
				bool IsFlakeDustingEnabled() { return m_doFlakeDusting; }

				// Enables or disables flake dusting
				void SetFlakeDusting(bool enable) { m_doFlakeDusting = enable; OnPropertyChanged(); }

				/* Gets the snow height to which the dusting should max. occur */
				PrecisionType GetFlakeDustingThreshold() { return m_flakeDustingThreshold; }

				/* Sets the snow height to which the dusting should max. occur */
				void SetFlakeDustingThreshold(PrecisionType threshold) { m_flakeDustingThreshold = threshold; OnPropertyChanged(); }

				/* Gets the snow height to which snow should not be drawn */
				PrecisionType GetSnowClippingThreshold() { return m_snowClippingThreshold; }

				/* Sets the snow height to which snow should not be drawn */
				void SetSnowClippingThreshold(PrecisionType threshold) { m_snowClippingThreshold = threshold; OnPropertyChanged(); }

				/* Gets the flake dusting texture. */
				Texture GetFlakeDustingTexture();

				void SetFlakeDustingTexture(Texture texture);

				/* Gets if there should be an explicit overlap check during edge group creation */
				bool DoExplicitOverlapCheck() { return m_EGGeneration_ExplicitOverlayCheck; }

				/* Sets if there should be an explicit overlap check during edge group creation */
				void DoExplicitOverlapCheck(bool do_check) { m_EGGeneration_ExplicitOverlayCheck = do_check; OnPropertyChanged(); }

				/* Gets the snow shader */
				Shading::Shader* GetSnowShader() { return m_snow_shader; }
				void SetSnowShader(Shading::Shader* shader)
				{
					if (m_snow_shader != NULL)
						delete m_snow_shader;

					m_snow_shader = shader;
					OnPropertyChanged();
				}

				bool UseSnowShader() { return m_use_snow_shader; }
				void UseSnowShader(bool use_shader);

				/// Gets a string that represents the mesh with
				/// sketch code
				template<class OutputStream>
				void getSketchRepresentation(OutputStream& os)
				{
					int eg = 0;
					for (unsigned int i=0; i < edge_groups.size(); ++i) {
							std::ostringstream stm;
							stm << "EdgeGroup" << eg;
							edge_groups[i]->getSketchRepresentation( os, stm.str() );
						++eg;
					}
				}

				/// Gets a string that represents the mesh with
				/// sketch code
				template<class OutputStream>
				void getObjRepresentation(OutputStream& os)
				{
					os << "# Snow mesh export (SnowSim)" << std::endl;

					int eg = 0;
					std::pair<int, int> count = make_pair(0, 0);

					for (unsigned int i=0; i < edge_groups.size(); ++i) {
							std::ostringstream stm;
							stm << "EdgeGroup" << eg;
							count = edge_groups[i]->getObjRepresentation( os, stm.str(), count.first, count.second );
						++eg;
					}
				}
			};
		}
}
