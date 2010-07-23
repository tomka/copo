#pragma once
#define FORCE_INTERSECTION_TEST 0

#include "Flake_Schema.h"
#include <Model/SnowModel.h>
#include <iostream>
#include <types.h>
#include <cgv/scene/geometry/ray.h>
#include <Utilities/Random.h>
#include <Utilities/math.h>

#include <fltk/Group.h>
#include <fltk/ValueInput.h>
#include <fltk/CheckButton.h>

using namespace SnowSim::Utilities;

namespace SnowSim {
	namespace Model {
		namespace FlakeSchema {

			template<class T = float>
			class Flake_Schema_DynStraight : public Flake_Schema<T>, fltk::Group
				{

				private:
					// the maximum angle the ray can alter its
					// direction to the straight up line
					PrecisionType max_angle;

					// True, if every flake should be varyed
					// False, if whole batches should be varyed
					bool m_vary_every_flake;

					// The random class
					::Utilities::Random m_random;

					// value input for changing the maximum angle
					fltk::ValueInput m_gui_viniput_max_angle;

					// check box for setting if to vary on every flake
					fltk::CheckButton m_gui_cb_vary_once_per_batch;

				protected:
					/// static callback for setting changes
					static inline void callback_settings(fltk::Widget *w,void *data)  {
						((Flake_Schema_DynStraight*)data)->callback_settings();
					}
					void callback_settings();

				public:
					Flake_Schema_DynStraight(Model::SnowModel* model, World* world)
							: 	Flake_Schema<T>(model, world),
								fltk::Group(0,0,100,120),
								max_angle(0.125 * PI),
								m_vary_every_flake(true),
								m_gui_viniput_max_angle(55,0,30,20, "Max. angle"),
								m_gui_cb_vary_once_per_batch(3,20,100,20, "Vary only per batch")
					{
						// GUI
						m_gui_viniput_max_angle.linesize(1.0);
						m_gui_viniput_max_angle.range(0.1f,90.0f);
						m_gui_viniput_max_angle.callback(callback_settings, this);
						m_gui_viniput_max_angle.value( ::Utilities::Math::RadToDeg(max_angle) );

						end();

						this->m_flakeFlutter = 0.0;

						add(m_gui_viniput_max_angle);
						add(m_gui_cb_vary_once_per_batch);
					}

					~Flake_Schema_DynStraight(void) { }

					// only make one straight shot to the sky
					void shootFlakes(ILaunchSite<T>* ls, int batch_size, ShootingResult& result) {

							point3f start(ls->GetPosition());
							point3f hit;
							PrecisionType sky_height = this->m_world->BoundingBox().max[1];
							ray3f ray( start, dir3f( 0, 1, 0 ) );

							// clear the result
							result.hits.clear();

							if (m_vary_every_flake) {
								#pragma region Vary every flake
								// Vary every flake

								point2f sky_hit;
								this->m_world->GetSky()->Intersect( ray, sky_hit );

								PrecisionType max_distance = tan(max_angle) * ( sky_height - ray.origin.z() );

								result.batch_size = batch_size;

								for (int i = 0; i < batch_size; ++i) {

									ray3f ray( start, dir3f( 0, 1, 0 ) );

									PrecisionType x_noise = sky_hit.x();
									x_noise = x_noise + m_random.random(-max_distance, max_distance);

									PrecisionType y_noise = sky_hit.y();
									y_noise = y_noise + m_random.random(-max_distance, max_distance);

									point3f end(x_noise, sky_height, y_noise );

									if( ! (this->m_world->Intersect(start, end, hit, false)) // intersection with world?
										//|| m_model->IsIntersectingModel( start, end, hit ) )
										) // intersection with snow model?
									{
										// add the hit point in the sky
										result.hits.push_back( point2f(x_noise, y_noise) );
									}
								}

								#pragma endregion
							} else {
								#pragma region Vary only once per batch
								// Vary only once per batch

								point2f sky_hit;
								this->m_world->GetSky()->Intersect( ray, sky_hit );


								PrecisionType max_distance = tan(max_angle) * ( sky_height - ray.origin.z() );

								PrecisionType x_noise = sky_hit.x();
								x_noise = x_noise + m_random.random(-max_distance, max_distance);

								PrecisionType y_noise = sky_hit.y();
								y_noise = y_noise + m_random.random(-max_distance, max_distance);

								point3f end(x_noise, sky_height, y_noise );

								if( this->m_world->Intersect(start, end, hit, false) // intersection with world?
									//|| m_model->IsIntersectingModel( start, end, hit ) // intersection with snow model?
									)
								{
									result.batch_size = batch_size;
								} else
								{
									sky_hit.x() = x_noise;
									sky_hit.y() = y_noise;

									result.batch_size = batch_size;

									// add batch_size times the hit point in the sky
									for (int i = 0; i < batch_size; ++i)
										result.hits.push_back(sky_hit);
								}

								#pragma endregion
							}
						}

					string getName() {
						return std::string("Straight with angle variation");
					}

					Flake_Schemata GetType() {
						return DynStraight;
					}

					fltk::Widget* GetPropertiesWidget() {
						return this;
					}
				};

			template<class T>
			void Flake_Schema_DynStraight<T>::callback_settings()
			{
				m_vary_every_flake = !( m_gui_cb_vary_once_per_batch.state() );
				max_angle = ::Utilities::Math::DegToRad( m_gui_viniput_max_angle.value() );
			}
		}
	}
}
