#pragma once

namespace cgv
{
	namespace math
	{
		namespace linalg
		{


/** A full nxm matrix with zero based indices.*/
template <class T>
class mat {
private:
	//number of rows
	int nn;
	//number of columns
	int mm;
	//matrix data storage
	T **v;
	
public:
	///creates a zero sized matrix
	mat();
	///creates a matrix with n rows and m
	mat(int n, int m);			
	///creates a nxm matrix initialized to constant value a
	mat(const T &a, int n, int m);	
	///creates a nxm matrix initialized to array a
	mat(const T *a, int n, int m);	
	///copy constructor
	mat(const mat &rhs);		
	///assign matrix rhs, if matrix and rhs have different sizes, matrix has been resized to match the size of
	mat & operator=(const mat &rhs);	
	///assign value a to every element
	mat & operator=(const T &a);	
	///access element (i,j) of matrix, i is the zero-based row index, j is the zero-based column index
	inline T& operator()(const int i,const int j);	
	///access element (i,j) of matrix, i is the zero-based row index, j is the zero-based column index
	inline const T& operator()(const int i, const int j) const;
	///return number of rows
	inline int nrows() const;
	///returns number of columns
	inline int ncols() const;
	///destructor
	~mat();
};
		}
	}
}



template <class T>
cgv::math::linalg::mat<T>::mat() : nn(0), mm(0), v(0) {}

template <class T>
cgv::math::linalg::mat<T>::mat(int n, int m) : nn(n), mm(m), v(new T*[n])
{
	v[0] = new T[m*n];
	for (int i=1; i< n; i++)
		v[i] = v[i-1] + m;
}

template <class T>
cgv::math::linalg::mat<T>::mat(const T &a, int n, int m) : nn(n), mm(m), v(new T*[n])
{
	int i,j;
	v[0] = new T[m*n];
	for (i=1; i< n; i++)
		v[i] = v[i-1] + m;
	for (i=0; i< n; i++)
		for (j=0; j<m; j++)
			v[i][j] = a;
}

template <class T>
cgv::math::linalg::mat<T>::mat(const T *a, int n, int m) : nn(n), mm(m), v(new T*[n])
{
	int i,j;
	v[0] = new T[m*n];
	for (i=1; i< n; i++)
		v[i] = v[i-1] + m;
	for (i=0; i< n; i++)
		for (j=0; j<m; j++)
			v[i][j] = *a++;
}

template <class T>
cgv::math::linalg::mat<T>::mat(const cgv::math::linalg::mat<T> &rhs) : nn(rhs.nn), mm(rhs.mm), v(new T*[nn])
{
	int i,j;
	v[0] = new T[mm*nn];
	for (i=1; i< nn; i++)
		v[i] = v[i-1] + mm;
	for (i=0; i< nn; i++)
		for (j=0; j<mm; j++)
			v[i][j] = rhs(i,j);
}



template <class T>
cgv::math::linalg::mat<T> & cgv::math::linalg::mat<T>::operator=(const cgv::math::linalg::mat<T> &rhs)
// postcondition: normal assignment via copying has been performed;
//		if matrix and rhs were different sizes, matrix
//		has been resized to match the size of rhs
{
	if (this != &rhs) {
		int i,j;
		if (nn != rhs.nn || mm != rhs.mm) {
			if (v != 0) {
				delete[] (v[0]);
				delete[] (v);
			}
			nn=rhs.nn;
			mm=rhs.mm;
			v = new T*[nn];
			v[0] = new T[mm*nn];
		}
		for (i=1; i< nn; i++)
			v[i] = v[i-1] + mm;
		for (i=0; i< nn; i++)
			for (j=0; j<mm; j++)
				v[i][j] = rhs(i,j);
	}
	return *this;
}

template <class T>
cgv::math::linalg::mat<T> & cgv::math::linalg::mat<T>::operator=(const T &a)	//assign a to every element
{
	for (int i=0; i< nn; i++)
		for (int j=0; j<mm; j++)
			v[i][j] = a;
	return *this;
}

template <class T>
inline T& cgv::math::linalg::mat<T>::operator()(const int i, const int j)	//subscripting: pointer to row i
{
	return v[i][j];
}

template <class T>
inline const T& cgv::math::linalg::mat<T>::operator()(const int i, const int j) const
{
	return v[i][j];
}

template <class T>
inline int cgv::math::linalg::mat<T>::nrows() const
{
	return nn;
}

template <class T>
inline int cgv::math::linalg::mat<T>::ncols() const
{
	return mm;
}

template <class T>
cgv::math::linalg::mat<T>::~mat()
{
	if (v != 0) {
		delete[] (v[0]);
		delete[] (v);
	}
}
