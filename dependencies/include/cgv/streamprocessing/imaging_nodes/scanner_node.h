#pragma once
#include <cgv/scanner/highspeed_camera.h>
#include <cgv/scanner/highspeed_projector.h>

#include <cgv/streamprocessing/imaging_nodes/raw_image.h>
#include <cgv/streamprocessing/streamprocessing.h>

namespace cgv {
	namespace streamprocessing {
		namespace imaging_nodes {

template<typename image_type>
class scanner_node: public stream_node
{
public:
	port_out<image_type> output;
	
		

private:
	cgv::scanner::highspeed_camera *camera;
	cgv::scanner::highspeed_projector *projector;
	
	image_type *workingimage;
	bool loop;
	
	

public:

	scanner_node(cgv::scanner::highspeed_camera *camera,cgv::scanner::highspeed_projector *projector,bool loop)
		: stream_node()
	{
		register_port_out("camera image",&output);
		
		workingimage = new image_type;
		this->camera = camera;
		this->projector = projector;
		this->loop = loop;
		projector->vd_edge(false);
		projector->set_slave();
	}

	~scanner_node()
	{

		delete workingimage;
	}
	
		
	void run()
	{
		bool complete=false;
		bool notfirst=false;
		unsigned int currentframe=0,lastframe=0;
		unsigned int framecpy=0;
		unsigned int bufnum;
		//unsigned int last_valid_buffer_index;
		unsigned char *bufaddr=NULL;
		std::cout << "start scanner node\n";	

		unsigned int size=0;

		camera->image_buffer_size(&size);
		projector->start();
		//Sleep(1000);
		
				
		if(loop)
		{
			
			camera->start(true);
		//	std::cout <<"starting:" <<b<<std::endl;
		
			
			

			while(!complete)
			{
				if(no_stop_request())
				{
					currentframe = camera->last_valid_frame();
					
					

					if(currentframe - framecpy > 10)
								std::cout << framecpy << ":" << currentframe<<std::endl;
					if(currentframe > framecpy)                								
					{

						if(camera->examine_buffer(framecpy,&bufnum,&bufaddr))
						{
							
							memcpy(workingimage->data,bufaddr,size);
							camera->release_buffer();
							framecpy++;
								
							if(notfirst)
								output << &workingimage;
							else
								notfirst=true;
							
						}
						
							
												
					}
					
				}
				else
				{
					camera->stop();
					complete=true;
					projector->stop();

				}			
				
			}
		}
		else
		{
			
			camera->start(false);
			projector->stop();
			
			for(int i = 0; i < camera->number_of_buffers();i++)
			{
				camera->copy_buffer(i+1,workingimage->data,false);
				output << &workingimage;
			}
			
			


		}

		image_type *null= NULL;
		output << &null;
		

		
		std::cout << "stop scanner node\n";
	}

		
};

		}
	}
}