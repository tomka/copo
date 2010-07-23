#include "ModifierTriangulate.h"
#include "Exceptions/PreConditionsViolatedException.h"
#include "../MeshPool.h"

namespace SnowSim {
	namespace Modifiers {

		ModifierTriangulate::ModifierTriangulate(World * world)
			: Modifier(200, ModifierTriangulate::getModifierName())
		{
				// GUI
				this->world = world;
		}

		ModifierTriangulate::~ModifierTriangulate(void) { }

		bool ModifierTriangulate::execute(void)
		{

			MeshPool* pool = world->GetBaseMeshs();

			for (unsigned int i=0; i < pool->getNrMeshs(); i++) {
				pool->getMesh(i)->mesh()->triangulate();
			}

			return true;
		}

		const char * ModifierTriangulate::getName(){
			return ModifierTriangulate::getModifierName();
		}
	}
}
