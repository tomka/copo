#pragma once
#include "Modifier.h"
#include "../polygonal_mesh.h"
#include <Model/World.h>

namespace SnowSim {
	namespace Modifiers {

		class ModifierTriangulate : public Modifier
		{
		private:
			SnowSim::Model::World * world;
			polygonal_mesh ** mesh;
			
			typedef SnowSim::Model::World World;

		public:
			ModifierTriangulate(SnowSim::Model::World * world);
			~ModifierTriangulate(void);

			bool execute(void);
			const char * getName();

			static const char * getModifierName() {
				return "Triangulate";
			}
		};

	}
}
