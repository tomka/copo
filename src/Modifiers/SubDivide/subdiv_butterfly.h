#pragma once

#include "subdiv_face_cutting_triangle.h"

/** 
* Implementation of butterfly subdivision. 
*/
class subdiv_butterfly : public subdiv_face_cutting_triangle {
private:
	/// whether the choosen rules are printed out
	bool b_debug_rules;
	/// checkbutton for b_debug_rules
	fltk::CheckButton *checkbutton_1;
	/// whether to use irregular interior rule
	bool b_use_irregular_interior_rule;
	/// whether to use irregular border rule
	bool b_use_irregular_border_rule;
	/// whether to use regular border rule 1
	bool b_use_regular_border_rule_1;
	/// whether to use regular border rule 2
	bool b_use_regular_border_rule_2;
	/// whether to use regular border rule 3
	bool b_use_regular_border_rule_3;
	/// whether to use standard rule
	bool b_use_standard_rule;
	/// whether to use border rule
	bool b_use_border_rule;
	/// checkbutton for b_use_irregular_interior_rule
	fltk::CheckButton *checkbutton_2;
	/// checkbutton for b_use_irregular_border_rule
	fltk::CheckButton *checkbutton_3;
	/// checkbutton for b_use_regular_border_rule_1
	fltk::CheckButton *checkbutton_4;
	/// checkbutton for b_use_regular_border_rule_2
	fltk::CheckButton *checkbutton_5;
	/// checkbutton for b_use_regular_border_rule_3
	fltk::CheckButton *checkbutton_6;
	/// checkbutton for b_use_standard_rule
	fltk::CheckButton *checkbutton_7;
	/// checkbutton for b_use_border_rule
	fltk::CheckButton *checkbutton_8;
private:

public:
	/// constructor
	subdiv_butterfly(int x, int y, int width, int height) : subdiv_face_cutting_triangle(x,y,width,height) {
		b_debug_rules = false;
		checkbutton_1 = new fltk::CheckButton(0,20,20,20,"debug rules");
		checkbutton_1->value(b_debug_rules);
		checkbutton_1->callback(checkbutton_1_cb,this);
		b_use_irregular_interior_rule = true;
		checkbutton_2 = new fltk::CheckButton(0,140,20,20,"use irregular interior rule");
		checkbutton_2->value(b_use_irregular_interior_rule);
		checkbutton_2->callback(checkbutton_2_cb,this);
		b_use_irregular_border_rule = true;
		checkbutton_3 = new fltk::CheckButton(0,160,20,20,"use irregular border rule");
		checkbutton_3->value(b_use_irregular_border_rule);
		checkbutton_3->callback(checkbutton_3_cb,this);
		b_use_regular_border_rule_1 = true;
		checkbutton_4 = new fltk::CheckButton(0,80,20,20,"use regular border rule 1");
		checkbutton_4->value(b_use_regular_border_rule_1);
		checkbutton_4->callback(checkbutton_4_cb,this);
		b_use_regular_border_rule_2 = true;
		checkbutton_5 = new fltk::CheckButton(0,100,20,20,"use regular border rule 2");
		checkbutton_5->value(b_use_regular_border_rule_2);
		checkbutton_5->callback(checkbutton_5_cb,this);
		b_use_regular_border_rule_3 = true;
		checkbutton_6 = new fltk::CheckButton(0,120,20,20,"use regular border rule 3");
		checkbutton_6->value(b_use_regular_border_rule_3);
		checkbutton_6->callback(checkbutton_6_cb,this);
		b_use_standard_rule = true;
		checkbutton_7 = new fltk::CheckButton(0,40,20,20,"use standard rule");
		checkbutton_7->value(b_use_standard_rule);
		checkbutton_7->callback(checkbutton_7_cb,this);
		b_use_border_rule = true;
		checkbutton_8 = new fltk::CheckButton(0,60,20,20,"use border rule");
		checkbutton_8->value(b_use_border_rule);
		checkbutton_8->callback(checkbutton_8_cb,this);
		add(checkbutton_1);
		add(checkbutton_2);
		add(checkbutton_3);
		add(checkbutton_4);
		add(checkbutton_5);
		add(checkbutton_6);
		add(checkbutton_7);
		add(checkbutton_8);
	}
	/// destructor
	~subdiv_butterfly() {
		delete checkbutton_1;
		delete checkbutton_2;
		delete checkbutton_3;
		delete checkbutton_4;
		delete checkbutton_5;
		delete checkbutton_6;
		delete checkbutton_7;
		delete checkbutton_8;
	}
	/// get name for subdivision scheme
	virtual const char* get_name() const {
		return "Butterfly";
	}
	

	/// returns the edge point in face corner cutting
	virtual point3f compute_edge_point(edge* e) 
	{
		//student begin
		int valence = e->m_halfedge->origin->valence; 
		bool border = e->border; 

		int upper_right = 1, upper_left = 1, lower_right = 1, lower_left = 1;
		point3f new_vertex = point3f();
		float	w_left = 0.5f,
				w_right = 0.5f,
				w_top = 1.0f/8,
				w_down = 1.0f/8,
				w_upper_left = (-1.0f/16),
				w_upper_right = (-1.0f/16),
				w_lower_left = (-1.0f/16),
				w_lower_right = (-1.0f/16);
		
		if (e->m_halfedge->m_edge->border) {

			// Check if origin-vertex is regular
			if (valence != 4) { 
			   return subdiv_face_cutting_triangle::compute_edge_point(e);
			}

			////////////////////////////////////////////////////
			// 4-Punkt-Regel für Randknoten
			////////////////////////////////////////////////////

			new_vertex = (9.0f/16)*(e->m_halfedge->origin->position);	// left vertex
			new_vertex = new_vertex + (9.0f/16)*(e->m_halfedge->next->origin->position);	// right vertex

			// most right vertex
			if (e->m_halfedge->next->origin->valence != 4)
				return subdiv_face_cutting_triangle::compute_edge_point(e);
			
			new_vertex = new_vertex + (-1.0f/16)*(e->m_halfedge->next->inverse->next->inverse->next->next->origin->position);

			// most left vertex
			new_vertex = new_vertex + (-1.0f/16)*(e->m_halfedge->prev->inverse->prev->inverse->prev->origin->position);

		} else {
			
			// Check if origin-vertex is regular
			if (valence != 6 || e->m_halfedge->next->origin->valence != 6) { 
			   return subdiv_face_cutting_triangle::compute_edge_point(e);
			}
		
			////////////////////////////////////////////////////
			// Reguläre Randstempel - Which faces are missing?
			////////////////////////////////////////////////////

			// "upper right"
			if (e->m_halfedge->next->m_edge->border) // the face exists not
				upper_right = 0;
			// "upper left"
			if (e->m_halfedge->prev->m_edge->border) // the face exists not
				upper_left = 0;
			// "lower left"
			if (e->m_halfedge->inverse->next->m_edge->border) // the face exists not
				lower_left = 0;
			// "lower right"
			if (e->m_halfedge->inverse->prev->m_edge->border) // the face exists not
				lower_right = 0;

			////////////////////////////////////////////////
			// Reguläre Randstempel - Fall 1 (one missing)
			////////////////////////////////////////////////
			
			// only upper right is missing
			if (!upper_right && upper_left && lower_left && lower_right) {
				w_left = 5.0f/8;
				w_right =  3.0f/8;
				w_top = 1.0f/16;
				w_down = 3.0f/16;
				w_lower_left = -1.0f/8;
				w_lower_right = -1.0f/16;
				w_upper_left = -1.0f/16;
			}
			// only upper left is missing
			if (!upper_left && upper_right && lower_left && lower_right) {
				w_right = 5.0f/8;
				w_left =  3.0f/8;
				w_top = 1.0f/16;
				w_down = 3.0f/16;
				w_lower_left = -1.0f/16;
				w_lower_right = -1.0f/8;
				w_upper_right = -1.0f/16;
			}
			// only lower left is missing
			if (upper_left && upper_right && !lower_left && lower_right) {
				w_right = 5.0f/8;
				w_left =  3.0f/8;
				w_top = 3.0f/16;
				w_down = 1.0f/16;
				w_upper_left = -1.0f/16;
				w_upper_right = -1.0f/8;
				w_lower_right = -1.0f/16;
			}
			// only lower right is missing
			if (upper_right && upper_left && lower_left && !lower_right) {
				w_left = 5.0f/8;
				w_right =  3.0f/8;
				w_top = 3.0f/16;
				w_down = 1.0f/16;
				w_upper_left = -1.0f/8;
				w_upper_right = -1.0f/16;
				w_lower_left = -1.0f/16;
			}
				
			////////////////////////////////////////////////
			// Reguläre Randstempel - Fall 2 (Dreieck)
			////////////////////////////////////////////////
			
			// only upper left and upper right are missing
			if (!upper_right && !upper_left && lower_left && lower_right) {
				w_right = 1.0f/2;
				w_left =  1.0f/2;
				w_top = 0;
				w_down = 1.0f/4;
				w_lower_left = -1.0f/8;
				w_lower_right = -1.0f/8;
			}
			// only lower left and lower right are missing
			if (upper_right && upper_left && !lower_left && !lower_right) {
				w_right = 1.0f/2;
				w_left =  1.0f/2;
				w_top = 1.0f/4;
				w_down = 0;
				w_upper_left = -1.0f/8;
				w_upper_right = -1.0f/8;
			}

			////////////////////////////////////////////////
			// Reguläre Randstempel - Fall 3 (Stripe)
			////////////////////////////////////////////////
			
			// only upper left and lower right are missing
			if (upper_right && !upper_left && lower_left && !lower_right) {
				w_right = 1.0f/2;
				w_left =  1.0f/2;
				w_top = 0;
				w_down = 0;
				w_lower_left = 0;
				w_upper_right = 0;
			}
			// only lower left and upper right are missing
			if (!upper_right && upper_left && !lower_left && lower_right) {
				w_right = 1.0f/2;
				w_left =  1.0f/2;
				w_top = 0;
				w_down = 0;
				w_upper_left = 0;
				w_lower_right = 0;
			}
		
			
			////////////////////////////////////////////////
			// Calculate new Vertex
			////////////////////////////////////////////////

			//std::cout << upper_left << upper_right << lower_left << lower_right << std::endl;

			new_vertex = w_left*(e->m_halfedge->origin->position);	// left vertex
			new_vertex = new_vertex + w_right*(e->m_halfedge->inverse->origin->position);	// right vertex

			new_vertex = new_vertex + w_top*(e->m_halfedge->prev->origin->position);	//"upper center" vertex
			new_vertex = new_vertex + w_down*(e->m_halfedge->inverse->prev->origin->position);	//"lower center" vertex
			
			if (upper_right) { // "upper right"
				new_vertex = new_vertex + w_upper_right*(e->m_halfedge->next->inverse->
				prev->origin->position);
			}
			if (upper_left) { // "upper left"
				new_vertex = new_vertex + w_upper_left*(e->m_halfedge->prev->
				inverse->prev->origin->position);
			}
			if (lower_left) { // "lower left"
				new_vertex = new_vertex + w_lower_left*(e->m_halfedge->inverse->next->inverse->
				prev->origin->position);
			}
			if (lower_right) { // "lower right"
				new_vertex = new_vertex + w_lower_right*(e->m_halfedge->inverse->next->
				next->inverse->prev->origin->position);
			}
		}

		return new_vertex;
		//student end

	}

	point3f move_vertex(point3f &base_vertex, point3f &direction_vertex, float amount) {
		
		return point3f(0.0f,0.0f,0.0f);
	}

	/// static callback for b_debug_rules checkbutton
	static inline void checkbutton_1_cb(fltk::Widget *w,void *data)  {
		((subdiv_butterfly*)data)->checkbutton_1_cb();
	}
	/// callback for b_debug_rules checkbutton
	void checkbutton_1_cb() {
		b_debug_rules = checkbutton_1->value();
		do_callback();
	}
	/// static callback for b_use_irregular_interior_rule checkbutton
	static inline void checkbutton_2_cb(fltk::Widget *w,void *data)  {
		((subdiv_butterfly*)data)->checkbutton_2_cb();
	}
	/// callback for b_use_irregular_interior_rule checkbutton
	void checkbutton_2_cb() {
		b_use_irregular_interior_rule = checkbutton_2->value();
		do_callback();
	}
	/// static callback for b_use_irregular_border_rule checkbutton
	static inline void checkbutton_3_cb(fltk::Widget *w,void *data)  {
		((subdiv_butterfly*)data)->checkbutton_3_cb();
	}
	/// callback for b_use_irregular_border_rule checkbutton
	void checkbutton_3_cb() {
		b_use_irregular_border_rule = checkbutton_3->value();
		do_callback();
	}
	/// static callback for b_use_regular_border_rule_1 checkbutton
	static inline void checkbutton_4_cb(fltk::Widget *w,void *data)  {
		((subdiv_butterfly*)data)->checkbutton_4_cb();
	}
	/// callback for b_use_regular_border_rule_1 checkbutton
	void checkbutton_4_cb() {
		b_use_regular_border_rule_1 = checkbutton_4->value();
		do_callback();
	}
	/// static callback for b_use_regular_border_rule_2 checkbutton
	static inline void checkbutton_5_cb(fltk::Widget *w,void *data)  {
		((subdiv_butterfly*)data)->checkbutton_5_cb();
	}
	/// callback for b_use_regular_border_rule_2 checkbutton
	void checkbutton_5_cb() {
		b_use_regular_border_rule_2 = checkbutton_5->value();
		do_callback();
	}
	/// static callback for b_use_regular_border_rule_3 checkbutton
	static inline void checkbutton_6_cb(fltk::Widget *w,void *data)  {
		((subdiv_butterfly*)data)->checkbutton_6_cb();
	}
	/// callback for b_use_regular_border_rule_3 checkbutton
	void checkbutton_6_cb() {
		b_use_regular_border_rule_3 = checkbutton_6->value();
		do_callback();
	}
	/// static callback for b_use_standard_rule checkbutton
	static inline void checkbutton_7_cb(fltk::Widget *w,void *data)  {
		((subdiv_butterfly*)data)->checkbutton_7_cb();
	}
	/// callback for b_use_standard_rule checkbutton
	void checkbutton_7_cb() {
		b_use_standard_rule = checkbutton_7->value();
		do_callback();
	}
	/// static callback for b_use_border_rule checkbutton
	static inline void checkbutton_8_cb(fltk::Widget *w,void *data)  {
		((subdiv_butterfly*)data)->checkbutton_8_cb();
	}
	/// callback for b_use_border_rule checkbutton
	void checkbutton_8_cb() {
		b_use_border_rule = checkbutton_8->value();
		do_callback();
	}
};