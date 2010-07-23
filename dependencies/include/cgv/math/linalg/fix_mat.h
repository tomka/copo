#pragma once

#include <cgv/type/size_type.h>

namespace cgv
{
	namespace math
	{
		namespace linalg
		{


/** A full nxm matrix with zero based indices of fixed dimension.*/
template <typename T, cgv::type::uint32 N, cgv::type::uint32 M>
class fix_mat {
private:
	//matrix data storage
	T v[N][M];
	
public:
	/// creates a not initialized matrix
	fix_mat();
	///creates a nxm matrix initialized to constant value a
	fix_mat(const T &a);	
	///creates a nxm matrix initialized to array a
	fix_mat(const T *a);
	///copy constructor
	fix_mat(const fix_mat<T,N,M> &rhs);		
	///assign matrix rhs, if matrix and rhs have different sizes, matrix has been resized to match the size of
	fix_mat<T,N,M>& operator=(const fix_mat<T,N,M> &rhs);	
	///assign value a to every element
	fix_mat<T,N,M>& operator=(const T &a);
	///access element (i,j) of matrix, i is the zero-based row index, j is the zero-based column index
	inline T& operator()(const int i,const int j);	
	///access element (i,j) of matrix, i is the zero-based row index, j is the zero-based column index
	inline const T& operator()(const int i, const int j) const;
	///return number of rows
	inline int nrows() const;
	///returns number of columns
	inline int ncols() const;
};
		}
	}
}


template <typename T, cgv::type::uint32 N, cgv::type::uint32 M>
cgv::math::linalg::fix_mat<T,N,M>::fix_mat() {}

template <typename T, cgv::type::uint32 N, cgv::type::uint32 M>
cgv::math::linalg::fix_mat<T,N,M>::fix_mat(const T &a)
{
	int i,j;
	for (i=0; i< N; i++)
		for (j=0; j<M; j++)
			v[i][j] = a;
}

template <typename T, cgv::type::uint32 N, cgv::type::uint32 M>
cgv::math::linalg::fix_mat<T,N,M>::fix_mat(const T *a)
{
	int i,j;
	for (i=0; i< N; i++)
		for (j=0; j<M; j++)
			v[i][j] = *a++;
}

template <typename T, cgv::type::uint32 N, cgv::type::uint32 M>
cgv::math::linalg::fix_mat<T,N,M>::fix_mat(const cgv::math::linalg::fix_mat<T,N,M> &rhs)
{
	int i,j;
	for (i=0; i< N; i++)
		for (j=0; j<M; j++)
			v[i][j] = rhs(i,j);
}



template <typename T, cgv::type::uint32 N, cgv::type::uint32 M>
cgv::math::linalg::fix_mat<T,N,M> & cgv::math::linalg::fix_mat<T,N,M>::operator=(const cgv::math::linalg::fix_mat<T,N,M> &rhs)
// postcondition: normal assignment via copying has been performed;
//		if matrix and rhs were different sizes, matrix
//		has been resized to match the size of rhs
{
	if (this != &rhs) {
		int i,j;
		for (i=0; i< N; i++)
			for (j=0; j<M; j++)
				v[i][j] = rhs(i,j);
	}
	return *this;
}

template <typename T, cgv::type::uint32 N, cgv::type::uint32 M>
cgv::math::linalg::fix_mat<T,N,M> & cgv::math::linalg::fix_mat<T,N,M>::operator=(const T &a)	//assign a to every element
{
	for (int i=0; i< N; i++)
		for (int j=0; j<M; j++)
			v[i][j] = a;
	return *this;
}

template <typename T, cgv::type::uint32 N, cgv::type::uint32 M>
inline T& cgv::math::linalg::fix_mat<T,N,M>::operator()(const int i, const int j)	//subscripting: pointer to row i
{
	return v[i][j];
}

template <typename T, cgv::type::uint32 N, cgv::type::uint32 M>
inline const T& cgv::math::linalg::fix_mat<T,N,M>::operator()(const int i, const int j) const
{
	return v[i][j];
}

template <typename T, cgv::type::uint32 N, cgv::type::uint32 M>
inline int cgv::math::linalg::fix_mat<T,N,M>::nrows() const
{
	return N;
}

template <typename T, cgv::type::uint32 N, cgv::type::uint32 M>
inline int cgv::math::linalg::fix_mat<T,N,M>::ncols() const
{
	return M;
}
