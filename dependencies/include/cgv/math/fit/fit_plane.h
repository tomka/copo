#pragma once

#include <cgv/scene/geometry/point.h>
#include <cgv/scene/geometry/plane.h>
#include <cgv/math/linalg/linalg.h>

namespace cgv{
	namespace math{
		namespace fit{


/**
* Fit an n dimension plane into a set of n dimensional points.
* 
* Example 1:
* 
* std::vector<cgv::scene::geometry::point<float,3> >pnts(30);
*	  
* pnts[0].x() = 1;
* pnts[0].y() = 0;
* pnts[0].z() = 0;
* ...
*  
* cgv::scene::geometry::plane<float,3> pl;
* cgv::math::fit::fit_plane_into_points(pnts.begin(),pnts.end(),&pl);
*
* Example 2:
* 
* cgv::scene::geometry::point<float,3> *pnts = new cgv::scene::geometry::point<float,3>[70];
*	  
* pnts[0].x() = 1;
* pnts[0].y() = 0;
* pnts[0].z() = 0;
* ...
*  
* cgv::scene::geometry::plane<float,3> pl;
* cgv::math::fit::fit_plane_into_points(pnts,pnts+70,&pl);	
*
**/
template<typename T, int N, typename _Iter>
void fit_plane_into_points(_Iter start, _Iter end,cgv::scene::geometry::plane<T,N> *p)
{
	
	cgv::scene::geometry::point<T,N> centroid((T)0);
	
	//calculate centroid
	int n=0;
	for(_Iter it = start; it != end;it++)
	{
		centroid = centroid + (cgv::scene::geometry::point<T,N>)(*it);				
		n++;
	}
	centroid = (T)(1.0/n)*centroid;

	//create covariance matrix
	cgv::math::linalg::fix_mat<T, N, N> cov_mat((T)0);
	for(_Iter it = start; it != end;it++)
	{
		for(int r = 0; r < N;r++)
		{
			for(int c = 0; c < N;c++)
			{
				cov_mat(r,c)+=(((cgv::scene::geometry::point<T,N>)(*it))(r) - centroid(r))*
					(((cgv::scene::geometry::point<T,N>)(*it))(c) - centroid(c));
			}
		}
	}
	cov_mat = (T)(1.0/n)*cov_mat;

	//calculate SVD
	cgv::math::linalg::fix_diag_mat<T,N> w;
	cgv::math::linalg::fix_mat<T,N,N> v;
	svd(cov_mat,w,v);

	//search smallest singular value
	int index = 0;
	T value=w(0);

	for(int i =1; i < N;i++)
	{
		if(w(i)< value)
		{
			index=i;
			value=w(i);
		}
	}

	//the plane normal is the singular vector with the biggest singular value
	cgv::scene::geometry::normal<T,N> plane_normal;
	for(int i =0; i < N;i++)
	{
		plane_normal(i) = v(i,index);
	}
	plane_normal=cgv::math::linalg::normalize(plane_normal);

	//calculate distance of the plane
	T distance = dot(plane_normal,centroid);


	p->normal=plane_normal;
	p->distance=distance;
	
}

		}
	}
}






