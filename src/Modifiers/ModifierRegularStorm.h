#pragma once

#include "Modifier.h"
#include <Model/World.h>

namespace SnowSim {
	namespace Modifiers {

		class ModifierRegularStorm : public Modifier
		{
		private:

			typedef SnowSim::Modifiers::Modifiers	Modifiers;
			typedef SnowSim::Model::World			World;

			SnowSim::Model::World * world;
			bool	m_isInitalized;

		public:

			ModifierRegularStorm(SnowSim::Model::World * world);
			~ModifierRegularStorm(void);

			bool execute(void);
			const char * getName();

			static const char * getModifierName() {
				return "Regular Storm";
			}
		};

	}
}
