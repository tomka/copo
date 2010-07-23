#pragma once
#include <cgv/math/linalg/mat.h>
#include <cgv/math/linalg/sqr_mat.h>

namespace cgv
{
	namespace math
	{
		namespace linalg
		{

template <class T>
class sym_mat {
private:
	int num;
	int nn;
	
	T *v;
	
public:
	sym_mat();
	sym_mat(int n);			// Zero-based array
	sym_mat(const T &a, int n);	//Initialize to constant
	sym_mat(const T *a, int n);	// Initialize to array
	sym_mat(const sym_mat &rhs);		// Copy constructor
	operator mat<T> ();
	operator sqr_mat<T>();
	sym_mat & operator=(const sym_mat &rhs);	//assignment
	sym_mat & operator=(const T &a);		//assign a to every element
	inline T& operator()(const int i,const int j);	//subscripting: pointer to row i
	inline const T& operator()(const int i, const int j) const;
	inline int nrows() const;
	inline int ncols() const;
	~sym_mat();
};



template <class T>
sym_mat<T>::sym_mat() :num(0), nn(0), v(0) {}

template <class T>
sym_mat<T>::sym_mat(int n) :num(n), nn(n*(n+1)/2), v(new T[nn])
{
}

template <class T>
sym_mat<T>::sym_mat(const T &a, int n) :num(n), nn(n*(n+1)/2), v(new T[nn])
{
	int i;
	for (i=0; i< nn; i++)
			v[i] = a;
}

template <class T>
sym_mat<T>::sym_mat(const T *a, int n) : num(n), nn(n*(n+1)/2), v(new T[nn])
{
	int i;
	for (i=0; i<nn; i++)
			v[i] = *a++;
}

template <class T>
sym_mat<T>::sym_mat(const sym_mat &rhs) : num(rhs.num), nn(rhs.nn), v(new T[nn])
{
	int i;
	for (i=0; i < nn; i++)
		v[i] = rhs.v[i];
}

template <class T>
sym_mat<T>::operator mat<T>()
{
	 mat<T> m(num,num);
	 for(int i = 0; i < num; i++)
		 for(int j = 0; j < num; j++)
		m(i,j)=(*this)(i,j);
	 return m;
}
	template <class T>
sym_mat<T>::operator sqr_mat<T>()
{
	 sqr_mat<T> m(num);
	 for(int i = 0; i < num; i++)
		 for(int j = 0; j < num; j++)
		m(i,j)=(*this)(i,j);
	 return m;
}



template <class T>
sym_mat<T> & sym_mat<T>::operator=(const sym_mat<T> &rhs)
// postcondition: normal assignment via copying has been performed;
//		if matrix and rhs were different sizes, matrix
//		has been resized to match the size of rhs
{
	if (this != &rhs) {
		int i,j;
		if (nn != rhs.nn) {
			if (v != 0) {
				delete[] v;
			}
			nn=rhs.nn;
			num = rhs.num;
			
			v = new T[nn];
		}
		for (i=0; i< nn; i++)
				v[i] = rhs.v[i];
	}
	return *this;
}

template <class T>
sym_mat<T> & sym_mat<T>::operator=(const T &a)	//assign a to every element
{
	for (int i=0; i< nn; i++)
			v[i] = a;
	return *this;
}

template <class T>
inline T& sym_mat<T>::operator()(const int i, const int j)	//subscripting: pointer to row i
{
	if(i <= j)
		return v[i*num - (i-1)*((i-1)+1)/2 + j - i];
	else
		return v[j*num - (j-1)*((j-1)+1)/2 + i - j];
}

template <class T>
inline const T& sym_mat<T>::operator()(const int i, const int j) const
{
	if(i <= j)
		return v[i*num - (i-1)*((i-1)+1)/2 + j - i];
	else
		return v[j*num - (j-1)*((j-1)+1)/2 + i - j];
}

template <class T>
inline int sym_mat<T>::nrows() const
{
	return num;
}

template <class T>
inline int sym_mat<T>::ncols() const
{
	return num;
}

template <class T>
sym_mat<T>::~sym_mat()
{
	if (v != 0) 
		delete[] v;
	
}

		}
	}
}

