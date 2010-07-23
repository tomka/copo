#pragma once
#include <cgv/math/linalg/fix_col_vec.h>
#include <cgv/math/linalg/fix_reflect.h>
#include <cgv/type/size_type.h>


namespace cgv {
	namespace math {
		namespace linalg {


template <typename T, cgv::type::uint32 N>
fix_col_vec<T,N> refract(const fix_col_vec<T,N> &v,const fix_col_vec<T,N> &n,
		T c1, T c2,bool* total_reflection)
{
	T NdotV =-dot(n,v);
	T c = c2/c1;

	T cosasqr = 1-(c*c)*(1-NdotV*NdotV);
	
	//total reflection
	if(cosasqr < 0)
	{
		*total_reflection=true;
		return reflect(v,n);
	}
	else
	{
		*total_reflection=false;
		return	c*v + (c*NdotV - sqrt(cosasqr) )*n;
		 
	}

}


		}
	}
}
