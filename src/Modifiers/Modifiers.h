#pragma once

namespace SnowSim {
	namespace Modifiers {

		enum Modifiers {	MOD_Triangulate, MOD_SubDivide, MOD_CreateEdgeGroups,
							MOD_CreateLaunchSites, MOD_AccumulateSnow, MOD_StabilizeSnow,
							MOD_RegularSnow, MOD_ShadeSnow, MOD_ExtraBorderPoints };
	}
}

#include "Modifier.h"
#include "ModifierAddObject.h"
#include "ModifierTriangulate.h"
#include "ModifierCreateEdgeGroups.h"
#include "ModifierCreateLaunchSites.h"
#include "ModifierAccumulateSnow.h"
#include "ModifierSnowStability.h"
#include "SubDivide/ModifierSubDivide.h"
#include "ModifierRegularStorm.h"
#include "ModifierShadeSnowModel.h"
#include "ModifierAddAdditionalBorderPoints.h"
#include "Exceptions/PreConditionsViolatedException.h"
