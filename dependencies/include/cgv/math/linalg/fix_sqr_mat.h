#pragma once

#include <cgv/type/size_type.h>
#include <cgv/math/linalg/fix_mat.h>

namespace cgv
{
	namespace math
	{
		namespace linalg
		{

template <typename T, cgv::type::uint32 N>
class fix_sqr_mat {
private:
	T v[N][N];	
public:
	fix_sqr_mat();
	fix_sqr_mat(const T &a);	//Initialize to constant
	fix_sqr_mat(const T *a);	// Initialize to array
	fix_sqr_mat(const fix_sqr_mat &rhs);		// Copy constructor
	operator fix_mat<T,N,N> () const;
	fix_sqr_mat & operator=(const fix_sqr_mat<T,N> &rhs);	//assignment
	fix_sqr_mat & operator=(const T &a);		//assign a to every element
	inline T& operator()(const int i,const int j);	//subscripting: pointer to row i
	inline const T& operator()(const int i, const int j) const;
	inline int nrows() const;
	inline int ncols() const;
};



template <typename T, cgv::type::uint32 N>
fix_sqr_mat<T,N>::fix_sqr_mat() {}

template <typename T, cgv::type::uint32 N>
fix_sqr_mat<T,N>::fix_sqr_mat(const T &a)
{
	int i,j;
	for (i=0; i< N; i++)
		for (j=0; j<N; j++)
			v[i][j] = a;
}

template <typename T, cgv::type::uint32 N>
fix_sqr_mat<T,N>::fix_sqr_mat(const T *a)
{
	int i,j;
	for (i=0; i< N; i++)
		for (j=0; j<N; j++)
			v[i][j] = *a++;
}

template <typename T, cgv::type::uint32 N>
fix_sqr_mat<T,N>::fix_sqr_mat(const fix_sqr_mat<T,N> &rhs)
{
	int i,j;
	for (i=0; i< N; i++)
		for (j=0; j<N; j++)
			v[i][j] = rhs(i,j);
}

template <typename T, cgv::type::uint32 N>
fix_sqr_mat<T,N>::operator fix_mat<T,N,N>() const
{
	 fix_mat<T,N,N> m;
	 for(int i = 0; i < N; i++)
		 for(int j = 0; j < N; j++)
			m(i,j)=v[i][j];
	 return m;
}

template <typename T, cgv::type::uint32 N>
fix_sqr_mat<T,N>& fix_sqr_mat<T,N>::operator=(const fix_sqr_mat<T,N> &rhs)
// postcondition: normal assignment via copying has been performed;
//		if matrix and rhs were different sizes, matrix
//		has been resized to match the size of rhs
{
	if (this != &rhs) {
		for (int i=0; i< N; i++)
			for (int j=0; j<N; j++)
				v[i][j] = rhs(i,j);
	}
	return *this;
}

template <typename T, cgv::type::uint32 N>
fix_sqr_mat<T,N> & fix_sqr_mat<T,N>::operator=(const T &a)	//assign a to every element
{
	for (int i=0; i< N; i++)
		for (int j=0; j<N; j++)
			v[i][j] = a;
	return *this;
}

template <typename T, cgv::type::uint32 N>
inline T& fix_sqr_mat<T,N>::operator()(const int i, const int j)	//subscripting: pointer to row i
{
	return v[i][j];
}

template <typename T, cgv::type::uint32 N>
inline const T& fix_sqr_mat<T,N>::operator()(const int i, const int j) const
{
	return v[i][j];
}

template <typename T, cgv::type::uint32 N>
inline int fix_sqr_mat<T,N>::nrows() const
{
	return N;
}

template <typename T, cgv::type::uint32 N>
inline int fix_sqr_mat<T,N>::ncols() const
{
	return N;
}

		}
	}
}

