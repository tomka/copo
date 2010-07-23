#pragma once
#include <cgv/math/linalg/svd.h>
#include <iostream>
#include <cgv/math/linalg/fix_col_vec.h>
#include <cgv/math/linalg/fix_row_vec.h>
#include <cgv/math/linalg/fix_mat.h>
#include <cgv/math/linalg/fix_diag_mat.h>

#include <cgv/math/linalg/fix_sqr_mat.h>


#include <limits> 




///singular value decomposition a = u * w * v^T
///result: a = u, w, v (not v^T!)
template <typename T, cgv::type::uint32 N, cgv::type::uint32 M>
void svd( cgv::math::linalg::fix_mat<T,N,M> &a, cgv::math::linalg::fix_diag_mat<T,N> &w,  cgv::math::linalg::fix_mat<T,N,M> &v)
{
	bool flag;
	int i,its,j,jj,k,l,nm;
	T anorm,c,f,g,h,s,scale,x,y,z;

	int m=a.nrows();
	int n=a.ncols();

	const T EPS=std::numeric_limits<T>::epsilon();
	cgv::math::linalg::fix_col_vec<T,N> rv1;
	g=scale=anorm=0.0;
	for (i=0;i<n;i++) {
		l=i+2;
		rv1(i)=scale*g;
		g=s=scale=0;
		if (i < m) {
			for (k=i;k<m;k++) scale += fabs(a(k,i));
			if (fabs(scale) > EPS) {
				for (k=i;k<m;k++) {
					a(k,i) /= scale;
					s += a(k,i)*a(k,i);
				}
				f=a(i,i);
				g = -SIGN(sqrt(s),f);
				h=f*g-s;
				a(i,i)=f-g;
				for (j=l-1;j<n;j++) {
					for (s=0,k=i;k<m;k++) s += a(k,i)*a(k,j);
					f=s/h;
					for (k=i;k<m;k++) a(k,j) += f*a(k,i);
				}
				for (k=i;k<m;k++) a(k,i) *= scale;
			}
		}
		w(i)=scale *g;
		g=s=scale=0.0;
		if (i+1 <= m && i+1 != n)  {
			for (k=l-1;k<n;k++) scale += fabs(a(i,k));
			if (fabs(scale) > EPS ) {
				for (k=l-1;k<n;k++) {
					a(i,k) /= scale;
					s += a(i,k)*a(i,k);
				}
				f=a(i,l-1);
				g = -SIGN(sqrt(s),f);
				h=f*g-s;
				a(i,l-1)=f-g;
				for (k=l-1;k<n;k++) rv1(k)=a(i,k)/h;
				for (j=l-1;j<m;j++) {
					for (s=0.0,k=l-1;k<n;k++) s += a(j,k)*a(i,k);
					for (k=l-1;k<n;k++) a(j,k) += s*rv1(k);
				}
				for (k=l-1;k<n;k++) a(i,k) *= scale;
			}
		}
		anorm=MAX(anorm,(fabs(w(i))+fabs(rv1(i))));
	}
	for (i=n-1;i>=0;i--) {
		if (i < n-1) {
			if (fabs(g) > EPS) {
				for (j=l;j<n;j++)
					v(j,i)=(a(i,j)/a(i,l))/g;
				for (j=l;j<n;j++) {
					for (s=0,k=l;k<n;k++) s += a(i,k)*v(k,j);
					for (k=l;k<n;k++) v(k,j) += s*v(k,i);
				}
			}
			for (j=l;j<n;j++) v(i,j)=v(j,i)=0.0;
		}
		v(i,i)=1.0;
		g=rv1(i);
		l=i;
	}
	for (i=MIN(m,n)-1;i>=0;i--) {
		l=i+1;
		g=w(i);
		for (j=l;j<n;j++) a(i,j)=0;
		if (fabs(g) > EPS) {
			g=1/g;
			for (j=l;j<n;j++) {
				for (s=0,k=l;k<m;k++) s += a(k,i)*a(k,j);
				f=(s/a(i,i))*g;
				for (k=i;k<m;k++) a(k,j) += f*a(k,i);
			}
			for (j=i;j<m;j++) a(j,i) *= g;
		} else for (j=i;j<m;j++) a(j,i)=0;
		++a(i,i);
	}
	for (k=n-1;k>=0;k--) {
		for (its=0;its<30;its++) {
			flag=true;
			for (l=k;l>=0;l--) {
				nm=l-1;
				
				if (fabs(rv1(l)) <= EPS*anorm) {
					flag=false;
					break;
				}
				
				if (fabs(w(nm)) <= EPS*anorm) break;
			}
			if (flag) {
				c=0.0;
				s=1.0;
				for (i=l;i<k+1;i++) {

					f=s*rv1(i);
					rv1(i)=c*rv1(i);
					
					if (fabs(f) <= EPS*anorm) break;
					g=w(i);
					h=pythag(f,g);
					w(i)=h;
					h=1/h;
					c=g*h;
					s = -f*h;
					for (j=0;j<m;j++) {
						y=a(j,nm);
						z=a(j,i);
						a(j,nm)=y*c+z*s;
						a(j,i)=z*c-y*s;
					}
				}
			}
			z=w(k);
			if (l == k) {
				if (z < 0.0) {
					w(k) = -z;
					for (j=0;j<n;j++) v(j,k) = -v(j,k);
				}
				break;
			}
			if (its == 29) std::cerr <<"no convergence in 30 svdcmp iterations";
			x=w(l);
			nm=k-1;
			y=w(nm);
			g=rv1(nm);
			h=rv1(k);
			f=((y-z)*(y+z)+(g-h)*(g+h))/(2*h*y);
			T one=1;
			g=pythag(f,one);
			f=((x-z)*(x+z)+h*((y/(f+SIGN(g,f)))-h))/x;
			c=s=1.0;
			for (j=l;j<=nm;j++) {
				i=j+1;
				g=rv1(i);
				y=w(i);
				h=s*g;
				g=c*g;
				z=pythag(f,h);
				rv1(j)=z;
				c=f/z;
				s=h/z;
				f=x*c+g*s;
				g=g*c-x*s;
				h=y*s;
				y *= c;
				for (jj=0;jj<n;jj++) {
					x=v(jj,j);
					z=v(jj,i);
					v(jj,j)=x*c+z*s;
					v(jj,i)=z*c-x*s;
				}
				z=pythag(f,h);
				w(j)=z;
				if (z) {
					z=1/z;
					c=f*z;
					s=h*z;
				}
				f=c*g+s*y;
				x=c*y-s*g;
				for (jj=0;jj<m;jj++) {
					y=a(jj,j);
					z=a(jj,i);
					a(jj,j)=y*c+z*s;
					a(jj,i)=z*c-y*s;
				}
			}
			rv1(l)=0.0;
			rv1(k)=f;
			w(k)=x;
		}
	}
}

