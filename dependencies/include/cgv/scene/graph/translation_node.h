#pragma once
#include <cgv/scene/graph/group_node.h>

namespace cgv{
	namespace scene{
		namespace graph{

class translation_node : public group_node
{
protected:
	float tx,ty,tz;

public:
	
	translation_node()
	{
		tx = 0;
		ty = 0;
		tz = 0;
	}

	translation_node(float tx, float ty, float tz)
	{
		set_translation(tx,ty,tz);		
	}

	void set_translation(float tx, float ty, float tz)
	{
		this->tx = tx;
		this->ty = ty;
		this->tz = tz;
	}



	void render()
	{
		glPushMatrix();
		glTranslatef(tx,ty,tz);
		group_node::render();
		glPopMatrix();
	}


};
		}
	}
}
