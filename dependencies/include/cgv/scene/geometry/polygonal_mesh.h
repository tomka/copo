#pragma once

#include <vector>
#include <cgv/scene/geometry/geometry.h>




namespace cgv {
	namespace scene {
		namespace geometry {
/**
* A polygonal mesh represented by a half edge data structure
*/
class polygonal_mesh 
{
	
	

public:
	typedef std::vector<int> texcoord_set;
	typedef cgv::scene::geometry::normal<float,3> normal3f;
	typedef cgv::scene::geometry::point<float,3> point3f;
	typedef cgv::math::linalg::fix_col_vec<float,3> texcoordf;
	typedef cgv::math::linalg::fix_col_vec<float,3> col_vecf;

	

	polygonal_mesh(){}
	
	virtual ~polygonal_mesh()
	{
	}


			
	struct he_vert;
	struct he_face;
	struct he_edge
	{
		he_edge(int o = -1,int no = -1,int ts=-1, int i = -1, int f = -1, int ne=-1): origin(o),normal(no),texcoordset(ts), inverse(i), face(f), next(ne) {}  
		int origin;   // vertex at the beginning of the half-edge
		int normal;	//normal at origin vertex corresponding to face of current half edge
		int texcoordset; //texcoordset of origin vertex
        int inverse;   // oppositely oriented adjacent half-edge 
        int face;   // face the half-edge borders
        int next;   // next half-edge around the face    
    };
	struct he_vert
    {
		he_vert( int p = -1, int e = -1 )
			:  edge(e), position(p) {}
		
		int position; //position of vertex
		int edge; //half edge with current vertex as origin
    
    };  
	struct he_face
    {
		he_face(int e = -1): edge(e) {}
       	int edge;  // one of the half-edges bordering the face
		
    };

	point3f& position(he_vert& v)
	{
		return positions[v.position];
	}

	he_edge& edge(he_vert& v)
	{
		return edges[v.edge];
	}	

	he_edge& edge(he_face& f)
	{
		return edges[f.edge];
	}
	
	he_vert& origin(he_edge& e)
	{
		return vertices[e.origin];
	}

	normal3f& normal(he_edge &e)
	{
		return normals[e.normal];
	}

	
	texcoord_set& texcoordset(he_edge &e)
	{
		return texcoordsets[e.texcoordset];
	}
	
	texcoordf& texcoord(he_edge &e, int set=0)
	{
		return texcoords[texcoordsets[e.texcoordset][set]];
	}

    he_edge& inverse(he_edge &e) 
	{
		return edges[e.inverse];
	}

    he_face& face(he_edge &e)
	{
		return faces[e.face];
	}

    he_edge& next(he_edge &e)
	{
		return edges[e.next];
	}

	

	bool normals_available()
	{
		return normals.size() != 0;
	}

	bool texcoords_available()
	{
		return texcoords.size() != 0;
	}


	void create_texcoords_from_worldpos()
	{
		texcoordsets.resize(edges.size());
		texcoords.resize(edges.size());

		for(int he = 0; he < (int)edges.size(); he++)
		{
			texcoordsets[he].push_back(he);
			edges[he].texcoordset = he;
			texcoords[he].set(positions[vertices[edges[he].origin].position].x(),
				positions[vertices[edges[he].origin].position].y(),
				positions[vertices[edges[he].origin].position].z());

		}
	}
	

	std::vector<he_vert> vertices;
	std::vector<he_edge> edges;
	std::vector<he_face> faces;
	std::vector<normal3f> normals;
	std::vector<point3f> positions;
	std::vector<texcoord_set> texcoordsets;
	std::vector<texcoordf > texcoords;
	
	
	
};
		}
	}
}




