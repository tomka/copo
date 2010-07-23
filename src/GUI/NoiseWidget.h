#pragma once
#include <fltk/GlWindow.h>
#include <fltk/gl.h>
#include <GL/glu.h>
#include <fltk/events.h>
#include <Utilities/Noise.h>

namespace SnowSim {
	namespace GUI {

		/* Draws a perlin noise */
		template<typename TPrecision>
		class NoiseWidget : public ::fltk::GlWindow
		{
			private:
				/* indicates if this widget has been correctly initalized */
				bool m_initok;

				/* the noise image */
				GLfloat* m_image;

				/* the octaves of the perlin noise */
				int m_perlinOctaves;

				/* the persistence of the perlin noise */
				float m_perlinPersistence;

				/* the zoom of perlin noise */
				float m_perlinZoom;

			public:
				void show()
				{
					make_current();
					fltk::Window::show();
				}

			public:
				NoiseWidget(int x,int y,int w,int h);

				/* Generates a new noise image */
				void Refresh();

				/* Sets the octaves of the perlin noise */
				void SetPerlinOctaves(int octaves)
				{
					m_perlinOctaves = octaves;
				}

				/* Gets the octaves of the perlin noise */
				int GetPerlinOctaves()
				{
					return m_perlinOctaves;
				}

				/* Sets the persistence of the perlin noise */
				void SetPerlinPersistence(float persistence)
				{
					m_perlinPersistence = persistence;
				}

				/* Gets the persistence of the perlin noise */
				float GetPerlinPersistence()
				{
					return m_perlinPersistence;
				}

				/* Sets the zoom of the perlin noise */
				void SetPerlinZoom(float zoom)
				{
					m_perlinZoom = zoom;
				}

				/* Gets the zoom of the perlin noise */
				float GetPerlinZoom()
				{
					return m_perlinZoom;
				}

				/* event handling */
				int handle(int event);

				/* Gets a 3D perlin noise texture */

			private:
				/* initializes the widget */
				void init();

				/* the GLUT reshape function */
				void reshape(int w, int h);

				/* the GLUT draw function */
				void draw();

				/* the render method */
				void render();

				/* generates the noise image */
				void makeImage();

				/* Gets a 3D texture */
		};


		template<typename T>
		NoiseWidget<T>::NoiseWidget(int x,int y,int w,int h)
			:	GlWindow(x,y,w,h),
				m_initok(false),
				m_image(NULL),
				m_perlinOctaves(4),
				m_perlinPersistence(0.25),
				m_perlinZoom(1.0)
		{}

		template<typename T>
		void NoiseWidget<T>::init()
		{
			glClearColor(1.0f,1.0f,1.0f,1.0f);
			glEnable(GL_POINT_SMOOTH);
			glDisable(GL_DEPTH_TEST);

			glShadeModel(GL_FLAT);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		}

		template<typename T>
		void NoiseWidget<T>::reshape(int w, int h)
		{
			glViewport(0, 0, w, h);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			gluOrtho2D(0,1,0,1);  // Set coordinates to the [0..1] x [0..1]

			//ortho();
			glMatrixMode(GL_MODELVIEW);      // Select The Modelview Matrix

			makeImage();
		}

		template<typename T>
		void NoiseWidget<T>::Refresh()
		{
			makeImage();
			redraw();
		}

		template<typename T>
		int  NoiseWidget<T>::handle(int event) {
			int result;
			switch(event) {
				case fltk::PUSH:
					make_current(); // make OpenGL context current
					Refresh();
					result = 1;
					break;
				default:
					// let the base class handle all other events:
					result = fltk::GlWindow::handle(event);
					break;
			}

			return result;
		}

		template<typename T>
		void NoiseWidget<T>::draw()
		{
			if(!valid())
			{
				if( !m_initok )
				{
					m_initok = true;
					init();
				}

				reshape( w(), h() );
			}

			render();
		}

		template<typename T>
		void NoiseWidget<T>::render()
		{
			glClear(GL_COLOR_BUFFER_BIT);
			glEnable (GL_BLEND);
			glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

			glRasterPos2i( 0, 0 );
			glDrawPixels( w(), h(), GL_ALPHA, GL_FLOAT, m_image );
			//glFlush();
		}

		template<typename T>
		void NoiseWidget<T>::makeImage()
		{
			if (m_image != NULL)
				delete(m_image);

			int width = w();
			int height = h();

			float inv_zoom = 1 / m_perlinZoom;

			m_image = (GLfloat*) malloc( sizeof(GLfloat) * width * height );

			for(int y=0; y < height; ++y){
				for(int x=0; x < width; ++x){
					m_image[x + y * width]
						= 1.0 - ( ::Utilities::Noise::PerlinNoise_3D<float>(x * inv_zoom , y * inv_zoom, 0.0f,
							m_perlinOctaves, m_perlinPersistence) + 1.0 ) * 0.5;
				}
			}
		}
	}
}
