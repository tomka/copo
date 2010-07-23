// Random.h
#ifndef RANDOM_H
#define RANDOM_H

#include "math.h"
#include <types.h>


namespace SnowSim {
	namespace Utilities {
		class Random {
			private:
				int seed;

				 /*
				 *  biggest int value
				 */
				 static const int MAX_RANDOM = 0x7fffffff;

				 /*
				 *	The class can be used as singleton, too.
				 */
				 static Random *m_instance;

			public:
				 /*
				 *  Constructor. Default seed value: 0
				 */
				 Random(const int seed = 0) {
					 this->seed = seed;
				 }

				 static Random* GetInstance() {
					 if ( m_instance == NULL ) {
						m_instance = new Random(27);
					 }

					 return m_instance;
				 }

				 /*
				 *  return random int between 0 and MAX_RANDOM
				 */
				 int randomInt() {
					 seed = seed * 16807 + 1;
					 return (seed & Random::MAX_RANDOM);
				 }
				 /*
				 *  return random float between 0 and 1
				 */
				 float randomFloat() {
					return ((float)randomInt() / Random::MAX_RANDOM);
				 }
				 /*
				 *  return random float between min and max
				 */
				 float randomFloat(const float min, const float max) {
					return ((randomFloat() * fabs(max - min)) + min);
				 }
				  /*
				 *  return random double between 0 and 1
				 */
				 double randomDouble() {
					return ((double)randomInt() / Random::MAX_RANDOM);
				 }
				 /*
				 *  return random double between min and max
				 */
				 double randomDouble(const double min, const double max) {
					return ((randomDouble() * fabs(max - min)) + min);
				 }

				PrecisionType random(const PrecisionType min, const PrecisionType max) {
					return ((random() * fabs(max - min)) + min);
				}

				PrecisionType random() {
					return ((PrecisionType)randomInt() / Random::MAX_RANDOM);
				}

				/// returns true with a propability of p
				bool randomBernoulli(PrecisionType p) {
					return ( random(0,1) < p ) ? true : false;
				}

				 /*
				 *  set the seed value
				 */
				 inline void setSeed(const int seed) {
					this->seed = seed;
				 }

				 /*
				 *  get the seed value
				 */
				 inline int getSeed() const {
					return seed;
				 }

		};

	}
}
#endif
