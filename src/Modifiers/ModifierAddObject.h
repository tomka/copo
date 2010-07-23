#pragma once
#include "Modifier.h"

namespace SnowSim {
	namespace Modifiers {

		class ModifierAddObject : public Modifier
		{
		public:
			ModifierAddObject(void);
			~ModifierAddObject(void);
			
			bool execute(void);
			const char * getName();

			static const char * getModifierName() {
				return "Add Object";
			}

		};

	}
}