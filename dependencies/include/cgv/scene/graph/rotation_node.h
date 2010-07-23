#pragma once
#include <cgv/scene/graph/group_node.h>

namespace cgv{
	namespace scene{
		namespace graph{
/**
* Transformation node representing a rotation around the axis (dx,dy,dz)^T  with given angle.
* The angle is stored in degree.
*/
class rotation_node : public group_node
{
	float dx,dy,dz,angle;
public:
	
	rotation_node()
	{
		set_rotation(0,0,1,0);
	}

	rotation_node(float angle, float dx, float dy, float dz)
	{
		set_rotation(angle,dx,dy,dz);		
	}

	void set_rotation(float angle, float dx, float dy, float dz)
	{
		this->dx=dx;
		this->dy=dy;
		this->dz=dz;
		this->angle=angle;
	}


	void render()
	{
		glPushMatrix();
		
		glRotatef(angle,dx,dy,dz);
		group_node::render();

		glPopMatrix();
	}


};

		}
	}
}
