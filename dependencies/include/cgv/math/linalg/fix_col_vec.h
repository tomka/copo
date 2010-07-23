#pragma once
#include <cgv/math/linalg/fix_mat.h>


namespace cgv
{
	namespace math
	{
		namespace linalg
		{
/** A column vector with zero based index.*/
template <typename T, cgv::type::uint32 N>
class fix_col_vec 
{
protected:
	//elements of vector
	T v[N];

public:
	///creates a column vector not initialized
	fix_col_vec();
	///creates a column vector initialized to constant value a
	fix_col_vec(const T &a);
	
	fix_col_vec(const T &x, const T &y);	
	fix_col_vec(const T &x, const T &y, const T &z);	
	fix_col_vec(const T &x, const T &y, const T &z,const T &w);	
	///creates a column vector initialized to array a
	fix_col_vec(const T *a);
	///copy constructor
	fix_col_vec(const fix_col_vec &rhs);	
	///assign vector rhs, if vector and rhs have different sizes, vector has been resized to match the size of
	fix_col_vec & operator=(const fix_col_vec &rhs);	
	///assign a to to every element
	fix_col_vec & operator=(const T &a);	
	///cast operator to fix_mat<T,N,1>
	operator fix_mat<T,N,1>();
	///first element
	inline T& x();
	///first element of const vector
	inline const T& x() const;
	///second element
	inline T& y();
	///second element of const vector
	inline const T& y() const;
	///third element
	inline T& z();
	///third element of const vector
	inline const T& z() const;
	///fourth element
	inline T& w();
	///fourth element of const vector
	inline const T& w() const;
	///access i'th element
	inline T & operator()(const int i);
	///access i'th element of const vector
	inline const T & operator()(const int i) const;
	///access i'th element
	inline T & operator()(const int i, const int j);
	///access i'th element of const vector
	inline const T & operator()(const int i, const int j) const;
	///return number of elements
	inline int size() const;
	///number of rows
	inline int nrows() const;
	///number of columns
	inline int ncols() const;	
	///cast into array. This allows calls like glVertex<N><T>v(p) instead of glVertex<N><T,N>(p.x(),p.y(),....)
	operator T*();
	
	///cast into const array
	operator const T*() const;
	
	///set the first two components
	void set(const T &x, const T &y);
	
	///set the first three components
	void set(const T &x, const T &y, const T &z);
	
	///set the first four components
	void set(const T &x, const T &y, const T &z, const T &w);
	
};
		}
	}
}




template <typename T, cgv::type::uint32 N>
cgv::math::linalg::fix_col_vec<T,N>::fix_col_vec() {}

template <typename T, cgv::type::uint32 N>
cgv::math::linalg::fix_col_vec<T,N>::fix_col_vec(const T& a) 
{
	for(int i=0; i<N; i++)
		v[i] = a;
}

template <typename T, cgv::type::uint32 N>
cgv::math::linalg::fix_col_vec<T,N>::fix_col_vec(const T &x, const T &y)
{
	v[0] = x;
	v[1] = y;
}

template <typename T, cgv::type::uint32 N>
cgv::math::linalg::fix_col_vec<T,N>::fix_col_vec(const T &x, const T &y, const T &z)
{
	v[0] = x;
	v[1] = y;
	v[2] = z;
}

template <typename T, cgv::type::uint32 N>
cgv::math::linalg::fix_col_vec<T,N>::fix_col_vec(const T &x, const T &y, const T &z,const T &w)
{
	v[0] = x;
	v[1] = y;
	v[2] = z;
	v[3] = w;
}

template <typename T, cgv::type::uint32 N>
cgv::math::linalg::fix_col_vec<T,N>::fix_col_vec(const T *a)
{
	for(int i=0; i<N; i++)
		v[i] = *a++;
}

template <typename T, cgv::type::uint32 N>
cgv::math::linalg::fix_col_vec<T,N>::fix_col_vec(const cgv::math::linalg::fix_col_vec<T,N> &rhs)
{
	for(int i=0; i<N; i++)
		v[i] = rhs(i);
}

template <typename T, cgv::type::uint32 N>
cgv::math::linalg::fix_col_vec<T,N>::operator cgv::math::linalg::fix_mat<T,N,1>()
{
	return cgv::math::linalg::fix_mat<T,N,1>(v);
}

template <typename T, cgv::type::uint32 N>
cgv::math::linalg::fix_col_vec<T,N> & cgv::math::linalg::fix_col_vec<T,N>::operator=(const cgv::math::linalg::fix_col_vec<T,N> &rhs)
{
	if (this != &rhs)
	{
		for (int i=0; i<N; i++)
			v[i]=rhs(i);
	}
	return *this;
}

template <typename T, cgv::type::uint32 N>
cgv::math::linalg::fix_col_vec<T,N> & cgv::math::linalg::fix_col_vec<T,N>::operator=(const T &a)	//assign a to every element
{
	for (int i=0; i<N; i++)
		v[i]=a;
	return *this;
}



template <typename T, cgv::type::uint32 N>
inline T & cgv::math::linalg::fix_col_vec<T,N>::x()
{
	return v[0];
}

template <typename T, cgv::type::uint32 N>
inline const T & cgv::math::linalg::fix_col_vec<T,N>::x() const	//subscripting
{
	return v[0];
}
template <typename T, cgv::type::uint32 N>
inline T & cgv::math::linalg::fix_col_vec<T,N>::y()
{
	return v[1];
}

template <typename T, cgv::type::uint32 N>
inline const T & cgv::math::linalg::fix_col_vec<T,N>::y() const
{
	return v[1];
}
template <typename T, cgv::type::uint32 N>
inline T & cgv::math::linalg::fix_col_vec<T,N>::z()	
{
	return v[2];
}

template <typename T, cgv::type::uint32 N>
inline const T & cgv::math::linalg::fix_col_vec<T,N>::z() const
{
	return v[2];
}
template <typename T, cgv::type::uint32 N>
inline T & cgv::math::linalg::fix_col_vec<T,N>::w()	
{
	return v[3];
}

template <typename T, cgv::type::uint32 N>
inline const T & cgv::math::linalg::fix_col_vec<T,N>::w() const
{
	return v[3];
}

template <typename T, cgv::type::uint32 N>
inline T & cgv::math::linalg::fix_col_vec<T,N>::operator()(const int i)	//subscripting
{
	return v[i];
}

template <typename T, cgv::type::uint32 N>
inline const T & cgv::math::linalg::fix_col_vec<T,N>::operator()(const int i) const	//subscripting
{
	return v[i];
}
template <typename T, cgv::type::uint32 N>
inline T & cgv::math::linalg::fix_col_vec<T,N>::operator()(const int i, const int j)	//subscripting
{
	return v[i];
}

template <typename T, cgv::type::uint32 N>
inline const T & cgv::math::linalg::fix_col_vec<T,N>::operator()(const int i, const int j) const	//subscripting
{
	return v[i];
}

template <typename T, cgv::type::uint32 N>
inline int cgv::math::linalg::fix_col_vec<T,N>::size() const
{
	return N;
}

template <typename T, cgv::type::uint32 N>
inline int cgv::math::linalg::fix_col_vec<T,N>::nrows() const
{
	return N;
}

template <typename T, cgv::type::uint32 N>
inline int cgv::math::linalg::fix_col_vec<T,N>::ncols() const
{
	return 1;
}


template <typename T, cgv::type::uint32 N>
cgv::math::linalg::fix_col_vec<T,N>::operator T*()
{
	return v;
}

template <typename T, cgv::type::uint32 N>
cgv::math::linalg::fix_col_vec<T,N>::operator const T*() const
{
	return v;
}

template <typename T, cgv::type::uint32 N>
void cgv::math::linalg::fix_col_vec<T,N>::set(const T &x, const T &y)
{
	v[0]=x;
	v[1]=y;
}
template <typename T, cgv::type::uint32 N>
void cgv::math::linalg::fix_col_vec<T,N>::set(const T &x, const T &y, const T &z)
{
	v[0]=x;
	v[1]=y;
	v[2]=z;
			
}

///set the first four components
template <typename T, cgv::type::uint32 N>
void cgv::math::linalg::fix_col_vec<T,N>::set(const T &x, const T &y, const T &z, const T &w)
{
	v[0]=x;
	v[1]=y;
	v[2]=z;
	v[3]=w;
}

