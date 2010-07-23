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
class lo_tri_mat {
private:
	int num;
	int nn;

	T nul;
	T *v;
	
public:
	lo_tri_mat();
	lo_tri_mat(int n);			// Zero-based array
	lo_tri_mat(const T &a, int n);	//Initialize to constant
	lo_tri_mat(const T *a, int n);	// Initialize to array
	lo_tri_mat(const lo_tri_mat &rhs);		// Copy constructor
	operator mat<T> ();
	operator sqr_mat<T> ();
	lo_tri_mat & operator=(const lo_tri_mat &rhs);	//assignment
	lo_tri_mat & operator=(const T &a);		//assign a to every element
	inline T& operator()(const int i,const int j);	//subscripting: pointer to row i
	inline const T& operator()(const int i, const int j) const;
	inline int nrows() const;
	inline int ncols() const;
	~lo_tri_mat();
};



template <class T>
lo_tri_mat<T>::lo_tri_mat() :num(0), nn(0), v(0),nul(0) {}

template <class T>
lo_tri_mat<T>::lo_tri_mat(int n) :num(n), nn(n*(n+1)/2), v(new T[nn]),nul(0)
{
}

template <class T>
lo_tri_mat<T>::lo_tri_mat(const T &a, int n) :num(n), nn(n*(n+1)/2), v(new T[nn]),nul(0)
{
	int i;
	for (i=0; i< nn; i++)
			v[i] = a;
}

template <class T>
lo_tri_mat<T>::lo_tri_mat(const T *a, int n) : num(n), nn(n*(n+1)/2), v(new T[nn]),nul(0)
{
	int i;
	for (i=0; i<nn; i++)
			v[i] = *a++;
}

template <class T>
lo_tri_mat<T>::lo_tri_mat(const lo_tri_mat &rhs) : num(rhs.num), nn(rhs.nn), v(new T[nn]),nul(0)
{
	int i;
	for (i=0; i < nn; i++)
		v[i] = rhs.v[i];
}

template <class T>
lo_tri_mat<T>::operator mat<T>()
{
	 mat<T> m(num,num);
	 for(int i = 0; i < num; i++)
		 for(int j = 0; j < num; j++)
		m(i,j)=(*this)(i,j);
	 return m;
}
template <class T>
lo_tri_mat<T>::operator sqr_mat<T>()
{
	 sqr_mat<T> m(num);
	 for(int i = 0; i < num; i++)
		 for(int j = 0; j < num; j++)
		m(i,j)=(*this)(i,j);
	 return m;
}



template <class T>
lo_tri_mat<T> & lo_tri_mat<T>::operator=(const lo_tri_mat<T> &rhs)
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
lo_tri_mat<T> & lo_tri_mat<T>::operator=(const T &a)	//assign a to every element
{
	for (int i=0; i< nn; i++)
			v[i] = a;
	return *this;
}

template <class T>
inline T& lo_tri_mat<T>::operator()(const int i, const int j)	//subscripting: pointer to row i
{
	if(i < j)
		return nul;
	else
		return v[j*num - (j-1)*((j-1)+1)/2 + i - j];
}

template <class T>
inline const T& lo_tri_mat<T>::operator()(const int i, const int j) const
{
	if(i < j)
		return nul;
	else
		return v[j*num - (j-1)*((j-1)+1)/2 + i - j];
}

template <class T>
inline int lo_tri_mat<T>::nrows() const
{
	return num;
}

template <class T>
inline int lo_tri_mat<T>::ncols() const
{
	return num;
}

template <class T>
lo_tri_mat<T>::~lo_tri_mat()
{
	if (v != 0) 
		delete[] v;
	
}

		}
	}
}

