#pragma once
#include <cgv/math/linalg/fix_mat.h>
#include <cgv/math/linalg/fix_sqr_mat.h>
#include <cgv/type/size_type.h>

namespace cgv
{
	namespace math
	{
		namespace linalg
		{

template <class T,cgv::type::uint32 N>
class fix_lo_tri_mat {
private:
	cgv::type::uint32 nn;

	T nul;
	T v[N*(N+1)/2];
	
public:
	fix_lo_tri_mat();
	fix_lo_tri_mat(const T &a);	//Initialize to constant
	fix_lo_tri_mat(const T *a);	// Initialize to array
	fix_lo_tri_mat(const fix_lo_tri_mat &rhs);		// Copy constructor
	operator fix_mat<T,N,N> ();
	operator fix_sqr_mat<T,N> ();
	fix_lo_tri_mat & operator=(const fix_lo_tri_mat &rhs);	//assignment
	fix_lo_tri_mat & operator=(const T &a);		//assign a to every element
	inline T& operator()(const int i,const int j);	//subscripting: pointer to row i
	inline const T& operator()(const int i, const int j) const;
	inline int nrows() const;
	inline int ncols() const;
	~fix_lo_tri_mat();
};



template <class T,cgv::type::uint32 N>
fix_lo_tri_mat<T,N>::fix_lo_tri_mat() : nn(N*(N+1)/2), nul(0) {}

template <class T,cgv::type::uint32 N>
fix_lo_tri_mat<T,N>::fix_lo_tri_mat(const T &a) : nul(0)
{
	nn = N*(N+1)/2;

	for (int i=0; i< nn; i++)
			v[i] = a;
}

template <class T,cgv::type::uint32 N>
fix_lo_tri_mat<T,N>::fix_lo_tri_mat(const T *a) : nul(0)
{
	nn = N*(N+1)/2;
	for (int i=0; i<nn; i++)
			v[i] = *a++;
}

template <class T,cgv::type::uint32 N>
fix_lo_tri_mat<T,N>::fix_lo_tri_mat(const fix_lo_tri_mat &rhs) :  nul(0)
{
	nn = rhs.nn;
	for (int i=0; i < nn; i++)
		v[i] = rhs.v[i];
}

template <class T,cgv::type::uint32 N>
fix_lo_tri_mat<T,N>::operator fix_sqr_mat<T,N>()
{
	 fix_sqr_mat<T,N> m;
	 for(int i = 0; i < N; i++)
		 for(int j = 0; j < N; j++)
		m(i,j)=(*this)(i,j);
	 return m;
}



template <class T,cgv::type::uint32 N>
fix_lo_tri_mat<T,N>::operator fix_mat<T,N,N>()
{
	 fix_mat<T,N,N> m;
	 for(int i = 0; i < N; i++)
		 for(int j = 0; j < N; j++)
		m(i,j)=(*this)(i,j);
	 return m;
}

template <class T,cgv::type::uint32 N>
fix_lo_tri_mat<T,N> & fix_lo_tri_mat<T,N>::operator=(const fix_lo_tri_mat<T,N> &rhs)
{
	int i;
	for (i=0; i< nn; i++)
			v[i] = rhs.v[i];
	return *this;
}

template <class T,cgv::type::uint32 N>
fix_lo_tri_mat<T,N> & fix_lo_tri_mat<T,N>::operator=(const T &a)	//assign a to every element
{
	for (int i=0; i< nn; i++)
			v[i] = a;
	return *this;
}

template <class T,cgv::type::uint32 N>
inline T& fix_lo_tri_mat<T,N>::operator()(const int i, const int j)	//subscripting: pointer to row i
{
	if(i < j)
		return nul;
	else
		return v[j*N - (j-1)*((j-1)+1)/2 + i - j];
}

template <class T,cgv::type::uint32 N>
inline const T& fix_lo_tri_mat<T,N>::operator()(const int i, const int j) const
{
	if(i < j)
		return nul;
	else
		return v[j*N - (j-1)*((j-1)+1)/2 + i - j];
}

template <class T,cgv::type::uint32 N>
inline int fix_lo_tri_mat<T,N>::nrows() const
{
	return N;
}

template <class T,cgv::type::uint32 N>
inline int fix_lo_tri_mat<T,N>::ncols() const
{
	return N;
}

template <class T,cgv::type::uint32 N>
fix_lo_tri_mat<T,N>::~fix_lo_tri_mat()
{
	
}

		}
	}
}

