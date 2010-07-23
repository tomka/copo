#include <cgv/scanner/highspeed_camera.h>
#include <math.h>
#include <memory.h>
#include <iostream>
#pragma warning (disable: 4311)

namespace cgv {
	namespace scanner {

unsigned int packframe(void *buffer,unsigned char command_id,bool write,unsigned char length,const void *data)
{
	unsigned char *frame = (unsigned char*)buffer;
	//little endian
	int i =0;
	unsigned char bcc;
//	*frame = new unsigned char[1+2+length+1+1];

	frame[0] = 0x02; // STX

	frame[1] = command_id; //Command ID
	bcc = frame[1];

	if (length < 0 || length > 0x7F)
		return NULL;//incorrect length (0..127)
	if(write)
        frame[2] = length;
	else
		frame[2] = 0x80 | length;
	bcc = bcc ^ frame[2];

	if(write)
	{
		for(i = 0; i < length;i++)
		{
			frame[3+i]=((unsigned char*)data)[i];
			bcc = bcc ^ frame[3+i];
		}
	}

	frame[3+i] = bcc;//BCC
	frame[4+i] = 0x03; //ETX
    
  
	return 4+i+1;

}


bool unpackframe(const void  *buffer,unsigned char *command_id,bool *write,unsigned char *length,void **data)
{
	//little endian
	int i=0;
	unsigned char bcc;
	unsigned char *frame = (unsigned char*)buffer;
	
    if(frame[0] != 0x02)
		return false;// STX

	*command_id = frame[1]; //Command ID
	bcc = frame[1];

	*write = (frame[2] & 0x80) == 0;
	*length = frame[2] & 0x7F;
	bcc = bcc ^ frame[2];
	*data = (void*) new unsigned char[*length];

	if(*write)
	{
		for(i = 0; i < *length;i++)
		{
			((unsigned char*)(*data))[i]=frame[3+i];
			bcc = bcc ^ frame[3+i];
		}
	}

	if(frame[3+i] != bcc)
		return false;//BCC
	if(frame[4+i] != 0x03)
		return false; //ETX
	return true;
    
  
}



bool highspeed_camera::tell_camera(unsigned char command_id,unsigned char length,void *data)
{
	 //flush
	if(imgSessionSerialFlush(sid)< 0)
		return false;

	//request
	unsigned char* frame=new unsigned char[5+length];
		
	

	unsigned int size = packframe(frame,command_id,true,length,data);
	if(imgSessionSerialWrite(sid,(Int8*)frame,(uInt32*)&size,1000)< 0)
	{
		delete[] ((unsigned char*)frame);
		return false;
	}

	delete[] ((unsigned char*)frame);
	unsigned char ack;
	size=1;
	imgSessionSerialReadBytes(sid,(Int8*)&ack,(uInt32*)&size,1000);
	return (ack == 0x06);
		
}

bool highspeed_camera::ask_camera(unsigned char command_id,unsigned char length,void *data)
{
	if(imgSessionSerialFlush(sid)< 0)
		return false;

	unsigned char* frame=new unsigned char[5];

	unsigned int size = packframe(frame,command_id,false,length,NULL);
	if(imgSessionSerialWrite(sid,(Int8*)frame,(uInt32*)&size,1000)< 0)
	{
		delete[] frame;
		return false;
	}

	delete[] frame;
		
	unsigned char ack;
	size=1;
	imgSessionSerialReadBytes(sid,(Int8*)&ack,(uInt32*)&size,1000);
	if(ack != 0x06)
		return false;

	frame=new unsigned char[5+length];

	size=5+length;
	imgSessionSerialReadBytes(sid,(Int8*)frame,(uInt32*)&size,1000);

	
	unsigned char command_idback,l;
	bool write;
	unsigned char *buff = NULL;
	
	

	if(unpackframe(frame,&command_idback,&write,&l,(void **)&buff))
	{
		if(command_idback == command_id&& write == true && l == length )
		{
			memcpy(data,buff,length);
			delete[] buff;
			delete[] frame;
			return true;
		}
			
	}

	delete[] (unsigned char*)frame;
	return false;

}



highspeed_camera::highspeed_camera(void)
{
	exposuretimeold = true;
	framerateold=true;
	iid = 0;
	sid = 0;	
	imagebuffers=NULL;
	skipbuffer=NULL;
	num_buffers=0;
	
}

highspeed_camera::~highspeed_camera(void)
{
	if(sid)
		imgClose (sid, TRUE);
	if(iid)
		imgClose (iid, TRUE);
	free_buffers();
}

bool highspeed_camera::initialize()
{
	
	if(imgInterfaceOpen("img0",&iid) < 0)
		return false;
	
	if(imgSessionOpen(iid, &sid) < 0)
		return false;
	return true;
}

/*bool highspeed_camera::reset_to_hardware_defaults()
{
	return (imgInterfaceReset(iid)>= 0);
}*/


std::string highspeed_camera::vendor()
{
	unsigned char vendor[16];
	ask_camera(0x01,16,vendor);
	return std::string((const char *)vendor);
}

std::string highspeed_camera::model()
{
	unsigned char model[16];
	ask_camera(0x02,16,model);
	return std::string((const char *)model);
}

std::string highspeed_camera::product_id()
{
	unsigned char productid[16];
	ask_camera(0x03,16,productid);
	return std::string((const char *)productid);
}

std::string highspeed_camera::serial_number()
{
	unsigned char serial[16];
	ask_camera(0x04,16,serial);
	return std::string((const char *)serial);


}

unsigned char highspeed_camera::temperature()
{
	unsigned char temperature;
	ask_camera(0x70,1,&temperature);
    return temperature;
}

unsigned char highspeed_camera::digital_shift()
{
	unsigned char shift;
	ask_camera(0xA5,1,&shift);
	return shift;
}

void highspeed_camera::digital_shift(unsigned char shift)
{
	if(shift == 0 || shift == 1 || shift == 2 || shift == 3)
		tell_camera(0xA5,1,&shift);
}


double highspeed_camera::optical_gain()
{
	
	unsigned char gain;
	ask_camera(0x80,1,&gain);
	double g = (double)gain;
	g-=295.38; 
	g*=-(0.0000203125/1.6);
	
    g= 1/g;
	return floor(g*10.0+0.5)/10.0;
}

void highspeed_camera::optical_gain(double gain)
{
	if(gain < 267)
		gain = 267;
	if(gain > 800)
		gain = 800;

	

	unsigned char gaina[1];
	if(gain == 0)
		gain=1;
	
	//double ga =295.38-(1.6/0.0000203125)*(1.0/gain);
	double ga = -1.0/(gain*(0.0000203125)/(1.6))+295.38 +0.5;


	gaina[0]=(ga>197?197:(unsigned char)ga);

	tell_camera(0x80,1,gaina);
}








void  highspeed_camera::exposure_time_control_mode(unsigned char mode)
{
	if (mode == 0x00 ||mode ==0x04 || mode ==0x05 ||mode ==0x06 || mode == 0x08)
	{
		tell_camera(0xA0,1,&mode);
	}

}


unsigned char highspeed_camera::exposure_time_control_mode()
{
	unsigned char mode;
	ask_camera(0xA0,1,&mode);
	return mode;

}


void highspeed_camera::long_exposure_compensation(unsigned char value)
{
	if(value < 0x46)
		value = 0x46;
	if(value > 0xA0)
		value =0xA0;
	tell_camera(0xBD,1,&value);
	

}

unsigned char highspeed_camera::long_exposure_compensation()
{
	unsigned char lec;
	ask_camera(0xBD,1,&lec);
	return lec;
}


void highspeed_camera::timer1(int time)
{
	
	
    	
	if(time < 10)
		time = 10;
	if(time > 33000)
		time = 33000;


	unsigned char buff[3];

	buff[0] = 0x000000FF & time;
	buff[1] = (0x0000FF00 & time)>>8;
	buff[2] = (0x00FF0000 & time)>>16;
	tell_camera(0xA6,3,buff);
	framerateold=true;
	exposuretimeold=true;
	

}

int highspeed_camera::timer1()
{
	unsigned char buff[3];
	ask_camera(0xA6,3,buff);
	int t = 0;
	t = buff[2];
	t = t <<8;
	t+=buff[1];
	t = t <<8;
	t+=buff[0];
	return t;
}

void highspeed_camera::timer2(int time)
{

	if(time < 3) time = 3;
	//if(time > 33554430) time = 33554430;
	if(time > 16777215) time = 16777215;

	unsigned char buff[3];
	buff[0] = 0x000000FF & time;
	buff[1] = (0x0000FF00 & time)>>8;
	buff[2] = (0x00FF0000 & time)>>16;

	tell_camera(0xA7,3,buff);
	framerateold=true;
	exposuretimeold =true;

}

int highspeed_camera::timer2()
{
	unsigned char buff[3];
	ask_camera(0xA7,3,buff);
	int t = 0;
	t = buff[2];
	t = t <<8;
	t+=buff[1];
	t = t <<8;
	t+=buff[0];
	return t;
}





void highspeed_camera::offset(int offs)
{
	unsigned char zero =0;
	if (offs < 0)
	{
		offs = abs(offs);
		unsigned char value = offs > 255 ? 255 : offs;
		tell_camera(0x86,1,&zero);
		tell_camera(0x84,1,&value);	
		
		
	}else
	{
		offs = abs(offs);
		unsigned char value = offs > 255 ? 255:offs;
		tell_camera(0x84,1,&zero);
		tell_camera(0x86,1,&value);
	}
}

int highspeed_camera::offset()
{
	unsigned char negoff,posoff;

	
	ask_camera(0x84,1,&negoff);
	ask_camera(0x86,1,&posoff);	
		
		
	return (int)posoff-(int)negoff;
}


void highspeed_camera::setup_exposure_time_control_FreeRunProg(int t1, int t2)
{
	exposuretimeold=true;
	framerateold=true;
	exposure_time_control_mode(0x00);
	timer1(t1);
	timer2(t2);
}
	
void highspeed_camera::setup_exposure_time_control_FreeRunVGA(int t1)
{
	exposuretimeold=true;
	framerateold=true;
	exposure_time_control_mode(0x08);
	timer1(t1);
}
    
void highspeed_camera::setup_exposure_time_control_ExSyncProg(int t1)
{
	exposuretimeold=true;
	framerateold=true;
	exposure_time_control_mode(0x05);
	timer1(t1);
}
	
void highspeed_camera::setup_exposure_time_control_ExSyncLevel()
{
	exposuretimeold=true;
	framerateold=true;
	exposure_time_control_mode(0x04);	
}
	
void highspeed_camera::setup_exposure_time_control_ExSyncEdge()
{
	exposuretimeold=true;
	framerateold=true;
	exposure_time_control_mode(0x06);
}



void highspeed_camera::standby(bool st)
{
	unsigned char value;
	if(st)
		value = 0x00;
	else
		value =0x01; 

	tell_camera(0xA2,1,&value);
}

bool highspeed_camera::is_in_standby()
{
	unsigned char standby;
	ask_camera(0xA2,1,&standby);
	return standby == 0x00;
}





int highspeed_camera::exposure_time()
{
	if(exposuretimeold)
	{
		unsigned char mode = exposure_time_control_mode();
		if(mode ==0x00 || mode == 0x05 || mode ==0x08)
		{
			exptime = timer1();
			return exptime;
		}
		else
		{
			exptime = -1;
			return -1;
		}
		exposuretimeold=false;
	}else
	{
		return exptime;
	}
	
}

double highspeed_camera::framerate()
{
	if(framerateold)
	{
		unsigned char mode =exposure_time_control_mode();
		if(mode ==0x00)
		{
			int t1 =timer1();
			int t2 =timer2();
			frate = 1000000.0/(t1+t2);
			//framerate = floor(framerate *10.0+0.5)/10.0;
			
		}else if(mode == 0x08)
		{
			
			frate = 60.0;
		}
		else frate =-1;
		framerateold =false;
		return frate;
	}
	else return frate;
}



/*
bool highspeed_camera::SetROIWindowWidth(unsigned int width)
{
	return (imgSetAttribute (sid, IMG_ATTR_ROI_WIDTH, width) >= 0);
}

bool highspeed_camera::SetROIWindowHeight(unsigned int height)
{
	return imgSetAttribute (sid, IMG_ATTR_ROI_HEIGHT, height) >= 0;
}

bool highspeed_camera::SetROIWindowTop(unsigned int top)
{
	return (imgSetAttribute (sid, IMG_ATTR_ROI_TOP, top) >= 0);
}

bool highspeed_camera::SetROIWindowLeft(unsigned int left)
{
	return imgSetAttribute (sid, IMG_ATTR_ROI_LEFT, left) >= 0;
}

bool highspeed_camera::GetROIWindowTop(unsigned int *top)
{
	return imgGetAttribute (sid, IMG_ATTR_ROI_TOP,(void*) top) >= 0;
		
}

bool highspeed_camera::GetROIWindowLeft(unsigned int *left)
{
	return imgGetAttribute (sid, IMG_ATTR_ROI_LEFT,(void*) left)>= 0;
}


bool highspeed_camera::GetROIWindowWidth(unsigned int *width)
{
	return imgGetAttribute (sid, IMG_ATTR_ROI_WIDTH,(void*) width) >= 0;
}

bool highspeed_camera::GetROIWindowHeight(unsigned int *height)
{
	return imgGetAttribute (sid, IMG_ATTR_ROI_HEIGHT,(void*) height)>= 0;		
}

bool highspeed_camera::GetRowPixels(unsigned int *rowpixels)
{
	return imgGetAttribute (sid, IMG_ATTR_ROWPIXELS,rowpixels) >= 0;
}

bool highspeed_camera::SetRowPixels(unsigned int rowpixels)
{
	return imgSetAttribute (sid, IMG_ATTR_ROWPIXELS,rowpixels) >= 0;
}
*/
bool highspeed_camera::set_roi_window(unsigned int left,unsigned int top,unsigned int width,unsigned int height)
{
	uInt32 ftop,fleft,fwidth,fheight;
	if(imgSessionFitROI(sid,IMG_ROI_FIT_LARGER, top,  left, height,  width, &ftop, & fleft, &fheight, &fwidth)==0)
	{
		int r =imgSessionConfigureROI(sid,ftop,fleft,fheight,fwidth);
		if(r!=0)
		{
			char text[256];
			imgShowError(r, text);
		}
		return r==0;
	}
	else return false;
}

bool highspeed_camera::get_roi_window(unsigned int *left,unsigned int *top,unsigned int *width,unsigned int *height)
{
	return imgSessionGetROI(sid,(uInt32*)top,(uInt32*)left,(uInt32*)height,(uInt32*)width)==0;
}

bool highspeed_camera::bits_per_pixel(unsigned int* bitsperpixel)
{
	return (imgGetAttribute (sid, IMG_ATTR_BITSPERPIXEL, &bitsperpixel) >= 0);
}

bool highspeed_camera::bytes_per_pixel(unsigned int* bytesperpixel)
{
	return (imgGetAttribute (sid, IMG_ATTR_BYTESPERPIXEL, &bytesperpixel) >= 0);
}

bool highspeed_camera::image_buffer_size(unsigned int *buffersize)
{
	uInt32 s;
	int r =imgSessionGetBufferSize(sid,(uInt32*)&s);
	*buffersize=(unsigned int)s;
	return (r== 0);
}

/*
bool highspeed_camera::SaveImageBuffer(const char *filename,unsigned char* imagebuffer)
{
	return (imgSessionSaveBufferEx(sid,(Int8 *)imagebuffer,(Int8 *)filename)>=0);
}

void highspeed_camera::ResetCamera()
{
	unsigned char value[2];
	value[0]=0xCF;
	value[1]=0x07;
	TellCamera(0x42,2,value);
}
*/







bool highspeed_camera::prepare_grab()
{
	num_buffers=1;
	if(imagebuffers!= NULL)
		free_buffers();
	imagebuffers= new unsigned char*[1];
	skipbuffer= new unsigned int[1];
	imagebuffers[0]=NULL;
	skipbuffer[0]=0;

	return prepare_buffers(imagebuffers,skipbuffer,1,true);
}

bool highspeed_camera::prepare_ring(unsigned int count)
{
	num_buffers=count;
	if(imagebuffers!= NULL)
		free_buffers();
	imagebuffers= new unsigned char*[count];
	skipbuffer= new unsigned int[count];
	for(unsigned int i = 0; i < count;i++)
	{
		imagebuffers[i]=NULL;
		skipbuffer[i]=0;
	}

	return prepare_buffers(imagebuffers,skipbuffer,count,true);
}
bool highspeed_camera::prepare_triggered_ring(unsigned int count)
{
	num_buffers=count;
	drive_trigger();
	if(imagebuffers!= NULL)
		free_buffers();

	imagebuffers= new unsigned char*[count+1];
	skipbuffer= new unsigned int[count+1];
	for(unsigned int i = 0; i < count+1;i++)
	{
		imagebuffers[i]=NULL;
		skipbuffer[i]=0;
	}
	

	return prepare_buffers(imagebuffers,skipbuffer,count,true);
}

bool highspeed_camera::prepare_snap()
{
	num_buffers=1;
	if(imagebuffers!= NULL)
		free_buffers();
	imagebuffers= new unsigned char*[1];
	skipbuffer= new unsigned int[1];
	imagebuffers[0]=NULL;
	skipbuffer[0]=0;

	return prepare_buffers(imagebuffers,skipbuffer,1,false);
}

void highspeed_camera::drive_trigger()
{
	imgSessionTriggerDrive2(sid, IMG_SIGNAL_EXTERNAL,2, IMG_TRIG_POLAR_ACTIVEL, IMG_TRIG_DRIVE_FRAME_START );
}

bool highspeed_camera::prepare_sequence(unsigned int count)
{
	num_buffers=count;
	
	if(imagebuffers!= NULL)
		free_buffers();
	imagebuffers= new unsigned char*[count];
	skipbuffer= new unsigned int[count];
	for(unsigned int i = 0; i < count;i++)
	{
		imagebuffers[i]=NULL;
		skipbuffer[i]=0;
	}

	return prepare_buffers(imagebuffers,skipbuffer,count,false);
}
bool highspeed_camera::prepare_triggered_sequence(unsigned int count)
{
	num_buffers=count;
	drive_trigger();
	if(imagebuffers!= NULL)
		free_buffers();
	imagebuffers= new unsigned char*[count+1];
	skipbuffer= new unsigned int[count+1];
	for(unsigned int i = 0; i < count+1;i++)
	{
		imagebuffers[i]=NULL;
		skipbuffer[i]=0;
	}

	return prepare_buffers(imagebuffers,skipbuffer,count+1,false);
}


bool highspeed_camera::prepare_buffers(unsigned char *imagebuffers[],unsigned int skippedBuffers[],unsigned count,bool loop)
{
	unsigned int bufCmd;
	unsigned int bufSize;
	unsigned int i;

	this->imagebuffers = imagebuffers;
	this->skipbuffer = skippedBuffers;
	

	if(imgCreateBufList(count, &bid)<0)
		return false;
	
	if(!image_buffer_size(&bufSize))
		return false;

	
	for (i = 0; i < count; i++)
	{
		if(this->imagebuffers[i] == NULL)
		{
			if(imgCreateBuffer(sid, IMG_HOST_FRAME, bufSize,(void **) &(this->imagebuffers[i]))<0)
				return false;
		}

		if(imgSetBufferElement(bid, i, IMG_BUFF_ADDRESS,(uInt32) (this->imagebuffers[i]))< 0)
			return false;

		if(imgSetBufferElement(bid, i, IMG_BUFF_SIZE, bufSize) <0)
			return false;

		if(loop)
			bufCmd = (i == (count - 1)) ? IMG_CMD_LOOP : IMG_CMD_NEXT;
		else
			bufCmd = (i == (count - 1)) ? IMG_CMD_STOP : IMG_CMD_NEXT;
        if(imgSetBufferElement(bid, i, IMG_BUFF_COMMAND, bufCmd)<0)
			return false;

		if(imgSetBufferElement(bid, i, IMG_BUFF_SKIPCOUNT, this->skipbuffer[i])<0)
			return false;
	}

	if(imgMemLock(bid)<0)
		return false;

	
	
	return true;
}

void highspeed_camera::reset_interface()
{
	imgInterfaceReset(iid);

}

bool highspeed_camera::free_buffers()
{
    if (bid != 0)
	{
		imgMemUnlock(bid);
		imgDisposeBufList(bid,TRUE);
	}
	if(imagebuffers != NULL)
	{
		delete[] imagebuffers;
		imagebuffers=NULL;
	}
	if(skipbuffer != NULL)
	{
		delete[] skipbuffer;
		skipbuffer=NULL;
	}
	return true;
}

bool highspeed_camera::start(bool async,CALL_BACK_PTR callback)
{
	if(imgSessionConfigure(sid, bid) !=0)
		return false;
	
	if(async)
	{
		if(imgSessionAcquire(sid, TRUE, callback)== 0)
			return true;
	}
	else
	{	
		if(imgSessionAcquire(sid, FALSE, callback)== 0)
			return true;
	}

	
	
	return false;

}

bool highspeed_camera::stop(unsigned int *lastavailablebufferindex)
{
	
	return (imgSessionAbort(sid,(uInt32*) lastavailablebufferindex) >= 0);
}

bool highspeed_camera::stop()
{
	
	unsigned int lastavailablebufferindex;
	return (imgSessionAbort(sid,(uInt32*) &lastavailablebufferindex) >= 0);
}

bool highspeed_camera::is_running()
{
	unsigned int inprogress;
	imgGetAttribute (sid, IMG_ATTR_ACQ_IN_PROGRESS, &inprogress);


	return inprogress == TRUE;
}

unsigned int highspeed_camera::last_valid_buffer_index()
{
	unsigned int currBufNum;
	imgGetAttribute (sid, IMG_ATTR_LAST_VALID_BUFFER, &currBufNum);

	while(currBufNum == 0xFFFFFFFF)
	{
		int i1 = imgSessionWaitSignal2(sid, IMG_SIGNAL_STATUS, IMG_FRAME_DONE, IMG_TRIG_POLAR_ACTIVEH, 100);
		int i2 = imgGetAttribute (sid, IMG_ATTR_LAST_VALID_BUFFER, &currBufNum);
		
	}
	return currBufNum;
}

unsigned int highspeed_camera::last_valid_frame()
{
	unsigned int currFrameNum;
	imgGetAttribute (sid, IMG_ATTR_LAST_VALID_FRAME, &currFrameNum);

	while(currFrameNum == 0xFFFFFFFF)
	{
			int error = imgSessionWaitSignal2(sid, IMG_SIGNAL_STATUS, IMG_FRAME_DONE, IMG_TRIG_POLAR_ACTIVEH, 100);
			int i2 = imgGetAttribute (sid, IMG_ATTR_LAST_VALID_FRAME, &currFrameNum);
			
			if(error)
			{
				char text[256];
				imgShowError(error,text);
				std::cout << text;
			}
			
			 
	}
	return currFrameNum;

}
/*
bool highspeed_camera::read_trigger_line(unsigned int number,unsigned int*status)
{
	return imgSessionTriggerRead2(sid, IMG_SIGNAL_EXTERNAL, number, IMG_TRIG_POLAR_ACTIVEH,(uInt32*)  status)==0;
}*/

int highspeed_camera::number_of_buffers()
{
	return num_buffers;
}


bool highspeed_camera::copy_buffer(unsigned int bufferindex,unsigned char *destbuffer,bool waitfornext)
{
	int error;
	if(waitfornext)
		error =imgSessionCopyBuffer(sid, bufferindex, destbuffer, TRUE);
	else
		error= imgSessionCopyBuffer(sid, bufferindex, destbuffer, FALSE);
	//char text[256];
	//if(error)
	//{
	//	imgShowError(error,text);
	//	return false;
	//}
	return true;
}


bool  highspeed_camera::examine_buffer(unsigned int whichBuffer, unsigned int* bufferNumber, unsigned char** bufferAddr)
{
	return imgSessionExamineBuffer(sid,whichBuffer,(uInt32*)bufferNumber,(uInt32*)bufferAddr)==0;
}
	
bool  highspeed_camera::release_buffer()
{
	return imgSessionReleaseBuffer(sid)==0;
}



/*

bool highspeed_camera::PlotImage(GUIHNDL hwnd,unsigned int imagewidth,unsigned int imageheight,unsigned char* imagebuffer)
{
    return (imgPlot(hwnd,imagebuffer,0,0,imagewidth,imageheight,0,0,IMGPLOT_MONO_8) >= 0);
}
*/

	}
}









