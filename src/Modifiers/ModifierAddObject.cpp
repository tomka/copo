#include "ModifierAddObject.h"

namespace SnowSim {
	namespace Modifiers {

		ModifierAddObject::ModifierAddObject(void)
			: Modifier(200, "Add Object") { }
 
		ModifierAddObject::~ModifierAddObject(void) { }

		bool ModifierAddObject::execute(void) {
			return true;
		}

		const char * ModifierAddObject::getName() {
			return ModifierAddObject::getModifierName();
		}
	}
}
