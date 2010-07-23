#include "World.h"
#include "SkyGrid.h"
#include "Mesh.h"
#include <boost/bind.hpp>

namespace SnowSim {
	namespace Model {
		World::World(MeshPool* meshPool)
			:	doRenderBoundingBox(false),
				doRenderSky(false),
				doRenderSkyHits(false),
				m_doRenderAxes(true),
				boundingBoxColor(red_color)
			{
				mesh_pool = meshPool;
				RebuildRangeTree(mesh_pool);

				// Add event handler

				m_mesh_changed_connection =
					mesh_pool->Changed.connect(boost::bind(&World::RebuildRangeTree, this, _1));

				snowModel = new SnowModel(this, meshPool);

				m_sky = new SkyGrid(this);

			}

		void World::RebuildRangeTree(MeshPool*)
		{

		}

		/// Gets the world bounding box,
		/// including meshes and snow
		BBox3f World::BoundingBox() const {
			BBox3f bb = mesh_pool->BoundingBox();
			// unite bb with the edge group and snow bounding box
			// TODO
			BBox3f snow_bb = snowModel->GetBoundingBox();

			point3f min = bb.min;
			point3f max = bb.max;

			if ( min.x() > snow_bb.min.x() ) min.x() = snow_bb.min.x();
			if ( min.y() > snow_bb.min.y() ) min.y() = snow_bb.min.y();
			if ( min.z() > snow_bb.min.z() ) min.z() = snow_bb.min.z();

			if ( max.x() < snow_bb.max.x() ) max.x() = snow_bb.max.x();
			if ( max.y() < snow_bb.max.y() ) max.y() = snow_bb.max.y();
			if ( max.z() < snow_bb.max.z() ) max.z() = snow_bb.max.z();

			return BBox3f(min, max);
		}

		void World::UpdateBoundingBox()
		{
			snowModel->checkBoundingBox();
		}

		/* Intersection test
		 * see Fearing PhD (p. 154) for range tree description
		*/
		bool World::Intersect(point3f& p1, point3f& p2, point3f& intersection, bool segment /* = true */ )
		{
			// go through all meshs in the pool
			for (int i = mesh_pool->getNrMeshs(); i != 0; --i)
			{
				// get a mesh
				Mesh* mesh = mesh_pool->getMesh(i - 1);
				SnowSim::polygonal_mesh* pmesh = mesh->mesh();

				// go through all faces
				polygonal_mesh::face* s_face = pmesh->face_source;
				if ( s_face == NULL )
					continue;
				do {
					PrecisionType t;

					if ( SnowSim::Utilities::Intersect::intersect(s_face, p1, p2, segment, t, false, intersection) )
					{
						if ( t > NearlyZero && t < ( 1.0 - NearlyZero) )
							return true;
					}

					s_face = s_face->next_face;
				} while (s_face != NULL);
			}

			return false;
		}

		/* Intersection test
		 * see Fearing PhD (p. 154) for range tree description
		*/
		bool World::Intersect(const ray3f& r, point3f& intersection)
		{
			// go through all meshs in the pool
			for (int i = mesh_pool->getNrMeshs(); i != 0; --i)
			{
				// get a mesh
				Mesh* mesh = mesh_pool->getMesh(i - 1);
				SnowSim::polygonal_mesh* pmesh = mesh->mesh();

				// go through all faces
				polygonal_mesh::face* s_face = pmesh->face_source;
				if ( s_face == NULL )
					continue;
				do {

					if ( SnowSim::Utilities::Intersect::intersect(s_face, r, false, intersection) )
						return true;

					s_face = s_face->next_face;
				} while (s_face != NULL);
			}

			return false;
		}

		void World::draw_BoundingBox() {

			glDisable(GL_LIGHTING);

			BBox3f bb = mesh_pool->BoundingBox();

			point3f * min = &bb.min;
			point3f * max = &bb.max;

			glColor3f(boundingBoxColor.x(), boundingBoxColor.y(), boundingBoxColor.z());
			glLineWidth(1.0f);
			glBegin(GL_LINE_LOOP);
				glVertex3f(min->x(), min->y(), min->z());
				glVertex3f(min->x(), max->y(), min->z());
				glVertex3f(max->x(), max->y(), min->z());
				glVertex3f(max->x(), min->y(), min->z());
			glEnd();
			glBegin(GL_LINE_LOOP);
				glVertex3f(min->x(), min->y(), max->z());
				glVertex3f(min->x(), max->y(), max->z());
				glVertex3f(max->x(), max->y(), max->z());
				glVertex3f(max->x(), min->y(), max->z());
			glEnd();
			glBegin(GL_LINES);
				glVertex3f(min->x(), min->y(), min->z());
				glVertex3f(min->x(), min->y(), max->z());
				glVertex3f(min->x(), max->y(), min->z());
				glVertex3f(min->x(), max->y(), max->z());
				glVertex3f(max->x(), min->y(), min->z());
				glVertex3f(max->x(), min->y(), max->z());
				glVertex3f(max->x(), max->y(), min->z());
				glVertex3f(max->x(), max->y(), max->z());
			glEnd();
		}

		World::~World(void)
		{
			// Remove event handler
			//mesh_pool->Changed += event_handler(this, &World::RebuildRangeTree);
			m_mesh_changed_connection.disconnect();
		}



		void World::render(Rendering::RenderingInfo& info) {

			mesh_pool->render();
			snowModel->render(info);

			if (doRenderBoundingBox)
				draw_BoundingBox();

			if (doRenderSky)
				m_sky->Render();

			if (doRenderSkyHits)
				m_sky->RenderHits();

			if (m_doRenderAxes)
			{
				glDisable(GL_LIGHTING);

				// render axes
				glLineWidth(2.0f);
				glColor3f(0.0f, 0.0f ,0.0f);
				glBegin(GL_LINES);
					glVertex4f(0.0f, 0.0f, 0.0f, 1.0f); // X direction
					glVertex4f(1.0f, 0.0f, 0.0f, 0.0f);
					glVertex4f(0.0f, 0.0f, 0.0f, 1.0f); // Y direction
					glVertex4f(0.0f, 1.0f, 0.0f, 0.0f);
					glVertex4f(0.0f, 0.0f, 0.0f, 1.0f); // Z direction
					glVertex4f(0.0f, 0.0f, 1.0f, 0.0f);
					for (int i = 0; i < 52; ++i) {
						float pos = static_cast<float>(i);

						glVertex4f(pos, 0.0f, 0.0f, 1.0f); // X direction
						glVertex4f(pos, -0.1f, 0.0f, 1.0f);
						glVertex4f(0.0f, pos, 0.0f, 1.0f); // Y direction
						glVertex4f(0.1f, pos, 0.0f, 1.0f);
						glVertex4f(0.0f, 0.0f, pos, 1.0f); // Z direction
						glVertex4f(0.0f, -0.1f, pos, 1.0f);
					}
				glEnd();
			}
		}
	}
}
