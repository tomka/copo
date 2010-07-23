#pragma once
#include <cgv/math/linalg/fix_col_vec.h>
#include <cgv/math/linalg/fix_row_vec.h>
#include <cgv/math/linalg/fix_normalize.h>

namespace cgv {
	namespace scene {
		namespace geometry {



			template<typename T, int N>
			class direction: public cgv::math::linalg::fix_col_vec<T,N>
			{

			public:

				direction() :cgv::math::linalg::fix_col_vec<T,N>(){}
				
				direction(const direction<T,N> &d) :cgv::math::linalg::fix_col_vec<T,N>((cgv::math::linalg::fix_col_vec<T,N>)d)
				{
					
				}

				direction(const cgv::math::linalg::fix_col_vec<T,N> &v) :cgv::math::linalg::fix_col_vec<T,N>(v)
				{
				}
				

				direction(const T x, const T y) :cgv::math::linalg::fix_col_vec<T,N>()
				{
					
					this->x()=x;
					this->y()=y;
							
				}

				direction(const T x, const T y, const T z) :cgv::math::linalg::fix_col_vec<T,N>()
				{
					this->x()=x;
					this->y()=y;
					this->z()=z;
				}

				direction(const T x, const T y, const T z, const T w) :cgv::math::linalg::fix_col_vec<T,N>()
				{
					this->x()=x;
					this->y()=y;
					this->z()=z;
					this->w()=w;
					
				}
				direction<T,N>& operator=(const direction<T,N> &n)
				{
					for(int i=0; i < N; i++) 
						this->v[i]=n(i);
					return *this;
				}

				direction<T,N>& operator=(const cgv::math::linalg::fix_row_vec<T,N> &v)
				{
					for(int i=0; i < N; i++) 
						this->v[i]=v(i);
					return *this;
				}

				static const int dim = N;

				operator const cgv::math::linalg::fix_row_vec<T,N>() const 
				{
					cgv::math::linalg::fix_row_vec<T,N> vv(this->v);
					return vv;
				}
			};

		}
	}
}
