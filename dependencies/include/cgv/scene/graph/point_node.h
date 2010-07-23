#pragma once

#include <cgv/scene/graph/node.h>
#include <vector>
#include <math.h>
#include <cgv/scene/geometry/point.h>

namespace cgv{
	namespace scene{
		namespace graph{

class point_node : public node
{
		

public:
	cgv::scene::geometry::point<float,3> *p;
	
	float size;
	
	point_node(cgv::scene::geometry::point<float,3> *p, float size)
	{
		this->p = p;
		this->size = size;
	}


	

	void render()
	{
		glPointSize(size);
		glEnable(GL_POINT_SMOOTH);
		
		glBegin(GL_POINTS);
			glVertex3fv(*p);
		glEnd();

		glPointSize(1.0f);
		
								
	
		
	}

};

		}
	}
}