#pragma once
/*
#include "delaunay_mesh_with_hierarchy.h"
#include <cgv/gui/fltk/event_source.h>
#include <cgv/gui/fltk/named_object_vector.h>

class gl_delaunay_mesh_draw : public cgv::gui::fltk::named_object_vector, public cgv::gui::fltk::event_source
{
public:
	/// the used coordinate type
	typedef double coord_type;
	/// the type of the triangle mesh
	typedef delaunay_mesh_with_hierarchy<> tm_type;
protected:
	/// whether to show the corner data structure
	bool show_corner_ds;
	/// whether to show original points
	bool show_points;
	/// whether to show the voronoi vertices
	bool show_voronoi_vertices;
	/// whether to show the voronoi edges
	bool show_voronoi_edges;
	/// whether to show the delaunay triangles
	bool show_delaunay_triangles;
	/// whether to label the vertices
	bool show_vertex_labels;
	///
	bool debug_next_insertion;
	bool debug_nearest_neighbor;
	bool debug_point_location;
	bool debug_flipable;
	bool debug_trace;
	///
	bool set_random_seed;
	///
	double primitive_scale;
	///
	unsigned int hierarchy_factor;
	///
	tm_type::ShapeType sample_shape;
	///
	tm_type::SamplingType sample_sampling;
	///
	unsigned int sample_size;
	///
	bool sample_shuffle;
	/// triangle mesh
	tm_type *tm, *tm_bak;
	/// next vertex index
	unsigned int vi;
	/// constraint point index
	unsigned int vc;
	/// currently used random seed
	unsigned int s;
private:
	// data that is used in several places during drawing
	std::vector<tm_type::trace_hit> trace_hits;
	std::vector<tm_type::point_type> vv;
	tm_type::point_location_info pli;
	unsigned int vnn;
public:
	/// construct with initialized mesh
	gl_delaunay_mesh_draw();
	/// initialize mesh to random point set
	void init_mesh(unsigned int n);
	/// measure the time to compute the delaunay triangulation of n random points
	void measure_time(unsigned int debug_point = -1);
	/// overload methods of drawable
	const char* get_name() const;
	void show_stats(int& y) const;
	void draw_edge(unsigned int ci) const;
	void draw_triangle(unsigned int ci) const;
	void compute_draw_data();
	void draw_text_elements() const;
	void draw_point_elements() const;
	void draw_line_elements() const;
	void clear_draw_data();
	void draw();
	void show_help(int& y) const;
	Result handle_event(int event);
	void on_set_receiver();
};
*/
