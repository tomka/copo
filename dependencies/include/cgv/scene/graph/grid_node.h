#pragma once
#include <cgv/scene/graph/node.h>
#include <vector>

namespace cgv{
	namespace scene{
		namespace graph{

/**
* A simple grid render node.
*/
class grid_node : public node
{
	int tess_u,tess_v;
	float dx,dy;
	bool visible;
	std::vector<cgv::scene::geometry::point<float,3> > points;

public:
	//constructs a grid with given subdivision and grid distance
	grid_node( int n=20, int m=20,float dx=10.0f, float dy=10.0f)
	{
		tess_u = n/2;
		tess_v = m/2;
		this->dx=dx;
		this->dy=dy;
		visible = true;
	}

	///contruct grid points
	void init()
	{
		cgv::scene::geometry::point<float,3> p;

		for(int u = -tess_u; u <= tess_u; u++)
		{
			for(int v = -tess_v; v <= tess_v; v++)
			{
				p.x() = u*dx; 
				p.y() = 0;
				p.z() = v*dy;
				points.push_back(p);

			}
		}

	}
	void set_visible(bool visible)
	{
		this->visible = visible;
	}

	bool is_visible()
	{
		return visible;
	}

	///render grid
	void render()
	{
		if(visible)
		{
			glDisable(GL_CULL_FACE);
			glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
			//drawgrid lines
			
			glColor3f(0,0,0);
			glBegin(GL_QUADS);
			for(int u = 0; u < 2*tess_u; u++)
			{
				for(int v = 0; v < 2*tess_v; v++)
				{
					glVertex3fv(points[u*(2*tess_u+1)+v]);
					glVertex3fv(points[u*(2*tess_u+1)+v+1]);
					glVertex3fv(points[(u+1)*(2*tess_u+1)+v+1]);
					glVertex3fv(points[(u+1)*(2*tess_u+1)+v]);
					

				}
			}
			glEnd();
			glPolygonMode(GL_FRONT,GL_FILL);
			
			

			
			glLineWidth(2.0f);
			glColor3f(0,0,0);
			glBegin(GL_LINES);
			glVertex3f(-tess_u*dx,0,0);
			glVertex3f(tess_u*dx,0,0);
			glVertex3f(0,0,-tess_v*dy);
			glVertex3f(0,0,tess_v*dy);
			glEnd();

			glLineWidth(3.0f);
			glBegin(GL_LINES);
			glColor3f(1,0,0);
			glVertex3f(0,0,0);
			glVertex3f(dx,0,0);
			
			glColor3f(0,1,0);
			glVertex3f(0,0,0);
			glVertex3f(0,dx,0);
			
			glColor3f(0,0,1);
			glVertex3f(0,0,0);
			glVertex3f(0,0,dx);
			glEnd();
		
			glLineWidth(1.0f);
			glEnable(GL_CULL_FACE);
		}
	}

};

		}
	}
}