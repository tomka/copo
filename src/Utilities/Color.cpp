#include "Color.h"

namespace SnowSim {
	namespace Utilities {
		namespace Color {

			/// Generates a random number
			point3f
			GetRandomColor()
			{
				SnowSim::Utilities::Random* ran = Utilities::Random::GetInstance();

				return point3f(
					ran->random(),
					ran->random(),
					ran->random());
			}
		}
	}
}
