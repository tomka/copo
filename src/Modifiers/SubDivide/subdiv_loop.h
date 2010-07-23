#pragma once

#define _USE_MATH_DEFINES
#ifndef M_PI
#define M_PI    3.14159265358979323846f
#endif

#include "subdiv_face_cutting_triangle.h"
#include <map>

/**
* Implementation of Loop subdivision.
*/
class subdiv_loop : public subdiv_face_cutting_triangle {
private:
	/// whether use limit positions
	bool b_use_limit_positions;
	/// whether use limit normals
	bool b_use_limit_normals;
	/// whether use the hoppe rules
	bool b_use_hoppe_rules;
	/// whether to enforce Interpolation
	bool b_enforce_interpolation;
	/// checkbutton for b_use_limit_positions
	fltk::CheckButton *checkbutton_1;
	/// checkbutton for b_use_limit_normals
	fltk::CheckButton *checkbutton_2;
	/// checkbutton for b_use_hoppe_rules
	fltk::CheckButton *checkbutton_3;
	/// checkbutton for b_enforce_interpolation
	fltk::CheckButton *checkbutton_4;
	/// whether to use wrong border limit
	bool b_use_wrong_border_limit;
	/// checkbotton for b_use_wrong_border_limit
	fltk::CheckButton *checkbutton_5;
	/// whether to use modified rules
	bool b_use_modified_rules;
	/// checkbotton for b_use_modified_rules
	fltk::CheckButton *checkbutton_6;

	// lookuptable for beta
	std::map<int, float> lookup_beta;
	// lookuptable for strange_symbol
	std::map<int, float> lookup_strange_symbol;
	// iterator
	std::map<int,float>::iterator it;
	// beta, strange_symbol
	float beta, strange_symbol;

public:
	/// constructor
	subdiv_loop(int x, int y, int width, int height)
		: subdiv_face_cutting_triangle(x,y,width,height),
			b_use_limit_positions(false),
			b_use_limit_normals(false),
			b_use_hoppe_rules(true),
			b_enforce_interpolation(false),
			b_use_wrong_border_limit(false),
			b_use_modified_rules(true)
	{
		checkbutton_1 = new fltk::CheckButton(0,20,20,20,"use limit positions");
		checkbutton_2 = new fltk::CheckButton(0,40,20,20,"use limit normals");
		checkbutton_3 = new fltk::CheckButton(0,60,20,20,"use hoppe rules");
		checkbutton_4 = new fltk::CheckButton(0,80,20,20,"enforce interpolation");
		checkbutton_5 = new fltk::CheckButton(0,100,20,20,"use wrong border limit");
		checkbutton_6 = new fltk::CheckButton(0,120,20,20,"use modified rules");
		checkbutton_1->value(b_use_limit_positions);
		checkbutton_2->value(b_use_limit_normals);
		checkbutton_3->value(b_use_hoppe_rules);
		checkbutton_4->value(b_enforce_interpolation);
		checkbutton_5->value(b_use_wrong_border_limit);
		checkbutton_6->value(b_use_modified_rules);
		checkbutton_1->callback(checkbutton_1_cb,this);
		checkbutton_2->callback(checkbutton_2_cb,this);
		checkbutton_3->callback(checkbutton_3_cb,this);
		checkbutton_4->callback(checkbutton_4_cb,this);
		checkbutton_5->callback(checkbutton_5_cb,this);
		checkbutton_6->callback(checkbutton_6_cb,this);
		add(checkbutton_1);
		add(checkbutton_2);
		add(checkbutton_3);
		add(checkbutton_4);
		add(checkbutton_5);
		add(checkbutton_6);

		// computing lookup tables for valences up to 20
		for (int i = 1; i < 21; i++) {
			beta = (1.0f/i)
						* (5.0f/8 - pow(3.0f/8 + 0.25f*cos( (2*M_PI)/i),2));
			std::cout << "[Beta-Lookup-Table] Pre-Added beta of " << beta << " for valence of " << i << std::endl;
			lookup_beta[i] = beta;

			strange_symbol = 1.0f/((3.0f/(8.0f*beta)) + i);
			std::cout << "[Strange-Symbol-Lookup-Table] Pre-Added strange symbol of " << beta << " for valence of " << i << std::endl;
			lookup_strange_symbol[i] = strange_symbol;
		}
	}

	/// destructor
	~subdiv_loop() {
		delete checkbutton_1;
		delete checkbutton_2;
		delete checkbutton_3;
		delete checkbutton_4;
		delete checkbutton_5;
		delete checkbutton_6;
	}
	///get name of scheme
	virtual const char* get_name() const {
		return "Loop";
	}
	///compute vertex point
	virtual point3f compute_vertex_point(vertex* v) {

		if (b_use_hoppe_rules) {
			//student begin
			return subdiv_face_cutting_triangle::compute_vertex_point(v);
			//student end
		}

		int valence = v->valence;
		float	left = 1.0f/8,
				right = 1.0f/8,
				self = 6.0f/8,
				beta = 0;

		point3f new_vertex = point3f();
		SnowSim::polygonal_mesh::halfedge* he = v->m_halfedge;


		if (v->border) {

			////////////////////////////////////////////////////
			// Randstempel für Randknoten
			////////////////////////////////////////////////////

			// get border halfedge on the left
			if (!he->inverse->m_face)
				he = he->inverse;
			else if (he->m_face) {	// our halfedge ain't on the border :/ - go and find one matching
				do {
					he = he->inverse->next;
				} while (!he->m_edge->border);
				if (!he->inverse->m_face)
					he = he->inverse;
			}

			new_vertex = self*(v->position)
				+ left*(he->next->origin->position)
				+ right*(he->origin->position);

		} else {

			////////////////////////////////////////////////////
			// Stempel f�r Fl�chenknoten
			////////////////////////////////////////////////////

			it = lookup_beta.find(valence);
			if (it == lookup_beta.end()) {
				beta = (1.0f/valence)
						* (5.0f/8 - pow(3.0f/8 + 0.25f*cos( (2*M_PI)/valence),2));
				std::cout << "[Beta-Lookup-Table] Added beta of " << beta << " for valence of " << valence << std::endl;
				lookup_beta[valence] = beta;
			} else {
				beta = it->second;
			}
			self = 1 - valence*beta;

			new_vertex = self*(v->position);

			for (int i=0; i < valence; i++) {
				he = he->inverse;
				new_vertex = new_vertex + beta*(he->origin->position);
				he = he->next;
			}
		}



		//return subdiv_face_cutting_triangle::compute_vertex_point(v);
		return new_vertex;

		//student end

	}
	///compute edge point
	virtual point3f compute_edge_point(edge* e) {

		if (b_use_hoppe_rules) {
			//student begin
			return subdiv_face_cutting_triangle::compute_edge_point(e);
			//student end
		}
		//student begin

		float	left = 3.0f/8,
				right = 3.0f/8,
				top = 1.0f/8,
				bottom = 1.0f/8;

		bool border = e->border;
		point3f new_vertex = point3f();
		SnowSim::polygonal_mesh::halfedge* he = e->m_halfedge;


		if (border) {

			////////////////////////////////////////////////////
			// Randstempel f�r Randknoten
			////////////////////////////////////////////////////

			// get border halfedge on the left
			if (!he->inverse->m_face)
				he = he->inverse;



			// odd
			left = 1.0f/2;
			right = 1.0f/2;

			new_vertex = left*(he->next->origin->position)
				+ right*(he->origin->position);

		} else {

			////////////////////////////////////////////////////
			// Stempel f�r Fl�chenknoten
			////////////////////////////////////////////////////

			// odd
			new_vertex = left*(he->origin->position)
				+ right*(he->next->origin->position)
				+ top*(he->prev->origin->position)
				+ bottom*(he->inverse->prev->origin->position);
		}

		return new_vertex;

		//return subdiv_face_cutting_triangle::compute_edge_point(e);
		//student end

	}
	/// computes limit position
	virtual point3f compute_limit_position(vertex *v) {

		//student begin
		if (b_use_hoppe_rules) {
			//student begin
			return subdiv_face_cutting_triangle::compute_vertex_point(v);
			//student end
		}

		int valence = v->valence;
		float	left = 1.0f/6,
				right = 1.0f/6,
				self = 4.0f/6;

		point3f new_vertex = point3f();
		SnowSim::polygonal_mesh::halfedge* he = v->m_halfedge;


		if (v->border) {

			////////////////////////////////////////////////////
			// Randstempel f�r Randknoten
			////////////////////////////////////////////////////

			// get border halfedge on the left
			if (!he->inverse->m_face)
				he = he->inverse;
			else if (he->m_face) {	// our halfedge ain't on the border :/ - go and find one matching
				do {
					he = he->inverse->next;
				} while (!he->m_edge->border);
				if (!he->inverse->m_face)
					he = he->inverse;
			}

			new_vertex = self*(v->position)
				+ left*(he->next->origin->position)
				+ right*(he->origin->position);

		} else {

			////////////////////////////////////////////////////
			// Stempel f�r Fl�chenknoten
			////////////////////////////////////////////////////

			it = lookup_beta.find(valence);
			if (it == lookup_beta.end()) {
				beta = (1.0f/valence)
						* (5.0f/8 - pow(3.0f/8 + 0.25f*cos( (2*M_PI)/valence),2));
				std::cout << "[Beta-Lookup-Table] Added beta of " << beta << " for valence of " << valence << std::endl;
				lookup_beta[valence] = beta;
			} else {
				beta = it->second;
			}

			it = lookup_strange_symbol.find(valence);
			if (it == lookup_strange_symbol.end()) {
				strange_symbol = 1.0f/((3.0f/(8.0f*beta)) + valence);
				std::cout << "[Strange-Symbol-Lookup-Table] Added strange symbol of " << beta << " for valence of " << valence << std::endl;
				lookup_strange_symbol[valence] = strange_symbol;
			} else {
				strange_symbol = it->second;
			}


			self = 1 - valence*strange_symbol;

			new_vertex = self*(v->position);

			for (int i=0; i < valence; i++) {
				he = he->inverse;
				new_vertex = new_vertex + beta*(he->origin->position);
				he = he->next;
			}
		}



		//return subdiv_face_cutting_triangle::compute_vertex_point(v);
		return new_vertex;

		//student end

	}
	/// compute limit normal
	virtual point3f compute_limit_normal(vertex *v) {

		//student begin
		int valence = v->valence;


		point3f along = point3f(0.0f,0.0f,0.0f);
		point3f across = point3f(0.0f,0.0f,0.0f);
		point3f t_c_vector = point3f(0.0f,0.0f,0.0f);
		point3f t_s_vector = point3f(0.0f,0.0f,0.0f);
		SnowSim::polygonal_mesh::halfedge* he = v->m_halfedge;
		SnowSim::polygonal_mesh::halfedge* h_1_1;


		if (v->border) {

			////////////////////////////////////////////////////
			// Normale für Randknoten
			////////////////////////////////////////////////////

			float theta = M_PI / (valence - 1);

			// get border halfedge on the left
			if (!he->inverse->m_face)
				he = he->inverse;
			else if (he->m_face) {	// our halfedge ain't on the border :/ - go and find one matching
				do {
					he = he->inverse->next;
				} while (!he->m_edge->border);
				if (!he->inverse->m_face)
					he = he->inverse;
			}

			along = he->prev->origin->position - he->next->origin->position;	// so h_k-1,1 is the prev-vertex
			h_1_1 = he->next->inverse->prev;	// the halfedge with origin p_1_1 leading to the border

			if (valence == 2) {
				across =  he->next->origin->position
						+ h_1_1->origin->position
						- (2.0f*(v->position));
			} else if (valence == 3) {
				across =  h_1_1->origin->position
						- v->position;
			} else if (valence > 3) {

				for (float i=1; i < (valence - 1) ; i++) {
					across = across + sin(i*theta) * h_1_1->origin->position;
					h_1_1 = h_1_1->prev->inverse->prev;
				}

				across =   2*(1 -cos(theta)) * 	across
						 - sin(theta)
						 * (he->next->origin->position + he->prev->origin->position);
			}

			return cross(across,along);

		} else {

			////////////////////////////////////////////////////
			// Normale f�r Fl�chenknoten
			////////////////////////////////////////////////////

			float	c,s;

			// the c's and s's
			for (float i=0; i < valence ; i++) {
				he = he->prev;
				c = 2.0f*M_PI*i/valence;
				s = sin(c);
				c = cos(c);
				t_c_vector = t_c_vector + c*(he->origin->position);
				t_s_vector = t_s_vector + s*(he->origin->position);
				he = he->inverse;
			}
			return cross(t_c_vector,t_s_vector);

		}
		return point3f(0.0f,0.0f,0.0f);
		//student end
	}

	/// static callback for b_use_limit_positions
	static inline void checkbutton_1_cb(fltk::Widget *w,void *data)  {
		((subdiv_loop*)data)->checkbutton_1_cb();
	}
	/// callback for b_use_limit_positions
	void checkbutton_1_cb() {
		b_use_limit_positions = checkbutton_1->value();
		do_callback();
	}
	/// static callback for b_use_limit_normals
	static inline void checkbutton_2_cb(fltk::Widget *w,void *data)  {
		((subdiv_loop*)data)->checkbutton_2_cb();
	}
	/// callback for b_use_limit_normals
	void checkbutton_2_cb() {
		b_use_limit_normals = checkbutton_2->value();
		do_callback();
	}
	/// static callback for b_use_hoppe_rules
	static inline void checkbutton_3_cb(fltk::Widget *w,void *data)  {
		((subdiv_loop*)data)->checkbutton_3_cb();
	}
	/// callback for b_use_hoppe_rules
	void checkbutton_3_cb() {
		b_use_hoppe_rules = checkbutton_3->value();
		do_callback();
	}
	/// static callback for b_enforce_interpolation
	static inline void checkbutton_4_cb(fltk::Widget *w,void *data)  {
		((subdiv_loop*)data)->checkbutton_4_cb();
	}
	/// callback for b_enforce_interpolation
	void checkbutton_4_cb() {
		b_enforce_interpolation = checkbutton_4->value();
		do_callback();
	}
	/// static callback for b_use_wrong_border_limit
	static inline void checkbutton_5_cb(fltk::Widget *w,void *data)  {
		((subdiv_loop*)data)->checkbutton_5_cb();
	}
	/// callback for b_use_wrong_border_limit
	void checkbutton_5_cb() {
		b_use_wrong_border_limit = checkbutton_5->value();
		do_callback();
	}
	/// static callback for b_use_modified_rules
	static inline void checkbutton_6_cb(fltk::Widget *w,void *data)  {
		((subdiv_loop*)data)->checkbutton_6_cb();
	}
	/// callback for b_use_modified_rules
	void checkbutton_6_cb() {
		b_use_modified_rules = checkbutton_6->value();
		do_callback();
	}

	/// calls the subdiv_step function (subdiv_steps times) and calls compute_limit_position/compute_limit_normal if selected
	virtual void subdivide(SnowSim::polygonal_mesh* mesh, int subdiv_steps)
	{
		if (b_enforce_interpolation) {
			if (debugSteps) {
				std::cout << "------------- enforce Interpolaton  ------- "<< std::endl;
			}
			enforce_interpolation(mesh);
		}

		subdiv_face_cutting_triangle::subdivide(mesh,subdiv_steps);

		std::vector<point3f> tmp_vp;
		std::vector<point3f> tmpn;

		if (b_use_limit_positions) {
			if (debugSteps) {
				std::cout << "------------- compute limit positions  ------- "<< std::endl;
			}
			tmp_vp.resize(mesh->number_vertices);
			vertex *v = mesh->vertex_source;
			while (v!=NULL) {
				tmp_vp[v->index] = compute_limit_position(v);
				v=v->next_vertex;
			}
		}
		if (b_use_limit_normals) {
			if (debugSteps) {
				std::cout << "------------- compute limit normals  ------- "<< std::endl;
			}
			tmpn.resize(mesh->number_vertices);
			vertex *v = mesh->vertex_source;
			while (v!=NULL) {
				tmpn[v->index] = compute_limit_normal(v);
				v=v->next_vertex;
			}
		}
		if (b_use_limit_positions) {
			vertex *v = mesh->vertex_source;
			while (v!=NULL) {
				v->position = tmp_vp[v->index];
				v=v->next_vertex;
			}
		}
		if (b_use_limit_normals) {
			face *f = mesh->face_source;
			while (f!=NULL) {
				point3f n(0.0f,0.0f,0.0f);
				halfedge *h = f->m_halfedge;
				halfedge *hi = h;
				do {
					n+=tmpn[hi->origin->index];
					hi=hi->next;
				} while (hi!=h);
				f->normal = normalize(n);
				f=f->next_face;
			}
		}
	}
	/// update the vertex locations such that loop subdivision surface interpolates the input vertices
	void enforce_interpolation(SnowSim::polygonal_mesh* mesh) {
	//student begin
	//student end
	}

};
