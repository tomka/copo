#pragma once

#include <cgv/scene/graph/node.h>
#include <math.h>
#include <cgv/scene/geometry/ray.h>
namespace cgv{
	namespace scene{
		namespace graph{

class ray_node : public node
{
	
	
	cgv::scene::geometry::ray<float,3> *r;
	
	
public:
	ray_node(cgv::scene::geometry::ray<float,3> *r)
	{
		this->r = r;
				
	}

	

	void render()
	{
		glBegin(GL_LINES);				
		glVertex3fv(r->origin);
		glVertex4f(r->direction.x(),r->direction.y(),r->direction.z(),0);
		glEnd();
	
		
	}

};
		}
	}
}