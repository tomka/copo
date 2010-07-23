#pragma once
#include <cgv/math/linalg/mat.h>
#include <cgv/math/linalg/sqr_mat.h>

namespace cgv
{
	namespace math
	{
		namespace linalg
		{

/** A diagonal matrix with zero based indices.*/
template <class T>
class diag_mat {
private:
	//row/column size
	int nn;
	//diagonal element storage
	T *v;
	
public:
	///creates diagonal-matrix with zero size
	diag_mat();
	///creates a diagonal matrix with size nxn
	diag_mat(int n);	
	///creates a nxn diagonal matrix with all diagonal elements initialized to constant value a
	diag_mat(const T &a, int n);	
	///creates a nxn diagonal matrix with diagonal elements initialized to array a
	diag_mat(const T *a, int n);	
	///copy constructor
	diag_mat(const diag_mat &rhs);	
	///cast operator to mat<T>
	operator mat<T>();
	///cast operator to sqr_mat<T>
	operator sqr_mat<T>();
	///assign  diagonal matrix rhs, current matrix will be resized if sizes doesn't match
	diag_mat & operator=(const diag_mat &rhs);	
	///assign constant value a to all diagonal elements of the matrix 
	diag_mat & operator=(const T &a);	
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
	~diag_mat();
};



template <class T>
diag_mat<T>::diag_mat() : nn(0), v(0) {}

template <class T>
diag_mat<T>::diag_mat(int n) : nn(n), v(new T[n])
{
	
}

template <class T>
diag_mat<T>::diag_mat(const T &a, int n) : nn(n), v(new T[n])
{
	int i;
	
	for (i=0; i< n; i++)
		v[i] = a;
}

template <class T>
diag_mat<T>::diag_mat(const T *a, int n) : nn(n), v(new T[n])
{
	int i;
	
	for (i=0; i< n; i++)
		v[i] = *a++;
}

template <class T>
diag_mat<T>::diag_mat(const diag_mat &rhs) : nn(rhs.nn), v(new T[nn])
{
	int i;
	
	for (i=0; i< nn; i++)
		v[i] = rhs(i);
}

template <class T>
diag_mat<T>::operator mat<T>()
{
	 mat<T> m(0.0,nn,nn);
	 for(int i = 0; i < nn; i++)
		m(i,i)=v[i];
	 return m;
}

template <class T>
diag_mat<T>::operator sqr_mat<T>()
{
	 sqr_mat<T> m(0.0,nn);
	 for(int i = 0; i < nn; i++)
		m(i,i)=v[i];
	 return m;
}


template <class T>
diag_mat<T> & diag_mat<T>::operator=(const diag_mat<T> &rhs)
// postcondition: normal assignment via copying has been performed;
//		if matrix and rhs were different sizes, matrix
//		has been resized to match the size of rhs
{
	if (this != &rhs) {
		int i;
		if (nn != rhs.nn ) {
			if (v != 0) {
				delete[] (v);
			}
			nn=rhs.nn;
			
			v = new T[nn];
			
		}
		
		for (i=0; i< nn; i++)
				v[i] = rhs(i);
	}
	return *this;
}



template <class T>
diag_mat<T> & diag_mat<T>::operator=(const T &a)	//assign a to every element
{
	for (int i=0; i< nn; i++)	
			v[i] = a;
	return *this;
}

template <class T>
inline T& diag_mat<T>::operator()(const int i)	//subscripting: pointer to row i
{
	return v[i];
}

template <class T>
inline const T& diag_mat<T>::operator()(const int i) const
{
	return v[i];
}

template <class T>
inline const T diag_mat<T>::operator()(const int i, const int j)	//subscripting: pointer to row i
{
	if(i!=j)
		return 0;
	else
		return v[i];
}

template <class T>
inline const T diag_mat<T>::operator()(const int i, const int j) const
{
	if(i!=j)
		return 0;
	else
		return v[i];
}

template <class T>
inline int diag_mat<T>::nrows() const
{
	return nn;
}

template <class T>
inline int diag_mat<T>::ncols() const
{
	return nn;
}



template <class T>
diag_mat<T>::~diag_mat()
{
	if (v != 0) {
		delete[] (v);
	}
}


		}
	}
}
