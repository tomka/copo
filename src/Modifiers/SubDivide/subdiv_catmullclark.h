#pragma once

#include "subdiv_face_cutting_quad.h"

/** 
* Implementation of Catmull-Clark subdivision. 
*/
class subdiv_catmullclark : public subdiv_face_cutting_quad {
public:
	/// constructor
	subdiv_catmullclark(int x, int y, int width, int height) : subdiv_face_cutting_quad(x,y,width,height) {
	}
	/// destructor
	~subdiv_catmullclark() {
	}
	/// get name of scheme
	virtual const char* get_name() const {
		return "Catmull-Clark";
	}
	/// compute vertex point
	virtual point3f compute_vertex_point(vertex* v) 
	{
		//student begin
		
		return subdiv_face_cutting_quad::compute_vertex_point(v);
		//student end
	}
	/// compute edge point
	virtual point3f compute_edge_point(edge* e) 
	{
		//student begin
		

		return subdiv_face_cutting_quad::compute_edge_point(e);
		//student end

	}
	/// compute face point
	virtual point3f compute_face_point(face* f) 
	{
		//student begin

		return subdiv_face_cutting_quad::compute_face_point(f);
		//student end
	}
};