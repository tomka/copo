#pragma once

#include <cgv/streamprocessing/streamprocessing.h>

#include <math.h>
using namespace cgv::streamprocessing;


#include <fltk/run.h>
#include <fltk/events.h>
#include <fltk/Window.h>
#include <fltk/GLWindow.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <fltk/visual.h>



namespace cgv {
	namespace streamprocessing {
		namespace imaging_nodes {



template< typename imagetype>
class  image_viewer : public ::fltk::GlWindow
{
private:
	GLuint texture;
	imagetype *localimage;
	double angle;
	
	bool update;
	int col;
	bool init_ok;
	float scale,oldy,oldx,posx,posy;
	
	int tex_width,tex_height;
	GLenum target;
	GLint internalformat;
	GLint interpolation;
    GLenum format;
	GLenum type;





public:

  
	image_viewer(int x,int y,int w,int h,const char *l=0,
		GLenum target=GL_TEXTURE_2D, GLint internalformat=GL_LUMINANCE8, 
		GLenum format=GL_LUMINANCE, GLenum type=GL_UNSIGNED_BYTE,GLint interpolation=GL_NEAREST)
    : GlWindow(x,y,w,h,l) 
	{
		
		this->target =target;
		this->internalformat = internalformat;
		this->format = format;
		this->type = type;
		this->interpolation = interpolation;
		scale =0;
		posx=0.0f;
		posy=0.0f;
		oldx=0.0f;
		oldy=0.0f;
		tex_width = (int)pow(2.0,ceil(log((double)localimage->width)/log(2.0)));
		tex_height = (int)pow(2.0,ceil(log((double)localimage->height)/log(2.0)));
		//tex_height = tex_width;
		
		
		localimage = new imagetype(0);
		
		
		update=true;
		init_ok=false;		
		::fltk::add_idle(idle_callback,this);
				
	}

	~image_viewer()
	{
		
		delete localimage;
		localimage=NULL;
		
		
	}

	bool ready()
	{
		return init_ok && visible();
	}
		


	void upload(imagetype *image)
	{

		
		
		if(localimage!= NULL)
			*localimage=*image;

		
		update=true;

				
		

	}

private:
	static inline void idle_callback(void* data)
	{
					((image_viewer*)data)->flush();
	}

		

	void draw()
	{
		int w = localimage->width;
		int h = localimage->height;
		int width = this->w();
		int height = this->h();
		
		if (!valid()) 
		{
				
			if(!init_ok)
			{
				//std::cout << "initGL"<<std::endl;
				glDisable(GL_DEPTH_TEST);

				// allocate a texture name
				glGenTextures( 1, &texture );
				

				glBindTexture( target, texture );
				glTexImage2D(target,0,internalformat,tex_width,tex_height,0,format,type,NULL);
				glTexParameteri(target,GL_TEXTURE_MIN_FILTER,interpolation );	
				glTexParameteri(target,GL_TEXTURE_MAG_FILTER,interpolation );	
				glClearColor(0.1f,0.1f,0.5f,1.0f);
				init_ok = true;
			}
			glMatrixMode(GL_PROJECTION);						
			glLoadIdentity();							
			gluOrtho2D(0,this->w(),0,this->h());
			glMatrixMode(GL_MODELVIEW);						
			glLoadIdentity();
			glViewport(0,0,this->w(),this->h());
			
		}
		glEnable(target);
		glBindTexture( target, texture );
		
		if(update)
		{
			
			glTexSubImage2D(target,0,0,0,localimage->width,localimage->height,format,type,localimage);
			update=false;
		
		}

		glClear(GL_COLOR_BUFFER_BIT);
		glLoadIdentity();

		
		float s = (float)pow(1.25f,scale);

		glTranslatef(this->w()/2.0f,this->h()/2.0f,0);
		glScalef(s,s,s);
		glTranslatef(posx,posy,0);
			

		glBegin(GL_QUADS);
		glTexCoord2f(0,h/(float)tex_height);
		glVertex3f(-w/2.0f,-w/1.25f/2.0f,0);
		
		glTexCoord2f(0,0);
		glVertex3f(-w/2.0f,(float)w/1.25f/2.0f,0);
		
		glTexCoord2f(w/(float)tex_width,0);
		glVertex3f((float)w/2,(float)w/1.25f/2.0f,0);
		
		glTexCoord2f(w/(float)tex_width,h/(float)tex_height);
		glVertex3f((float)w/2,-(float)w/1.25f/2.0f,0);
		glEnd();

		
		
	}

	int handle(int event) 
	{
		 switch(event) {
		case ::fltk::PUSH:
			if(fltk::event_button()==1)
			{
				oldx=(float)::fltk::event_x();
				oldy=(float)::fltk::event_y();
			}
			this->flush();
			return 1;

		case ::fltk::DRAG:
			if(::fltk::event_button()==1)
		{
			float s = (float)pow(1.25f,scale);
			posx += (float)(::fltk::event_x()-oldx)/s;
			posy += (float)(oldy-::fltk::event_y())/s;
			oldx=(float)::fltk::event_x();
			oldy=(float)::fltk::event_y();
			this->flush();
		}
		return 1;

  
		case ::fltk::MOUSEWHEEL:
			if(::fltk::event_dy() > 0)
				 scale++;
			 else
				 scale--;
			this->flush();
			 return 1;
 
		 default:
		// let the base class handle all other events:
			 return ::fltk::GlWindow::handle(event);
	 }
	}


	

	
};



template <typename imagetype>
class image_viewer_node: public stream_node
{
public:
	port_in<imagetype> input;
	port_out<imagetype> output;

	
	


private:
	
	imagetype *workingimage;
	image_viewer<imagetype> *gui;
	unsigned long sleep_value;

public:

	image_viewer_node(int buffersize=1,unsigned long sleepvalue=0):stream_node(),input(buffersize)
	{
		sleep_value = sleepvalue;
		register_port_in("image",&input);
		register_port_out("rendered image",&output);
		workingimage = new imagetype;
		gui = NULL;
	}

	
	~image_viewer_node()
	{	
		delete workingimage;
		

	}

	image_viewer<imagetype>* create_gui(int x, int y, int w,int h,const char* l=0,	GLenum target=GL_TEXTURE_2D, GLint internalformat=GL_LUMINANCE8, 
		GLenum format=GL_LUMINANCE, GLenum type=GL_UNSIGNED_BYTE,GLint interpolation=GL_NEAREST)
	{
		if(!gui)
		{
			gui = new image_viewer<imagetype>(x,y,w,h,l,target,internalformat,format,type,interpolation);
		}
		return gui;


		
	}

			
	void run()
	{
		std::cout << "start viewer\n";			
		while(no_stop_request() )
		{	
			input >> &workingimage;	
			
					
			if(workingimage)
			{
			
				if(gui &&gui->ready())
				{
					gui->upload(workingimage);
					
				}
				
				if(sleep_value)
					Sleep(sleep_value);
								
				output <<&workingimage;	
			}
			
			else
			{
				output <<&workingimage;	
				break;
			}
			
			
		}
		
		std::cout << "stop viewer\n";
	}
};

		}
	}
}