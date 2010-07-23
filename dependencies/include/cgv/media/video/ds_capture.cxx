#include <cgv/storage/collection/vector.h>
#include <cgv/range/operators.h>
#include <cgv/media/video/ds_capture.h>
#include <cgv/media/video/ds_common.h>
#include <cgv/media/video/ds_capture_device.h>
#include <cgv/media/video/ds_audio_device_iterator.h>
#include <cgv/media/video/ds_video_device_iterator.h>

namespace cgv {
	namespace media {
		namespace video {

ds_capture_device*& cd(void*& ptr) { return (ds_capture_device*&) ptr; }
const ds_capture_device* cd(const void* ptr) { return (const ds_capture_device*) ptr; }

cgv::storage::collection::vector<std::string>& ds_capture::video_device_names()
{
	static cgv::storage::collection::vector<std::string> names;
	return names;
}

cgv::storage::collection::vector<std::string>& ds_capture::audio_device_names()
{
	static cgv::storage::collection::vector<std::string> names;
	return names;
}


void ds_capture::scan_video_devices()
{

	video_device_names().clear();
	for (ds_video_device_iterator di; !di.at_end(); ++di) {
		std::string name = di.get_name();
		video_device_names().push_back(name);
	}		
}		

void ds_capture::scan_audio_devices()
{

	audio_device_names().clear();
	for (ds_audio_device_iterator di; !di.at_end(); ++di) {
		std::string name = di.get_name();
		audio_device_names().push_back(name);
	}		
}		

/// convert a format enum into a string
const char* ds_capture::get_format_type_name(format_type ft)
{
	static const char* format_type_names[] = { 
		"none"       ,
		"YUY2"       ,
		"IYUV"       ,
		"I420"       ,
		"NV12"       ,
		"UYVY"       ,        
		"IMC1"       ,
		"IMC3"       ,
		"IMC2"       ,
		"IMC4"       ,
		"YV12"       ,
		"NV12"       ,
		"Y411"       ,
		"Y422"       ,
		"Y41P"       ,
		"Y800"       ,
		"Y211"       ,
		"YVYU"       ,
		"YVU9"       ,
		"IF09"       ,
		"RGB1"       ,
		"RGB4"       ,
		"RGB8"       ,
		"RGB555"     ,
		"RGB565"     ,
		"RGB24"	     ,
		"RGB32"	     ,
		"ARGB1555"   ,
		"ARGB32"     ,
		"ARGB4444"   ,
		"A2R10G10B10",
		"A2B10G10R10"
	};
	return format_type_names[ft];
}

const char* ds_capture::get_property_name(property_id pid)
{
	static const char* property_names[] = { 
		"Brightness",
		"Contrast",
		"Hue",
		"Saturation",
		"Sharpness",
		"Gamma",
		"ColorEnable",
		"WhiteBalance",
		"BacklightCompensation",
		"Gain",
		"NrProperties"
	};
	return property_names[pid];
}

/// possible property adjustment modes
enum property_adjustment { MANUAL, AUTO, UNDEF_ADJUSTMENT };
///
const char* ds_capture::get_adjustment_name(property_adjustment pa)
{
	static const char* adjustment_names[] = { 
		"manual",
		"auto",
		"none"
	};
	return adjustment_names[pa];
}

ds_capture::ds_capture()
{
	device = 0;
	preview_enabled = false;
}
ds_capture::~ds_capture()
{
	if (device) {
		delete cd(device);
		device = 0;
	}
}

/// check is preview window is shown
bool ds_capture::is_preview_shown() const
{
	if (!device)
		return false;
	return cd(device)->is_preview_shown();
}

void ds_capture::show_preview()
{
	if (!device)
		device = new ds_capture_device(is_preview_enabled());
	cd(device)->show_preview();
}

void ds_capture::hide_preview()
{
	if (!device)
		return;
	cd(device)->hide_preview();
}

/// try to attach to given device and return whether this was successful
bool ds_capture::attach_video(int i)
{
	if (!ensure_com())
		return false;
	if (!device)
		device = new ds_capture_device(is_preview_enabled());

	ds_video_device_iterator di;
	for (int j = 0; j < i; ++j) {
		++di;
		if (di.at_end())
			return false;
	}
	if (di.at_end())
		return false;
	return cd(device)->attach_source(di.operator->());
}

/// detach device
void ds_capture::detach_video()
{
	if (device)
		cd(device)->detach_source();
}

bool ds_capture::enum_format_caps(std::vector<format_caps>& formats) const
{
	if (device)
		return cd(device)->enum_format_caps(formats);
	return false;
}
ds_capture::format ds_capture::get_format() const
{
	if (device)
		return cd(device)->get_format();
	return format();
}

/// print out the format capabilities
void ds_capture::show_caps(std::ostream& os)
{
	cgv::storage::collection::vector<ds_capture::format_caps> format_caps;
	enum_format_caps(format_caps);
	os << format_caps << std::endl;
}

void ds_capture::set_format(const format& fm)
{
	if (device)
		cd(device)->set_format(fm);
}

void ds_capture::set_size(int w, int h)
{
	format f = get_format();
	set_format(format(w,h,f.ft,f.fps));
}

void ds_capture::set_fps(float fps)
{
	format f = get_format();
	set_format(format(f.w,f.h,f.ft,fps));
}

/// set current format type. Video capture has to be stopped for this.
void ds_capture::set_format_type(format_type ft)
{
	format f = get_format();
	set_format(format(f.w,f.h,ft,f.fps));
}

int ds_capture::get_width() const
{
	return get_format().w;
}
int ds_capture::get_height() const
{
	return get_format().h;
}

ds_capture::format_type ds_capture::get_format_type() const
{
	return get_format().ft;
}

float ds_capture::get_fps() const
{
	return get_format().fps;
}


bool ds_capture::has_property(property_id pid) const
{
	if (device)
		return cd(device)->has_property(pid);
	return false;
}

/// query a property caps structure
ds_capture::property_caps ds_capture::get_property_caps(property_id pid) const
{
	if (device)
		return cd(device)->get_property_caps(pid);
	return property_caps();
}

std::pair<int,int> ds_capture::get_property_range(property_id pid) const
{
	property_caps pc = get_property_caps(pid);
	return std::pair<int,int>(pc.min_value,pc.max_value);
}

/// query the default value of a property
int ds_capture::get_property_default(property_id pid) const
{
	property_caps pc = get_property_caps(pid);
	return pc.default_value;
}
/// query the value step size that should be used to navigate through the range of valid values
int ds_capture::get_property_step_size(property_id pid) const
{
	property_caps pc = get_property_caps(pid);
	return pc.value_step_size;
}
/// check if a property allows automatic adjustment
bool ds_capture::allows_auto_property(property_id pid) const
{
	property_caps pc = get_property_caps(pid);
	return pc.allows_auto_adjustment;
}
/// check if a property allows manual adjustment
bool ds_capture::allows_manual_property(property_id pid) const
{
	property_caps pc = get_property_caps(pid);
	return pc.allows_manual_adjustment;
}


/// get the property adjustment mode
ds_capture::property_adjustment ds_capture::get_property_adjustment(property_id pid) const
{
	if (device)
		return cd(device)->get_property_adjustment(pid);
	return UNDEF_ADJUSTMENT;
}
/// set the property adjustment mode
void ds_capture::set_property_adjustment(property_id pid, property_adjustment pa)
{
	if (device)
		cd(device)->set_property_adjustment(pid, pa);
}
/// get the value of a property
int ds_capture::get_property(property_id pid) const
{
	if (device)
		return cd(device)->get_property(pid);
	return 0;
}


/// print out the properties
void ds_capture::show_properties(std::ostream& os)
{
	for (ds_capture::property_id i=(ds_capture::property_id)0; i<ds_capture::NrProperties; ++(int&)i)
		if (has_property(i))
			os	<< ds_capture::get_property_name(i) << " = " 
				<< get_property(i) << ", "
				<< ds_capture::get_adjustment_name(get_property_adjustment(i)) << " { "
				<< get_property_caps(i) << " } " << std::endl;
}

/// set the value of a property
void ds_capture::set_property(property_id pid, int value)
{
	if (device)
		cd(device)->set_property(pid, value);
}

bool ds_capture::start()
{
	return cd(device)->start();
}

bool ds_capture::pause()
{
	return cd(device)->pause();
}

bool ds_capture::stop()
{
	return cd(device)->stop();
}

///
void ds_capture::enable_preview()
{
	if (!is_preview_enabled()) {
		preview_enabled = true;
		if (device)
			cd(device)->enable_preview();
	}
}

void ds_capture::disable_preview()
{
	if (is_preview_enabled()) {
		preview_enabled = false;
		if (device)
			cd(device)->disable_preview();
	}
}
///
bool ds_capture::has_source_dlg() const
{
	return cd(device)->has_source_dlg();
}
void ds_capture::show_source_dlg()
{
	cd(device)->show_source_dlg();
}
///
bool ds_capture::has_format_dlg() const
{
	return cd(device)->has_format_dlg();
}
void ds_capture::show_format_dlg()
{
	cd(device)->show_format_dlg();
}
///
//bool ds_capture::has_display_dlg() const
//{
//	return false;
//}
//void ds_capture::show_display_dlg()
//{
//}

void ds_capture::run()
{
	ds_capture_device::run();
}

/// run a windows loop that also asks the given capture devices
//void ds_capture::run(const std::vector<ds_capture*>& devices)
//{
//	std::vector<ds_capture_device*> cdevices;
//	cdevices.resize(devices.size());
//	for (unsigned int i=0; i<devices.size(); ++i)
//		cdevices[i] = (ds_capture_device*)devices[i]->device;
//	ds_capture_device::run(cdevices);
//}


		}
	}
}