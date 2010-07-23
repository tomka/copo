#pragma once
#include <cgv/scene/graph/group_node.h>

namespace cgv{
	namespace scene{
		namespace graph{
/**
* Euler rotation node specified with roll, pitch, yaw angles in degree (XYZ-convention).
*/
class euler_rotation_node : public group_node
{
	float yaw,pitch,roll;
public:
	
	///standard constructor
	euler_rotation_node()
	{
		set_rotation(0,0,0);		
	}

	///create rotation  with given angles in degree
	euler_rotation_node( float roll, float pitch, float yaw)
	{
		set_rotation(roll,pitch,yaw);		
	}

	///set rotation angles in degree
	void set_rotation(float roll, float pitch, float yaw)
	{
		this->yaw =yaw;
		this->pitch = pitch;
		this->roll = roll;
	}

	///apply opengl matrices
	void render()
	{
		glPushMatrix();
		
		glRotatef(yaw,0,0,1);
		glRotatef(pitch,0,1,0);
		glRotatef(roll,1,0,0);
	
		group_node::render();

		glPopMatrix();
	}


};

		}
	}
}
