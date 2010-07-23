#pragma once
#include <cgv/scene/graph/group_node.h>

namespace cgv{
	namespace scene{
		namespace graph{

class scaling_node : public group_node
{
	float sx,sy,sz;
public:	
	
	scaling_node()
	{
		set_scaling(1,1,1);
	}

	scaling_node(float sx, float sy, float sz)
	{
		set_scaling(sx,sy,sz);
	}

	void set_scaling(float s)
	{
		sx=sy=sz=s;
	}

	void set_scaling(float sx, float sy, float sz)
	{
		this->sx = sx;
		this->sy = sy;
		this->sz = sz;
	}

	void render()
	{
		glPushMatrix();
		glScalef(sx,sy,sz);
		group_node::render();
		glPopMatrix();
	}


};
		}
	}
}
