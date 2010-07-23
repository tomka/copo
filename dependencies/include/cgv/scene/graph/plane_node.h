#pragma once
#include <cgv/scene/graph/node.h>
#include <cgv/scene/geometry/plane.h>

namespace cgv {
	namespace scene{
		namespace graph{
/**
* A render node to show a given infinite plane.
*/

class plane_node : public node
{
	cgv::scene::geometry::plane<float,3> *p;
	

public:
	///construct node for a given plane
	plane_node(cgv::scene::geometry::plane<float,3> *p)
	{
		this->p = p;
		
	}

	
	///render infinite plane
	void render()
	{
		glDisable(GL_CULL_FACE);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	
		cgv::scene::geometry::point<float,3> pos = p->distance*(cgv::math::linalg::fix_col_vec<float,3>)p->normal;
		
		cgv::math::linalg::fix_col_vec<float,3> v(0.0f);
		v(0)=1.0f;
		if(1.0f-fabs(dot(v,(cgv::math::linalg::fix_col_vec<float,3>)p->normal))< 0.1)
		{
			v(0)=0.0f;
			v(1)=1.0f;
		}

		cgv::math::linalg::fix_col_vec<float,3> v1 = normalize(cross(v,(cgv::math::linalg::fix_col_vec<float,3>)p->normal));
		cgv::math::linalg::fix_col_vec<float,3> v2 = normalize(cross(v1,(cgv::math::linalg::fix_col_vec<float,3>)p->normal));
		cgv::math::linalg::fix_col_vec<float,3> v3 = -v1;
		cgv::math::linalg::fix_col_vec<float,3> v4 = -v2;
		glColor3f(1,1,1);
		glBegin(GL_TRIANGLE_FAN);
		glNormal3fv(p->normal);
		glVertex3fv(pos);
		
		glVertex4f(v1.x(),v1.y(),v1.z(),0);
		glVertex4f(v4.x(),v4.y(),v4.z(),0);
		glVertex4f(v3.x(),v3.y(),v3.z(),0);
		glVertex4f(v2.x(),v2.y(),v2.z(),0);
		glVertex4f(v1.x(),v1.y(),v1.z(),0);

		

		glEnd();

		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	
		
		
		
	}

};

		}
	}
}