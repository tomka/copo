#pragma once

#ifdef WIN32
#include <windows.h>
#endif

#include <fltk/gl.h>
#include "polygonal_mesh.h"
#include <fltk/Group.h>
#include <fltk/CheckButton.h>
#include "objects.h"

/**
* The Drawable class is responsible for drawing the mesh.
*/
namespace SnowSim {
class polygonal_mesh_drawable : public fltk::Group {
protected:
	/// halfedge
	typedef SnowSim::polygonal_mesh::halfedge halfedge;
	/// edge
	typedef SnowSim::polygonal_mesh::edge edge;
	/// vertex
	typedef SnowSim::polygonal_mesh::vertex vertex;
	/// face
	typedef SnowSim::polygonal_mesh::face face;
	/// whether render normals
	bool b_render_normals;
	/// wheter render faces
	bool b_render_faces;
	/// whether render vertices
	bool b_render_vertices;
	/// whether render edges
	bool b_render_edges;
	/// color of vertices
	point3f vertex_color;
	/// color of edges
	point3f edge_color;
	/// color of normals
	point3f normal_color;
	/// color of marked vertices
	point3f marked_color;
	/// color of faces
	point3f face_color;
	/// color of the marked vertex
	point3f marked_vertex_color;
	/// size of vertices
	float vertex_size;
	/// size of border vertices
	float border_vertex_size;
	/// width of edges
	float edge_width;
	/// width of normals
	float normal_width;
	/// width of border edges
	float border_wdge_width;
	/// determines the highlight of selected vertices and edges, value between 0.0f and 100.0f
	float select_offset;
	/// whether mesh is visible
	bool b_visible;
	/// checkbutton for b_render_normals
	fltk::CheckButton *checkbutton_1;
	/// checkbutton for b_visible
	fltk::CheckButton *checkbutton_2;
	/// checkbutton for b_render_vertices
	fltk::CheckButton *checkbutton_3;
	/// checkbutton for b_render_edges
	fltk::CheckButton *checkbutton_4;
	/// checkbutton for b_render_faces
	fltk::CheckButton *checkbutton_5;
public:
	/// the mesh that has to be drawn
	SnowSim::polygonal_mesh* mesh;
	/// the currently selected edge
	edge *selected_edge;
	/// the currently selected vertex
	vertex *selected_vertex;
	/// the currently selected face
	face *selected_face;
	/// marked Vertex
	vertex *marked_vertex;
	/// vertex drag mode
	Vertex_Drag_Mode vertex_drag_mode;

	bool orthographic;
public:
	/// contructor
	polygonal_mesh_drawable(SnowSim::polygonal_mesh* mesh, int x, int y, int width, int height, const char *name = "drawable")
		: fltk::Group(x,y,width,height,name,true) {
		this->mesh=mesh;

		orthographic = false;
		b_render_faces = true;
		b_render_edges = true;
		b_render_vertices = true;
		b_visible = true;
		this->b_render_normals = false;
		vertex_color = 0.7f * green_color;
		edge_color = 0.8f * green_color;
		face_color = 0.8f * yellow_color;
		marked_vertex_color = 0.8f*blue_color;
		normal_color = red_color;
		select_offset = 70.0f;
		marked_color = red_color;
		selected_edge = NULL;
		selected_vertex = NULL;
		selected_face = NULL;
		marked_vertex = NULL;
		vertex_size = 5.0f;
		border_vertex_size = 8.0f;
		edge_width = 1.0f;
		normal_width = 1.0f;
		border_wdge_width = 1.0f;
		checkbutton_1 = new fltk::CheckButton(0,80,20,20,"render normals");
		checkbutton_1->value(b_render_normals);
		checkbutton_1->callback(checkbutton_1_cb,this);
		checkbutton_2 = new fltk::CheckButton(0,0,20,20,"visible");
		checkbutton_2->value(b_visible);
		checkbutton_2->callback(checkbutton_2_cb,this);
		checkbutton_3 = new fltk::CheckButton(0,20,20,20,"render vertices");
		checkbutton_3->value(b_render_vertices);
		checkbutton_3->callback(checkbutton_3_cb,this);
		checkbutton_4 = new fltk::CheckButton(0,40,20,20,"render edges");
		checkbutton_4->value(b_render_edges);
		checkbutton_4->callback(checkbutton_4_cb,this);
		checkbutton_5 = new fltk::CheckButton(0,60,20,20,"render faces");
		checkbutton_5->value(b_render_faces);
		checkbutton_5->callback(checkbutton_5_cb,this);
		vertex_drag_mode = NO;
		end();
	}
	/// destructor
	~polygonal_mesh_drawable() {
		delete mesh;
		delete checkbutton_1;
		delete checkbutton_2;
		delete checkbutton_3;
		delete checkbutton_4;
		delete checkbutton_5;
	}
	/// render function
	void render() {
		if ((!mesh) || (!b_visible)) {
			return;
		}
		render_faces();
		render_edges();
		render_vertices();
		render_normals();
		if ((vertex_drag_mode) && (selected_vertex)) {
			render_vertex_drag_frame(selected_vertex);
		}
	}
	/// renders faces
	void render_faces() {
		if ((!b_render_faces) || (!mesh) || (!b_visible)) {
			return;
		}
		face* f = mesh->face_source;
		while (f!=NULL) {
			render_face(f);
			f=f->next_face;
		};
	}
	/// renders edges
	void render_edges() {
		if ((!b_render_edges) || (!mesh) || (!b_visible)) {
			return;
		}
		edge* e = mesh->edge_source;
		while (e!=NULL) {
			render_edge(e);
			e=e->next_edge;
		};
	}
	/// renders vertices
	void render_vertices() {
		if ((!b_render_vertices) || (!mesh) || (!b_visible)) {
			return;
		}
		vertex* v = mesh->vertex_source;
		while (v!=NULL) {
			render_vertex(v);
			v=v->next_vertex;
		};
	}
	/// renders normals
	void render_normals() {
		if ((!b_render_normals) || (!mesh) || (!b_visible)) {
			return;
		}
		face* f = mesh->face_source;
		while (f!=NULL) {
			render_normal(f);
			f=f->next_face;
		};
	}
	/// renders vertex v
	void render_vertex(vertex* v) {
		point3f color;
		if (v == marked_vertex) {
			color = marked_vertex_color;
		} else {
			if (v->marked) {
				color = marked_color;
			} else {
				color = vertex_color;
			}
		}
		if (selected_vertex == v) {
			color = color + select_offset * (0.01f * (white_color - color));
		}
		glColor3f(color.x(), color.y(), color.z());
		if (v->border) {
			glPointSize(border_vertex_size);
		} else {
			glPointSize(vertex_size);
		}

		//glPushName((GLuint)v);

		glBegin(GL_POINTS);
		glVertex3fv(v->position);

		//glEnd();

		glPopName();
	}
	/// renders the normal of face f
	void render_normal(face* f) {
		glColor3f(normal_color.x(), normal_color.y(), normal_color.z());
		glLineWidth(normal_width);
		point3f c = f->compute_center();
		point3f n = f->normal;
		n = mesh->avg_edge_length * n;
		glBegin(GL_LINE_LOOP);
		glVertex3fv(c);
		glVertex3fv(c + n);
		glEnd();
	}
	/// renders edge e
	void render_edge(edge* e) {
		glDisable(GL_POLYGON_OFFSET_FILL);
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		if (e->border) {
			glLineWidth(border_wdge_width);
		} else {
			glLineWidth(edge_width);
		}
		point3f color;
		if (e->marked) {
			color = marked_color;
		} else {
			color = edge_color;
		}
		if (selected_edge == e) {
			color = color + select_offset * (0.01f * (white_color - color));
		}
		glColor3f(color.x(), color.y(), color.z());

		//glPushName((GLuint)e);

		glBegin(GL_LINE_LOOP);
		glVertex3fv(e->m_halfedge->origin->position);
		glVertex3fv(e->m_halfedge->inverse->origin->position);
		glEnd();

		//glPopName();

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	/// renders face f
	void render_face(face* f) {
		if(orthographic)
		{


			halfedge* h_start = f->m_halfedge;
			halfedge* h = h_start;

			glBegin(GL_POLYGON);
			glNormal3fv(f->normal);
			do {
				float c = h->origin->position.y();
				c = c > 1 ? 1: c;
				c = c < 0 ? 0: c;
				glColor3f(c,c,c);
				glVertex3fv(h->origin->position);
				h=h->next;
			} while (h != h_start);
			glEnd();


		}
		else
		{
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(1.0, 1.0);
			glEnable(GL_LIGHTING);
			glEnable(GL_LIGHT0);
			point3f color = face_color;
			if (selected_face == f)
			{
				color = color + select_offset * (0.01f * (white_color - color));
			}


			float material[] = {color.x(),color.y(),color.z(),1.0f};
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material);
			halfedge* h_start = f->m_halfedge;
			halfedge* h = h_start;

			//glPushName((GLuint)f);

			glBegin(GL_POLYGON);
			glNormal3fv(f->normal);
			do {
				glVertex3fv(h->origin->position);
				h=h->next;
			} while (h != h_start);
			glEnd();

			//glPopName();

			glDisable(GL_LIGHTING);
			glDisable(GL_LIGHT0);
		}
	}
	/// renders Vertex drag frame
	void render_vertex_drag_frame(vertex *v) {
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		if ((vertex_drag_mode & X) != 0) {
			float material[] = {red_color(0),red_color(1),red_color(2),1.0f};
			glMaterialfv(GL_FRONT, GL_DIFFUSE, material);
		} else {
			float material[] = {gray_color(0),gray_color(1),gray_color(2),1.0f};
			glMaterialfv(GL_FRONT, GL_DIFFUSE, material);
		}
		render_arrow(v->position, v->position+point3f(mesh->avg_edge_length,0.0f,0.0f));
		if ((vertex_drag_mode & Y) != 0) {
			float material[] = {green_color(0),green_color(1),green_color(2),1.0f};
			glMaterialfv(GL_FRONT, GL_DIFFUSE, material);
		} else {
			float material[] = {gray_color(0),gray_color(1),gray_color(2),1.0f};
			glMaterialfv(GL_FRONT, GL_DIFFUSE, material);
		}
		render_arrow(v->position, v->position+point3f(0.0f,mesh->avg_edge_length,0.0f));
		if ((vertex_drag_mode & Z) != 0) {
			float material[] = {blue_color(0),blue_color(1),blue_color(2),1.0f};
			glMaterialfv(GL_FRONT, GL_DIFFUSE, material);
		} else {
			float material[] = {gray_color(0),gray_color(1),gray_color(2),1.0f};
			glMaterialfv(GL_FRONT, GL_DIFFUSE, material);
		}
		render_arrow(v->position, v->position+point3f(0.0f,0.0f,mesh->avg_edge_length));
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	}
	/// static callback for b_render_normals
	static inline void checkbutton_1_cb(fltk::Widget *w,void *data)  {
		((polygonal_mesh_drawable*)data)->checkbutton_1_cb();
	}
	/// callback for b_render_normals
	void checkbutton_1_cb() {
		b_render_normals = checkbutton_1->value();
		do_callback();
	}
	/// static callback for b_visible
	static inline void checkbutton_2_cb(fltk::Widget *w,void *data)  {
		((polygonal_mesh_drawable*)data)->checkbutton_2_cb();
	}
	/// callback for b_visible
	void checkbutton_2_cb() {
		b_visible = checkbutton_2->value();
		do_callback();
	}
	/// static callback for b_render_vertices
	static inline void checkbutton_3_cb(fltk::Widget *w,void *data)  {
		((polygonal_mesh_drawable*)data)->checkbutton_3_cb();
	}
	/// callback for b_render_vertices
	void checkbutton_3_cb() {
		b_render_vertices = checkbutton_3->value();
		do_callback();
	}
	/// static callback for b_render_edges
	static inline void checkbutton_4_cb(fltk::Widget *w,void *data)  {
		((polygonal_mesh_drawable*)data)->checkbutton_4_cb();
	}
	/// callback for b_render_edges
	void checkbutton_4_cb() {
		b_render_edges = checkbutton_4->value();
		do_callback();
	}
	/// static callback for b_render_faces
	static inline void checkbutton_5_cb(fltk::Widget *w,void *data)  {
		((polygonal_mesh_drawable*)data)->checkbutton_5_cb();
	}
	/// callback for b_render_faces
	void checkbutton_5_cb() {
		b_render_faces = checkbutton_5->value();
		do_callback();
	}
	/// sets b_render_normals
	void set_render_normals(bool b_render_normals) {
		checkbutton_1->value(b_render_normals);
		checkbutton_1_cb();
	}
	/// sets b_render_faces
	void set_render_faces(bool b_render_faces) {
		checkbutton_5->value(b_render_faces);
		checkbutton_5_cb();
	}
	/// sets b_render_vertices
	void set_render_vertices(bool b_render_vertices) {
		checkbutton_3->value(b_render_vertices);
		checkbutton_3_cb();
	}
	/// sets b_render_edges
	void set_render_edges(bool b_render_edges) {
		checkbutton_4->value(b_render_edges);
		checkbutton_4_cb();
	}
	/// sets vertex_size
	void set_vertex_size(float vertex_size) {
		this->vertex_size = vertex_size;
	}
	/// sets border_vertex_size
	void set_border_vertex_size(float border_vertex_size) {
		this->border_vertex_size = border_vertex_size;
	}
	/// sets edge_width
	void set_edge_width(float edge_width) {
		this->edge_width = edge_width;
	}
	/// sets normal_width
	void set_normal_width(float normal_width) {
		this->normal_width = normal_width;
	}
	/// sets border_wdge_width
	void set_border_edge_width(float border_wdge_width) {
		this->border_wdge_width = border_wdge_width;
	}
	/// sets select_offset
	void set_select_offset(float select_offset) {
		this->select_offset = select_offset;
	}
	/// sets b_visible
	void set_visible(bool b_visible) {
		checkbutton_2->value(b_visible);
		checkbutton_2_cb();
	}
	/// returns b_visible
	bool get_visible() {
		return b_visible;
	}
	/// sets vertex_color
	void set_vertex_color(point3f vertex_color) {
		this->vertex_color = vertex_color;
	}
	/// sets edge_color
	void set_edge_color(point3f edge_color) {
		this->edge_color = edge_color;
	}
	/// sets normal_color
	void set_normal_color(point3f normal_color) {
		this->normal_color = normal_color;
	}
	/// sets marked_color
	void set_marked_vertex_color(point3f marked_color) {
		this->marked_color = marked_color;
	}
};
}
