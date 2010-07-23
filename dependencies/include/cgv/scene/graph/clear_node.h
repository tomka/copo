#pragma once
#include <cgv/scene/graph/group_node.h>

namespace cgv{
	namespace scene{
		namespace graph{

class clear_node : public group_node
{
protected:
	float cr,cg,cb,ca;
	float ar,ag,ab,aa;
	GLbitfield mask;
	GLclampd depth;
	GLint stencil;
	GLfloat index;

public:
	
	
	clear_node()
	{
		cr=cg=cb=0.7f;
		ca=1.0f;
		ar=ag=ab=aa=0.0f;
		depth=1.0f;
		index=0.0f;
		stencil=0;
		mask = GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT;
	}

	clear_node(float r, float g, float b, float a=1.0f,
		GLbitfield mask =GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)
	{
		ar=ag=ab=aa=0.0f;
		depth=1.0f;
		index=0.0f;
		stencil=0;
		clear_color(r,g,b,a);
		this->mask =mask;
	}

	void init()
	{
		glClearColor(cr,cg,cb,ca);
		glClearDepth(depth);
		glClearStencil(stencil);
		glClearIndex(index);
		glClearAccum(ar,ag,ab,aa);
		group_node::init();
 

	}

	void clear_color(float r, float g, float b,float a=1.0f)
	{
		cr = r;
		cg = g;
		cb = b;
		ca = a;
		glClearColor(cr,cg,cb,ca);
	}

	void clear_accum(float r, float g, float b,float a=1.0f)
	{
		ar = r;
		ag = g;
		ab = b;
		aa = a;
		glClearColor(ar,ag,ab,aa);
	}
	void clear_stencil(GLint stencil)
	{
		this->stencil=stencil;
		glClearStencil(stencil);
	}	
	void clear_index(GLfloat index)
	{
		this->index=index;
		glClearIndex(index);
	}	

	void clear_depth(GLclampd depth)
	{
		this->depth=depth;
		glClearDepth(depth);
	}



	void enable(GLbitfield mask)
	{
		this->mask = this->mask|mask;
	}

	void disable(GLbitfield mask)
	{
		this->mask =this->mask & !mask;
	}



	void render()
	{
		glClear(mask);
		group_node::render();
		
	}


};
		}
	}
}
