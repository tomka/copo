#pragma once
#include <cgv/math/linalg/mat.h>

namespace cgv
{
	namespace math
	{
		namespace linalg
		{

template <class T>
class sqr_mat {
private:
	int nn;
	
	T **v;
	
public:
	sqr_mat();
	sqr_mat(int n);			// Zero-based array
	sqr_mat(const T &a, int n);	//Initialize to constant
	sqr_mat(const T *a, int n);	// Initialize to array
	sqr_mat(const sqr_mat &rhs);		// Copy constructor
	operator mat<T> ();
	sqr_mat & operator=(const sqr_mat &rhs);	//assignment
	sqr_mat & operator=(const T &a);		//assign a to every element
	inline T& operator()(const int i,const int j);	//subscripting: pointer to row i
	inline const T& operator()(const int i, const int j) const;
	inline int nrows() const;
	inline int ncols() const;
	~sqr_mat();
};



template <class T>
sqr_mat<T>::sqr_mat() : nn(0), v(0) {}

template <class T>
sqr_mat<T>::sqr_mat(int n) : nn(n), v(new T*[n])
{
	v[0] = new T[n*n];
	for (int i=1; i< n; i++)
		v[i] = v[i-1] + n;
}

template <class T>
sqr_mat<T>::sqr_mat(const T &a, int n) : nn(n), v(new T*[n])
{
	int i,j;
	v[0] = new T[n*n];
	for (i=1; i< n; i++)
		v[i] = v[i-1] + n;
	for (i=0; i< n; i++)
		for (j=0; j<n; j++)
			v[i][j] = a;
}

template <class T>
sqr_mat<T>::sqr_mat(const T *a, int n) : nn(n), v(new T*[n])
{
	int i,j;
	v[0] = new T[n*n];
	for (i=1; i< n; i++)
		v[i] = v[i-1] + n;
	for (i=0; i< n; i++)
		for (j=0; j<n; j++)
			v[i][j] = *a++;
}

template <class T>
sqr_mat<T>::sqr_mat(const sqr_mat &rhs) : nn(rhs.nn), v(new T*[nn])
{
	int i,j;
	v[0] = new T[nn*nn];
	for (i=1; i< nn; i++)
		v[i] = v[i-1] + nn;
	for (i=0; i< nn; i++)
		for (j=0; j<nn; j++)
			v[i][j] = rhs(i,j);
}

template <class T>
sqr_mat<T>::operator mat<T>()
{
	 mat<T> m(nn,nn);
	 for(int i = 0; i < nn; i++)
		 for(int j = 0; j < nn; j++)
		m(i,j)=v[i][j];
	 return m;
}

template <class T>
sqr_mat<T> & sqr_mat<T>::operator=(const sqr_mat<T> &rhs)
// postcondition: normal assignment via copying has been performed;
//		if matrix and rhs were different sizes, matrix
//		has been resized to match the size of rhs
{
	if (this != &rhs) {
		int i,j;
		if (nn != rhs.nn) {
			if (v != 0) {
				delete[] (v[0]);
				delete[] (v);
			}
			nn=rhs.nn;
			
			v = new T*[nn];
			v[0] = new T[nn*nn];
		}
		for (i=1; i< nn; i++)
			v[i] = v[i-1] + nn;
		for (i=0; i< nn; i++)
			for (j=0; j<nn; j++)
				v[i][j] = rhs(i,j);
	}
	return *this;
}

template <class T>
sqr_mat<T> & sqr_mat<T>::operator=(const T &a)	//assign a to every element
{
	for (int i=0; i< nn; i++)
		for (int j=0; j<nn; j++)
			v[i][j] = a;
	return *this;
}

template <class T>
inline T& sqr_mat<T>::operator()(const int i, const int j)	//subscripting: pointer to row i
{
	return v[i][j];
}

template <class T>
inline const T& sqr_mat<T>::operator()(const int i, const int j) const
{
	return v[i][j];
}

template <class T>
inline int sqr_mat<T>::nrows() const
{
	return nn;
}

template <class T>
inline int sqr_mat<T>::ncols() const
{
	return nn;
}

template <class T>
sqr_mat<T>::~sqr_mat()
{
	if (v != 0) {
		delete[] (v[0]);
		delete[] (v);
	}
}

		}
	}
}

