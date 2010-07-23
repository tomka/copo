#pragma once
#include <cgv/math/linalg/fix_sqr_mat.h>
#include <cgv/math/linalg/fix_col_vec.h>


namespace cgv
{
	namespace math
	{
		namespace linalg
		{
/** A column vector with zero based index.*/
template <typename T>
class fix_quaternion
{
protected:
	//elements of vector
	T v[4];

public:
	///creates a quaternion not initialized
	fix_quaternion();

	fix_quaternion(fix_col_vec<T,3> axis,const T angle)
	{
		normalize(axis);
		T sin_a = sin( angle / (T)2.0 );
		T cos_a = cos( angle / (T)2.0 );
		v[0]    = axis.x() * sin_a;
		v[1]    = axis.y() * sin_a;
		v[2]    = axis.z() * sin_a;
		v[3]    = cos_a;

	}

	fix_quaternion(T x, T y, T z, T w)
	{
		v[0]    = x;
		v[1]    = y;
		v[2]    = z;
		v[3]    = w;

	}
	
	///creates a quaternion initialized to array a
	fix_quaternion(const T *a);
	///copy constructor
	fix_quaternion(const fix_quaternion &rhs);	
	///assign quaternion rhs
	fix_quaternion & operator=(const fix_quaternion &rhs);	
	///assign a to to every element
	fix_quaternion & operator=(const T &a);	
	///cast operator to fix_mat<T,3,3>
	operator fix_sqr_mat<T,3>() const;
	///cast operator to fix_mat<T,4,4>
	operator fix_sqr_mat<T,4>() const;
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
	inline const T & operator()(const int i) const;
	

	
	///set the first four components
	void set(const T x, const T y, const T z, const T w);
	void set(fix_col_vec<T,3> axis, const T angle); 

	
	
};

template <typename T>
fix_quaternion<T> normalize(const fix_quaternion<T>& q)
{
	T l = q.x()*q.x() +q.y()*q.y() + q.z()*q.z() +q.w()*q.w();
	fix_quaternion<T> qr;
	qr.set(q.x()/l, q.y()/l, q.z()/l, q.w()/l);
	return qr;
}
		}
	}
}


template <typename T>
cgv::math::linalg::fix_quaternion<T>::fix_quaternion() {}


template <typename T>
cgv::math::linalg::fix_quaternion<T>::fix_quaternion(const T *a)
{
	for(int i=0; i<4; i++)
		v[i] = *a++;
}

template <typename T>
cgv::math::linalg::fix_quaternion<T>::fix_quaternion(const cgv::math::linalg::fix_quaternion<T> &rhs)
{
	for(int i=0; i<4; i++)
		v[i] = rhs.v[i];
}

template <typename T>
cgv::math::linalg::fix_quaternion<T>::operator cgv::math::linalg::fix_sqr_mat<T,3>() const
{
	cgv::math::linalg::fix_sqr_mat<T,3> mat;
	double xx      = v[0] * v[0];
    double xy      = v[0] * v[1];
    double xz      = v[0] * v[2];
    double xw      = v[0] * v[3];
    double yy      = v[1] * v[1];
    double yz      = v[1] * v[2];
    double yw      = v[1] * v[3];
    double zz      = v[2] * v[2];
    double zw      = v[2] * v[3];
    mat(0,0)  = 1 - 2 * ( yy + zz );
    mat(0,1)  =     2 * ( xy - zw );
    mat(0,2)  =     2 * ( xz + yw );
    mat(1,0)  =     2 * ( xy + zw );
    mat(1,1)  = 1 - 2 * ( xx + zz );
    mat(1,2)  =     2 * ( yz - xw );
    mat(2,0)  =     2 * ( xz - yw );
    mat(2,1)  =     2 * ( yz + xw );
    mat(2,2) = 1 - 2 * ( xx + yy );
	return mat;
   	
}


template <typename T>
cgv::math::linalg::fix_quaternion<T>::operator cgv::math::linalg::fix_sqr_mat<T,4>() const
{
	cgv::math::linalg::fix_sqr_mat<T,4> mat;
	double xx      = v[0] * v[0];
    double xy      = v[0] * v[1];
    double xz      = v[0] * v[2];
    double xw      = v[0] * v[3];
    double yy      = v[1] * v[1];
    double yz      = v[1] * v[2];
    double yw      = v[1] * v[3];
    double zz      = v[2] * v[2];
    double zw      = v[2] * v[3];
    mat(0,0)  = 1 - 2 * ( yy + zz );
    mat(0,1)  =     2 * ( xy - zw );
    mat(0,2)  =     2 * ( xz + yw );
    mat(1,0)  =     2 * ( xy + zw );
    mat(1,1)  = 1 - 2 * ( xx + zz );
    mat(1,2)  =     2 * ( yz - xw );
    mat(2,0)  =     2 * ( xz - yw );
    mat(2,1)  =     2 * ( yz + xw );
    mat(2,2) = 1 - 2 * ( xx + yy );
	mat(3,0) = mat(3,1) =mat(3,2)=mat(0,3)=mat(1,3)=mat(2,3) = 0;
	mat(3,3) = 1;
	return mat;
   

	
}

template <typename T>
cgv::math::linalg::fix_quaternion<T> & cgv::math::linalg::fix_quaternion<T>::operator=(const cgv::math::linalg::fix_quaternion<T> &rhs) 
{
	if (this != &rhs)
	{
		for (int i=0; i<4; i++)
			v[i]=rhs(i);
	}
	return *this;
}





template <typename T>
inline T & cgv::math::linalg::fix_quaternion<T>::x()
{
	return v[0];
}

template <typename T>
inline const T & cgv::math::linalg::fix_quaternion<T>::x() const	//subscripting
{
	return v[0];
}
template <typename T>
inline T & cgv::math::linalg::fix_quaternion<T>::y()
{
	return v[1];
}

template <typename T>
inline const T & cgv::math::linalg::fix_quaternion<T>::y() const
{
	return v[1];
}
template <typename T>
inline T & cgv::math::linalg::fix_quaternion<T>::z()	
{
	return v[2];
}

template <typename T>
inline const T & cgv::math::linalg::fix_quaternion<T>::z() const
{
	return v[2];
}
template <typename T>
inline T & cgv::math::linalg::fix_quaternion<T>::w()	
{
	return v[3];
}

template <typename T>
inline const T & cgv::math::linalg::fix_quaternion<T>::w() const
{
	return v[3];
}

template <typename T>
inline T & cgv::math::linalg::fix_quaternion<T>::operator()(const int i)	//subscripting
{
	return v[i];
}

template <typename T>
inline const T&  cgv::math::linalg::fix_quaternion<T>::operator()(const int i) const	//subscripting
{
	return v[i];
}



///set the first four components
template <typename T>
void cgv::math::linalg::fix_quaternion<T>::set(const T x, const T y, const T z, const T w)
{
	v[0]=x;
	v[1]=y;
	v[2]=z;
	v[3]=w;
}




template <typename T>
void cgv::math::linalg::fix_quaternion<T>::set(fix_col_vec<T,3> axis, const T angle) 
{
	normalize(axis);
	T sin_a = (T)sin( angle / (T)2.0 );
	T cos_a = (T)cos( angle / (T)2.0 );
	v[0]    = axis.x() * sin_a;
	v[1]    = axis.y() * sin_a;
	v[2]    = axis.z() * sin_a;
	v[3]    = cos_a;

}

