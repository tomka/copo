#pragma once

#include <cgv/scene/graph/node.h>
#include <math.h>
#include <cgv/scene/geometry/straight_line.h>
namespace cgv{
	namespace scene{
		namespace graph{

	
class straight_line_node : public node
{
	
	
	cgv::scene::geometry::straight_line<float,3> *l;
	
	
public:
	straight_line_node(cgv::scene::geometry::straight_line<float,3> *l)
	{
		this->l = l;
				
	}

	

	void render()
	{
		glBegin(GL_LINES);				
		glVertex3fv(l->position);
		glVertex4f(l->direction.x(),l->direction.y(),l->direction.z(),0);
		glVertex3fv(l->position);
		glVertex4f(-l->direction.x(),-l->direction.y(),-l->direction.z(),0);
		glEnd();
	
		
	}

};
		}
	}
}