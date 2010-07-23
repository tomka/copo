#pragma once
#include <fltk/GlWindow.h>
#include <fltk/gl.h>
#include <GL/glu.h>
#include <fltk/events.h>
#include <Utilities/Noise.h>
#include <Utilities/math.h>

namespace SnowSim {
	namespace GUI {

		/* Draws a perlin noise */
		template<typename TPrecision>
		class AORWidget : public ::fltk::GlWindow
		{
			public:
				// An enumeration declaring the different Angle of repose types
				enum AngleOfReposeFunction
				{
					None,
					Constant,
					DuoConstant,
					Linear
				};

			private:
				/* indicates if this widget has been correctly initalized */
				bool m_initok;

				/* the constant 1 */
				TPrecision m_constant1;

				/* constant 2*/
				TPrecision m_constant2;

				/* seperating angle beween different terms */
				TPrecision m_sep_angle;

				TPrecision m_linear_m;

				TPrecision m_linear_n;

				TPrecision m_stableAngle;

				AngleOfReposeFunction m_selected_type;

			public:
				void show()
				{
					make_current();
					fltk::Window::show();
				}

			public:
				AORWidget(int x,int y,int w,int h);

				/* Generates a new noise image */
				void Refresh();

				void SetConstant1(TPrecision c1)
				{
					m_constant1 = c1;
					redraw();
				}

				TPrecision GetConstant1()
				{
					return m_constant1;
				}

				void SetConstant2(TPrecision c2)
				{
					m_constant2 = c2;
					redraw();
				}

				TPrecision SetConstant2()
				{
					return m_constant2;
				}

				void SetSeparatingAngle(TPrecision sep_angle)
				{
					m_sep_angle = sep_angle;
					redraw();
				}

				TPrecision GetSeparatingAngle()
				{
					return m_sep_angle;
				}

				void SetLinearM(TPrecision m)
				{
					m_linear_m = m;
					redraw();
				}

				TPrecision GetLinearM()
				{
					return m_linear_m;
				}

				void SetLinearN(TPrecision n)
				{
					m_linear_m = n;
					redraw();
				}

				TPrecision GetLinearN()
				{
					return m_linear_n;
				}

				void SetCurrentType(AngleOfReposeFunction type)
				{
					m_selected_type = type;
					redraw();
				}

				AngleOfReposeFunction GetCurrentType()
				{
					return m_selected_type;
				}

				void SetValues(TPrecision c1, TPrecision sep, TPrecision c2, TPrecision m, TPrecision n );

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
		AORWidget<T>::AORWidget(int x,int y,int w,int h)
			:	GlWindow(x,y,w,h),
				m_initok(false),
				m_constant1(1.0),
				m_constant2(0.0),
				m_sep_angle(1.0),
				m_linear_m(-1.14592),
				m_linear_n(1.600002),
				m_stableAngle(1.22),
				m_selected_type(None)
		{}

		template<typename T>
		void AORWidget<T>::SetValues(T c1, T sep, T c2, T m, T n )
		{
			m_constant1 = c1;
			m_constant2 = c2;
			m_sep_angle = sep;
			m_linear_m = m;
			m_linear_n = n;

			redraw();
		}

		template<typename T>
		void AORWidget<T>::init()
		{
			glClearColor(1.0f,1.0f,1.0f,1.0f);
			glEnable(GL_POINT_SMOOTH);
			glDisable(GL_DEPTH_TEST);

			glShadeModel(GL_FLAT);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		}

		template<typename T>
		void AORWidget<T>::reshape(int w, int h)
		{
			glViewport(0, 0, w, h);
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();

			T x_min = Utilities::Math::DegToRad(-10.0);
			T x_max = Utilities::Math::DegToRad(100.0);

			gluOrtho2D( x_min, x_max, -0.2,1.1);

			//ortho();
			glMatrixMode(GL_MODELVIEW);      // Select The Modelview Matrix
		}

		template<typename T>
		void AORWidget<T>::Refresh()
		{
			redraw();
		}

		template<typename T>
		int  AORWidget<T>::handle(int event) {
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
		void AORWidget<T>::draw()
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
		void AORWidget<T>::render()
		{
			glClear(GL_COLOR_BUFFER_BIT);
			glDisable (GL_BLEND);

			glColor3f(0.8,0.8,0.8);
			glLineWidth(1.0);

			glBegin(GL_LINES);
				// coordinate system
				glVertex2f(0.0,0.0);
				glVertex2f(PI_2,0.0);
				glVertex2f(0.0,0.0);
				glVertex2f(0.0,1.0);
				// marks
				glVertex2f(PI_2 * 0.25, 0.0);
				glVertex2f(PI_2 * 0.25, -0.05);
				glVertex2f(PI_2 * 0.5 + PI_2 * 0.25, 0.0);
				glVertex2f(PI_2 * 0.5 + PI_2 * 0.25, -0.05);

				// 45° mark
				glVertex2f(PI_2 * 0.5, 0.0);
				glVertex2f(PI_2 * 0.5, -0.1);
				// 90° mark
				glVertex2f(PI_2, 0.0);
				glVertex2f(PI_2, -0.1);
				// p - marks
				glVertex2f(0.0, 0.5);
				glVertex2f(-0.1, 0.5);

			glEnd();

			glColor3f(0,0,0);
			glLineWidth(2.0);

			if (m_selected_type == Constant) {
				glBegin(GL_LINES);
					glVertex2f( 0.0, m_constant1 );
					glVertex2f( PI_2, m_constant1 );
				glEnd();
				return;
			} else 	{
				if (m_selected_type == DuoConstant) {
					glBegin(GL_LINE_STRIP);
						glVertex2f( 0.0, m_constant1 );
						glVertex2f( m_sep_angle, m_constant1 );
						glVertex2f( m_sep_angle, m_constant2 );
						glVertex2f( PI_2, m_constant2 );
					glEnd();
				} else if (m_selected_type == Linear) {
					T seperation = m_sep_angle;

					T y_lin_1 = m_linear_m * m_sep_angle + m_linear_n;
					if (y_lin_1 > 1.0)
					{
						y_lin_1 = 1.0;
						seperation = max(m_sep_angle, min(static_cast<T>(90.0), (static_cast<T>(1.0) - m_linear_n) / m_linear_m));
					} else if (y_lin_1 < 0.0) {
						y_lin_1 = 0.0;
					}

					T x_lin_0 = (0 - m_linear_n) / m_linear_m;
					T y_lin_0 = 0.0;
					if (x_lin_0 > PI_2) {
						x_lin_0 = PI_2;
						y_lin_0 = m_linear_m * PI_2 + m_linear_n;

					} else if (x_lin_0 < seperation) {
						x_lin_0 = seperation;
					}

					glBegin(GL_LINE_STRIP);
						glVertex2f( 0.0, m_constant1 );
						glVertex2f( seperation, m_constant1 );
						glVertex2f( seperation, y_lin_1 );
						glVertex2f( x_lin_0, y_lin_0 );
						glVertex2f( PI_2, 0.0 );
					glEnd();

					glBegin(GL_POINTS);
						glColor3f(1,0,0);
						glVertex2f(0.0, y_lin_1);
						glVertex2f(x_lin_0 , 0.0 );
					glEnd();
				} else {
					return;
				}

				glBegin(GL_LINES);
					glColor3f(0.1,0.9,0.0);
					glVertex2f(m_sep_angle, 1.0);
					glVertex2f(m_sep_angle, 1.05);
				glEnd();
			}
		}
	}
}
