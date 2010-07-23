#include "view.h"
#include "Exceptions/ArgumentException.h"
#include <GL/glut.h>
#include <Rendering/RenderingInfo.h>
#include <Utilities/Random.h>

namespace SnowSim {

//
// static members
//
view* view::m_instance = 0;

void view::init(void) {
	glClearColor(0.80f, 0.80f, 0.90f, 1.0f);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);

	/* Hide fragments with alpha <= 0.1f */
	glAlphaFunc(GL_GREATER, 0.1f);

	glEnable(GL_NORMALIZE);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	create_noise_texture();
}

void view::reshape(int w, int h) {
	glViewport(0, 0, w, h);
	img_width = w;
	img_height = h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLdouble aspect = ((float)w) / ((float)(h==0?1:h));
	gluPerspective(45, aspect, 0.1, 10000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glShadeModel(GL_SMOOTH);
}

view::~view() {
	delete choice_1;
	delete choice_2;
	delete output;
}

void view::Refresh(World* world)
{
	redraw();
}

view::view(int x,int y,int w,int h, World* world)
	:	GlWindow(x,y,w,h-25),
		m_hasAllExtensions(false)
{

	if (!world)
		throw new SnowSim::Exceptions::ArgumentException("parameter world can not be NULL!");

	this->m_world = world;

	// gets called from window
	//this->m_world->Changed += event_handler(this, &view::Refresh);

	init_ok =false;
	ortho_sh=false;
	basemesh_drawable = NULL;
	modifiedmesh_drawable = NULL;
	m = cgv::math::linalg::fix_diag_mat<float,4>(1.0f);
	vec_mouse = cgv::math::linalg::fix_col_vec<float,4>(0.0f);
	mouse_x_old = 0;
	mouse_y_old = 0;
	reset_camera();
	fltk::Item* item;
	interaction_mode=VIEWPOINT;
	choice_1 = new fltk::Choice(x+100,h,150,20,"interaction mode");
	choice_1->begin();
		item = new fltk::Item("change camera");
		//item = new fltk::Item("mark vertex/edge (hoppe rules)");
		//item = new fltk::Item("translate vertex");
		//item = new fltk::Item("split edge");
		//item = new fltk::Item("insert edge (select vertices)");
	choice_1->end();
	choice_1->value(interaction_mode);
	choice_1->callback(choice_1_cb,this);
	vertex_drag_mode=X;
	choice_2 = new fltk::Choice(x+255,h,150,20);
	choice_2->begin();
		item = new fltk::Item("X (right mouse button)");
		item = new fltk::Item("Y (right mouse button)");
		item = new fltk::Item("XY (right mouse button)");
		item = new fltk::Item("Z (right mouse button)");
		item = new fltk::Item("XZ (right mouse button)");
		item = new fltk::Item("YZ (right mouse button)");
	choice_2->end();
	choice_2->value(vertex_drag_mode-1);
	choice_2->callback(choice_2_cb,this);
	output = new fltk::Output(x+410,h,190,20,"");
	end();
	if (interaction_mode!=TRANSLATE) {
		choice_2->hide();
	} else {
		choice_2->show();
	}
}

/// event handling
int view::handle(int event) {
	int mx;
	int my;
	int dx=0;
	int dy=0;
	int mouse_x;
	int mouse_y;
	unsigned int button;
	int result;
	switch(event) {
		case fltk::SHORTCUT:
		case fltk::KEY:
			make_current(); // make OpenGL context current
			// keypress, key is in fltk::event_key(), ascii in fltk::event_text()
			// Return 1 if you understand/use the keyboard event, 0 otherwise...
			KeyPressed(FltkToSnowKey(fltk::event_key()));
			return 1;
			break;
		case fltk::MOVE:
			make_current(); // make OpenGL context current
			mouse_x = fltk::event_x();
			mouse_y = fltk::event_y();
			if (m_world) {
				mouse_move(mouse_x,mouse_y);
			}
			result = 1;
			break;
		case fltk::PUSH:
			make_current(); // make OpenGL context current
			mx = fltk::event_x();
			my = fltk::event_y();
			mouse_x_old = mx;
			mouse_y_old = my;

			take_focus();

			button = fltk::event_button();
			if (m_world) {
				mouse_push(button);
			}
			result = 1;
			break;
		case fltk::RELEASE:
			make_current(); // make OpenGL context current
			mx = fltk::event_x();
			my = fltk::event_y();
			dx = mx-mouse_x_old;
			dy = my-mouse_y_old;
			mouse_x_old = mx;
			mouse_y_old = my;
			button = fltk::event_button();
			return 1;
			break;
		case fltk::DRAG:
			make_current(); // make OpenGL context current
			mx = fltk::event_x();
			my = fltk::event_y();
			dx = mx-mouse_x_old;
			dy = my-mouse_y_old;
			mouse_x_old = fltk::event_x();
			mouse_y_old = fltk::event_y();
			button = fltk::event_button();
			if (m_world) {
				mouse_move_button_pressed(button, dx, dy);
			}
			result = 1;
			break;
		case fltk::FOCUS:
			return 1;
			break;
		case fltk::UNFOCUS:
			return 1;
			break;
		default:
			// let the base class handle all other events:
			result = fltk::GlWindow::handle(event);
			break;
	}
	redraw();
	return result;
}

/// mouse button pressed
void view::mouse_move_button_pressed(unsigned button, int dx, int dy) {
	float mouse_sensitivity = 0.02f;
	if (/*((interaction_mode == MARK) && (!basemesh_drawable->selected_vertex) && (!basemesh_drawable->selected_edge))||
		((!basemesh_drawable->selected_vertex) && (interaction_mode == TRANSLATE))||
		((interaction_mode == SPLIT) && (!basemesh_drawable->selected_edge))||
		((interaction_mode == INSERT) && (!basemesh_drawable->selected_vertex))||*/
		 (interaction_mode == VIEWPOINT)) {
		vec_mouse.x() = -(float)dx;
		vec_mouse.y() = (float)dy;
		vec_mouse.z() = 0;
		switch (button) {
			case fltk::LeftButton:
				cam_azimut += mouse_sensitivity*float(dx);
				cam_polar += mouse_sensitivity*float(dy);
				break;
			case fltk::RightButton:
				cam_distance = max(-20000.0, min(20000.0, static_cast<double>(cam_distance + mouse_sensitivity*(float(dx) + float(dy)) + mouse_sensitivity*(float(dx) + float(dy)) * (1.0f / 5000.0f) * pow(cam_distance,2.0f))));
				break;
			case fltk::MiddleButton:
				cam_target += (0.2f * mouse_sensitivity * (invert(m)*vec_mouse));
				break;
			default:
				break;
		}
	}

	// TODO: Check if selected mesh is visible, if it is not - return
	//if (!m_world->GetBaseMeshs().) {
	//	return;
	//}
	return;

	if ((interaction_mode == TRANSLATE) && (basemesh_drawable->selected_vertex) && (button == fltk::LeftButton)) {
		cgv::math::linalg::fix_col_vec<float,4> e1,e2;
		point3f e11,e22;
		e1.set(1.0f,0.0f,0.0f,1.0f);
		e2.set(0.0f,1.0f,0.0f,1.0f);
		cgv::math::linalg::fix_sqr_mat<float,4> mt = transpose(m);
		e1 = mt*e1;
		e2 = mt*e2;
		e11.set(e1.x()/e1.w(),e1.y()/e1.w(),e1.z()/e1.w());
		e22.set(e2.x()/e2.w(),e2.y()/e2.w(),e2.z()/e2.w());
		//todo:put pixel characteristics
		point3f delta = ((float)dx*e11) + ((float)-dy*e22);
		if ((vertex_drag_mode & X) == 0) {
			delta.x() = 0;
		}
		if ((vertex_drag_mode & Y) == 0) {
			delta.y() = 0;
		}
		if ((vertex_drag_mode & Z) == 0) {
			delta.z() = 0;
		}
		basemesh_drawable->selected_vertex->position += (0.5f * mouse_sensitivity * basemesh_drawable->mesh->avg_edge_length) * delta;
		do_callback();
	}
}
/// mouse push
void view::mouse_push(unsigned button) {

	// TODO: Check if selected mesh is visible, if it is not - return
	//if (!m_world->GetBaseMeshs().) {
	//	return;
	//}
	return;

	if ((interaction_mode==MARK) && (button == fltk::LeftButton)) {
		if (basemesh_drawable->selected_edge) {
			basemesh_drawable->selected_edge->marked = !basemesh_drawable->selected_edge->marked;
			do_callback();
		}
		if (basemesh_drawable->selected_vertex) {
			basemesh_drawable->selected_vertex->marked = !basemesh_drawable->selected_vertex->marked;
			do_callback();
		}
	}
	if ((interaction_mode == TRANSLATE) && (button == fltk::RightButton) && (basemesh_drawable->selected_vertex)) {
		if (vertex_drag_mode == YZ) {
			vertex_drag_mode = X;
		} else {
			((int&)vertex_drag_mode)++;
		}
		choice_2->value(vertex_drag_mode-1);
		basemesh_drawable->vertex_drag_mode = vertex_drag_mode;
	}
	if ((interaction_mode == SPLIT) && (button == fltk::LeftButton) && (basemesh_drawable->selected_edge)) {
		basemesh_drawable->mesh->split_edge(basemesh_drawable->selected_edge);
		do_callback();
	}
	if ((interaction_mode == INSERT) && (button == fltk::LeftButton) && (basemesh_drawable->selected_vertex)) {
		if (!basemesh_drawable->marked_vertex) {
			basemesh_drawable->marked_vertex = basemesh_drawable->selected_vertex;
		} else {
			if (basemesh_drawable->selected_vertex == basemesh_drawable->marked_vertex) {
				basemesh_drawable->marked_vertex = NULL;
			} else {
				if (basemesh_drawable->mesh->insert_edge(basemesh_drawable->marked_vertex,basemesh_drawable->selected_vertex)) {
					basemesh_drawable->marked_vertex = NULL;
					do_callback();
				}
			}
		}
	}
}
/// mouse move
void view::mouse_move(int mouse_x, int mouse_y) {
	// TODO: Check if selected mesh is visible, if it is not - return
	//if (!m_world->GetBaseMeshs().) {
	//	return;
	//}
	return;

	basemesh_drawable->selected_vertex = get_selected_vertex(mouse_x,mouse_y);
	basemesh_drawable->selected_edge = NULL;
	basemesh_drawable->selected_face = NULL;
	if (basemesh_drawable->selected_vertex == NULL) {
		basemesh_drawable->selected_edge = get_selected_edge(mouse_x,mouse_y);
		if (basemesh_drawable->selected_edge == NULL) {
			basemesh_drawable->selected_face = get_selected_face(mouse_x,mouse_y);
		}
	}
	std::stringstream s;
	s << "";
	if (basemesh_drawable->selected_edge) {
		s << "edge " << basemesh_drawable->selected_edge->index << " (halfedges " << basemesh_drawable->selected_edge->m_halfedge->index << " and " << basemesh_drawable->selected_edge->m_halfedge->inverse->index << ")";
	}
	if (basemesh_drawable->selected_vertex) {
		s << "vertex " << basemesh_drawable->selected_vertex->index << " (halfedge " << basemesh_drawable->selected_vertex->m_halfedge->index << ")";
	}
	if (basemesh_drawable->selected_face) {
		s << "face " << basemesh_drawable->selected_face->index << " (halfedge " << basemesh_drawable->selected_face->m_halfedge->index << ")";
	}
	output->value(s.str().c_str());
}

/// reset camera
void
view::reset_camera()	{
	cgv::math::linalg::fix_col_vec<float,4> target(0.0f);
	set_view(-45.0f, 45.0f, 50.0f, target);
}

/// set a specific view
void
view::set_view(float azimut, float polar, float distance, cgv::math::linalg::fix_col_vec<float,4>& target)
{
	cam_azimut = azimut;
	cam_polar = polar;
	cam_distance = distance;
	cam_target = target;
}

/// creates a random noise texture
void view::create_noise_texture(int width /* = 512 */, int height /* = 512 */, int seed /* = 1 */ )
{
		Pixel* data = new Pixel[width * height];
		if(data == 0) {
			std::cout << "shit, memory full:(\n";
			exit(1);
		}

		Utilities::Random r(seed);

		for(int i = 0; i < 200; ++i) {
			int x = r.randomFloat() * width;
			int y = r.randomFloat() * height;

			int pos = x + y * width;
			// get random normal
			float theta = r.randomFloat(0.0, PI);
			float phi = r.randomFloat(0.0, PI_2);
			point3f randomNormal(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			data[pos].r = (unsigned char)255.0f * randomNormal.x();
			data[pos].g = (unsigned char)255.0f * randomNormal.y();
			data[pos].b = (unsigned char)255.0f * randomNormal.z();
			data[pos].a = 255;
		}

//		for(int i = 0; i < width; ++i) {
//			for(int j = 0; j < height; ++j) {
//				int pos = i + j * width;
//				data[pos].r = 255;
//				data[pos].g = 0;
//				data[pos].b = 0;
//			}
//		}

		unsigned int texId;
		glGenTextures(1, &texId);
		glBindTexture(GL_TEXTURE_2D, texId);

		m_noise_tex.level = 0;
		m_noise_tex.internal_format = GL_RGBA8;
		m_noise_tex.width = width;
		m_noise_tex.height = height;
		m_noise_tex.border = 0;
		m_noise_tex.format = GL_RGBA;
		m_noise_tex.type = GL_UNSIGNED_BYTE;
		m_noise_tex.pixels = NULL;
		m_noise_tex.bound = true;
		m_noise_tex.texname = texId;

		glTexImage2D(	GL_TEXTURE_2D, // target
						m_noise_tex.level,
						m_noise_tex.internal_format,
						m_noise_tex.width,
						m_noise_tex.height,
						m_noise_tex.border,
						m_noise_tex.format,
						m_noise_tex.type,
						data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		delete[] data;
}


/// callback for vertex_drag_mode
void view::choice_2_cb() {
	((int&)vertex_drag_mode) = choice_2->value()+1;
}

/// callback for interaction_mode
void view::choice_1_cb() {
	interaction_mode = (Interaction_Mode)choice_1->value();
	if ((interaction_mode != INSERT) && (basemesh_drawable)) {
		basemesh_drawable->marked_vertex = NULL;
	}
	if (interaction_mode != TRANSLATE) {
		choice_2->hide();
		if (basemesh_drawable) {
			basemesh_drawable->vertex_drag_mode = NO;
		}
	} else {
		choice_2->show();
		if (basemesh_drawable) {
			basemesh_drawable->vertex_drag_mode = vertex_drag_mode;
		}
	}
}


void view::orthographic_screenshot(const std::string &filename)
{
	this->make_current();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(-0.5,0.5,-0.5,0.5,-100,100);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glTranslatef(0,0,-10);
	glRotatef(90,1,0,0);

	modifiedmesh_drawable->orthographic = true;
	this->ortho_sh = true;
	filename_screenshot = filename;

	flush();

	modifiedmesh_drawable->orthographic = false;
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	this->ortho_sh = false;
}

/// render the image and pixelgrid if enabled
void view::draw()
{
	if(!valid()) {
		glViewport (0, 0, w(), h());

		// Reset Modelview:
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		if(!init_ok) {
			init_ok=true;
			m_hasAllExtensions = Utilities::OpenGL::extInit();
			init();
		}

		reshape( w(), h() );
	}
	render();

	GLenum errCode;
	const GLubyte *errString;

	if ((errCode = glGetError()) != GL_NO_ERROR) {
		errString = gluErrorString(errCode);
		std::cout << "[Error] OpenGL Error: " << errString << std::endl;
	}
}

/// returns the selected edge, NULL if nothing selected
view::edge* view::get_selected_edge(int x, int y) {
	GLuint select_buf[SELECT_BUF_SIZE];
	glSelectBuffer(SELECT_BUF_SIZE, select_buf);
	glRenderMode(GL_SELECT);
	glInitNames();
	GLdouble projection_matrix[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projection_matrix);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	gluPickMatrix (GLdouble(x), GLdouble(viewport[3] - y), SELECT_TOL, SELECT_TOL, viewport);
	glMultMatrixd(projection_matrix);
	basemesh_drawable->render_edges();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(projection_matrix);
	glFlush();
	GLint hits = glRenderMode(GL_RENDER);
	if (hits > 0) {
		int selected_edge = select_buf[3];
		int depth = select_buf[1];
		for (int i = 1; i < hits; i++) {
			if (select_buf[i*4+1] < GLuint(depth))  {
				selected_edge = select_buf[i*4+3];
				depth = select_buf[i*4+1];
			}
		}
		return (edge*)selected_edge;
	}
	return NULL;
}

/// returns the selected vertex, NULL if nothing selected
view::vertex* view::get_selected_vertex(int x, int y) {
	GLuint select_buf[SELECT_BUF_SIZE];
	glSelectBuffer(SELECT_BUF_SIZE, select_buf);
	glRenderMode(GL_SELECT);
	glInitNames();
	GLdouble projection_matrix[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projection_matrix);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	gluPickMatrix (GLdouble(x), GLdouble(viewport[3] - y), SELECT_TOL, SELECT_TOL, viewport);
	glMultMatrixd(projection_matrix);
	basemesh_drawable->render_vertices();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(projection_matrix);
	glFlush();
	GLint hits = glRenderMode(GL_RENDER);
	if (hits > 0) {
		int selected_vertex = select_buf[3];
		int depth = select_buf[1];
		for (int i = 1; i < hits; i++) {
			if (select_buf[i*4+1] < GLuint(depth))  {
				selected_vertex = select_buf[i*4+3];
				depth = select_buf[i*4+1];
			}
		}
		return (vertex*)selected_vertex;
	}
	return NULL;
}


/// returns the selected face, NULL if nothing selected
view::face* view::get_selected_face(int x, int y) {
	GLuint select_buf[SELECT_BUF_SIZE];
	glSelectBuffer(SELECT_BUF_SIZE, select_buf);
	glRenderMode(GL_SELECT);
	glInitNames();
	GLdouble projection_matrix[16];
	glGetDoublev(GL_PROJECTION_MATRIX, projection_matrix);
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity ();
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	gluPickMatrix (GLdouble(x), GLdouble(viewport[3] - y), SELECT_TOL, SELECT_TOL, viewport);
	glMultMatrixd(projection_matrix);
	basemesh_drawable->render_faces();
	glMatrixMode(GL_PROJECTION);
	glLoadMatrixd(projection_matrix);
	glFlush();
	GLint hits = glRenderMode(GL_RENDER);
	if (hits > 0) {
		int selected_face = select_buf[3];
		int depth = select_buf[1];
		for (int i = 1; i < hits; i++) {
			if (select_buf[i*4+1] < GLuint(depth))  {
				selected_face = select_buf[i*4+3];
				depth = select_buf[i*4+1];
			}
		}
		return (face*)selected_face;
	}
	return NULL;
}

/// render
void view::render() {

	// Clear Screen And Depth Buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	if (!m_world) {
		return;
	}

	if(ortho_sh)
	{
		glClearColor(0,0,0,1);
		modifiedmesh_drawable->set_render_faces(true);
		modifiedmesh_drawable->set_render_edges(false);
		modifiedmesh_drawable->set_render_normals(false);
		modifiedmesh_drawable->set_render_vertices(false);
		modifiedmesh_drawable->render();
		glFlush();
		int size = w()*h()*3;

		unsigned char *data = new unsigned char[size];
		glReadPixels(0,0, w(), h(),GL_RGB,GL_UNSIGNED_BYTE,data);

		ilInit();
		iluInit();
		ilutInit();

		ILuint ImageName;
		ilGenImages(1, &ImageName);
		ilBindImage(ImageName);
		ilTexImage(w(),h(),1,3,IL_RGB,IL_UNSIGNED_BYTE,data);

		//iluFlipImage();
		ilEnable(IL_FILE_OVERWRITE);

#ifdef _WIN32
		// Did not compile in linux:
		ilSaveImage(reinterpret_cast<const wchar_t *>(filename_screenshot.c_str()));
#else
		ilSaveImage(filename_screenshot.c_str());
#endif

		delete[] data;
		glClearColor(0.80f, 0.80f, 0.90f, 1.0f);
	}
	else
	{
		// Reset Modelview:
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);

		// Set modelviewmatrix
		m =	translate_44(0.0f, 0.0f, -cam_distance)
				* rotate_x_44(cam_polar)
				* rotate_y_44(cam_azimut)
				* translate_44(-cam_target.x(), -cam_target.y(), -cam_target.z() );

		// Transpose (here: invert) matrix:
		Mat44f mat_trans = transpose(m);

		point3f camera_pos(0.0, 0.0, 0.0);
		camera_pos = invert(m) * camera_pos;

		glLoadMatrixf( &mat_trans(0,0) );

		// Create light components
		GLfloat ambientLight[]	= { 0.0f, 0.0f, 0.0f, 1.0f };
		GLfloat diffuseLight[]	= { 1.0f, 1.0f, 1.0f, 1.0f };
		GLfloat specularLight[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		GLfloat position[]		= { 1.0f, 1.0f, 1.0f, 0.0f };

		GLfloat globalAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);

		// Assign created components to GL_LIGHT1
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
		glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
		glLightfv(GL_LIGHT0, GL_POSITION, position);

		Rendering::RenderingInfo info( camera_pos, m_noise_tex );

		/* render the world */
		m_world->render( info );

		renderCoordinateSystem();

		//render_noise_texture();
	}
}

void view::render_noise_texture()
{
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);
	glDisable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);
	//glColor4f(1.0f,1.0f,1.0f,1.0f);
	glBindTexture(GL_TEXTURE_2D, m_noise_tex.texname);

	glMatrixMode(GL_PROJECTION);

		glPushMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);
			glVertex2f(0.0f, 0.0f);

			glTexCoord2f(0.0f, 1.0f);
			glVertex2f(0.0f, 1.0f);

			glTexCoord2f(1.0f, 1.0f);
			glVertex2f(1.0f, 1.0f);

			glTexCoord2f(1.0f, 0.0f);
			glVertex2f(1.0f, 0.0f);
		glEnd();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();

	glDisable(GL_TEXTURE_2D);
}

void view::renderCoordinateSystem()
	{
		glDisable(GL_LIGHTING);

		// render coodinate system arrows
		glClear(GL_DEPTH_BUFFER_BIT); // everything after here is drawn on top

		glLoadIdentity();
		glTranslatef(-6.2f,-6.0f,-16.0f);
		glRotatef(cam_polar * 180 / PI, 1.0f, 0.0f, 0.0f);
		glRotatef(cam_azimut * 180 / PI, 0.0f, 1.0f, 0.0f);

		point3f pos = point3f(0.0f, 0.0f, 0.0f);

		glColor3f(1.0f, 0.0f, 0.0f); // X axis
		glLineWidth(1.0f);
		render_arrow(pos, point3f(1.0f,0.0f,0.0f), 0.45f, 0.3f);
		glBegin(GL_LINES);
			glVertex3f(1.1f,0.1f,0.0f);
			glVertex3f(1.2f,-0.1f,0.0f);
			glVertex3f(1.2f,0.1f,0.0f);
			glVertex3f(1.1f,-0.1f,0.0f);
		glEnd();

		glColor3f(0.0f, 1.0f, 0.0f); // Y axis
		render_arrow(pos, point3f(0.0f,1.0f,0.0f), 0.4f, 0.3f);
		glLineWidth(1.0f);
		glBegin(GL_LINES);
			glVertex3f(-0.1f, 1.25f, 0.0f);
			glVertex3f( 0.0f, 1.15f, 0.0f);
			glVertex3f( 0.1f, 1.25f, 0.0f);
			glVertex3f( 0.0f, 1.15f, 0.0f);
			glVertex3f( 0.0f, 1.15f, 0.0f);
			glVertex3f( 0.0f, 1.0f,  0.0f);
		glEnd();

		glColor3f(0.0f, 0.0f, 1.0f); // Z axis
		render_arrow(pos, point3f(0.0f,0.0f,1.0f), 0.4f, 0.3f);
		glLineWidth(1.0f);
		glBegin(GL_LINE_STRIP);
			glVertex3f( 0.0f, 0.1f, 1.1f);
			glVertex3f( 0.0f, 0.1f, 1.2f);
			glVertex3f( 0.0f,-0.1f, 1.1f);
			glVertex3f( 0.0f,-0.1f, 1.2f);
		glEnd();
	}
}
