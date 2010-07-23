#pragma once

#include "subdiv_face_cutting_quad.h"

/** 
* Implementation of Kobbelt subdivision. 
*/
class subdiv_kobbelt : public subdiv_face_cutting_quad {
public:
	/// constructor
	subdiv_kobbelt(int x, int y, int width, int height) : subdiv_face_cutting_quad(x,y,width,height) {
	}
	/// destructor
	~subdiv_kobbelt() {
	}
	/// get name of scheme
	virtual const char* get_name() const {
		return "Kobbelt";
	}
	/// compute vertex point
	virtual point3f compute_vertex_point(vertex* v) {

		return v->position;

	}
	/// compute edge point
	virtual point3f compute_edge_point(edge* e) {

		halfedge *h = e->m_halfedge;
		if (e->border) {
			if (!h->border) {
				h=h->inverse;
			}
			return 0.5625f*(h->origin->position+h->inverse->origin->position)-0.0625f*(h->prev->origin->position+h->next->inverse->origin->position);
		}
		return 0.5625f*(h->origin->position+h->inverse->origin->position)-0.0625f*(h->prev->inverse->prev->origin->position+h->next->inverse->next->inverse->origin->position);		

	}
	/// compute face point
	virtual point3f compute_face_point(face* f) {

		point3f p(0.0f,0.0f,0.0f);
		halfedge *h = f->m_halfedge;
		p += 0.5625f*(compute_edge_point(h->m_edge)+compute_edge_point(h->prev->prev->m_edge))-0.0625f*(compute_edge_point(h->inverse->next->next->m_edge)+compute_edge_point(h->next->next->inverse->next->next->m_edge)); 
		h=h->next;
		p += 0.5625f*(compute_edge_point(h->m_edge)+compute_edge_point(h->prev->prev->m_edge))-0.0625f*(compute_edge_point(h->inverse->next->next->m_edge)+compute_edge_point(h->next->next->inverse->next->next->m_edge)); 
		p = 0.5f * p;
		return p;

	}
};