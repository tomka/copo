#pragma once
#include <cgv/math/linalg/fix_col_vec.h>
#include <cgv/math/linalg/fix_row_vec.h>
#include <cgv/math/linalg/fix_mat.h>
#include <cgv/math/linalg/fix_sqr_mat.h>
#include <cgv/math/linalg/fix_sym_mat.h>
#include <cgv/math/linalg/fix_up_tri_mat.h>
#include <cgv/math/linalg/fix_lo_tri_mat.h>
#include <cgv/math/linalg/fix_quaternion.h>


namespace cgv
{
	namespace math
	{
		namespace linalg
		{

template <typename T>
fix_quaternion<T> operator*(const fix_quaternion<T>& q1,const fix_quaternion<T>& q2)
{
	fix_quaternion<T> r;
	r.w() = q1.w()*q2.w() - q1.x()*q2.x() - q1.y()*q2.y() - q1.z()*q2.z();
	r.x() = q1.w()*q2.x() + q1.x()*q2.w() + q1.y()*q2.z() - q1.z()*q2.y();
	r.y() = q1.w()*q2.y() + q1.y()*q2.w() + q1.z()*q2.x() - q1.x()*q2.z();
	r.z() = q1.w()*q2.z() + q1.z()*q2.w() + q1.x()*q2.y() - q1.y()*q2.x();

	return r;
}

template <typename T, cgv::type::uint32 N>
T operator*(const fix_row_vec<T,N>& v1,const fix_col_vec<T,N>& v2)
{
	T p = 0;

	for(int i = 0; i < N; i++)
		p += v1(i) * v2(i);

	return p;
}

template <typename T, cgv::type::uint32 N>
fix_sqr_mat<T,N> operator*(const fix_col_vec<T,N>& v1,const fix_row_vec<T,N>& v2)
{
	fix_sqr_mat<T,N> m;

	for(int i = 0;i < N;i++)
		for(int j = 0; j < N;j++)
		{
			m(i,j)=v1(i)*v2(j);
		}
		

	return m;
}


template <typename T, cgv::type::uint32 N>
fix_row_vec<T,N> operator*(const fix_row_vec<T,N>& m1, const fix_diag_mat<T,N>& m2)
{
		
	fix_row_vec<T,N> r;

	for(int c = 0;c < r.size();c++)
	{
		for(int i = 0;i < m2.size();i++)
		{
			r(c) += m2(i)*m1(i,c); 
		}
	}

	return r;
}



template <typename T, cgv::type::uint32 N, cgv::type::uint32 M>
fix_row_vec<T,M> operator*(const fix_row_vec<T,N>& v,const fix_mat<T,N,M>& m)
{
		
	fix_row_vec<T,M> r(T(0));

	for(int c = 0;c < M;c++)
	{
		for(int i = 0;i < N;i++)
		{
			r(c) += v(i)*m(i,c); 
		}
	}

	return r;
}

template <typename T, cgv::type::uint32 N, cgv::type::uint32 M>
fix_row_vec<T,M>& operator*=(fix_row_vec<T,M>& t, const fix_mat<T,N,M>& m)
{
	t = t*m;
	return t;
}

template <typename T, cgv::type::uint32 N>
fix_row_vec<T,N> operator*(const fix_row_vec<T,N>& v, const fix_sqr_mat<T,N>& m)
{
	fix_row_vec<T,N> r(T(0));

	for(int c = 0;c < N;c++)
	{
		for(int i = 0;i < N;i++)
		{
			r(c) += v(i)*m(i,c); 
		}
	}

	return r;
}

template <typename T, cgv::type::uint32 N>
fix_row_vec<T,N> operator*=(fix_row_vec<T,N>& t, const fix_sqr_mat<T,N>& m)
{
	t = t*m;
	return t;
}

template <typename T, cgv::type::uint32 N, cgv::type::uint32 M>
fix_col_vec<T,N> operator*(const fix_mat<T,N,M>& m, const fix_col_vec<T,M>& v)
{
		
	fix_col_vec<T,N> r(T(0));

	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < M;j++)
			r(i) += m(i,j)*v(j);
		
	}

	return r;
}

template <typename T, cgv::type::uint32 N>
fix_col_vec<T,N> operator*(const fix_sqr_mat<T,N>& m, const fix_col_vec<T,N>& v)
{
		
	fix_col_vec<T,N> r(T(0));

	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < N;j++)
			r(i) += m(i,j)*v(j);
		
	}

	return r;
}

template <typename T, cgv::type::uint32 N, cgv::type::uint32 M, cgv::type::uint32 L>
fix_mat<T,N,M> operator*(const fix_mat<T,N,L>& m1, const fix_mat<T,L,M>& m2)
{
		
	fix_mat<T,N,M> p(T(0));
	
	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < M;j++)
		{
			for(int k = 0; k < L; k++)
				p(i,j) += m1(i,k)*m2(k,j); 
		}
		
	}
	

	return p;
}

template <typename T, cgv::type::uint32 N, cgv::type::uint32 M>
fix_mat<T,N,M> operator*(const fix_mat<T,N,M>& m1, const fix_sqr_mat<T,M>& m2)
{
	fix_mat<T,N,M> p(T(0));
	
	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < M;j++)
		{
			for(int k = 0; k < M; k++)
				p(i,j) += m1(i,k)*m2(k,j); 
		}
		
	}
	

	return p;
}

template <typename T, cgv::type::uint32 N, cgv::type::uint32 M>
fix_mat<T,N,M> operator*(const fix_sqr_mat<T,N>& m1, const fix_mat<T,N,M>& m2)
{

	fix_mat<T,N,M> p(T(0));
	
	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < M;j++)
		{
			for(int k = 0; k < N; k++)
				p(i,j) += m1(i,k)*m2(k,j); 
		}
		
	}

	return p;
}

template <typename T, cgv::type::uint32 N>
fix_sqr_mat<T,N> operator*(const fix_sqr_mat<T,N>& m1, const fix_sqr_mat<T,N>& m2)
{
		
	fix_sqr_mat<T,N> p(T(0));
	
	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < N;j++)
		{
			for(int k = 0; k < N; k++)
				p(i,j) += m1(i,k)*m2(k,j); 
		}
		
	}
	

	return p;
}

template <typename T, cgv::type::uint32 N>
fix_sqr_mat<T,N> operator*=(fix_sqr_mat<T,N>& t, const fix_sqr_mat<T,N>& m)
{
	t = t*m;
	return t;
}

template <typename T, cgv::type::uint32 N>
fix_mat<T,N,N> operator*(const fix_mat<T,N,N>& m1, const fix_diag_mat<T,N>& m2)
{
		
	int nrows=m1.nrows();
	int ncols=m2.ncols();
	

	fix_mat<T,N,N> p(0.0);
	
	for(int i = 0; i < nrows; i++)
	{
		for(int j = 0; j < ncols;j++)
		{
			
				p(i,j) += m1(i,j)*m2(j,j); 
		}
		
	}
	

	return p;
}

template <typename T, cgv::type::uint32 N>
fix_sqr_mat<T,N> operator*(const fix_sqr_mat<T,N>& m1, const fix_diag_mat<T,N>& m2)
{
		
	int nrows=m1.nrows();
	int ncols=m2.ncols();
	

	fix_sqr_mat<T,N> p((T)0);
	
	for(int i = 0; i < nrows; i++)
	{
		for(int j = 0; j < ncols;j++)
		{
			
				p(i,j) += m1(i,j)*m2(j,j); 
		}
		
	}
	

	return p;
}



template <typename T, cgv::type::uint32 N>
fix_mat<T,N,N> operator*( const fix_diag_mat<T,N>& m1,const fix_mat<T,N,N>& m2)
{
		
	int nrows=m1.nrows();
	int ncols=m2.ncols();
	

	fix_mat<T,N,N> p((T)0.0);
	
	for(int i = 0; i < nrows; i++)
	{
		for(int j = 0; j < ncols;j++)
		{
			
				p(i,j) += m2(i,j)*m1(j,j); 
		}
		
	}
	

	return p;
}

template <typename T, cgv::type::uint32 N>
fix_sqr_mat<T,N> operator*( const fix_diag_mat<T,N>& m1,const fix_sqr_mat<T,N>& m2)
{
		
	int nrows=m1.nrows();
	int ncols=m2.ncols();
	

	fix_sqr_mat<T,N> p((T)0);
	
	for(int i = 0; i < nrows; i++)
	{
		for(int j = 0; j < ncols;j++)
		{
			
				p(i,j) += m2(i,j)*m1(j,j); 
		}
		
	}
	

	return p;
}






template <typename T, cgv::type::uint32 N>
fix_col_vec<T,N> operator*(const fix_diag_mat<T,N>& m, const fix_col_vec<T,N>& v)
{
		
	fix_col_vec<T,N> r=(T)0;

	for(int i = 0; i < m.nrows(); i++)
	{
		
			r(i) = m(i,i)*v(i);
		
	}

	return r;
}

template <typename T, cgv::type::uint32 N>
fix_col_vec<T,N> operator*(const fix_sym_mat<T,N>& m, const fix_col_vec<T,N>& v)
{
		
	fix_col_vec<T,N> r(T(0));

	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < N;j++)
			r(i) += m(i,j)*v(j);
		
	}

	return r;
}

template <typename T, cgv::type::uint32 N>
fix_sqr_mat<T,N> operator*(const fix_sym_mat<T,N>& m1, const fix_sym_mat<T,N>& m2)
{
		
	fix_sqr_mat<T,N> p(T(0));

	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < N;j++)
		{
			for(int k = 0; k < N; k++)
				p(i,j) += m1(i,k)*m2(k,j); 
		}
		
	}
	return p;
}

template <typename T, cgv::type::uint32 N>
fix_sqr_mat<T,N> operator*(const fix_sqr_mat<T,N>& m1, const fix_sym_mat<T,N>& m2)
{
		
	fix_sqr_mat<T,N> p(T(0));

	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < N;j++)
		{
			for(int k = 0; k < N; k++)
				p(i,j) += m1(i,k)*m2(k,j); 
		}
		
	}
	return p;
}

template <typename T, cgv::type::uint32 N>
fix_sqr_mat<T,N> operator*(const fix_sym_mat<T,N>& m1, const fix_sqr_mat<T,N>& m2)
{
		
	fix_sqr_mat<T,N> p(T(0));

	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < N;j++)
		{
			for(int k = 0; k < N; k++)
				p(i,j) += m1(i,k)*m2(k,j); 
		}
		
	}
	return p;
}

template <typename T, cgv::type::uint32 N>
fix_mat<T,N,N> operator*(const fix_sym_mat<T,N>& m1, const fix_mat<T,N,N>& m2)
{
		
	fix_mat<T,N,N> p(T(0));

	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < N;j++)
		{
			for(int k = 0; k < N; k++)
				p(i,j) += m1(i,k)*m2(k,j); 
		}
		
	}
	return p;
}

template <typename T, cgv::type::uint32 N>
fix_mat<T,N,N> operator*(const fix_mat<T,N,N>& m1, const fix_sym_mat<T,N>& m2)
{
		
	fix_mat<T,N,N> p(T(0));

	for(int i = 0; i < N; i++)
	{
		for(int j = 0; j < N;j++)
		{
			for(int k = 0; k < N; k++)
				p(i,j) += m1(i,k)*m2(k,j); 
		}
		
	}
	return p;
}



template <typename T, cgv::type::uint32 N>
fix_col_vec<T,N> operator*(const fix_up_tri_mat<T,N>& m, const fix_col_vec<T,N>& v)
{
		
	fix_col_vec<T,N> r(T(0));

	for(int j = 0; j < N; j++)
	{
		for(int i = j; i < N;i++)
			r(i) += m(i,j)*v(j);
		
	}

	return r;
}



template <typename T, cgv::type::uint32 N>
fix_col_vec<T,N> operator*(const fix_lo_tri_mat<T,N>& m, const fix_col_vec<T,N>& v)
{
		
	fix_col_vec<T,N> r(T(0));

	for(int j = 0; j < N; j++)
	{
		for(int i = 0; i < j+1;i++)
			r(i) += m(i,j)*v(j);
		
	}

	return r;
}



		}
	}
}






