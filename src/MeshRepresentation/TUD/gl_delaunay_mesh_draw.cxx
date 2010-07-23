
#include "gl_delaunay_mesh_draw.h"
#include <cgv/gui/events.h>
#include <cgv/utils/stopwatch.h>
#include <cgv/render/gpu/gl/gl_tools.h>
#include <cgv/render/gpu/gl/gl_format.h>
#include <cgv/gui/fltk/fltk_key_double_control.h>
#include <iostream>

using namespace cgv::gui::fltk;
using namespace cgv::render::gpu::gl;

gl_delaunay_mesh_draw::gl_delaunay_mesh_draw()
{
	show_corner_ds = false;
	show_points = true;
	show_voronoi_vertices = false;
	show_voronoi_edges = false;
	show_delaunay_triangles = true;
	show_vertex_labels = false;
	debug_point_location = false;
	debug_flipable = false;
	debug_trace = false;
	debug_nearest_neighbor = false;
	debug_next_insertion = true;
	set_random_seed = true;
	tm = 0;
	tm_bak = 0;
	s  = 312;
	primitive_scale = 1.0;
	hierarchy_factor = 32;
	sample_shape = tm_type::SQUARE;
	sample_sampling = tm_type::RANDOM_HIGH_RES;
	sample_size = 100;
	sample_shuffle = true;
	add_object(new fltk_key_double_control(
		primitive_scale, 3.0, 0.1, 10.0, '8', '9', false));

	init_mesh(25);
}

void gl_delaunay_mesh_draw::init_mesh(unsigned int n)
{
	if (tm)
		delete tm;
	if (tm_bak)
		delete tm_bak;
	tm_bak = 0;
	if (set_random_seed)
		srand(s);
	tm = new tm_type();
	tm->generate_sample_data_set(n, sample_shape, sample_sampling, sample_shuffle);
	tm->set_hierarchy_factor(hierarchy_factor);
	vi = 0;
	vc = 0;
}

void gl_delaunay_mesh_draw::on_set_receiver()
{
	emit(cgv::gui::SET_EYE_POINT, cgv::gui::event_point(0,0,1));
	emit(cgv::gui::SCALE_EVENT,2.0);
}

const char* gl_delaunay_mesh_draw::get_name() const
{
	return "gl_delaunay_mesh_draw";
}


void gl_delaunay_mesh_draw::draw_edge(unsigned int ci) const
{
	glArrayElement(tm->vi_of_ci(tm->next(ci)));
	glArrayElement(tm->vi_of_ci(tm->prev(ci)));
}

void gl_delaunay_mesh_draw::draw_triangle(unsigned int ci) const
{
	glArrayElement(tm->vi_of_ci(pli.ci));
	glArrayElement(tm->vi_of_ci(tm->next(pli.ci)));
	glArrayElement(tm->vi_of_ci(tm->prev(pli.ci)));
}

void gl_delaunay_mesh_draw::compute_draw_data()
{
	if ( (show_voronoi_vertices || show_voronoi_edges) && tm->get_nr_triangles() > 0) {
		unsigned int T = tm->get_nr_triangles();
		vv.resize(T);
		unsigned int ti;
		for (ti=0; ti<T; ++ti)
			vv[ti] = tm->compute_circum_center(tm->ci_of_ti(ti));
	}
	if (debug_trace && vi < tm->get_nr_vertices() && vi > 0 && vc < tm->get_nr_vertices() && vc != vi-1)
		tm->trace_segment(vc, vi-1, trace_hits);

	if (debug_point_location && tm->get_nr_triangles() > 0) {
		pli = tm->localize_point(tm->p_of_vi(vi),
			tm->ci_of_vi(tm->find_nearest_neighbor(tm->p_of_vi(vi))));
	}
	if (debug_nearest_neighbor && tm->get_nr_triangles() > 0)
		vnn = tm->find_nearest_neighbor(tm->p_of_vi(vi));
}

void gl_delaunay_mesh_draw::clear_draw_data()
{
	vv.clear();
	trace_hits.clear();
	vnn = -1;
}

void gl_delaunay_mesh_draw::draw_text_elements() const
{
	if (show_vertex_labels) {
		glColor3f(0.7f,0.7f,0.7f);
		for (unsigned int vj=0; vj < (vi==0?tm->get_nr_vertices():vi); ++vj) {
			const tm_type::point_type& p0 = tm->p_of_vi(vj);
			set_cursor(p0.x(),p0.y());
			gl_printf("v%d", vj);
		}
	}
}
void gl_delaunay_mesh_draw::draw_point_elements() const
{
	glVertexPointer(2, map_to_gl(gl_traits<coord_type>::type), 0, &(tm->p_of_vi(0).x()));
	glEnableClientState(GL_VERTEX_ARRAY);
	if (debug_next_insertion) {
		glPointSize(2*(float)primitive_scale);
		glColor3f(1.0f, 1.0f, 0.0f);
		glDrawArrays(GL_POINTS, vi, (tm->get_nr_triangles() == 0) ? 3 : 1);
	}
	if (debug_nearest_neighbor && tm->get_nr_triangles() > 0) {
		glPointSize(3*(float)primitive_scale);
		glColor3f(1.0f, 1.0f, 1.0f);
		glDrawArrays(GL_POINTS, vnn, 1);
	}
	if (debug_trace && vi < tm->get_nr_vertices() && vi > 0 && vc < tm->get_nr_vertices() && vc != vi-1) {
		// draw constraint vertices
		glPointSize(2*(float)primitive_scale);
		glColor3f(0.5f, 0.5f, 0.5f);
		glDrawArrays(GL_POINTS, vc, 1);
		glDrawArrays(GL_POINTS, vi-1, 1);
		// draw incident vertices
		glColor3f(0.9f,0.6f,0.3f);
		for (unsigned int i=0; i<trace_hits.size(); ++i)
			if (trace_hits[i].type == tm_type::INCIDENT_VERTEX)
			glDrawArrays(GL_POINTS, trace_hits[i].vi_of_incident_vertex, 1);
	}
	if (show_points) {
		glPointSize(2*(float)primitive_scale);
		glColor3f(1.0f, 0.0f, 0.0f);
		glDrawArrays(GL_POINTS, 0, vi == 0 ? tm->get_nr_vertices() : vi);
	}
	if (show_voronoi_vertices && tm->get_nr_triangles() > 0) {
		glVertexPointer(2, map_to_gl(gl_traits<coord_type>::type), 0, &(vv[0].x()));
		glPointSize(2*(float)primitive_scale);
		glColor3f(0.0f, 1.0f, 0.0f);
		glDrawArrays(GL_POINTS, 0, tm->get_nr_triangles());
	}
	glDisableClientState(GL_VERTEX_ARRAY);
}

void gl_delaunay_mesh_draw::draw_line_elements() const
{
	if (show_corner_ds) {
		glLineWidth(0.5f*(float)primitive_scale);
		glColor3f(1.0f,1.0f,1.0f);
		glBegin(GL_LINES);
		for (unsigned int ci=0; ci<tm->get_nr_corners(); ++ci) {
			if (tm->is_opposite_to_border(ci))
				continue;
			const tm_type::point_type& p0 = tm->p_of_vi(tm->vi_of_ci(ci));
			const tm_type::point_type& p1 = tm->p_of_vi(tm->vi_of_ci(tm->next(ci)));
			const tm_type::point_type& p2 = tm->p_of_vi(tm->vi_of_ci(tm->prev(ci)));
			const tm_type::point_type& p3 = tm->p_of_vi(tm->vi_of_ci(tm->inv(ci)));
			tm_type::point_type c = tm_type::point_type((coord_type)(0.8*p0.x()+0.1*p1.x()+0.1*p2.x()),
													(coord_type)(0.8*p0.y()+0.1*p1.y()+0.1*p2.y()));
			tm_type::point_type d = tm_type::point_type((coord_type)(0.8*c.x()+0.2*p3.x()),
				                           (coord_type)(0.8*c.y()+0.2*p3.y()));
			glVertex2d(p0.x(),p0.y());
			glVertex2d(c.x(),c.y());
			glVertex2d(c.x(),c.y());
			glVertex2d(d.x(),d.y());
		}
		glEnd();
	}
	glLineWidth((float)primitive_scale);
	if (show_voronoi_edges && tm->get_nr_triangles() > 0) {
		unsigned int T = tm->get_nr_triangles();
		glColor3f(0.0f, 1.0f, 0.5f);
		glVertexPointer(2, map_to_gl(gl_traits<coord_type>::type), 0, &(vv[0].x()));
		glEnableClientState(GL_VERTEX_ARRAY);
		glBegin(GL_LINES);
		for (unsigned int ci=0; ci<tm->get_nr_corners(); ++ci)
			if (!tm->is_opposite_to_border(ci)) {
				unsigned int cj = tm->inv(ci);
				if (cj < ci)
					continue;
				glArrayElement(tm->ti_of_ci(ci));
				glArrayElement(tm->ti_of_ci(cj));
			}
		glEnd();
		glDisableClientState(GL_VERTEX_ARRAY);
		glBegin(GL_LINES);
		for (unsigned int ci=0; ci<tm->get_nr_corners(); ++ci)
			if (tm->is_opposite_to_border(ci)) {
				glVertex2d(vv[tm->ti_of_ci(ci)].x(),vv[tm->ti_of_ci(ci)].y());
				const tm_type::point_type& p0 = tm->p_of_vi(tm->vi_of_ci(tm->next(ci)));
				const tm_type::point_type& p1 = tm->p_of_vi(tm->vi_of_ci(tm->prev(ci)));
				glVertex4d(p1.y()-p0.y(),p0.x()-p1.x(),0,0);
			}
		glEnd();
	}
	glVertexPointer(2, map_to_gl(gl_traits<coord_type>::type), 0, &(tm->p_of_vi(0).x()));
	glEnableClientState(GL_VERTEX_ARRAY);

	if (debug_trace && vi < tm->get_nr_vertices() && vi > 0 && vc < tm->get_nr_vertices() && vc != vi-1) {
		// draw constraint edge
		glColor3f(0.5f,0.5f,0.5f);
		glBegin(GL_LINES);
		glArrayElement(vi-1);
		glArrayElement(vc);
		glEnd();

		// draw intersected edges
		unsigned int i;
		glColor3f(0.3f,0.6f,0.7f);
		glBegin(GL_LINES);
		for (i=0; i<trace_hits.size(); ++i)
			if (trace_hits[i].type == tm_type::INTERSECTED_EDGE)
				draw_edge(trace_hits[i].ci_of_intersected_edge);
		glEnd();

		// draw incident edges
		glColor3f(0.9f,0.6f,0.3f);
		glBegin(GL_LINES);
		for (i=0; i<trace_hits.size(); ++i)
			if (trace_hits[i].type == tm_type::INCIDENT_EDGE)
				draw_edge(trace_hits[i].ci_of_incident_edge);
		glEnd();
	}
	if (debug_flipable) {
		// draw all not flipable edges
		glColor3f(0.7f, 0.6f, 0.0f);
		glBegin(GL_LINES);
		for (unsigned int ci=0; ci<tm->get_nr_corners(); ++ci) {
			if (!tm->is_opposite_to_border(ci) && tm->inv(ci) < ci)
				continue;
			if (tm->is_edge_flip_valid(ci))
				continue;
			glArrayElement(tm->vi_of_ci(tm->next(ci)));
			glArrayElement(tm->vi_of_ci(tm->prev(ci)));
		}
		glEnd();
	}
	if (debug_point_location && tm->get_nr_triangles() > 0)
		if (pli.is_outside) {
			glColor3f(0.0f,1.0f,1.0f);
			glBegin(GL_LINES);
				draw_edge(pli.ci);
			glEnd();
		}
		else {
			glColor3f(1.0f,0.0f,1.0f);
			glBegin(GL_TRIANGLES);
				draw_triangle(pli.ci);
			glEnd();
		}

	if (show_delaunay_triangles && tm->get_nr_triangles() > 0) {
		glColor3f(0.0f,0.0f,1.0f);
		glBegin(GL_TRIANGLES);
		for (unsigned int ci=0; ci<tm->get_nr_corners(); ++ci)
			glArrayElement(tm->vi_of_ci(ci));
		glEnd();
	}
	glDisableClientState(GL_VERTEX_ARRAY);
}

void gl_delaunay_mesh_draw::draw()
{
	if (!tm || tm->get_nr_vertices() == 0)
		return;

	// configure gl
	glPolygonMode(GL_FRONT, GL_LINE);
	glDisable(GL_CULL_FACE);
	glEnable(GL_POINT_SMOOTH);
	glDisable(GL_LIGHTING);

	// compute necessary data
	compute_draw_data();

	// draw all text first
	draw_text_elements();

	// second draw all points
	draw_point_elements();

	// finally draw all lines
	draw_line_elements();

	clear_draw_data();

	glEnable(GL_LIGHTING);
	glPolygonMode(GL_FRONT, GL_FILL);
	glDisable(GL_POINT_SMOOTH);
	glPointSize(1.0f);
}

void gl_delaunay_mesh_draw::measure_time(unsigned int debug_point)
{
	init_mesh(sample_size);
	unsigned int n = tm->get_nr_vertices();
	unsigned int m = n/20;
	if (m < 1000)
		m = 1000;
	if (debug_point != -1)
		n = debug_point;
	if (n == 0)
		return;
	std::cout << "test delaunay runtime" << std::endl;
	{
		cgv::utils::stopwatch w;
		tm->add_triangle(0,1,2);
		for (vi = 3; vi < n; ++vi) {
			if (vi%m == 0)
				std::cout << vi << std::endl;
			tm->insert_vertex(vi);
		}
	}
	vi = 0;
}

void gl_delaunay_mesh_draw::show_stats(int& y) const
{
	const char* shape_strings[]    = { "square", "triangle", "circle" };
	const char* sampling_strings[] = { "random", "random high res", "uniform" };
	const char* bool_strings[] = { "false", "true" };

	gl_printf(10,y+=20,"Delaunay Mesh Drawable:");
	gl_printf(20,y+=20,"   sample: n=%d <N/n>, sampling=%s <s>, shape=%s <S>, shuffle=%s <u>, set seed=%s <0>",
		sample_size, sampling_strings[sample_sampling], shape_strings[sample_shape], bool_strings[sample_shuffle], bool_strings[set_random_seed]);
	gl_printf(20,y+=20,"   |P|=%d, |T|=%d, vi=%d, vc=%d <c/C>, s=%d <R/r>, h=%d <H/h>",
		tm->get_nr_vertices(),
		tm->get_nr_triangles(), vi, vc, s, hierarchy_factor);
}

void gl_delaunay_mesh_draw::show_help(int& y) const
{
	const char* toggle_strings[] = { "on ", "off" };
	gl_printf(10,y+=20,"Delaunay Mesh Drawable:");
	gl_printf(20,y+=20,"  toggle: vertex <l>abels %s, points %s <d>, delaunay %s <D>, c<o>rners %s",
		toggle_strings[show_vertex_labels], toggle_strings[show_points], toggle_strings[show_delaunay_triangles], toggle_strings[show_corner_ds]);
	gl_printf(20,y+=20,"          voronoi points %s <v>/diagram %s <V>, ",
		toggle_strings[show_voronoi_vertices], toggle_strings[show_voronoi_edges]);
	gl_printf(20,y+=20,"  debug:  next insertion %s <1>, point location %s <2>, nearest neighbor %s <3>",
		toggle_strings[debug_next_insertion], toggle_strings[debug_point_location], toggle_strings[debug_nearest_neighbor]);
	gl_printf(20,y+=20,"          flipable edges %s <4>, trace %s <5>",
		toggle_strings[debug_flipable], toggle_strings[debug_trace]);
	gl_printf(20,y+=20,"  point and line scale=%f <8/9>", primitive_scale);
	gl_printf(20,y+=20,"  clear triangles <x> /all <X>, init <i[small]/I>, add <[t]riangle/[p]oint>, skip <P>oint");
	gl_printf(20,y+=20,"  measure <m/M[reinit to small]");
}

gl_delaunay_mesh_draw::Result gl_delaunay_mesh_draw::handle_event(int event)
{
	if (event == fltk::KEY) {
		switch (fltk::event_key()) {
		case 'f' :
			if (tm_bak) {
				delete tm;
				tm = tm_bak;
				tm_bak = 0;
			}
			else {
				// flip constraints
				std::vector<tm_type::trace_hit> trace_hits;
				tm->trace_segment(vc, vi-1, trace_hits);
				tm_bak = new tm_type(*tm);
				std::vector<unsigned int> intersected_edge_cis;
				for (unsigned int i=0; i<trace_hits.size(); ++i)
					if (trace_hits[i].type == tm_type::INTERSECTED_EDGE)
						intersected_edge_cis.push_back(trace_hits[i].ci_of_intersected_edge);
					else
						break;
				if (!intersected_edge_cis.empty())
					tm->flip_edges_to_insert_segment(vc, vi-1, intersected_edge_cis);
			}
			break;
		case 'n' :
			if (fltk::event_state() == fltk::SHIFT) {
				if (sample_size > 10)
					sample_size /= 10;
			}
			else
				sample_size *= 10;
			return HANDLED_AND_REDRAW;
		case 's' :
			if (fltk::event_state() == fltk::SHIFT) {
				if (sample_shape == tm_type::CIRCLE)
					sample_shape = tm_type::SQUARE;
				else
					++(int&)sample_shape;
			}
			else {
				if (sample_sampling == tm_type::UNIFORM)
					sample_sampling = tm_type::RANDOM;
				else
					++(int&)sample_sampling;
			}
			return HANDLED_AND_REDRAW;
		case 'u' :
			sample_shuffle = !sample_shuffle;
			return HANDLED_AND_REDRAW;
		case '0' :
			set_random_seed = !set_random_seed;
			return HANDLED_AND_REDRAW;
		case 'd' :
			if (fltk::event_state() == fltk::SHIFT)
				show_delaunay_triangles = !show_delaunay_triangles;
			else
				show_points = !show_points;
			return HANDLED_AND_REDRAW;
		case 'l' :
			show_vertex_labels = !show_vertex_labels;
			return HANDLED_AND_REDRAW;
		case 'v' :
			if (fltk::event_state() == fltk::SHIFT)
				show_voronoi_edges = !show_voronoi_edges;
			else
				show_voronoi_vertices = !show_voronoi_vertices;
			return HANDLED_AND_REDRAW;
		case 'o' :
			show_corner_ds = !show_corner_ds;
			return HANDLED_AND_REDRAW;
		case 'c' :
			if (tm_bak) {
				delete tm;
				tm = tm_bak;
				tm_bak = 0;
			}
			if (fltk::event_state() == fltk::SHIFT) {
				if (vc > 0)
					--vc;
			}
			else {
				if (vc+1 < vi)
					++vc;
			}
			return HANDLED_AND_REDRAW;
		case 'x' :
			if (fltk::event_state() == fltk::SHIFT)
				tm->clear();
			else
				tm->clear_triangles();
			return HANDLED_AND_REDRAW;
		case 'i' :
			if (fltk::event_state() == fltk::SHIFT)
				init_mesh(sample_size);
			else
				init_mesh(25);
			return HANDLED_AND_REDRAW;
		case 't' :
			{
				unsigned int N = tm->get_nr_vertices();
				tm->add_triangle(vi, (vi+1)%N, (vi+2)%N);
				vi = (vi+3)%N;
				//tm->is_ci_of_vi_consistent(vi);
				return HANDLED_AND_REDRAW;
			}
		case 'p' :
			if (tm_bak) {
				delete tm;
				tm = tm_bak;
				tm_bak = 0;
			}
			if (fltk::event_state() != fltk::SHIFT)
				tm->insert_vertex(vi);
			if (++vi >= tm->get_nr_vertices())
				vi = 0;
			//tm->is_ci_of_vi_consistent(vi);
			return HANDLED_AND_REDRAW;
		case 'r' :
			if (fltk::event_state() == fltk::SHIFT)
				--s;
			else
				++s;
			return HANDLED_AND_REDRAW;
		case 'h' :
			if (fltk::event_state() == fltk::SHIFT) {
				if (hierarchy_factor > 2)
					--hierarchy_factor;
			}
			else
				++hierarchy_factor;
			return HANDLED_AND_REDRAW;
		case 'm' :
			measure_time();
			if (fltk::event_state() == fltk::SHIFT)
				init_mesh(25);
			return HANDLED_AND_REDRAW;
		case '1' :
			debug_next_insertion = !debug_next_insertion;
			return HANDLED_AND_REDRAW;
		case '2' :
			debug_point_location = !debug_point_location;
			return HANDLED_AND_REDRAW;
		case '3' :
			debug_nearest_neighbor = !debug_nearest_neighbor;
			return HANDLED_AND_REDRAW;
		case '4' :
			debug_flipable = !debug_flipable;
			return HANDLED_AND_REDRAW;
		case '5' :
			debug_trace = !debug_trace;
			return HANDLED_AND_REDRAW;
		}
	}
	return named_object_vector::handle_event(event);
}
