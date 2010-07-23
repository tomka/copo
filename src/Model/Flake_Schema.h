#pragma once

#include <cgv/scene/geometry/straight_line.h>
#include <cgv/scene/geometry/ray.h>
#include <types.h>
#include <string>
#include <fltk/Widget.h>

using namespace std;
using namespace cgv::scene::geometry;

namespace SnowSim {
	namespace Model {

		// forward declarations
		template<class T> class ILaunchSite;
		class SnowModel;
		class World;

		namespace FlakeSchema {

			// wird nicht mehr benoetigt
			struct ShootingResult
			{
				public:
					int batch_size;

					// hits of flakes in the sky
					std::vector<point2f> hits;

					ShootingResult() {
						batch_size = 0;
					};
			};

			enum Flake_Schemata
			{
				Straight,
				DynStraight,
				Small,
				Normal
			};

			template<class T = float>
			class Flake_Schema// : public iterator<ray<T, 3>>
				{
				protected:
					float m_flakeFlutter;
					SnowModel* m_model;
					World* m_world;

					Flake_Schema(Model::SnowModel* model, World* world)
						: m_model(model), m_world(world)
					 { }

				public:
					//~Flake_Schema(void);

					float getFlakeFlutter() {
						return m_flakeFlutter;
					}

					// Calculates a
					virtual void shootFlakes(ILaunchSite<T>* ls, int batch_size, ShootingResult& result) = 0;
					//virtual void shootFlakes(ILaunchSite<T>* ls, int batch_size, std::vector<point2d>& hits) = 0;

					// Gets the name of the schema
					virtual string getName() = 0;

					// Gets the type of the schema
					virtual Flake_Schemata GetType() = 0;

					// Could be implemented to return a property widget
					virtual fltk::Widget* GetPropertiesWidget()
					{
						return NULL;
					}

				};
		}
	}
}
