#pragma once
#include <cgv/config/global.h>

#define _NIWIN  
#include <niimaq.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cgv/scanner/acq_parameters.h>
#include <cgv/scanner/lib_begin.h>

namespace cgv {
	namespace scanner {



//Exposure time control modes
#define FREE_RUN_PROGRAMMABLE 0x00
#define FREE_RUN_VGA 0x08
#define EX_SYNC_PROGRAMMABLE 0x05
#define EX_SYNC_LEVEL_CONTROLLED 0x04
#define EX_SYNC_EDGE_CONTROLLED 0x06

//digital shift values
#define DIGITAL_SHIFT_NONE 0x00 //no digital shift
#define DIGITAL_SHIFT_1 0x01 // multiplies output x2
#define DIGITAL_SHIFT_2 0x02 // multiplies output x4
#define DIGITAL_SHIFT_3 0x03 // multiplies output x8



class CGV_API highspeed_camera
{
private:
	INTERFACE_ID iid;
	
	BUFLIST_ID   bid;
	unsigned char **imagebuffers;
	unsigned int *skipbuffer;
	
	bool framerateold;
	double frate;
	bool exposuretimeold;
	int exptime;
	int num_buffers;


	bool ask_camera(unsigned char command_id, unsigned char length, void *data);
	bool tell_camera(unsigned char command_id, unsigned char length, void *data);

	SESSION_ID sid;

public:

	

	highspeed_camera(void);
	~highspeed_camera(void);


	bool initialize();

	std::string vendor();
	
	std::string model();
	
	std::string product_id();
	
	std::string serial_number();

	unsigned char temperature();

	
	void standby(bool st);
	
	bool is_in_standby();

	int number_of_buffers();

	
	void setup_exposure_time_control_FreeRunProg(int timer1, int timer2);
	
	void setup_exposure_time_control_FreeRunVGA(int timer1);
    
	void setup_exposure_time_control_ExSyncProg(int timer1);
	
	void setup_exposure_time_control_ExSyncLevel();
	
	void setup_exposure_time_control_ExSyncEdge();

	void timer1(int time);
	int timer1();

	void timer2(int time);
	int timer2();

	
	//framerate in fps
	double framerate();

	// exposure time in µs 
	int exposure_time();

	void exposure_time_control_mode(unsigned char mode);
	unsigned char exposure_time_control_mode();

	void long_exposure_compensation(unsigned char value);
	unsigned char long_exposure_compensation();

	void offset(int offset);
	int offset();

	unsigned char digital_shift();
	void digital_shift(unsigned char shift);

	// optical gain in DN/(lx*s)
	double optical_gain();
	void optical_gain(double gain);

	double min_gain();
	double max_Gain();

	void reset_camera();
	void reset_interface();
	
	
	
	///width must be devisable by 10 minimum is 10; height >=2
	bool set_roi_window(unsigned int left=0,unsigned int top=0,unsigned int width=1280,unsigned int height=1024);
	bool get_roi_window(unsigned int *left,unsigned int *top,unsigned int *width,unsigned int *height);

	
	bool row_pixels(unsigned int rowpixels);
	bool row_pixels(unsigned int *rowpixels);

 	bool bits_per_pixel(unsigned int *bitsperpixel);
	bool bytes_per_pixel(unsigned int *bytesperpixel);
	bool image_buffer_size(unsigned int *buffersize);




	     
	bool prepare_buffers(unsigned char *imagebuffers[],unsigned int skippedBuffers[],unsigned count,bool loop);
	
	bool prepare_grab();

	bool prepare_ring(unsigned int count);

	bool prepare_triggered_ring(unsigned int count);

	bool prepare_snap();

	bool prepare_sequence(unsigned int count);

	bool prepare_triggered_sequence(unsigned int count);

	bool free_buffers();


	bool start(bool async,CALL_BACK_PTR callback =NULL);
	
	unsigned int last_valid_buffer_index();
	unsigned int last_valid_frame();
	
	bool copy_buffer(unsigned int bufferindex,unsigned char *destbuffer,bool waitfornext);

	bool examine_buffer(unsigned int whichBuffer, unsigned int* bufferNumber, unsigned char** bufferAddr);
	bool release_buffer();
	

	void drive_trigger();

	
	acq_parameters extract_acq_parameters()
	{
		acq_parameters params;
		get_roi_window(&(params.x),&(params.y),&(params.width),&(params.height));
		params.expt = exposure_time();
		params.framerate = framerate();
		params.gain = optical_gain();
		params.ds = digital_shift();
		return params;
	}


    	
	bool stop(unsigned int *lastavailablebufferindex);
	
	bool stop();

	bool is_running();

	




/*

	//Helper functions
	bool SaveImageBuffer(const char *filename,unsigned char* imagebuffer);
	bool PlotImage(GUIHNDL hwnd,unsigned int imagewidth,unsigned int imageheight,unsigned char* imagebuffer);
	
	*/

	
};





	}
}
#include <cgv/config/lib_end.h>