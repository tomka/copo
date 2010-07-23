#include <cgv/scanner/highspeed_projector.h>
#include <cgv/utils/big_binary_file.h>

namespace cgv {
	namespace scanner {

highspeed_projector::highspeed_projector()
{
	_initialized = false;
	pattern_filename="";
}

highspeed_projector::~highspeed_projector()
{
	
	if(_initialized)
	{
		stop();
		AlpDevHalt(_deviceid);
		AlpDevFree(_deviceid);
	}
}

bool highspeed_projector::initialize()
{
	_initialized = (AlpDevAlloc(ALP_DEFAULT,ALP_DEFAULT,&_deviceid)==ALP_OK);		
	return _initialized;
}

bool highspeed_projector::initialize(long serial_number)
{
	_initialized = (AlpDevAlloc(serial_number,ALP_DEFAULT,&_deviceid)==ALP_OK);
	return 	_initialized;	
}


void highspeed_projector::trigger_polarity(bool high)
{
	if(high)
		AlpDevControl(_deviceid,ALP_TRIGGER_POLARITY,ALP_LEVEL_HIGH);
	else
		AlpDevControl(_deviceid,ALP_TRIGGER_POLARITY,ALP_LEVEL_LOW);

}

bool highspeed_projector::trigger_polarity()
{
	long trigpol;
	AlpDevInquire(_deviceid,ALP_TRIGGER_POLARITY,&trigpol);
	return trigpol == ALP_LEVEL_HIGH;
}

void highspeed_projector::vd_edge(bool falling_edge)
{
	if(falling_edge)
		AlpDevControl(_deviceid,ALP_VD_EDGE,ALP_EDGE_FALLING);
	else
		AlpDevControl(_deviceid,ALP_VD_EDGE,ALP_EDGE_RISING);
}

bool highspeed_projector::vd_edge()
{
	long vdedge;
	AlpDevInquire(_deviceid,ALP_VD_EDGE,&vdedge);
	return vdedge == ALP_EDGE_FALLING;
}

long highspeed_projector::serial_device_number()
{
	long num;
	AlpDevInquire(_deviceid,ALP_DEVICE_NUMBER,&num);
	return num;
}

long highspeed_projector::ALP_version()
{
	long num;
	AlpDevInquire(_deviceid,ALP_VERSION,&num);
	return num;
}

long highspeed_projector::available_device_memory()
{
	long mem;
	AlpDevInquire(_deviceid,ALP_AVAIL_MEMORY,&mem);
	return mem;
}

bool highspeed_projector::is_device_busy()
{
	long state;
	AlpDevInquire(_deviceid,ALP_DEV_STATE,&state);
	return state == ALP_DEV_BUSY;
}

bool highspeed_projector::is_device_ready()
{
	long state;
	AlpDevInquire(_deviceid,ALP_DEV_STATE,&state);
	return state == ALP_DEV_READY;
}

bool highspeed_projector::is_device_idle()
{
	long state;
	AlpDevInquire(_deviceid,ALP_DEV_STATE,&state);
	return state == ALP_DEV_IDLE;
}

bool highspeed_projector::prepare_sequence(long bitplanes,long picnum,bool uninterrupted)
{
	AlpSeqAlloc(_deviceid,bitplanes,picnum,&_sequenceid);
	if(uninterrupted)
		return AlpSeqControl(_deviceid,_sequenceid,ALP_BIN_MODE,ALP_BIN_UNINTERRUPTED)== ALP_OK;
	else
		return AlpSeqControl(_deviceid,_sequenceid,ALP_BIN_MODE,ALP_BIN_NORMAL)== ALP_OK;

	
}

bool highspeed_projector::free_sequence()
{
	pattern_filename="";
	return (AlpSeqFree(_deviceid,_sequenceid)==ALP_OK);
}

bool highspeed_projector::upload_picture(long picindex,void *picturedata)
{
	return (AlpSeqPut(_deviceid,_sequenceid,picindex,1,picturedata) ==ALP_OK);
}

bool highspeed_projector::upload_picture_sequence(long picindex,long picnum,void *picturedata)
{
	return (AlpSeqPut(_deviceid,_sequenceid,picindex,picnum,picturedata)==ALP_OK);
}

int highspeed_projector::load_pattern_sequence_from_file(const std::string &filename)
{
	if(is_projecting())
	{
		this->stop();
		free_sequence();
	}


	cgv::utils::big_binary_file file(filename);
	int number_of_pattern = (int)(file.size()/(1024*768));
	file.open(cgv::utils::big_binary_file::READ);

	prepare_sequence(1,number_of_pattern,true);
	unsigned char *data = new unsigned char[1024*768];
	int i =0;
	int j = number_of_pattern;
	while (j > 0)
	{
		file.read(data,1024*768);
		upload_picture(i,data);
		i++;
		j--;
		
	}	
	delete [] data;
	if(number_of_pattern > 0)
		pattern_filename=filename;
	
	return number_of_pattern;
}



void highspeed_projector::set_master()
{
    AlpProjHalt(_deviceid);
	
	AlpProjControl(_deviceid,ALP_PROJ_MODE,ALP_SLAVE_VD);    
}

void highspeed_projector::set_slave()
{
    AlpProjHalt(_deviceid);
	
	AlpProjControl(_deviceid,ALP_PROJ_MODE,ALP_SLAVE_VD);    
}

bool highspeed_projector::is_master()
{
	long mode;
	AlpProjInquire(_deviceid,ALP_PROJ_MODE,&mode);
	return (mode == ALP_MASTER);
}

bool highspeed_projector::is_slave()
{
	long mode;
	AlpProjInquire(_deviceid,ALP_PROJ_MODE,&mode);
	return (mode != ALP_MASTER);
}



bool highspeed_projector::is_projecting()
{
	long isprojecting;
	AlpProjInquire(_deviceid,ALP_PROJ_STATE,&isprojecting);
	return (isprojecting == ALP_PROJ_ACTIVE);
}

bool highspeed_projector::start()
{		
	return (AlpProjStartCont(_deviceid,_sequenceid)==ALP_OK);	
}

bool highspeed_projector::start_one_sequence()
{		
	AlpProjStartCont(_deviceid,_sequenceid);	
	return (AlpProjHalt(_deviceid)==ALP_OK);
}

bool highspeed_projector::stop()
{
	return (AlpProjHalt(_deviceid)==ALP_OK);
}

std::string highspeed_projector::pattern_sequence_filename()
{
	return pattern_filename;
}



bool highspeed_projector::set_sequence_properties(long first, long last,long bitnum)
{
	long illuminatetime,picturetime,triggerdelay,triggerpulsewidth,vddelay;
	get_sequence_timing(&illuminatetime,&picturetime,&triggerdelay,&triggerpulsewidth,&vddelay);
	
	if(AlpSeqControl(_deviceid,_sequenceid,ALP_FIRSTFRAME,first)!= ALP_OK)
		return false;
	if(AlpSeqControl(_deviceid,_sequenceid,ALP_LASTFRAME,last)!= ALP_OK)
		return false;

	if(AlpSeqControl(_deviceid,_sequenceid,ALP_BITNUM,bitnum)!= ALP_OK)
		return false;
	
	return AlpSeqTiming(_deviceid,_sequenceid,illuminatetime,picturetime,triggerdelay,
		triggerpulsewidth,vddelay)==ALP_OK;
}

bool highspeed_projector::get_sequence_properties(long *first, long *last,long *bitnum)
{
	
	if(AlpSeqInquire(_deviceid,_sequenceid,ALP_FIRSTFRAME,first)!= ALP_OK)
		return false;
	if(AlpSeqInquire(_deviceid,_sequenceid,ALP_LASTFRAME,last)!= ALP_OK)
		return false;
	return (AlpSeqInquire(_deviceid,_sequenceid,ALP_BITNUM,bitnum)== ALP_OK);
}

long highspeed_projector::number_of_bit_planes()
{
	long bitplanes;
	AlpSeqInquire(_deviceid,_sequenceid,ALP_BITPLANES,&bitplanes);
	return bitplanes;
		
}
	
long highspeed_projector::number_of_pics()
{
	long picnum;
	AlpSeqInquire(_deviceid,_sequenceid,ALP_PICNUM,&picnum);
	return picnum;
}
	
long highspeed_projector::min_picture_time()
{
	long minpictime;
	AlpSeqInquire(_deviceid,_sequenceid,ALP_MIN_PICTURE_TIME,&minpictime);
	return minpictime;

}

long highspeed_projector::min_illuminate_time()
{
	long itime;
	AlpSeqInquire(_deviceid,_sequenceid,ALP_ILLUMINATE_TIME,&itime);
	return itime;
}

bool highspeed_projector::set_sequence_timing(long illuminatetime,long picturetime, long triggerdelay, 
										   long triggerpulsewidth, long vddelay)
{
	
	return AlpSeqTiming(_deviceid,_sequenceid,illuminatetime,picturetime,triggerdelay,
		triggerpulsewidth,vddelay)==ALP_OK;
}

bool highspeed_projector::get_sequence_timing(long *illuminatetime,long *picturetime, long *triggerdelay, 
										   long *triggerpulsewidth, long *vddelay)
{
	if(AlpSeqInquire(_deviceid,_sequenceid,ALP_ILLUMINATE_TIME,illuminatetime)!= ALP_OK)
		return false;
	if(AlpSeqInquire(_deviceid,_sequenceid,ALP_PICTURE_TIME,picturetime)!= ALP_OK)
		return false;
	if(AlpSeqInquire(_deviceid,_sequenceid,ALP_TRIGGER_DELAY,triggerdelay)!= ALP_OK)
		return false;
	if(AlpSeqInquire(_deviceid,_sequenceid,ALP_TRIGGER_PULSEWIDTH,triggerpulsewidth)!= ALP_OK)
		return false;
	return (AlpSeqInquire(_deviceid,_sequenceid,ALP_VD_DELAY,vddelay)== ALP_OK);	
}

long highspeed_projector::picture_time()
{
	long picturetime;
	AlpSeqInquire(_deviceid,_sequenceid,ALP_PICTURE_TIME,&picturetime);
	return picturetime;
}

long highspeed_projector::illuminate_time()
{
	long illuminatetime;
	AlpSeqInquire(_deviceid,_sequenceid,ALP_ILLUMINATE_TIME,&illuminatetime);
	return illuminatetime;
}

long highspeed_projector::dark_time()
{
	long picturetime,illuminatetime;
	picturetime = picture_time();
	illuminatetime = illuminate_time();
	return picturetime - illuminatetime;

}

long highspeed_projector::max_trigger_delay()
{
	long maxtriggerdelay;	
	AlpSeqInquire(_deviceid,_sequenceid,ALP_MAX_TRIGGER_DELAY,&maxtriggerdelay);	
	return maxtriggerdelay;

}

long highspeed_projector::max_vd_delay()
{
	long maxvddelay;	
	AlpSeqInquire(_deviceid,_sequenceid,ALP_MAX_VD_DELAY,&maxvddelay);	
	return maxvddelay;
}

	}
}







