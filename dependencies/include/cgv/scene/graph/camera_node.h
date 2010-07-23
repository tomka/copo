
#pragma once
#include <cgv/scene/graph/group_node.h>

namespace cgv{
	namespace scene{
		namespace graph{

class camera_node : public group_node
{
protected:
	float projection[16];
	float view[16];

public:
	
	camera_node()
	{
		for(int i =0; i < 4; i++)
		{
			for(int j = 0; j < 4;j++)
			{
				if(i!=j)
				projection[i*4+j] = 0;
				modelview[i*4+j] = 0;
			}
		}
	}

	void set_projection(float *projection)
	{
		for(int i=0; i < 16; i++)
			this->projection[i]=projection[i];
	}

	void set_view(float *projection)
	{
		for(int i=0; i < 16; i++)
			this->view[i]=view[i];
	}




	void render()
	{
		

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		
		glLoadIdentity();
		glLoadMatrixf(projection);
		
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		
		glLoadMatrix(view);
		group_node::render();
		
		glPopMatrix();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
	}


};
		}
	}
}
