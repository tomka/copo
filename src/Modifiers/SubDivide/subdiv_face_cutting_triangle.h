#pragma once

#include "subdiv_scheme.h"

/**
* Implementation of face cutting subdivision.
*/
class subdiv_face_cutting_triangle : public subdiv_scheme {
public:
	/// constructor
	subdiv_face_cutting_triangle(int x, int y, int width, int height)
		: subdiv_scheme(x,y,width,height)
	{}

	/// destructor
	~subdiv_face_cutting_triangle()
	{}

	/// get name of scheme
	virtual const char* get_name() const {
		return "Face cutting (triangle)";
	}

	/// subdivision step, calls compute_vertex_point and compute_edge_point
	virtual void subdiv_step(SnowSim::polygonal_mesh* mesh)
	{
		/* also works without test
		// test whether triangulated
		bool tria = true;
		face *f = mesh->face_source;
		while (f!=NULL) {
			tria &= f->valence == 3;
			if (!tria) {
				return;
			}
			f=f->next_face;
		}
		*/

		//compute vertex points
		std::vector<point3f> tmp_vp(mesh->number_vertices);
		vertex *v = mesh->vertex_source;
		while (v!=NULL) {
			tmp_vp[v->index] = compute_vertex_point(v);
			v=v->next_vertex;
		}
		//compute edge points
		std::vector<point3f> tmp_ep(mesh->number_edges);
		edge *e = mesh->edge_source;
		while (e!=NULL) {
			tmp_ep[e->index] = compute_edge_point(e);
			e=e->next_edge;
		}
		//set vertices
		v = mesh->vertex_source;
		while (v!=NULL) {
			v->position = tmp_vp[v->index];
			v=v->next_vertex;
		}
		//split edges
		halfedge *h = mesh->halfedge_source;
		while (h!=NULL) {
			if ( 	( h->origin->level < step )
				&& 	( h->inverse->origin->level < step ) )
			{
				halfedge *h_new = mesh->split_edge(h, tmp_ep[h->m_edge->index]);
				h_new->origin->level = step;
			}
			h=h->next_halfedge;
		};
		//insert edges
		h=mesh->halfedge_source;
		while (h!=NULL) {
			if (	( h->origin->level == step )
				&& 	( h->inverse->origin->level < step )
				&& 	(!h->border ) )
			{
				mesh->insert_edge(h, h->next->next);
			}
			h=h->next_halfedge;
		};
	}

	/// compute vertex point
	virtual point3f compute_vertex_point(vertex* v) {
		return v->position;
	}
	/// compute edge point
	virtual point3f compute_edge_point(edge* e) {
		return e->compute_center();
	}
};
