#pragma once

#include "Flake_Schema.h"
#include <Model/ILaunchSite.h>
#include <boost/random.hpp>
#include <ctime>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/lagged_fibonacci.hpp>
#include <Utilities/Random.h>

#include <fltk/Group.h>
#include <fltk/ValueInput.h>

using namespace cgv::scene::geometry;

namespace SnowSim {
	namespace Model {
		namespace FlakeSchema {

			template<class T = float>
			class Flake_Schema_Normal : public Flake_Schema<T>, fltk::Group
				{
				protected:
					typedef typename boost::normal_distribution<T>						distribution;
					typedef	typename boost::lagged_fibonacci19937						engine;
					typedef typename boost::variate_generator< engine, distribution >	generator;
					typedef typename Utilities::Random								small_ran_gen;

					// the base height step for path interpolation
					T m_base_height_step;

					// the mean value for the circle radius normal distribution
					T m_mean;
					// the standard derivation for the circle radius normal distribution
					T m_std_derivation;

					distribution	m_distribution;
					engine 			m_ran_engine;
					generator		m_sampler;

					// out own small random generator
					small_ran_gen	m_random_gen;

					// GUI Elements

					// ValueInput for changing the base height step
					fltk::ValueInput m_gui_vinput_base_height;
					// ValueInput for changing the mean
					fltk::ValueInput m_gui_vinput_mean;
					// ValueInput for changing the std derivation
					fltk::ValueInput m_gui_vinput_std_derivation;

				private:

					T normal_dist();

					/// static callback for setting changes
					static inline void callback_settings(fltk::Widget *w,void *data)  {
						((Flake_Schema_Normal*)data)->callback_settings();
					}
					void callback_settings();

				protected:

					// updates the normal_distribution with the currently
					// set mean and standard derivation
					void update_distribution();

					// updates the ui elements
					void update_ui();


				public:
					Flake_Schema_Normal(SnowModel* model, World* world)
						:	Flake_Schema<T>(model, world),
							fltk::Group(0,0,100,120),
							m_base_height_step(5.0),
							m_mean(4.0),
							m_std_derivation(1.0),
							m_distribution(m_mean, m_std_derivation),
							m_sampler(m_ran_engine, m_distribution),
							m_random_gen(std::time(0)),
							m_gui_vinput_base_height(65,0,20,20, "Height step"),
							m_gui_vinput_mean(65,20,20,20, "Mean"),
							m_gui_vinput_std_derivation(65,40,20,20, "Std. Derivation")

					{
						// unit: centimeter
						this->m_flakeFlutter = m_mean;

						// GUI

						m_gui_vinput_base_height.linesize(0.1);
						m_gui_vinput_base_height.range(0.1f,std::numeric_limits<double>::infinity());
						m_gui_vinput_base_height.callback(callback_settings, this);

						m_gui_vinput_mean.linesize(0.1);
						m_gui_vinput_mean.range(0.0f,std::numeric_limits<double>::infinity());
						m_gui_vinput_mean.callback(callback_settings, this);

						m_gui_vinput_std_derivation.linesize(0.1);
						m_gui_vinput_std_derivation.range(0.0f,std::numeric_limits<double>::infinity());
						m_gui_vinput_std_derivation.callback(callback_settings, this);

						end();

						add(m_gui_vinput_base_height);
						add(m_gui_vinput_mean);
						add(m_gui_vinput_std_derivation);

						update_ui();
					}

					~Flake_Schema_Normal(void) { }

					void shootFlakes(ILaunchSite<PrecisionType>* ls, int batch_size, ShootingResult& result)
					{
						// get position of launch site
						point3f start( ls->GetPosition() );
						point3f curr_location(start);
						point3f last_location;
						point3f hit; // a variable for intersection testing
						PrecisionType sky_height = this->m_world->BoundingBox().max[1];

						// get the sky hit point
						result.hits.clear();
						result.batch_size = batch_size;

						// the radius for the circle is determined by a
						// normal distribution with
						// standard derivation f_o and mean f_u
						// (see Fearing p. 126)

						for (int f=0; f<batch_size; ++f)
						{
							// optimistically assume that we hit the sky ;)
							bool sky_hit = true;

							// as long as the the current location
							// is below the sky do...
							do {

								T heightstep = m_base_height_step;
								T radius =  m_sampler();

								// Pick random point on circle
								// from: http://mathworld.wolfram.com/CirclePointPicking.html
								T x1,x2,sq;
								do
								{
									x1 = m_random_gen.random(-1.0, 1.0);
									x2 = m_random_gen.random(-1.0, 1.0);
									sq = (x1*x1 + x2*x2);
								} while ( sq > 1.0 );

								sq = 1 / sq;

								// x_circ and y_circ will be a random point on a unit circle
								T x_circ = (x1 * x1 - x2 * x2) * sq;
								T y_circ = (2.0 * x1 * x2) * sq;


								// remember last location
								last_location = curr_location;

								// new flake path point
								curr_location.x() += x_circ * radius;
								curr_location.y() += heightstep;
								curr_location.z() += y_circ * radius;

								// check for intersection of current path segment with world
								if( (this->m_world->Intersect(last_location, curr_location, hit, true))
									//|| m_model->IsIntersectingModel( start, end, hit ) )
									) // intersection with snow model?
								{
									sky_hit = false;
									break;
								}

							} while (curr_location.y() < sky_height);

							// did we hit the sky?
							if ( sky_hit == true )
							{
								point2f sky_hit;
								point3f dir = curr_location - last_location;
								ray3f ray( last_location, dir );
								this->m_world->GetSky()->Intersect ( ray, sky_hit );

								result.hits.push_back( point2f( sky_hit.x(), sky_hit.y() ) );
							} else {
								// reset sky_hit assumption
								sky_hit = true;
							}

							// reset start location
							curr_location = start;
						}
					}

					string getName() {
						return std::string("Normal helix");
					}

					Flake_Schemata GetType() {
						return Normal;
					}

					fltk::Widget* GetPropertiesWidget() {
						return this;
					}
				};

			template<class T>
			T Flake_Schema_Normal<T>::normal_dist()
			{
				distribution l_norm_dist(m_mean, m_std_derivation);
				static engine l_ran_engine;

				generator l_sampler(l_ran_engine, l_norm_dist);

				return l_sampler();
			}

			template<class T>
			void Flake_Schema_Normal<T>::callback_settings()
			{
				m_base_height_step = static_cast<T>( m_gui_vinput_base_height.value() );
				m_mean = static_cast<T>( m_gui_vinput_mean.value() );
				m_std_derivation = static_cast<T>( m_gui_vinput_std_derivation.value() );

				this->m_flakeFlutter = m_mean;

				update_distribution();
			}

			// updates the normal_distribution with the currently
			// set mean and standard derivation
			template<class T>
			void Flake_Schema_Normal<T>::update_distribution()
			{
				this->m_distribution = distribution( this->m_mean, this->m_std_derivation );
				this->m_sampler = generator( this->m_ran_engine, this->m_distribution );
			}

			// updates the ui elements
			template<class T>
			void Flake_Schema_Normal<T>::update_ui()
			{
				m_gui_vinput_base_height.value(m_base_height_step);
				m_gui_vinput_mean.value(m_mean);
				m_gui_vinput_std_derivation.value(m_std_derivation);
			}
		}
	}
}
