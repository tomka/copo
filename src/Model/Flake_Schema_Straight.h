#pragma once
#define FORCE_INTERSECTION_TEST 0

#include "Flake_Schema.h"
#include <Model/SnowModel.h>
#include <iostream>
#include <types.h>
#include <cgv/scene/geometry/ray.h>

namespace SnowSim {
	namespace Model {
		namespace FlakeSchema {

			template<class T = float>
			class Flake_Schema_Straight : public Flake_Schema<T>
				{
				public:
					Flake_Schema_Straight(Model::SnowModel* model, World* world)
							: Flake_Schema<T>(model, world)
					{
						this->m_flakeFlutter = 0.0;
					}

					~Flake_Schema_Straight(void) { }

					// only make one straight shot to the sky
					void shootFlakes(ILaunchSite<T>* ls, int batch_size, ShootingResult& result) {

							point3f start(ls->GetPosition());
							point3f end(ls->x(), this->m_world->BoundingBox().max[1], ls->z() );
							point3f hit;

							if( this->m_world->Intersect(start, end, hit, false) // intersection with world?
								||
								this->m_model->IsIntersectingModel( start, end, hit ) ) // intersection with snow model?
							{
								result.batch_size = batch_size;
							} else
							{
								result.batch_size = batch_size;

								// get the sky hit point
								result.hits.clear();

								// no real intersection test is needed, since we only use straight lines
		#if FORCE_INTERSECTION_TEST
								point2f sky_hit;
								m_world->GetSky()->Intersect(
									ray3f( ls->GetPosition(), dir3f( 0, 1, 0 ) ), sky_hit );
		#else
								point2f sky_hit(ls->x(), ls->z());
		#endif

								// add batch_size times the hit point in the sky
								for (int i = 0; i < batch_size; ++i)
									result.hits.push_back(sky_hit);
							}
						}

					string getName() {
						return std::string("Straight");
					}

					Flake_Schemata GetType() {
						return Straight;
					}
				};
		}
	}
}
