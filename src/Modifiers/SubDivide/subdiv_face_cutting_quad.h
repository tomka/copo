#pragma once

#include "subdiv_scheme.h"

/**
* Implementation of face cutting subdivision.
*/
class subdiv_face_cutting_quad : public subdiv_scheme {
public:
	/// constructor
	subdiv_face_cutting_quad(int x, int y, int width, int height) : subdiv_scheme(x,y,width,height) {
	}
	/// destructor
	~subdiv_face_cutting_quad() {
	}
	/// get name of scheme
	virtual const char* get_name() const {
		return "Face cutting (quad)";
	}
	/// subdivision step, calls compute_vertex_point, compute_edge_point and compute_face_point
	virtual void subdiv_step(SnowSim::polygonal_mesh* mesh) {
		/* also works without test
		//test whether quad
		bool quad = true;
		face *f = mesh->face_source;
		while (f!=NULL) {
			quad &= f->valence == 4;
			if (!quad) {
				return;
			}
			f=f->next_face;
		}
		*/
		// compute face points
		std::vector<point3f> tmp_fp(mesh->number_faces);
		face *f = mesh->face_source;
		while (f!=NULL) {
			tmp_fp[f->index] = compute_face_point(f);
			f=f->next_face;
		}
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
			point3f p;
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
			if ((h->origin->level < step) && (h->inverse->origin->level < step)) {
				halfedge *h_new = mesh->split_edge(h, tmp_ep[h->m_edge->index]);
				h_new->origin->level = step;
			}
			h=h->next_halfedge;
		};
		//insert edges
		h=mesh->halfedge_source;
		while (h!=NULL) {
			if ((h->origin->level == step) && (h->inverse->origin->level < step)&&(h->prev->origin->level < step) && (!h->border)) {
				int f_old = h->m_face->index;
				halfedge *h_new_1 = mesh->insert_edge(h, h->next->next->next->next);
				halfedge *h_new_2 = mesh->split_edge(h_new_1, tmp_fp[f_old]);
				h_new_2->origin->level = step;
				mesh->insert_edge(h_new_2,h_new_2->next->next->next);
				halfedge *h1 = h_new_1->inverse;
				halfedge *h2 = h_new_1->inverse->next->next->next;
				do {
					mesh->insert_edge(h1,h2);
					h1 = h2->prev;
					h2 = h2->next->next;
				} while (h2 != h_new_2->inverse);
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
	/// compute face point
	virtual point3f compute_face_point(face* f) {
		return f->compute_center();
	}
};
