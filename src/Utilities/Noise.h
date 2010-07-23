#pragma once
#include <types.h>
#include <math.h>

namespace SnowSim {
	namespace Utilities {
		namespace Noise
		{
			#pragma region Interpolation

			/// Lineare interpolation:
			/// Looks awful, like those cheap 'plasmas' that everyone
			/// uses to generate landscapes. It's a simple algorithm though,
			/// and I suppose would be excusable if you were trying
			/// to do perlin noise in realtime.
			template<typename T>
			T Linear_Interpolate(T a, T b, T x)
			{
				return  a*(1-x) + b*x;
			}

			/// Cosine interpolation:
			/// This method gives a much smother curve than Linear
			/// Interpolation. It's clearly better and worth the
			/// effort if you can afford the very slight loss in speed.
			template<typename T>
			T Cosine_Interpolate(T a, T b, T x)
			{
				T ft = x * 3.1415927;
				T f = (1 - cos(ft)) * 0.5;

				return  a*(1-f) + b*f;
			}

			/// Cubic interpolation:
			/// This method gives very smooth results indeed,
			/// but you pay for it in speed. To be quite honest,
			/// I'm not sure if it would give noticeably better
			/// results than Cosine Interpolation, but here it is
			/// anyway if you want it. It's a little more complicated,
			/// so pay attention. Whereas before, the interpolation functions
			/// took three inputs, the cubic interpolation takes five.
			/// Instead of just a and b, you now need v0, v1, v2 and v3,
			/// along with x as before. These are:
			template<typename T>
			T Cubic_Interpolate(T v0, T v1, T v2, T v3, T x)
			{
				T P = (v3 - v2) - (v0 - v1);
				T Q = (v0 - v1) - P;
				T R = v2 - v0;
				T S = v1;

				return P*x*x*x + Q*x*x + R*x + S;
			}

			#pragma endregion

			template<typename T>
			T Noise1(int x)
			{
				x = (x << 13) ^ x;
				return (T) ( 1.0 - ( (x * (x * x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
			}

			template<typename T>
			T Noise1(T x, T y)
			{
				int n = static_cast<int>(x + y * 57);
				n = (n<<13) ^ n;

				return (T) ( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
			}

			template<typename T>
			T Noise1(T x, T y, T z)
			{
				int n = static_cast<int>(x + y * 57 + z * 57 * 57);
				n = (n<<13) ^ n;

				return (T) ( 1.0 - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0);
			}

			template<typename T>
			T SmoothedNoise_1(T x)
			{
				return Noise1<T>(x) * 0.5  +  Noise1<T>(x-1) * 0.25  +  Noise1<T>(x+1) * 0.25;
			}

			template<typename T>
			T SmoothedNoise_1(T x, T y)
			{
				T corners = ( Noise1<T>(x-1, y-1) + Noise1<T>(x+1, y-1) + Noise1<T>(x-1, y+1) + Noise1<T>(x+1, y+1) ) * (1.0 / 16.0);
				T sides   = ( Noise1<T>(x-1, y)   + Noise1<T>(x+1, y)   + Noise1<T>(x, y-1)   + Noise1<T>(x, y+1) ) * (1.0 / 8.0);
				T center  =  Noise1<T>(x, y) * ( 1.0 / 4.0);

				return corners + sides + center;
			}

			template<typename T>
			T SmoothedNoise_1(T x, T y, T z)
			{
				T corners = ( Noise1<T>(x-1, y-1, z-1) + Noise1<T>(x+1, y-1, z-1) + Noise1<T>(x-1, y+1, z-1) + Noise1<T>(x+1, y+1, z-1)
							+ Noise1<T>(x-1, y-1, z+1) + Noise1<T>(x+1, y-1, z+1) + Noise1<T>(x-1, y+1, z+1) + Noise1<T>(x+1, y+1, z+1)
							) * (1.0 / 32.0);
				T sides   = ( Noise1<T>(x-1, y, z) + Noise1<T>(x+1, y, z) + Noise1<T>(x, y-1, z) + Noise1<T>(x, y+1, z)
							+ Noise1<T>(x, y, z+1) + Noise1<T>(x, y, z-1)
							) * (1.0 / 12.0);
				T center  =  Noise1<T>(x, y, z) * ( 1.0 / 4.0);

				return corners + sides + center;
			}

			template<typename T>
			T InterpolatedNoise_1(T x)
			{
			  int integer_X		= int(x);
			  T fractional_X	= x - integer_X;

			  T v1 = SmoothedNoise1(static_cast<T>(integer_X));
			  T v2 = SmoothedNoise1(static_cast<T>(integer_X + 1));

			  return Cosine_Interpolate<T>(v1 , v2 , fractional_X);
			}

			template<typename T>
			T InterpolatedNoise_1(T x, T y)
			{
				int integer_X	= int(x);
				T fractional_X	= x - integer_X;

				int integer_Y	= int(y);
				T fractional_Y 	= y - integer_Y;

				T v1 = SmoothedNoise_1<T>(static_cast<T>(integer_X),     static_cast<T>(integer_Y));
				T v2 = SmoothedNoise_1<T>(static_cast<T>(integer_X + 1), static_cast<T>(integer_Y));
				T v3 = SmoothedNoise_1<T>(static_cast<T>(integer_X),     static_cast<T>(integer_Y + 1));
				T v4 = SmoothedNoise_1<T>(static_cast<T>(integer_X + 1), static_cast<T>(integer_Y + 1));

				T i1 = Cosine_Interpolate<T>(v1 , v2 , fractional_X);
				T i2 = Cosine_Interpolate<T>(v3 , v4 , fractional_X);

				return Cosine_Interpolate<T>(i1 , i2 , fractional_Y);
			}

			template<typename T>
			T InterpolatedNoise_1(T x, T y, T z)
			{
				int integer_X	= int(x);
				T fractional_X	= x - integer_X;

				int integer_Y	= int(y);
				T fractional_Y	= y - integer_Y;

				int integer_Z	= int(z);
				T fractional_Z	= z - integer_Z;

				T v1 = SmoothedNoise_1<T>(static_cast<T>(integer_X),     static_cast<T>(integer_Y),		static_cast<T>(integer_Z));
				T v2 = SmoothedNoise_1<T>(static_cast<T>(integer_X + 1), static_cast<T>(integer_Y),		static_cast<T>(integer_Z));
				T v3 = SmoothedNoise_1<T>(static_cast<T>(integer_X),     static_cast<T>(integer_Y + 1), static_cast<T>(integer_Z));
				T v4 = SmoothedNoise_1<T>(static_cast<T>(integer_X + 1), static_cast<T>(integer_Y + 1), static_cast<T>(integer_Z));
				T v5 = SmoothedNoise_1<T>(static_cast<T>(integer_X),     static_cast<T>(integer_Y),		static_cast<T>(integer_Z + 1));
				T v6 = SmoothedNoise_1<T>(static_cast<T>(integer_X + 1), static_cast<T>(integer_Y),		static_cast<T>(integer_Z + 1));
				T v7 = SmoothedNoise_1<T>(static_cast<T>(integer_X),     static_cast<T>(integer_Y + 1), static_cast<T>(integer_Z + 1));
				T v8 = SmoothedNoise_1<T>(static_cast<T>(integer_X + 1), static_cast<T>(integer_Y + 1), static_cast<T>(integer_Z + 1));

				T i1 = Cosine_Interpolate<T>(v1 , v2 , fractional_X);
				T i2 = Cosine_Interpolate<T>(v3 , v4 , fractional_X);
				T i3 = Cosine_Interpolate<T>(v5 , v6 , fractional_X);
				T i4 = Cosine_Interpolate<T>(v7 , v8 , fractional_X);

				T i5 = Cosine_Interpolate<T>(i1 , i2 , fractional_Y);
				T i6 = Cosine_Interpolate<T>(i3 , i4 , fractional_Y);

				return Cosine_Interpolate<T>(i5 , i6 , fractional_Z);
			}


			template<typename T>
			T PerlinNoise_1D(T x, int octaves, float persistence)
			{
				T total = 0;

				for (int i = 0; i < octaves; ++i) {
				  int frequency = pow(2,i);
				  T amplitude = pow(persistence,i);

				  total = total + InterpolatedNoise_1<T>(x * frequency) * amplitude;
				}

				return total;
			}

			/// Generates perlin noise between -1.0 and 1.0
			template<typename T>
			T PerlinNoise_2D(float x, float y, int octaves, float persistence)
			{
			  T total = 0;

			  /* loop through the octaves */
			  for (int i = 0; i < octaves; ++i) {

				  /* This increases the frequency with every loop of the octave. */
				  T frequency = pow(2.0, i);
				  /* This decreases the amplitude with every loop of the octave. */
				  T amplitude = pow(persistence, i);

				  total += InterpolatedNoise_1<T>(x * frequency, y * frequency) * amplitude;
			  }

			  return total;
			}

			/// Generates perlin noise between -1.0 and 1.0
			template<typename T>
			T PerlinNoise_3D(float x, float y, float z, int octaves, float persistence)
			{
			  T total = 0;

			  /* loop through the octaves */
			  for (int i = 0; i < octaves; ++i) {

				  /* This increases the frequency with every loop of the octave. */
				  T frequency = pow(2.0, i);
				  /* This decreases the amplitude with every loop of the octave. */
				  T amplitude = pow(persistence, i);

				  total += InterpolatedNoise_1<T>(x * frequency, y * frequency, z * frequency) * amplitude;
			  }

			  return total;
			}

		}
	}
}
