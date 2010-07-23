#include <cgv/scene/graph/node.h>
#include <vector>
#include <cgv/scene/geometry/polygonal_mesh.h>

namespace cgv{
	namespace scene{
		namespace graph{

class polygonal_mesh_node : public node
{
	cgv::scene::geometry::polygonal_mesh *mesh;
	
	
public:
	polygonal_mesh_node(cgv::scene::geometry::polygonal_mesh *mesh)
	{
		this->mesh = mesh;		
	}

	

	void render()
	{
		
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glColor3f(1,1,1);
	
		int hi,hj;
		
		for(int fi=0;fi < (int)mesh->faces.size(); fi++)
		{
			hi = hj = mesh->faces[fi].edge;
		
			glBegin(GL_POLYGON);
			do
			{
				if(mesh->normals_available() && mesh->edges[hi].normal!=-1)
					glNormal3fv(mesh->normal(mesh->edges[hi]));
				glVertex3fv(mesh->position(mesh->origin(mesh->edges[hi])));
				hi=mesh->edges[hi].next;
			}
			while(hi!=hj);

			glEnd();
		}
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
		
		
	
	}

};

		}
	}
}