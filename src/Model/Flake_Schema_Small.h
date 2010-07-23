#pragma once

#include <Model/Flake_Schema_Normal.h>
#include <Model/ILaunchSite.h>

using namespace cgv::scene::geometry;

namespace SnowSim {
	namespace Model {
		namespace FlakeSchema {

			template<class T = float>
			class Flake_Schema_Small : public Flake_Schema_Normal<T>
				{
				private:
					typedef Flake_Schema_Normal<T>			Base;
					typedef typename Base::distribution		distribution;
					typedef typename Base::generator		generator;

				public:
					Flake_Schema_Small(SnowModel* model, World* world)
						: 	Flake_Schema_Normal<T>(model, world)
					{

						this->m_base_height_step = 1.0;
						this->m_mean = 1.0;
						this->m_std_derivation = 0.5;
						this->update_distribution();
						this->m_flakeFlutter = this->m_mean;

						Base::update_ui();
					}

					~Flake_Schema_Small(void) { }

					string getName() {
						return std::string("Small helix");
					}

					Flake_Schemata GetType() {
						return Small;
					}

				};
		}
	}
}
