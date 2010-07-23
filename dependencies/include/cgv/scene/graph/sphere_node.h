#pragma once

#include <cgv/scene/graph/node.h>
#include <vector>
#include <math.h>
#include <cgv/scene/geometry/sphere.h>

namespace cgv{
	namespace scene{
		namespace graph{

class sphere_node : public node
{
	
	int tess_u,tess_v;
	cgv::scene::geometry::sphere<float,3> *s;
	float r;
	cgv::scene::geometry::point<float,3> pos;

	
	std::vector<cgv::scene::geometry::point<float,3> > points;
	std::vector<cgv::scene::geometry::normal<float,3> > normals;

public:
	sphere_node(cgv::scene::geometry::sphere<float,3> *s,int tess_u=10,int tess_v=15)
	{
		this->s = s;
		
		this->tess_u=tess_u;
		this->tess_v=tess_v;
		
		
	}

	void init()
	{
		pos=s->center;
		r = s->radius;
		points.clear();
		normals.clear();

		float PI = 3.14159f;

		cgv::scene::geometry::point<float,3> p;
		cgv::scene::geometry::normal<float,3> n;

		for(int u =0; u < tess_u; u++)
		{

			for(int v = 0; v < tess_v; v++)
			{
				p.y() = s->radius*(float)sin(PI/2.0f - u*PI/(tess_u-1));
				p.x() =	(float)sin(v*2.0*PI/(tess_v-1))*cos(PI/2.0f - u*PI/(tess_u-1))*s->radius;
				p.z() =	(float)cos(v*2.0*PI/(tess_v-1))*cos(PI/2.0f - u*PI/(tess_u-1))*s->radius;
				
				n.set(p.x(),p.y(),p.z());
				p= p+(cgv::math::linalg::fix_col_vec<float,3>)s->center;
				points.push_back(p);
				normals.push_back(n);
				if(u == 0 || u == tess_u-1)
					break;
			}
		}
		
	}

	void render()
	{
		if(r != s->radius || pos != s->center)
			init();

		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	/*	glBegin(GL_POINTS);
		for(int i=0;i < (int)points.size();i++)
			glVertex3fv(points[i]);
		glEnd();*/
		
		
		
		glBegin(GL_TRIANGLE_FAN);
		glNormal3fv(normals[0]);
		glVertex3fv(points[0]);
		for(int v = 0; v < tess_v; v++)
		{
			glNormal3fv(normals[1+v]);		
			glVertex3fv(points[1+v]);					
		}
		glEnd();

		
		for(int u = 0; u < tess_u-3;u++)
		{
			for(int v = 0; v < tess_v-1; v++)
			{
				glBegin(GL_QUADS);
				glNormal3fv(normals[v+1+tess_v*u]);			
				glVertex3fv(points[v+1+tess_v*u]);

				glNormal3fv(normals[v+1+(u+1)*tess_v]);
				glVertex3fv(points[v+1+(u+1)*tess_v]);					

				glNormal3fv(normals[v+2+(u+1)*tess_v]);	
				glVertex3fv(points[v+2+(u+1)*tess_v]);	

				glNormal3fv(normals[v+2+tess_v*u]);					
				glVertex3fv(points[v+2+tess_v*u]);					
				glEnd();
			}
			
		}
		
		
		glBegin(GL_TRIANGLE_FAN);
		glNormal3fv(normals[points.size()-1]);
		glVertex3fv(points[points.size()-1]);
		for(int v = 0; v < tess_v; v++)
		{
			glNormal3fv(normals[points.size()-2-v]);					
			glVertex3fv(points[points.size()-2-v]);					
		}
		glEnd();

		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		
					
	
		
	}

};

		}
	}
}