#pragma once
#include <cgv/math/linalg/mat.h>


namespace cgv
{
	namespace math
	{
		namespace linalg
		{
/** A column vector with zero based index.*/
template <class T>
class col_vec 
{
protected:
	// size of array. upper index is nn-1
	int nn;	
	//elements of vector
	T *v;

public:
	///creates a column vector with zero size
	col_vec();
	///creates a column vector with n elements
	explicit col_vec(int n);	
	///creates a column vector with n elements initialized to constant value a
	col_vec(const T &a, int n);	
	///creates a column vector with n elements initialized to array a
	col_vec(const T *a, int n);
	///copy constructor
	col_vec(const col_vec &rhs);	
	///assign vector rhs, if vector and rhs have different sizes, vector has been resized to match the size of
	col_vec & operator=(const col_vec &rhs);	
	///assign a to to every element
	col_vec & operator=(const T &a);	
	///cast operator to mat<T>
	operator mat<T>();
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
	///destructor
	~col_vec();
	///cast into array. This allows calls like glVertex<n><T>v(p) instead of glVertex<n><T>(p.x(),p.y(),....)
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

template <class T>
cgv::math::linalg::col_vec<T>::col_vec() : nn(0), v(0) {}

template <class T>
cgv::math::linalg::col_vec<T>::col_vec(int n) : nn(n), v(new T[n]) {}

template <class T>
cgv::math::linalg::col_vec<T>::col_vec(const T& a, int n) : nn(n), v(new T[n])
{
	for(int i=0; i<n; i++)
		v[i] = a;
}

template <class T>
cgv::math::linalg::col_vec<T>::col_vec(const T *a, int n) : nn(n), v(new T[n])
{
	for(int i=0; i<n; i++)
		v[i] = *a++;
}

template <class T>
cgv::math::linalg::col_vec<T>::col_vec(const cgv::math::linalg::col_vec<T> &rhs) : nn(rhs.nn), v(new T[nn])
{
	for(int i=0; i<nn; i++)
		v[i] = rhs(i);
}

template <class T>
cgv::math::linalg::col_vec<T>::operator cgv::math::linalg::mat<T>()
{
	return cgv::math::linalg::mat<T>(v,nn,1);
}

template <class T>
cgv::math::linalg::col_vec<T> & cgv::math::linalg::col_vec<T>::operator=(const cgv::math::linalg::col_vec<T> &rhs)
{
	if (this != &rhs)
	{
		if (nn != rhs.nn) {
			if (v != 0) delete [] (v);
			nn=rhs.nn;
			v= new T[nn];
		}
		for (int i=0; i<nn; i++)
			v[i]=rhs(i);
	}
	return *this;
}

template <class T>
cgv::math::linalg::col_vec<T> & cgv::math::linalg::col_vec<T>::operator=(const T &a)	//assign a to every element
{
	for (int i=0; i<nn; i++)
		v[i]=a;
	return *this;
}



template <class T>
inline T & cgv::math::linalg::col_vec<T>::x()
{
	return v[0];
}

template <class T>
inline const T & cgv::math::linalg::col_vec<T>::x() const	//subscripting
{
	return v[0];
}
template <class T>
inline T & cgv::math::linalg::col_vec<T>::y()
{
	return v[1];
}

template <class T>
inline const T & cgv::math::linalg::col_vec<T>::y() const
{
	return v[1];
}
template <class T>
inline T & cgv::math::linalg::col_vec<T>::z()	
{
	return v[2];
}

template <class T>
inline const T & cgv::math::linalg::col_vec<T>::z() const
{
	return v[2];
}
template <class T>
inline T & cgv::math::linalg::col_vec<T>::w()	
{
	return v[3];
}

template <class T>
inline const T & cgv::math::linalg::col_vec<T>::w() const
{
	return v[3];
}

template <class T>
inline T & cgv::math::linalg::col_vec<T>::operator()(const int i)	//subscripting
{
	return v[i];
}

template <class T>
inline const T & cgv::math::linalg::col_vec<T>::operator()(const int i) const	//subscripting
{
	return v[i];
}
template <class T>
inline T & cgv::math::linalg::col_vec<T>::operator()(const int i, const int j)	//subscripting
{
	return v[i];
}

template <class T>
inline const T & cgv::math::linalg::col_vec<T>::operator()(const int i, const int j) const	//subscripting
{
	return v[i];
}

template <class T>
inline int cgv::math::linalg::col_vec<T>::size() const
{
	return nn;
}

template <class T>
inline int cgv::math::linalg::col_vec<T>::nrows() const
{
	return nn;
}

template <class T>
inline int cgv::math::linalg::col_vec<T>::ncols() const
{
	return 1;
}

template <class T>
cgv::math::linalg::col_vec<T>::~col_vec()
{
	if (v != 0)
		delete[] (v);
}

template <class T>
cgv::math::linalg::col_vec<T>::operator T*()
{
	return v;
}

template <class T>
cgv::math::linalg::col_vec<T>::operator const T*() const
{
	return v;
}

template <class T>
void cgv::math::linalg::col_vec<T>::set(const T &x, const T &y)
{
	v[0]=x;
	v[1]=y;
}
template <class T>
void cgv::math::linalg::col_vec<T>::set(const T &x, const T &y, const T &z)
{
	v[0]=x;
	v[1]=y;
	v[2]=z;
			
}

///set the first four components
template <class T>
void cgv::math::linalg::col_vec<T>::set(const T &x, const T &y, const T &z, const T &w)
{
	v[0]=x;
	v[1]=y;
	v[2]=z;
	v[3]=w;
}

		}
	}
}
