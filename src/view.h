#pragma once

#include <Shading/Texture.h>
#include <Utilities/OpenGL.h>

#ifdef WIN32
#include <windows.h>
#pragma warning (disable:4311)
#endif

#include <cgv/math/linalg/linalg.h>
#include <fltk/GlWindow.h>
#include <fltk/events.h>
#include <fltk/draw.h>
#include <fltk/Choice.h>
#include <fltk/Output.h>
#include <fltk/Item.h>
#include <fltk/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <iostream>
#include <vector>
#include "polygonal_mesh.h"
#include "polygonal_mesh_drawable.h"
#include "types.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>
#include <string>
#include <sstream>
#include "Model/World.h"
#include <Utilities/Keys.h>
#include <boost/signal.hpp>
#include <Exceptions/NotInitializedException.h>

/// size of the selection buffer
const int SELECT_BUF_SIZE = 4000;
/// selection tolerance
const GLdouble SELECT_TOL = 20;

namespace SnowSim {

	struct Pixel {
		unsigned char r, g, b, a;

		Pixel() {
			r = g = b = 0;
			a = 0;
		}
	};

	/**
	* The view class is an opengl-widget for graphical representation of meshes.
	*/
	class view : public fltk::GlWindow {

		typedef SnowSim::Model::World	World;

	private:
		/// constructor
		view(int x,int y,int w,int h, World* world);
		// Not defined, to prevent copying
		view(const view& );
		view& operator =(const view& other);

		// Does the graphics card support all needed extensions?
		bool m_hasAllExtensions;

		void renderCoordinateSystem();

		// renders the noise texture to the screen
		void render_noise_texture();


	protected:
		/// the singleton instance
		static view* m_instance;

		/// the bodyPool holds bodies, hierarchies and contact finder
		World* m_world;


		/// halfedge
		typedef SnowSim::polygonal_mesh::halfedge halfedge;
		/// edge
		typedef SnowSim::polygonal_mesh::edge edge;
		/// vertex
		typedef SnowSim::polygonal_mesh::vertex vertex;
		/// face
		typedef SnowSim::polygonal_mesh::face face;

		/// interaction mode
		Interaction_Mode interaction_mode;
		/// vertex drag mode
		Vertex_Drag_Mode vertex_drag_mode;

		/// Camera's position
		GLfloat cam_azimut;
		/// Camera's position
		GLfloat cam_polar;
		/// Camera's position
		GLfloat cam_distance;
		/// Camera's position
		Vec4f cam_target;

		/// width of image
		int img_width;
		/// height of image
		int img_height;

		/// pointer to the control mesh drawable
		SnowSim::polygonal_mesh_drawable* basemesh_drawable;
		/// pointer to the fine mesh drawavle
		SnowSim::polygonal_mesh_drawable* modifiedmesh_drawable;
		/// pointer to the fine mesh drawavle
		SnowSim::polygonal_mesh_drawable* launchsites_drawable;

		// have openGL initializations been done
		bool initalized;

		/// modelviewmatrix
		cgv::math::linalg::fix_sqr_mat<float,4> m;
		/// init ok
		bool init_ok;

		/// vec mouse
		Vec4f vec_mouse;
		/// old mouse x position
		int mouse_x_old;
		/// old mouse y position
		int mouse_y_old;

		/// choice for interaction mode
		fltk::Choice *choice_1;
		/// choice for vertex drag mode
		fltk::Choice *choice_2;
		/// output
		fltk::Output *output;

		bool ortho_sh;

		std::string filename_screenshot;

		/// does openGL initializations
		void init(void);
		/// sets viewport and projection matrix, after resizing the view
		void reshape(int w, int h);
		/// render
		void render();

		////////////
		// Callbacks
		////////////

		/// static callback for vertex_drag_mode
		static inline void choice_2_cb(fltk::Widget *w,void *data) {
			((view*)data)->choice_2_cb();
		}
		/// callback for vertex_drag_mode
		void choice_2_cb();

		/// static callback for interaction_mode
		static inline void choice_1_cb(fltk::Widget *w,void *data) {
			((view*)data)->choice_1_cb();
		}
		/// callback for interaction_mode
		void choice_1_cb();

		/// render the image and pixelgrid if enabled
		void draw();

		/// returns the selected edge, NULL if nothing selected
		edge* get_selected_edge(int x, int y);
		/// returns the selected vertex, NULL if nothing selected
		vertex* get_selected_vertex(int x, int y);
		/// returns the selected face, NULL if nothing selected
		face* get_selected_face(int x, int y);

		/// stores an predefined noise texture for arbitary use
		SnowSim::Shading::Texture m_noise_tex;

		void Refresh(World* world);

	public:

		/// destructor
		~view();

		/// event handling
		int handle(int event);

		/// mouse button pressed
		void mouse_move_button_pressed(unsigned button, int dx, int dy);
		/// mouse push
		void mouse_push(unsigned button);
		/// mouse move
		void mouse_move(int mouse_x, int mouse_y);

		/// reset camera
		void reset_camera();

		/// set a specific view
		void set_view(float azimut, float polar, float distance, cgv::math::linalg::fix_col_vec<float,4>& target);

		float get_azimut() { return cam_azimut; }
		float get_polar() { return cam_polar; }
		float get_distance() { return cam_distance; }

		cgv::math::linalg::fix_col_vec<float,4> get_target() { return cam_target; }

		/// creates a random noise texture
		void create_noise_texture(int width = 512, int height = 512, int seed = 1);

		void orthographic_screenshot(const std::string &filename);

		/// register drawables
		void register_drawables(SnowSim::polygonal_mesh_drawable* basemesh_drawable,
			SnowSim::polygonal_mesh_drawable* modifiedmesh_drawable) {
			this->basemesh_drawable = basemesh_drawable;
			this->modifiedmesh_drawable = modifiedmesh_drawable;
			if (interaction_mode == TRANSLATE) {
				basemesh_drawable->vertex_drag_mode = vertex_drag_mode;
			}
		}

		/// unregister drawables
		void unregister_drawables() {
			basemesh_drawable = NULL;
			modifiedmesh_drawable = NULL;
			launchsites_drawable = NULL;
		}

		boost::signal<void (SNOW_KEY)> KeyPressed;

		static view* CreateInstance(int x,int y,int w,int h, World* world)
		{
			m_instance = new view(x, y, w, h, world);
			return m_instance;
		}

		static view* Instance() {
			if (m_instance == NULL)
				throw Exceptions::NotInitializedException();

			return m_instance;
		}

		static SNOW_KEY FltkToSnowKey(unsigned int fltk_key)
		{
			//fltk::UpKey

			return (SNOW_KEY)fltk_key;
		}
	};
}
