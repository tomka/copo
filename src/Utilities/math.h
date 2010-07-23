#pragma once

#include <types.h>

namespace SnowSim {
	namespace Utilities {
		namespace Math {

			template<class T>
			T DegToRad(T degree)
			{
				return (2 * PI) * (degree / 360.0);
			}

			template<class T>
			T RadToDeg(T radiants)
			{
				return radiants * 360.0 / ( 2 * PI );
			}
		}
	}
}
