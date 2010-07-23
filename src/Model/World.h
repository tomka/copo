#pragma once

#include "types.h"
#include "SnowModel.h"
#include "ISky.h"
#include <CGAL/Range_tree_d.h>
#include <boost/signal.hpp>
#include <Rendering/RenderingInfo.h>

namespace SnowSim {
	namespace Model {
		class World
			{
			private:
				/// shall we render the bounding box
				bool doRenderBoundingBox;
				/// shall we render the sky
				bool doRenderSky;
				/// shall we render the sky hits
				bool doRenderSkyHits;
				/// shall we render the axes
				bool m_doRenderAxes;

				/// color of the bounding box
				point3f boundingBoxColor;

				SnowModel* snowModel;
				MeshPool* mesh_pool;

				void draw_BoundingBox();

				// the sky
				ISky* m_sky;

				// the geometry range tree
				//typedef CGAL::Range_tree_d<>	RangeTree;
				//RangeTree	m_rangeTree;

				boost::signals::connection m_mesh_changed_connection;

			public:
				World(MeshPool* meshPool);
				~World(void);

				SnowModel* GetSnowModel() { return snowModel; };
				MeshPool* GetBaseMeshs() { return mesh_pool; };

				void RebuildRangeTree(MeshPool*);

				void render(Rendering::RenderingInfo& info);

				void onChanged() {
					Changed(this);
				}

				boost::signal<void (World*)> Changed;

				/// Gets the world bounding box,
				/// including meshes and snow
				BBox3f BoundingBox() const;

				/// Updates the bounding box information.
				void UpdateBoundingBox();

				/// return whether the boundingbox is rendered
				bool renderBoundingBox() const { return doRenderBoundingBox; }
				/// set whether the boundingbox is rendered
				void setRenderBoundingBox(bool render) { doRenderBoundingBox = render; }

				/// return whether the axes are rendered
				bool GetRenderAxes() const { return m_doRenderAxes; }
				/// set whether the axes should be rendered or not
				void SetRenderAxes(bool render) { m_doRenderAxes = render; }

				/// set whether the sky is rendered
				void SetRenderSky(bool render) { doRenderSky = render; }
				bool GetRenderSky() { return doRenderSky; }

				/// set whether the flake hits in the sky is rendered
				void SetRenderSkyHits(bool render) { doRenderSkyHits = render; }
				bool GetRenderSkyHits() { return doRenderSkyHits; }

				// intersection tests
				bool Intersect(point3f& p1, point3f& p2, point3f& intersection, bool segment = true );
				bool Intersect(const ray3f& r, point3f& intersection);

				ISky* GetSky() {
					return m_sky;
				}

				// Gets the camera eye position
				point3f GetEyePosition();
			};
	}
}
