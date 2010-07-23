#pragma once

#include <cgv/type/size_type.h>
#include <cgv/math/linalg/fix_col_vec.h>
#include <cgv/math/linalg/fix_row_vec.h>
#include <cgv/math/linalg/fix_mat.h>
#include <cgv/math/linalg/fix_sqr_mat.h>
#include <cgv/math/linalg/fix_diag_mat.h>

namespace cgv
{
	namespace math
	{
		namespace linalg
		{

/// return a const ref to an identity matrix
template <typename T, cgv::type::uint32 N>
const fix_sqr_mat<T,N>& identity()
{
	static fix_sqr_mat<T,N> m(T(0));
	if (m(0,0) == 0) {
		for (int i=0; i<N; ++i)
			m(i,i) = 1;
	}
	return m;
}
template <typename T>
const fix_diag_mat<T,4> scale_44(T s)
{
	fix_diag_mat<T,4> m;
	
	m(0) = s;
	m(1) = s;
	m(2) = s;
	m(3) = 1;
	
	return m;
}

template <typename T>
const fix_diag_mat<T,4> scale_44(T x, T y, T z)
{
	fix_diag_mat<T,4> m;
	
	m(0) = x;
	m(1) = y;
	m(2) = z;
	m(3) = 1;
	
	return m;
}

template <class T>
const fix_sqr_mat<T,4> translate_44(const T&x, const T &y, const T&z)
{
	fix_sqr_mat<T,4> m;
	m(0,0)=1; m(0,1)= 0; m(0,2)= 0; m(0,3)= x;
	m(1,0)=0; m(1,1)= 1; m(1,2)= 0; m(1,3)= y;
	m(2,0)=0; m(2,1)= 0; m(2,2)= 1; m(2,3)= z;
	m(3,0)=0; m(3,1)= 0; m(3,2)= 0; m(3,3)= 1;
	return m;
}

template <class T>
const fix_sqr_mat<T,4> translate_44(const fix_row_vec<T,3> &v)
{
	fix_sqr_mat<T,4> m;
	m(0,0)=1; m(0,1)= 0; m(0,2)= 0; m(0,3)= v.x();
	m(1,0)=0; m(1,1)= 1; m(1,2)= 0; m(1,3)= v.y();
	m(2,0)=0; m(2,1)= 0; m(2,2)= 1; m(2,3)= v.z();
	m(3,0)=0; m(3,1)= 0; m(3,2)= 0; m(3,3)= 1;
	return m;
}

template <class T>
const fix_sqr_mat<T,4> translate_44(const fix_col_vec<T,3> &v)
{
	fix_sqr_mat<T,4> m;
	m(0,0)=1; m(0,1)= 0; m(0,2)= 0; m(0,3)= v.x();
	m(1,0)=0; m(1,1)= 1; m(1,2)= 0; m(1,3)= v.y();
	m(2,0)=0; m(2,1)= 0; m(2,2)= 1; m(2,3)= v.z();
	m(3,0)=0; m(3,1)= 0; m(3,2)= 0; m(3,3)= 1;
	return m;
}

template<class T>
const fix_diag_mat<T,4> rote_44(const T&sx, const T &sy, const T&sz)
{
	fix_diag_mat<T,4> m;
	m(0)=sx; 
	m(1)= sy; 
	m(2)= sz;
	m(3)= 1;
	return m;
}

template<class T>
const fix_diag_mat<T,4> rote_44(const fix_row_vec<T,4> &v)
{
	fix_diag_mat<T,4> m;
	m(0)=v.x(); 
	m(1)= v.y(); 
	m(2)= v.z();
	m(3)= 1;
	return m;
}

template<class T>
const fix_diag_mat<T,4> rote_44(const fix_col_vec<T,4> &v)
{
	fix_diag_mat<T,4> m;
	m(0)=v.x(); 
	m(1)= v.y(); 
	m(2)= v.z();
	m(3)= 1;
	return m;
}



template<class T>
const fix_diag_mat<T,4> rote_44(const T&s)
{
	fix_diag_mat<T,4> m;
	m(0)= s; 
	m(1)= s; 
	m(2)= s; 
	m(3)= 1;
	return m;
}


///create a householder matrix which defines a reflection at the hyperplane defined by the normal v 
template<class T, cgv::type::uint32 N>
const fix_sqr_mat<T,N> householder(const fix_col_vec<T,N>& v)
{
	fix_col_vec<T,N> vn = normalize(v);
	fix_diag_mat<T,N> i((T)1);
	return i - (T)2*vn*transpose(vn);
}

///create givens matrix G^T*x rotates vector x within the (i,k)-plane with the given angle in rad
template<class T, cgv::type::uint32 N>
const fix_sqr_mat<T,N> givens(int i, int k, T angle)
{
	fix_sqr_mat<T,N> G = fix_diag_mat<T,N>((T)1);
	G(i,i)=cos(angle);
	G(k,k)=cos(angle);
	G(i,k)=sin(angle);
	G(k,i)=-sin(angle);
	return G;

}

///create permutation matrix, a multiplation P(i,j)*M gives a matrix M with rows i and j interchanged, a multiplication M*P(i,j) gives a matrix M with columns i and j interchanged
template<class T, cgv::type::uint32 N>
const fix_sqr_mat<T,N> permutation(int i, int k)
{
	fix_sqr_mat<T,N> P = fix_diag_mat<T,N>((T)1);
	P(i,i)=(T)0;
	P(k,k)=(T)0;
	P(i,k)=1;
	P(k,i)=1;
	return P;
}

template<class T>
const fix_sqr_mat<T,4> rotate_x_44(const T& angle)
{
   fix_sqr_mat<T,4> m;
  
    m(0,0)=1; m(0,1)= 0; m(0,2)= 0; m(0,3)= 0;
	m(1,0)=0; m(1,1)= cos(angle); m(1,2)= -sin(angle); m(1,3)= 0;
	m(2,0)=0; m(2,1)= sin(angle); m(2,2)= cos(angle); m(2,3)= 0;
	m(3,0)=0; m(3,1)= 0; m(3,2)= 0; m(3,3)= 1;
    return m;
}

template<class T>
const fix_sqr_mat<T,4> rotate_y_44(const T& angle)
{
    fix_sqr_mat<T,4> m;
    m(0,0)=cos(angle); m(0,1)= 0; m(0,2)= sin(angle); m(0,3)= 0;
	m(1,0)=0; m(1,1)=1; m(1,2)= 0; m(1,3)= 0;
	m(2,0)=-sin(angle); m(2,1)= 0; m(2,2)= cos(angle); m(2,3)= 0;
	m(3,0)=0; m(3,1)= 0; m(3,2)= 0; m(3,3)= 1;
    return m;
}

template<class T>
const fix_sqr_mat<T,4> rotate_z_44(const T& angle)
{
	fix_sqr_mat<T,4> m;
    m(0,0)=cos(angle); m(0,1)= -sin(angle); m(0,2)= 0; m(0,3)= 0;
	m(1,0)=sin(angle); m(1,1)= cos(angle); m(1,2)= 0; m(1,3)= 0;
	m(2,0)=0; m(2,1)= 0; m(2,2)= 1; m(2,3)= 0;
	m(3,0)=0; m(3,1)= 0; m(3,2)= 0; m(3,3)= 1;
    return m;
}

template<class T>
const fix_sqr_mat<T,3> cross_prod_33(const T&x, const T&y, const T&z)
{
	
	fix_sqr_mat<T,3> m;
	m(0,0)=0; m(0,1)= -z; m(0,2)= y;
	m(1,0)=z; m(1,1)= 0; m(1,2)= -x; 
	m(2,0)=-y; m(2,1)= x; m(2,2)= 0; 
    return m;
}

template<class T>
const fix_sqr_mat<T,3> cross_prod_33(const fix_col_vec<T,3> &v)
{
	
	fix_sqr_mat<T,3> m;
	m(0,0)=0; m(0,1)= -v(2); m(0,2)= v(1);
	m(1,0)=v(2); m(1,1)= 0; m(1,2)= -v(0); 
	m(2,0)=-v(1); m(2,1)= v(0); m(2,2)= 0; 
    return m;

}

template<class T>
const fix_sqr_mat<T,4> rotate_44(const T& dirx,const T &diry,const T & dirz, const T& angle)
{
	fix_col_vec<T,3> dirn;
	dirn(0)=dirx;
	dirn(1)=diry;
	dirn(2)=dirz;

    dirn=normalize(dirn);

	fix_sqr_mat<T,3> ddt = dirn*transpose(dirn);

	fix_sqr_mat<T,3> S=cross_prod_33(dirn);
	

    
    T one = 1;
    fix_sqr_mat<T,3> rot= ddt + (T)cos(angle)*(fix_diag_mat<T,3>(one) - ddt) + (T)sin(angle)*S;
	fix_sqr_mat<T,4> m;
	m(0,0)=rot(0,0); m(0,1)= rot(0,1); m(0,2)= rot(0,2); m(0,3)= 0;
	m(1,0)=rot(1,0); m(1,1)= rot(1,1); m(1,2)= rot(1,2); m(1,3)= 0;
	m(2,0)=rot(2,0); m(2,1)= rot(2,1); m(2,2)= rot(2,2); m(2,3)= 0;
	m(3,0)=0; m(3,1)= 0; m(3,2)= 0; m(3,3)= 1;
	

    return m;
}

template<class T>
const fix_sqr_mat<T,4> rotate_44(const fix_row_vec<T,3> dir, const T& angle)
{
	

    dir=normalize(dir);

	fix_sqr_mat<T,3> ddt = dir*transpose(dir);

	fix_sqr_mat<T,3> S=cross_prod(dir);
	

    
    T one = 1;
    fix_sqr_mat<T,3> rot= ddt + (T)cos(angle)*(fix_diag_mat<T,3>(one) - ddt) + (T)sin(angle)*S;
	fix_sqr_mat<T,4> m;
	m(0,0)=rot(0,0); m(0,1)= rot(0,1); m(0,2)= rot(0,2); m(0,3)= 0;
	m(1,0)=rot(1,0); m(1,1)= rot(1,1); m(1,2)= rot(1,2); m(1,3)= 0;
	m(2,0)=rot(2,0); m(2,1)= rot(2,1); m(2,2)= rot(2,2); m(2,3)= 0;
	m(3,0)=0; m(3,1)= 0; m(3,2)= 0; m(3,3)= 1;
	

    return m;
}

template<class T>
const fix_sqr_mat<T,4> rotate_44(fix_col_vec<T,3> dir, const T& angle)
{
	

    dir=normalize(dir);

	fix_sqr_mat<T,3> ddt = dir*transpose(dir);

	fix_sqr_mat<T,3> S=cross_prod_33(dir);
	

    
    T one = 1;
    fix_sqr_mat<T,3> rot= ddt + (T)cos(angle)*(fix_diag_mat<T,3>(one) - ddt) + (T)sin(angle)*S;
	fix_sqr_mat<T,4> m;
	m(0,0)=rot(0,0); m(0,1)= rot(0,1); m(0,2)= rot(0,2); m(0,3)= 0;
	m(1,0)=rot(1,0); m(1,1)= rot(1,1); m(1,2)= rot(1,2); m(1,3)= 0;
	m(2,0)=rot(2,0); m(2,1)= rot(2,1); m(2,2)= rot(2,2); m(2,3)= 0;
	m(3,0)=0; m(3,1)= 0; m(3,2)= 0; m(3,3)= 1;
	

    return m;
}

template<class T> 
const fix_sqr_mat<T,4> rotate_euler_44(const T& yaw, const T& pitch,const T& roll)
{
    T cosy =(T) cos(yaw);
    T siny =(T) sin(yaw);
    T cosp =(T) cos(pitch);
    T sinp =(T) sin(pitch);
    T cosr =(T) cos(roll);
    T sinr =(T) sin(roll);
    fix_sqr_mat<T,4> m;
    
    m(0,0) = cosr*cosy - sinr*sinp*siny;
    m(0,1) = -sinr*cosp;
    m(0,2) = cosr*siny + sinr*sinp*cosy;
    m(0,3) = 0;

    m(1,0) = sinr*cosy + cosr*sinp*siny;
    m(1,1) = cosr*cosp;
    m(1,2) = sinr*siny - cosr*sinp*cosy;
    m(1,3) = 0;

    m(2,0) = -cosp*siny;
    m(2,1) = sinp;
    m(2,2) = cosp*cosy;
    m(2,3) = 0;

    m(3,0) = m(3,1)= m(3,2) =0;
    m(3,3) = 1;
    return m;
}

template<class T> 
const fix_sqr_mat<T,3> rotate_euler_33(const T& yaw, const T& pitch,const T& roll)
{
    T cosy =(T) cos(yaw);
    T siny =(T) sin(yaw);
    T cosp =(T) cos(pitch);
    T sinp =(T) sin(pitch);
    T cosr =(T) cos(roll);
    T sinr =(T) sin(roll);
    fix_sqr_mat<T,3> m;
    
    m(0,0) = cosr*cosy - sinr*sinp*siny;
    m(0,1) = -sinr*cosp;
    m(0,2) = cosr*siny + sinr*sinp*cosy;
  

    m(1,0) = sinr*cosy + cosr*sinp*siny;
    m(1,1) = cosr*cosp;
    m(1,2) = sinr*siny - cosr*sinp*cosy;
   

    m(2,0) = -cosp*siny;
    m(2,1) = sinp;
    m(2,2) = cosp*cosy;
  

      
    return m;
}

template<class T>
const fix_sqr_mat<T,4> shear_xy_44(const T&shx, const T&shy)
{
	fix_sqr_mat<T,4> m;
	m(0,0)=1; m(0,1)= 0; m(0,2)= shx; m(0,3)= 0;
	m(1,0)=0; m(1,1)= 1; m(1,2)= shy; m(1,3)= 0;
	m(2,0)=0; m(2,1)= 0; m(2,2)= 1; m(2,3)= 0;
	m(3,0)=0; m(3,1)= 0; m(3,2)= 0; m(3,3)= 1;
	return m;
}


template<class T>
const fix_sqr_mat<T,4> shear_xz_44(const T&shx, const T&shz)
{
	fix_sqr_mat<T,4> m;
	m(0,0)=1; m(0,1)= shx; m(0,2)= 0; m(0,3)= 0;
	m(1,0)=0; m(1,1)= 1; m(1,2)= 0; m(1,3)= 0;
	m(2,0)=0; m(2,1)= shz; m(2,2)= 1; m(2,3)= 0;
	m(3,0)=0; m(3,1)= 0; m(3,2)= 0; m(3,3)= 1;
	return m;
}

template<class T>
const fix_sqr_mat<T,4> shear_yz_44(const T&shy, const T&shz)
{
	fix_sqr_mat<T,4> m;
	m(0,0)=1; m(0,1)= 0; m(0,2)= 0; m(0,3)= 0;
	m(1,0)=shy; m(1,1)= 1; m(1,2)= 0; m(1,3)= 0;
	m(2,0)=shz; m(2,1)= 0; m(2,2)= 1; m(2,3)= 0;
	m(3,0)=0; m(3,1)= 0; m(3,2)= 0; m(3,3)= 1;
	return m;
}
template<class T>
const fix_sqr_mat<T,4> perspective_44(const T& fovy, const T&aspect, const T& znear, const T& zfar)
{
	T f = cos(fovy/2.0f)/sin(fovy/2.0f);
	fix_sqr_mat<T,4> m;
	m(0,0)=f/aspect; m(0,1)= 0; m(0,2)= 0; m(0,3)= 0;
	m(1,0)=0; m(1,1)= f; m(1,2)= 0; m(1,3)= 0;
	m(2,0)=0; m(2,1)= 0; m(2,2)= (zfar+znear)/(znear-zfar); m(2,3)= (2*zfar*znear)/(znear-zfar);
	m(3,0)=0; m(3,1)= 0; m(3,2)= -1; m(3,3)= 0;
	return m;
}

template<class T>
const fix_sqr_mat<T,4> look_at_44(const T &eyex, const T &eyey, const T &eyez,
		const T& centerx, const T& centery, const T& centerz, 
		const T& upx, const T& upy, const T& upz)
{
	fix_col_vec<T,3> center;
	center.set(centerx,centery,centerz);
	fix_col_vec<T,3> eye;
	eye.set(eyex,eyey,eyez);
	fix_col_vec<T,3> up;
	up.set(upx,upy,upz);

	fix_col_vec<T,3> f = normalize(center-eye);
	up=normalize(up);
	fix_col_vec<T,3> s = cross(f,up);
	fix_col_vec<T,3> u = cross(s,f);



	fix_sqr_mat<T,4> m;
	m(0,0)=s(0); m(0,1)=s(1) ; m(0,2)= s(2); m(0,3)= 0;
	m(1,0)=u(0); m(1,1)=u(1) ; m(1,2)= u(2); m(1,3)= 0;
	m(2,0)=-f(0); m(2,1)= -f(1); m(2,2)= -f(2); m(2,3)= 0;
	m(3,0)=0; m(3,1)= 0; m(3,2)= 0; m(3,3)= 1;
    m = m*translate_44(-eyex,-eyey,-eyez);
	return m;
}

template<class T>
const fix_sqr_mat<T,4> frustrum_44(const T& left, const T&right,
        const T& bottom, const T& top,
        const T& znear, const T& zfar)
{
    T e = 2*znear/(right - left);
    T f = 2*znear/(top-bottom);
	T A = (right +left)/(right -left);
    T B = (top+bottom)/(top-bottom);
    T C = (zfar + znear)/(zfar-znear);
    T D = (2*zfar*znear)/(zfar-znear);
	fix_sqr_mat<T,4> m;
	m(0,0)=e; m(0,1)= 0; m(0,2)= A; m(0,3)= 0;
	m(1,0)=0; m(1,1)= f; m(1,2)= B; m(1,3)= 0;
	m(2,0)=0; m(2,1)= 0; m(2,2)= C; m(2,3)= D;
	m(3,0)=0; m(3,1)= 0; m(3,2)= -1; m(3,3)= 0;
	return m;
}
    

template<class T>
const fix_sqr_mat<T,4> ortho_44(const T& left, const T&right,
        const T& bottom, const T& top,
        const T& znear, const T& zfar)
{
    T a = 2/(right - left);
    T b = 2/(top-bottom);
	T c = -2/(zfar-znear);
    T tx = (right+left)/(right-left);
    T ty = (top+bottom)/(top-bottom);
    T tz = (zfar+znear)/(zfar-znear);
	fix_sqr_mat<T,4> m;
	m(0,0)=a; m(0,1)= 0; m(0,2)= 0; m(0,3)= tx;
	m(1,0)=0; m(1,1)= b; m(1,2)= 0; m(1,3)= ty;
	m(2,0)=0; m(2,1)= 0; m(2,2)= c; m(2,3)= tz;
	m(3,0)=0; m(3,1)= 0; m(3,2)= 0; m(3,3)= 1;
	return m;
}


template<class T>
const fix_sqr_mat<T,4> ortho2d_44(const T& left, const T&right,
        const T& bottom, const T& top)
{
	return ortho_44<T>(left,right,bottom,top,-1.0,1);
}




		}
	}
}





