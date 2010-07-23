#pragma once

#include "IAngleOfRepose.h"
#include <types.h>
#include <boost/bind.hpp>
#include <boost/function.hpp>

namespace SnowSim {
	namespace Model {

		/// Represents an angle of repose - this is a 2D function
		/// which gives back a propability to an angle.
		template<class T = PrecisionType>
		class AORDefault
			: public IAngleOfRepose<T>
		{
			private:

				/// A function pointer to the actual evaluation
				/// function variants in this class.
				boost::function< T (T angle) > m_func_evaluate;

				/// Represents an angle of repose - this is a 2D function
				/// which gives back a propability to an angle.
				/// Private constructor - we need some initialization data.
				AORDefault();

				/// Constant evaluation function.
				T evalConstant(T angle);

				/// Linear evaluation function.
				T evalLinear(T angle);

				/// Combination Constant/Linear evaluation function.
				T evalCombinedConstLin(T angle);

				// Combination Constant/Constant evaluation function.
				T evalCombinedConstConst(T angle);

				/// The variable used to store a constant
				/// like for constant evaluation or the
				/// n-component in the linear case.
				T m_constant;

				/// The variable used to store a second constant
				/// like for constant evaluation or the
				/// n-component in the linear case.
				T m_constant_2;

				/// Stores the border angle if combination is used.
				T m_borderAngle;

				/// Stores the m-value of linear evaluation.
				T m_m;

				/// Stores the n-value (constant) of linear evaluation.
				T m_n;

				/// The stable angle
				T m_aor;

			public:

				//AngleOfRepose(std::string filename);

				/// An constructor for creating a constant
				/// angle of repose instance
				/// Form: x = c
				/// The stable angle is defined by aor.
				AORDefault(T aor, T c);

				/// An constructor for creating a linear
				/// angle of repose instance:
				/// Form: y = m*x + n
				/// The stable angle is defined by aor.
				AORDefault(T aor, T m, T n);

				/// An constructor for creating a combination
				/// of constant and linear angle of repose instance:
				/// Form < border_anlge: y = c
				/// Form >= border anlge: y = m*x + n
				/// The stable angle is defined by aor.
				AORDefault(T aor, T constant, T border_angle, T m, T n);

				/// An constructor for creating a combination
				/// of constant and linear angle of repose instance:
				/// Form < border_anlge: y = c
				/// Form >= border anlge: y = c2
				/// The stable angle is defined by aor.
				AORDefault(T aor, T constant, T border_angle, T constant2);

				/// Gets the probability that the snow will stay
				/// where it is (e. g. will not be put onto the
				/// lower neighbors)
				T GetProbability(T angle);

				/// Calculates a potentially stable angle
				/// lower then the given "startAngle"
				/// Measured in radiants.
				T GetStableAngle(T startAngle);
		};

		#pragma region Class implementation

		/// An constructor for creating a constant
		/// angle of repose instance
		/// Form: x = c
		template<class T>
		AORDefault<T>::AORDefault(T aor, T c)
			:	m_aor(aor),
				m_m(0),
				m_constant(c),
				m_n(0),
				m_borderAngle(0.0)

		{
			m_func_evaluate = boost::bind( &AORDefault::evalConstant, this, _1 );
		}

		/// An constructor for creating a linear
		/// angle of repose instance:
		/// Form: y = m*x + n
		template<class T>
		AORDefault<T>::AORDefault(T aor, T m, T n)
			:	m_aor(aor),
				m_m(m),
				m_n(n),
				m_constant(0),
				m_borderAngle(0.0)
		{
			m_func_evaluate = boost::bind( &AORDefault::evalLinear, this, _1 );
		}

		/* TODO:

		/// An constructor for creating a linear
		/// angle of repose instance by calculating m and n:
		/// Form: y = m*x + n
		template<class T>
		AORDefault<T>::AORDefault(point<T, 2> a, point<T, 2> a)
			:	m_m(m),
				m_n(n),
				m_constant(0),
				m_borderAngle(0.0)
		{
			m_func_evaluate = boost::bind( &AORDefault::evalLinear, this, _1 );
		}

		*/

		/// An constructor for creating a combination
		/// of constant and linear angle of repose instance:
		/// Form < border_anlge: y = c
		/// Form >= border anlge: y = m*x + n
		template<class T>
		AORDefault<T>::AORDefault(T aor, T constant, T border_angle, T m, T n)
			:	m_aor(aor),
				m_m(m),
				m_n(n),
				m_constant(constant),
				m_borderAngle(border_angle)
		{
			m_func_evaluate = boost::bind( &AORDefault::evalCombinedConstLin, this, _1 );
		}

		/// An constructor for creating a combination
		/// of constant and linear angle of repose instance:
		/// Form < border_anlge: y = c
		/// Form >= border anlge: y = c2
		/// The stable angle is defined by aor.
		template<class T>
		AORDefault<T>::AORDefault(T aor, T constant, T border_angle, T constant2)
			:	m_aor(aor),
				m_m(0),
				m_n(0),
				m_constant(constant),
				m_borderAngle(border_angle),
				m_constant_2(constant2)
		{
			m_func_evaluate = boost::bind( &AORDefault::evalCombinedConstConst, this, _1 );
		}


		/// Gets the probability that the snow will stay
		/// where it is (e. g. will not be put onto the
		/// lower neighbors)
		template<class T>
		T AORDefault<T>::GetProbability(T angle)
		{
			T p = m_func_evaluate(angle);

			if (p > 1.0) p = 1.0;
			else if (p < 0.0) p = 0.0;

			return p;
		}

		/// Calculates a potentially stable angle
		/// lower then the given "startAngle".
		/// Measured in radiants.
		template<class T>
		T AORDefault<T>::GetStableAngle(T startAngle)
		{
			// TODO: retun something other then the temporary 70� (in radiants)
			return min(m_aor, startAngle);
		}

		/// Constant evaluation function.
		template<class T>
		T AORDefault<T>::evalConstant(T angle)
		{
			return m_constant;
		}

		/// Linear evaluation function.
		template<class T>
		T AORDefault<T>::evalLinear(T angle)
		{
			return m_m * angle + m_n;
		}

		/// Combination Constant/Linear evaluation function.
		template<class T>
		T AORDefault<T>::evalCombinedConstLin(T angle)
		{
			if (angle < m_borderAngle) {
				return evalConstant(angle);
			} else {
				return evalLinear(angle);
			}
		}

		/// Combination Constant/Constant evaluation function.
		template<class T>
		T AORDefault<T>::evalCombinedConstConst(T angle)
		{
			if (angle < m_borderAngle) {
				return m_constant;
			} else {
				return m_constant_2;
			}
		}

		#pragma endregion
	}
}
