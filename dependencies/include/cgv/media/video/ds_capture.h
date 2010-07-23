#pragma once

#include <cgv/storage/collection/vector.h>
#include <cgv/media/video/lib_begin.h>

namespace cgv {
	namespace media {
		namespace video {

class CGV_API ds_capture
{
public:
	/**@name static method*/
	//@{
	/// list of capture device names which is set by the scan_video_devices method
	static cgv::storage::collection::vector<std::string>& video_device_names();
	/// construct list of video capture devices
	static void scan_video_devices();
	/// list of capture device names which is set by the scan_audio_devices method
	static cgv::storage::collection::vector<std::string>& audio_device_names();
	/// construct list of audio capture devices
	static void scan_audio_devices();
	///
	static void run();
	//@}
private:
	void* device;
	bool preview_enabled;
public:
	/// construct
	ds_capture();
	~ds_capture();

	/**@name format */
	//@{
	/// different format types define how the video image is represented in memory
	enum format_type { 
		YUY2       ,  // 									       
		IYUV       ,  //									       
		I420       ,  //									       
		UYVY       ,  // UYVY 	4:2:2	Packed	8						       
		IMC1       ,  // IMC1	4:2:0	Planar	8						       
		IMC3       ,  // IMC2	4:2:0	Planar	8						       
		IMC2       ,  // IMC3	4:2:0	Planar	8						       
		IMC4       ,  // IMC4	4:2:0	Planar	8						       
		YV12       ,  // YV12	4:2:0	Planar	8						       
		NV12       ,  // NV12	4:2:0	Planar	8						       
		Y411       ,  // Y411	4:1:1	Packed	8						       
		Y422       ,  // Y422	4:2:2	Packed	8						       
		Y41P       ,  // Y41P	4:1:1	Packed	8						       
		Y800       ,  // Y800
		Y211       ,  // Y211	See remarks.	Packed	8					       
		YVYU       ,  // YVYU	4:2:2	Packed	8						       
		YVU9       ,  // YVU9	See remarks.	Planar	8					       
		IF09       ,  // Indeo YVU9								       
		RGB1       ,  // RGB, 1 bit per pixel (bpp), palettized				       
		RGB4       ,  // RGB, 4 bpp, palettized						       
		RGB8       ,  // RGB, 8 bpp								       
		RGB555     ,  // RGB 555, 16 bpp							       
		RGB565     ,  // RGB 565, 16 bpp							       
		RGB24	   ,  // RGB, 24 bpp								       
		RGB32	   ,  // RGB, 32 bpp								       
		ARGB1555   ,  // RGB 555 with alpha channel						       
		ARGB32     ,  // RGB 32 with alpha channel						       
		ARGB4444   ,  // 16-bit RGB with alpha channel; 4 bits per channel			       
		A2R10G10B10,  // 32-bit RGB with alpha channel; 10 bits per RGB channel plus 2 bits for alpha.
		A2B10G10R10,  // 32-bit BGR with alpha channel; 10 bits per BGR channel plus 2 bits for alpha.
		NrFormatTypes,
		UNDEFINED_FORMAT_TYPE
	};
	/// convert a format enum into a string
	static const char* get_format_type_name(format_type ft);
	/// base struct for format and format caps. Defines width, height and format type
	struct format_base
	{
		int w;
		int h;
		format_type ft;
		format_base() : w(0), h(0), ft(UNDEFINED_FORMAT_TYPE) {}
		format_base(int _w, int _h, format_type _ft) : w(_w), h(_h), ft(_ft) {}
		friend std::ostream& operator << (std::ostream& os, const format_base& f)
		{
			return os << f.w << "x" << f.h << "|" << get_format_type_name(f.ft);
		}
	};
	/// format caps extend the format_base by minimal and maximal frame rates
	struct format_caps : public format_base
	{
		float min_fps;
		float max_fps;
		format_caps() : min_fps(0), max_fps(10000) {}
		format_caps(int _w, int _h, format_type _ft, float _min_fps, float _max_fps) 
			: format_base(_w, _h, _ft), min_fps(_min_fps), max_fps(_max_fps) {}
		friend std::ostream& operator << (std::ostream& os, const format_caps& fc)
		{
			const format_base& f = fc;
			return os << f << "<" << fc.min_fps << "-" << fc.max_fps << ">";
		}
	};
	/// format combines format_base with a frame rate
	struct format : public format_base
	{
		float fps;
		format() : fps(30) {}
		format(int _w, int _h, format_type _ft, float _fps) 
			: format_base(_w, _h, _ft), fps(_fps) {}
		friend std::ostream& operator << (std::ostream& os, const format& fm)
		{
			const format_base& f = fm;
			return os << f << ":" << fm.fps;
		}
	};
	/// query supported formats after device is attached
	bool enum_format_caps(std::vector<format_caps>& formats) const;
	/// get current format 
	format get_format() const;
	/// return current frame width
	int get_width() const;
	/// return current frame height
	int get_height() const;
	/// return current frame rate
	float get_fps() const;
	/// return current format type
	format_type get_format_type() const;
	/// print out the format capabilities
	void show_caps(std::ostream& os);

	/// set current format. Video capture has to be stopped for this.
	void set_format(const format& fm);
	/// set current frame size. Video capture has to be stopped for this.
	void set_size(int w, int h);
	/// set current frame rate. Video capture has to be stopped for this.
	void set_fps(float fps);
	/// set current format type. Video capture has to be stopped for this.
	void set_format_type(format_type ft);
	//@}

	/**@name properties*/
	//@{
	/// different properties that can by adjusted
	enum property_id {
		Brightness,
		Contrast,
		Hue,
		Saturation,
		Sharpness,
		Gamma,
		ColorEnable,
		WhiteBalance,
		BacklightCompensation,
		Gain,
		NrProperties
	};
	/// convert property into string
	static const char* get_property_name(property_id pid);
	
	/**@name property capabilities*/
	//@{
	/// the capabilities of a property
	struct property_caps
	{
		int min_value;
		int max_value;
		int default_value;
		int value_step_size;
		bool allows_manual_adjustment;
		bool allows_auto_adjustment;
		property_caps() : min_value(0), max_value(0), default_value(0), value_step_size(0), allows_manual_adjustment(false), allows_auto_adjustment(false) {}
		friend std::ostream& operator << (std::ostream& os, const property_caps& pc)
		{
			os << "[" << pc.min_value << "," << pc.max_value << "|" << pc.value_step_size 
			   << "]<" << pc.default_value;
			if (pc.allows_manual_adjustment)
				os << ",manual";
			if (pc.allows_auto_adjustment)
				os << ",auto";
			return os << ">";
		}
	};
	/// check if a property is supported by the attached device
	bool has_property(property_id pid) const;
	/// query a property caps structure
	property_caps get_property_caps(property_id pid) const;
	/// query the range of valid property values
	std::pair<int,int> get_property_range(property_id pid) const;
	/// query the default value of a property
	int get_property_default(property_id pid) const;
	/// query the value step size that should be used to navigate through the range of valid values
	int get_property_step_size(property_id pid) const;
	/// check if a property allows automatic adjustment
	bool allows_auto_property(property_id pid) const;
	/// check if a property allows manual adjustment
	bool allows_manual_property(property_id pid) const;
	/// print out the properties
	void show_properties(std::ostream& os);
	//@}

	/// possible property adjustment modes
	enum property_adjustment { MANUAL, AUTO, UNDEF_ADJUSTMENT };
	/// convert property adjustment into string
	static const char* get_adjustment_name(property_adjustment pa);
	/// get the property adjustment mode
	property_adjustment get_property_adjustment(property_id pid) const;
	/// set the property adjustment mode
	void set_property_adjustment(property_id pid, property_adjustment pa);
	/// get the value of a property
	int get_property(property_id pid) const;
	/// set the value of a property
	void set_property(property_id pid, int value);
	//@}

	/**@name graph options*/
	//@{
	/// return whether preview is enabled
	bool is_preview_enabled() const { return preview_enabled; }
	/// enable preview window
	void enable_preview();
	/// disable preview window
	void disable_preview();
	/// check is preview window is shown
	bool is_preview_shown() const;
	/// show preview window if used
	void show_preview();
	/// hide preview window if used
	void hide_preview();

	/// check if video is attached
	bool is_video_attached() const;
	/// attach video to device i
	bool attach_video(int i);
	/// detach video device
	void detach_video();

/*	/// check if audio is attached
	bool is_audio_attached() const;
	/// attach audio to device i
	bool attach_audio(int i);
	/// detach audio device
	void detach_audio();

	/// check if avi output file is attached
	bool is_avi_output_attached() const;
	/// attach to avi output file
	bool attach_avi_output(const std::string& file_name);
	/// detach avi output file and close it
	void detach_avi_output();

	/// attach a callback
	enum graph_location { BEFORE_PREVIEW, BEFORE_AVI_OUTPUT, BEFORE_TEE 
	void insert_transform(
	*/
	//@}

	/**@name controls and dialogs*/
	//@{
	/// start video capture
	bool start();
	/// pause video capture
	bool pause();
	/// stop video capture
	bool stop();

	/// check for source dialog
	bool has_source_dlg() const;
	/// show source dialog and wait for user to press ok or cancel
	void show_source_dlg();

	/// check for format dialog
	bool has_format_dlg() const;
	/// show format dialog and wait for user to press ok or cancel. During format selection, the device is detached
	void show_format_dlg();

	///
	//bool has_display_dlg() const;
	//void show_display_dlg();
	//@}
};

		}
	}
}

#include <cgv/config/lib_end.h>
