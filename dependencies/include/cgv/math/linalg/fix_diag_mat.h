#pragma once
#include <cgv/math/linalg/fix_mat.h>
#include <cgv/math/linalg/fix_sqr_mat.h>
#include <cgv/math/linalg/fix_sym_mat.h>

namespace cgv
{
	namespace math
	{
		namespace linalg
		{

/** A diagonal matrix with zero based indices.*/
template <typename T, cgv::type::uint32 N>
class fix_diag_mat {
private:
	
	//diagonal element storage
	T v[N];
	
public:
	///creates diagonal-matrix with zero size
	fix_diag_mat();
		
	///creates a nxn diagonal matrix with all diagonal elements initialized to constant value a
	fix_diag_mat(const T &a);	
	///creates a nxn diagonal matrix with diagonal elements initialized to array a
	fix_diag_mat(const T *a);	
	///copy constructor
	fix_diag_mat(const fix_diag_mat<T,N> &rhs);	
	///cast operator to mat
	operator fix_mat<T,N,N>() const;
	///cast operator to sqr_mat
	operator fix_sqr_mat<T,N>() const;
	operator fix_sym_mat<T,N>() const;
	///assign  diagonal matrix rhs, current matrix will be resized if sizes doesn't match
	fix_diag_mat & operator=(const fix_diag_mat<T,N> &rhs);	
	///assign constant value a to all diagonal elements of the matrix 
	fix_diag_mat & operator=(const T &a);	
	///read/write access to the diagonal element (i,i), i zero-based row/column index
	inline T& operator()(const int i);	
	///access to the diagonal element (i,i), i zero-based row/column index
	inline const T& operator()(const int i) const;
	///read only access to element (i,j) of the matrix, i is the zero base row index, j is the zero-based column index
	inline const T operator()(const int i, const int j);
	///read only access to element (i,j) of the matrix, i is the zero base row index, j is the zero-based column index
	inline const T operator()(const int i, const int j) const;
	///return number of rows
	inline int nrows() const;
	///return number of columns 
	inline int ncols() const;
	///destructor
	~fix_diag_mat();
};




template <typename T, cgv::type::uint32 N>
fix_diag_mat<T,N>::fix_diag_mat() 
{	
}

template <typename T, cgv::type::uint32 N>
fix_diag_mat<T,N>::fix_diag_mat(const T &a)
{
	int i;
	
	for (i=0; i< N; i++)
		v[i] = a;
}

template <typename T, cgv::type::uint32 N>
fix_diag_mat<T,N>::fix_diag_mat(const T *a)   
{
	int i;
	
	for (i=0; i< N; i++)
		v[i] = *a++;
}

template <typename T, cgv::type::uint32 N>
fix_diag_mat<T,N>::fix_diag_mat(const fix_diag_mat<T,N> &rhs) 
{
	int i;
	
	for (i=0; i< N; i++)
		v[i] = rhs(i);
}

template <typename T, cgv::type::uint32 N>
fix_diag_mat<T,N>::operator fix_mat<T,N,N>() const
{
	 fix_mat<T,N,N> m(0.0);
	 for(int i = 0; i < N; i++)
		m(i,i)=v[i];
	 return m;
}

template <typename T, cgv::type::uint32 N>
fix_diag_mat<T,N>::operator fix_sqr_mat<T,N>() const
{
	fix_sqr_mat<T,N> m(0.0);
	 for(int i = 0; i < N; i++)
		m(i,i)=v[i];
	 return m;
}
template <typename T, cgv::type::uint32 N>
fix_diag_mat<T,N>::operator fix_sym_mat<T,N>() const
{
	fix_sym_mat<T,N> m(0.0);
	 for(int i = 0; i < N; i++)
		m(i,i)=v[i];
	 return m;
}




template <typename T, cgv::type::uint32 N>
fix_diag_mat<T,N> & fix_diag_mat<T,N>::operator=(const fix_diag_mat<T,N> &rhs)
// postcondition: normal assignment via copying has been performed;
//		if matrix and rhs were different sizes, matrix
//		has been resized to match the size of rhs
{
	
		
	for (int i=0; i< N; i++)
			v[i] = rhs(i);
	
	return *this;
}



template <typename T, cgv::type::uint32 N>
fix_diag_mat<T,N> & fix_diag_mat<T,N>::operator=(const T &a)	//assign a to every element
{
	for (int i=0; i< N; i++)	
			v[i] = a;
	return *this;
}

template <typename T, cgv::type::uint32 N>
inline T& fix_diag_mat<T,N>::operator()(const int i)	//subscripting: pointer to row i
{
	return v[i];
}

template <typename T, cgv::type::uint32 N>
inline const T& fix_diag_mat<T,N>::operator()(const int i) const
{
	return v[i];
}

template <typename T, cgv::type::uint32 N>
inline const T fix_diag_mat<T,N>::operator()(const int i, const int j)	//subscripting: pointer to row i
{
	if(i!=j)
		return 0;
	else
		return v[i];
}

template <typename T, cgv::type::uint32 N>
inline const T fix_diag_mat<T,N>::operator()(const int i, const int j) const
{
	if(i!=j)
		return 0;
	else
		return v[i];
}

template <typename T, cgv::type::uint32 N>
inline int fix_diag_mat<T,N>::nrows() const
{
	return N;
}

template <typename T, cgv::type::uint32 N>
inline int fix_diag_mat<T,N>::ncols() const
{
	return N;
}



template <typename T, cgv::type::uint32 N>
fix_diag_mat<T,N>::~fix_diag_mat()
{
}


		}
	}
}
