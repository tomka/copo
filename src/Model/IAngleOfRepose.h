#pragma once

#include "types.h"
#include <string>

namespace SnowSim {

	namespace Model {

		/// Represents an angle of repose - this is a 2D function
		/// which gives back a propability to an angle.
		template<class T = PrecisionType>
		class IAngleOfRepose {
			public:

				/// Gets the probability that the snow will stay
				/// where it is (e. g. will not be put onto the
				/// lower neighbors)
				virtual T GetProbability(T angle) = 0;

				/// Calculates a potentially stable angle
				/// lower then the given "startAngle"
				/// Measured in radiants.
				virtual T GetStableAngle(T startAngle) = 0;
		};
	}
}
