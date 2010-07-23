#pragma once

#include <cgv/scene/geometry/geometry.h>
#include <cgv/math/linalg/linalg.h>
#include "types.h"
#include "utilities.h"
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>
#include <iosfwd>

/**
* Half edge polygon mesh class. The half edge structure is implemented as a two sided list.
*/

namespace SnowSim {
	class polygonal_mesh {
		friend class polygonal_mesh_obj_reader;
		friend class Mesh;
	public:
		typedef cgv::scene::geometry::normal<float,3> normal3f;
		typedef cgv::math::linalg::fix_col_vec<float,3> texcoordf;
		typedef cgv::math::linalg::fix_col_vec<float,3> col_vecf;

		/// number of halfedges
		int number_halfedges;
		/// number of edges
		int number_edges;
		/// number of vertices
		int number_vertices;
		/// number of faces
		int number_faces;

		/// average edge length
		float avg_edge_length;

		/// halfedge
		struct halfedge;
		/// vertex
		struct vertex;
		/// edge
		struct edge;
		/// face
		struct face;

		#pragma region Halfedge

		struct halfedge {
		public:
			/// vertex at the beginning of the half-edge
			vertex* origin;
			/// oppositely oriented adjacent half-edge
			halfedge* inverse;
			/// face the half-edge borders
			face* m_face;
			/// next half-edge around the face
			halfedge* next;
			/// previous half-edge
			halfedge* prev;
			/// whether halfedge is at border
			bool border;
			/// next half edge in list
			halfedge* next_halfedge;
			/// previous half edge in list
			halfedge* previous_halfedge;
			/// corresponding edge
			edge* m_edge;
			/// index number
			int index;
			/// computes and returns the center of the halfedge
			point3f compute_center();
			/// computes and returns halfedge length
			float compute_length();
			/// check whether vertex origin is convex
			bool compute_convex();
		};

		#pragma endregion

		#pragma region Edge

		/// edge
		struct edge {
			/// one halfedge bordering the edge
			halfedge* m_halfedge;
			/// next_edge in list
			edge* next_edge;
			/// previous in list
			edge* previous_edge;
			/// whether lies on border
			bool border;
			/// index number
			int index;
			/// whether marked
			bool marked;
			/// computes and returns the center of the edge
			point3f compute_center();
			/// computes and returns edge length
			float compute_length();
		};

		#pragma endregion

		#pragma region Vertex

		///vertex
		struct vertex {
			/// position of vertex
			point3f position;
			/// normal of this vertex
			point3f normal;
			/// half edge with current vertex as origin
			halfedge* m_halfedge;
			/// next vertex in list
			vertex* next_vertex;
			/// previous vertex in list
			vertex* previous_vertex;
			/// whether lies on border
			bool border;
			/// corresponding index
			int index;
			/// wheter marked
			bool marked;
			/// subdivision level
			int level;
			/// valence
			int valence;
			/// check whether vertex is convex
			bool compute_convex();
			/// computes and returns the unit normal of the face
			point3f compute_normal();

			/// the constructor
			vertex()
				: normal(0,0,0)
			{
			}
		};

		#pragma endregion

		#pragma region Face

		///face
		struct face {
			/// one of the half-edges bordering the face
       		halfedge* m_halfedge;
			/// next face in list
			face* next_face;
			/// previou face in list
			face* previous_face;
			/// corresponding index
			int index;
			/// normal of the face
			point3f normal;
			/// valence
			int valence;
			/// area
			float area;
			/// slope angle
			float slope_angle;
			/// faces the face the outside of a model
			bool faces_outside;
			/// material index
			int material;

			/// general purpose mark
			bool marked;

			/// computes and returns the unit normal of the face
			point3f compute_normal();
			/// computes and returns the center of the face
			point3f compute_center();
			/// find ear
			polygonal_mesh::halfedge* find_ear();
			/// compute area
			float compute_area();
			//computes the XZ projected area
			float compute_projected_area();

			float compute_slope();
		};

		#pragma endregion

		/// source pointer of halfedge list
		halfedge* halfedge_source;
		/// source pointer of vertex list
		vertex* vertex_source;
		/// source pointer of face list
		face* face_source;
		/// source pointer of edge list
		edge* edge_source;
		/// source pointer of halfedge list
		halfedge* halfedge_end;
		/// source pointer of vertex list
		vertex* vertex_end;
		/// source pointer of face list
		face* face_end;
		/// source pointer of edge list
		edge* edge_end;

		// axis aligned bounding box
		BBox3f m_bounding_box;

		//std::vector<texcoord_set> texcoordsets;
		//std::vector<texcoordf > texcoords;

		bool IsTriangulated() {
			return isTriangulated;
		}

	private:

		/// whether triangulated
		bool isTriangulated;

		/// inserts halfedge h_new
		void insert_halfedge_in_list(halfedge *h_new) {
			if (halfedge_end == NULL) {
				h_new->next_halfedge = NULL;
				h_new->previous_halfedge = NULL;
				halfedge_end = h_new;
				halfedge_source = h_new;
			} else {
				h_new->next_halfedge = NULL;
				h_new->previous_halfedge = halfedge_end;
				halfedge_end->next_halfedge = h_new;
				halfedge_end=h_new;
			}
			h_new->index = number_halfedges;
			number_halfedges++;
			isTriangulated = false;
		}

		/// inserts edge e_new
		void insert_edge_in_list(edge *e_new) {
			if (edge_end == NULL) {
				e_new->next_edge = NULL;
				e_new->previous_edge = NULL;
				edge_end = e_new;
				edge_source = e_new;
			} else {
				e_new->next_edge = NULL;
				e_new->previous_edge = edge_end;
				edge_end->next_edge = e_new;
				edge_end=e_new;
			}
			e_new->index = number_edges;
			number_edges++;
			isTriangulated = false;
		}
		/// inserts face f_new
		void insert_face_in_list(face *f_new) {
			if (face_end == NULL) {
				f_new->next_face = NULL;
				f_new->previous_face = NULL;
				face_end = f_new;
				face_source = f_new;
			} else {
				f_new->next_face = NULL;
				f_new->previous_face = face_end;
				face_end->next_face = f_new;
				face_end=f_new;
			}
			f_new->index = number_faces;
			number_faces++;
			isTriangulated = false;
		}
		/// inserts vertex v_new
		void insert_vertex_in_list(vertex *v_new) {
			if (vertex_end == NULL) {
				v_new->next_vertex = NULL;
				v_new->previous_vertex = NULL;
				vertex_end = v_new;
				vertex_source = v_new;
			} else {
				v_new->next_vertex = NULL;
				v_new->previous_vertex = vertex_end;
				vertex_end->next_vertex = v_new;
				vertex_end=v_new;
			}
			v_new->index = number_vertices;
			number_vertices++;
			isTriangulated = false;
		}
		/// deletes vertex in list
		void delete_vertex_in_list(vertex* v) {
			if (v->previous_vertex != NULL) {
				v->previous_vertex->next_vertex=v->next_vertex;
			} else {
				if (v->next_vertex != NULL) {
					vertex_source = v->next_vertex;
					v->next_vertex->previous_vertex = NULL;
				} else {
					vertex_source = NULL;
				}
			}
			if (v->next_vertex != NULL) {
				v->next_vertex->previous_vertex=v->previous_vertex;
				vertex *vi = v->next_vertex;
				while (vi!=NULL) {
					vi->index--;
					vi=vi->next_vertex;
				}
			} else {
				if (v->previous_vertex != NULL) {
					vertex_end = v->previous_vertex;
					v->previous_vertex->next_vertex = NULL;
				} else {
					vertex_end = NULL;
				}
			}
			number_vertices--;
			delete v;
			isTriangulated = false;
		}
		/// deletes face in list
		void delete_face_in_list(face* f) {
			if (f->previous_face != NULL) {
				f->previous_face->next_face=f->next_face;
			} else {
				if (f->next_face != NULL) {
					face_source = f->next_face;
					f->next_face->previous_face = NULL;
				} else {
					face_source = NULL;
				}
			}
			if (f->next_face != NULL) {
				f->next_face->previous_face=f->previous_face;
				face *fi = f->next_face;
				while (fi!=NULL) {
					fi->index--;
					fi=fi->next_face;
				}
			} else {
				if (f->previous_face != NULL) {
					face_end = f->previous_face;
					f->previous_face->next_face = NULL;
				} else {
					face_end = NULL;
				}
			}
			number_faces--;
			delete f;
			isTriangulated = false;
		}
		/// deletes edge in list
		void delete_edge_in_list(edge* e) {
			if (e->previous_edge != NULL) {
				e->previous_edge->next_edge=e->next_edge;
			} else {
				if (e->next_edge != NULL) {
					edge_source = e->next_edge;
					e->next_edge->previous_edge = NULL;
				} else {
					edge_source = NULL;
				}
			}
			if (e->next_edge != NULL) {
				e->next_edge->previous_edge=e->previous_edge;
				edge *ei = e->next_edge;
				while (ei!=NULL) {
					ei->index--;
					ei=ei->next_edge;
				}
			} else {
				if (e->previous_edge != NULL) {
					edge_end = e->previous_edge;
					e->previous_edge->next_edge = NULL;
				} else {
					edge_end = NULL;
				}
			}
			number_edges--;
			delete e;
			isTriangulated = false;
		}
		/// deletes halfedge in list
		void delete_halfedge_in_list(halfedge* h) {
			if (h->previous_halfedge != NULL) {
				h->previous_halfedge->next_halfedge=h->next_halfedge;
			} else {
				if (h->next_halfedge != NULL) {
					halfedge_source = h->next_halfedge;
					h->next_halfedge->previous_halfedge = NULL;
				} else {
					halfedge_source = NULL;
				}
			}
			if (h->next_halfedge != NULL) {
				h->next_halfedge->previous_halfedge=h->previous_halfedge;
				halfedge *hi = h->next_halfedge;
				while (hi!=NULL) {
					hi->index--;
					hi=hi->next_halfedge;
				}
			} else {
				if (h->previous_halfedge != NULL) {
					halfedge_end = h->previous_halfedge;
					h->previous_halfedge->next_halfedge = NULL;
				} else {
					halfedge_end = NULL;
				}
			}
			number_halfedges--;
			delete h;
			isTriangulated = false;
		}
	public:
		/// contructor
		polygonal_mesh() {
			halfedge_source = NULL;
			edge_source = NULL;
			vertex_source = NULL;
			face_source = NULL;
			halfedge_end = NULL;
			edge_end = NULL;
			vertex_end = NULL;
			face_end = NULL;
			number_halfedges = 0;
			number_faces = 0;
			number_edges = 0;
			number_vertices = 0;
			avg_edge_length = 0;
			isTriangulated = false;
		}

		polygonal_mesh(polygonal_mesh* copy) {
			this->copy_from(copy);
		}

		/// destructor
		~polygonal_mesh()	{
			while (halfedge_end!=NULL) {
				halfedge *h = halfedge_end->previous_halfedge;
				delete halfedge_end;
				halfedge_end = h;
			}
			while (edge_end!=NULL) {
				edge *e = edge_end->previous_edge;
				delete edge_end;
				edge_end = e;
			}
			while (face_end!=NULL) {
				face *f = face_end->previous_face;
				delete face_end;
				face_end = f;
			}
			while (vertex_end!=NULL) {
				vertex *v = vertex_end->previous_vertex;
				delete vertex_end;
				vertex_end = v;
			}
		}

		/// copies a mesh
		void copy_from(polygonal_mesh* mesh);

		/// splits edge e at the point p. returns the new edge.
		edge* split_edge(edge* e, point3f& p);

		/// splits edge with halfedge h at the point p. returns the new halfedge, with the new vertex as origin and h as prev halfedge.
		halfedge* split_edge(halfedge* h, point3f& p);

		/// splits edge e at edge center. returns the new edge.
		edge* split_edge(edge* e);

		/// splits edge with halfedge h at halfedge center. returns the new halfedge, with the new vertex as origin and h as prev halfedge.
		halfedge* split_edge(halfedge* h);

		/// inserts an edge that connects the vertices v1 and v2 and returns the new edge. v1 and v2 have to share the same face. it is not allowed that v1 and v2 already share an edge.
		edge* insert_edge(vertex *v1, vertex *v2);

		/// inserts an edge that connects the origin vertices if halfedges h1
		/// and h2 and returns the new halfedge. h1 and h2 have to lie in the
		/// same face and not at border. between h1 and h2 there has to be at
		/// least one halfedge. h2 will be in the new face. returns the halfedge
		/// of the new edge that lies in the old face.
		halfedge* insert_edge(halfedge* h1, halfedge* h2);

		/// computes all face normals and stores them in the list
		void compute_face_normals();

		/// computes all face normals and stores them in the list
		void compute_vertex_normals();

		/// computes all face areas
		void compute_face_areas();

		/// computes all face areas
		void compute_projected_face_areas();

		/// saves a model
		bool save(const char* filename);

		/// triangulates a face
		void triangulate(face *f);

		/// triangulates the mesh
		void triangulate();

		/// set all marks
		void set_all_marks(bool marked);

		};
}
