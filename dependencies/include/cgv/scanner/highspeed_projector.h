#pragma once
#include <cgv/config/global.h>

#include <alp.h>
#include <string>

#include <cgv/scanner/lib_begin.h>

namespace cgv {
	namespace scanner {

	/**
	* highspeed_projector class to access vialux highspeed projector hardware (zSnapper).
	*
	* 1. Initialize 
	* 2. Set Trigger parameters (optional)
	* 3. Sequence Setup
	* 4. Set additional sequence control parameters
	* 5. Set additional sequence timing parameters
	* 6. Projector setup (optional)
	* 7. Run projection
	*/
	class CGV_API highspeed_projector
	{
	private:
		ALP_ID _deviceid;
		ALP_ID _sequenceid;
		bool _initialized;
		std::string pattern_filename;


	public:
		///standard constructor
		highspeed_projector();
		
		///destructor
		virtual ~highspeed_projector();
		
		///initialize alp default device 
		bool initialize();

		///initialize device identified by serialnumber
		bool initialize(long serialnumber);

		
		///serial number of device
		long serial_device_number();

		///version umber of alp api
		long ALP_version();

		///available device memory (number of images)
		long available_device_memory();

		///test if device is busy
		bool is_device_busy();
		///test if device is ready
		bool is_device_ready();
		///test if device is idle
		bool is_device_idle();
		
		///sequence setup
		bool prepare_sequence(long bitplanes, long picnum, bool uninterrupted);
		
		///upload single image 
		bool upload_picture(long picindex,void *picturedata);
		
		///upload image sequence
		bool upload_picture_sequence(long picindex,long picnum,void *picturedata);

		///prepare and upload image sequence from binary file
		int load_pattern_sequence_from_file(const std::string &filename);

		///get loaded pattern sequence filename if available
		std::string pattern_sequence_filename();
		

		///free sequence
		bool free_sequence();
		
		///set sequence control parameters
		bool set_sequence_properties(long first, long last,long bitnum);
		
		///get sequence control parameters
		bool get_sequence_properties(long *first, long *last,long *bitnum);

		///return the number of bit planes
		long number_of_bit_planes();

		///return number of pics in uploaded image sequence
		long number_of_pics();
	   
		///set trigger polarity high/low 
		void trigger_polarity(bool high);

		///return true if trigger polarity is high
		bool trigger_polarity();
		
		///set vd edge falling edge /rising edge
		void vd_edge(bool falling_edge);

		/// return true if vd edge is set to falling edge
		bool vd_edge();
		
		///return the minimum picture time
		long min_picture_time();
		///return the minimum illumination time
		long min_illuminate_time();
		///return the maximum trigger delay time
		long max_trigger_delay();
		///return the maximum vd delay time
		long max_vd_delay();

		///set projection sequence parameters
		bool set_sequence_timing(long illuminatetime,long picturetime=ALP_DEFAULT, long triggerdelay=ALP_DEFAULT, 
							   long triggerpulsewidth=ALP_DEFAULT, long vddelay=ALP_DEFAULT);
		//get projection sequence parameters
		bool get_sequence_timing(long *illuminatetime,long *picturetime, long *triggerdelay, 
							   long *triggerpulsewidth, long *vddelay);

		///return curent picture time picturetime = illumination time + dark time
		long picture_time();
		///return current illumination time
		long illuminate_time();
		///return current darktime 
		long dark_time();


		///projector is controlled by its internal trigger
		void set_master();
		///projector is contolled by externel trigger
		void set_slave();
		///return true if projector is in master mode
		bool is_master();
		///return true if projector is in slave mode
		bool is_slave();
		///return true if projector is working
		bool is_projecting();

		

		///run projection in loop until stop is called
		bool start();
		///run inly one sequence
		bool start_one_sequence();
		///stop loop of projection
		bool stop();
		

	};
	


	}
}
#include <cgv/config/lib_end.h>


