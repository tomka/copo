#pragma once
#include <MainControler.h>
#include "ModifierRegularStorm.h"
#include <Utilities/Timer.h>

namespace SnowSim {
	namespace Modifiers {

			ModifierRegularStorm::ModifierRegularStorm(World * world)
				:	Modifier(200, ModifierRegularStorm::getModifierName()),
					m_isInitalized(false)
			{
			}

			ModifierRegularStorm::~ModifierRegularStorm(void)
			{
			}

			bool ModifierRegularStorm::execute(void)
			{
				if ( !m_isInitalized )
				{
					Utilities::Timer t;

					MainControler* mc = MainControler::GetInstance();

					t.start();
					mc->AddModifier( MOD_Triangulate );
					mc->AddModifier( MOD_CreateEdgeGroups );
					mc->AddModifier( MOD_CreateLaunchSites );
					mc->AddModifier( MOD_AccumulateSnow );
					mc->AddModifier( MOD_StabilizeSnow );
					t.stop();
					m_isInitalized = true;

					//std::cout << "[Modifier] Regular storm:" << std::endl << "\t*Time: " << t.diffTimeInMSec() << "msec" << std::endl;
				}

				return true;
			}

			const char * ModifierRegularStorm::getName()
			{
				return ModifierRegularStorm::getModifierName();
			}
	}
}
