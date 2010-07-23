#pragma once
#include <cgv/utils/obj_reader.h>
#include <cgv/scene/geometry/polygonal_mesh.h>
#include <cgv/math/linalg/fix_vec_prod.h>
#include <cgv/math/linalg/fix_add.h>
#include <vector>


namespace cgv{
	namespace scene{
		namespace geometry{

			class polygonal_mesh_obj_reader : public cgv::utils::obj_reader
{
private:
	polygonal_mesh *mesh;
	
	
public:
	polygonal_mesh_obj_reader(const std::string& filename):cgv::utils::obj_reader(filename)

	{
		mesh = NULL;	
	}
	
	cgv::scene::geometry::polygonal_mesh* load()
	{
		std::cout << "loading model "<<this->filename<<"...";

		mesh=new polygonal_mesh;
		if(open())
		{
			read();
			std::cout<< "ready\n" << "computing half edge conectivity...";
			find_inverses();
			std::cout << "ready\n";
			if(mesh->normals.empty())
			{
				std::cout << "no normals available, computing face normals...";
				compute_face_normals();
				std::cout << "ready\n";
				
			}
			return mesh;
		}
		else
		{
			delete mesh;
			return NULL;
		}
		
	}

	void compute_face_normals()
	{
		for(int fi =0; fi < (int)mesh->faces.size();fi++)
		{
			int hi = mesh->faces[fi].edge;
			int nindex =-1;
			
			
			polygonal_mesh::col_vecf v(0.0f);
			polygonal_mesh::col_vecf pp = mesh->positions[mesh->vertices[mesh->edges[hi].origin].position];
			int hj = hi;
			do
			{
				int hk = mesh->edges[hj].next;
				polygonal_mesh::col_vecf np = mesh->positions[mesh->vertices[mesh->edges[hk].origin].position];
				v += cross(pp,np);
				hj=hk;
				pp = np;
			}while (hj!=hi);
			
			polygonal_mesh::normal3f n = v;
			mesh->normals.push_back(n);
			nindex=(int)mesh->normals.size()-1;
			hi = mesh->faces[fi].edge;
			hj = hi;
			do
			{
				mesh->edges[hi].normal = nindex;
				hi=mesh->edges[hi].next;
			}while (hj!=hi);
		}
		

		
	}

	void find_inverses()
	{
		//vertexindex of orig-> edgeindex
		typedef std::vector<int>  edgeids;
		std::vector<edgeids*> orig_to_edge;

		for(unsigned i=0; i < mesh->vertices.size();i++)
		{
			orig_to_edge.push_back(new edgeids);
		}

		for (unsigned i=0; i < mesh->edges.size(); i++) 
		{
			
			orig_to_edge[mesh->edges[i].origin]->push_back(i);
		}

		for(unsigned i = 0; i < mesh->edges.size();i++)
		{
			if(mesh->edges[i].inverse == -1)
			{
				int eorig = mesh->edges[i].origin;
				int enextorig = mesh->edges[mesh->edges[i].next].origin;


				edgeids* candidates = orig_to_edge[enextorig];
			
				for(unsigned j = 0; j < candidates->size();j++)
				{
					int ej = mesh->edges[(*candidates)[j]].next;
					if(mesh->edges[ej].origin==eorig)
					{
						mesh->edges[(*candidates)[j]].inverse = i;
						mesh->edges[i].inverse=(*candidates)[j];
						break;
					}
				}
			}
						
		}

		for(unsigned i=0; i < mesh->vertices.size();i++)
		{
			delete orig_to_edge[i];
		}
		orig_to_edge.clear();
	

	}

	void process_comment(int index,const std::string& comment)
	{
		
	}

	void process_vertex(int index, float x, float y, float z)
	{
		mesh->positions.push_back(polygonal_mesh::point3f(x,y,z));
		mesh->vertices.push_back(polygonal_mesh::he_vert(index-1));

		
	}
	
	void process_texcoord(int index, float u, float v, float w=0)
	{
		
		polygonal_mesh::texcoordf t(3);
		t(0)=u;
		t(1)=v;
		t(2)=w;
		mesh->texcoords.push_back(t);
	}
	
	void process_normal(int index, float x, float y, float z)
	{
		
		mesh->normals.push_back(polygonal_mesh::normal3f(x,y,z));

	}
	
	void process_face(int index, int vcount, int *vertices, int *texcoords=NULL, int *normals=NULL)
	{
		
		

		for(int i = 0; i < vcount; i++)
		{
			polygonal_mesh::he_edge e;
			
			e.face = (index-1);
			e.origin = (vertices[i]-1);
			e.inverse = -1;
			if(normals)
				e.normal = normals[i]-1;
			



			/*if(texcoords)
			{
				mesh->texcoordset
				e.texcoordset 
			
			int texcoordset; //texcoordset of origin vertex
		*/
       

			mesh->edges.push_back(e);

			//vertex->edge
			mesh->vertices[e.origin].edge = int(mesh->edges.size()-1);
			
			//next
			if ( i < (vcount-1)) 
				mesh->edges[mesh->edges.size()-1].next = int(mesh->edges.size());
			else
				mesh->edges[mesh->edges.size()-1].next = int(mesh->edges.size()-vcount);
			
			
			
			
			
			
			
		}
		
		polygonal_mesh::he_face f;
		
		mesh->faces.push_back(f);

		mesh->faces[(index-1)].edge = int(mesh->edges.size()-1);

		
		
	}

	
};

		}
	}
}

