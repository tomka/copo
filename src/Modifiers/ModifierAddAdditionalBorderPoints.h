/*
 * ModifierAddAdditionalBorderPoints.h
 *
 *  Created on: Sep 6, 2009
 *      Author: tom
 */

#ifndef MODIFIERADDADDITIONALBORDERPOINTS_H_
#define MODIFIERADDADDITIONALBORDERPOINTS_H_

#include "Modifier.h"
#include <Model/World.h>

namespace SnowSim {
	namespace Modifiers {

	class ModifierAddAdditionalBorderPoints
		 : public Modifier
	{
		private:
			// the world
			SnowSim::Model::World* m_world;

		 public:
			 ModifierAddAdditionalBorderPoints(World * world);
			 virtual ~ModifierAddAdditionalBorderPoints();

			 // Command
			bool execute(void);

			const char * getName();
			static const char * getModifierName() {
				return "Extra border points";
			}
	};
	}
}

#endif /* MODIFIERADDADDITIONALBORDERPOINTS_H_ */
