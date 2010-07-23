#pragma once

#pragma warning(disable:4275)

#include <string>
#include <cgv/type/size_type.h>
#include <cgv/storage/collection/vector.h>
#include <cgv/threading/thread.h>
#include <cgv/media/video/lib_begin.h>

namespace cgv {
	namespace media {
		namespace video {

	using cgv::type::uint32;
	using cgv::type::size_type;

/** this is the base class for callbacks that handle video frames */
class capture_callback
{
public:
	struct Info
	{
		char* data_ptr;
		int width;
		int height;
		int buffer_size;
		int nr_bytes;
		int ellapsed_time;
		bool is_keyframe;
	};
	virtual bool view(const Info& info) { return true; }
	virtual bool stream(const Info& info) { return true; }
};


/** base class for all video capture classes. Windows only implementation. */
class CGV_API capture_base
{
protected:
	void* handle;
	void* capabilities;
	bool overlayed;
	capture_callback* callback;
public:
	/// list of capture device names which is set by the scan_devices method
	static storage::collection::vector<std::string>& device_names();
	static storage::collection::vector<std::string>& device_versions();
	static void scan_devices();
	/// construct a capture device
	capture_base();
	/// construction method
	void construct(int w, int h, bool show);
	/// destructor
	~capture_base();
	/// try to attach to given device and return whether this was successful
	bool attach(int i);
	/// detach device
	void detach();
	///
	void repaint();
	///
	void enable_preview(float fps);
	void disable_preview();
	/// return whether device is attached
	bool is_attached() const { return capabilities != 0; }
	/// overlay
	bool has_overlay() const;
	bool enable_overlay();
	void disable_overlay();
	bool is_overlayed() const;
	///
	bool has_source_dlg() const;
	void show_source_dlg();
	///
	bool has_format_dlg() const;
	void show_format_dlg();
	///
	bool has_display_dlg() const;
	void show_display_dlg();

	bool set_callback(capture_callback* c);
	capture_callback* get_callback() const { return callback; }

	bool is_initialized() const;

	/** controll of video format */

	/// query video format
	uint32 w() const;
	uint32 h() const;
	uint32 nr_planes() const;
	uint32 nr_bits() const;
	size_type size() const;
	/// set video format. If a parameter is 0, keep previous value. Return if this was possible.
	bool set_format(uint32 w, uint32 h = 0, uint32 nr_planes = 0, uint32 nr_bits = 0);

	/** controll of capture parameters */
	float get_fps() const;
	void set_fps(float fps);

	void enable_threading();
	void disable_threading();
	bool is_threading() const;

	void enable_start_dlg();
	void disable_start_dlg();

	void set_maximal_drop_fraction(float frac);

	void enable_audio();
	void disable_audio();
};

/** capture class to perform single step capturing triggered by the application. */
class CGV_API capture : public capture_base
{
public:
	/// construct a capture device
	capture(int w = 160, int h = 120, bool show = false, int i = -1);

	/** perform capture */
	bool start();
	bool capture_frame();
	void stop();
};

/** class that allows to capture video from cameras. Windows only implementation. */
class CGV_API capture_avi : public capture_base
{
public:
	/// construct a capture device
	capture_avi(int w = 160, int h = 120, bool show = false, int i = -1);

	void set_duration(float seconds);
	/** perform capture */
	bool start();
	bool start(const std::string& file_name);
	void stop();
};

/** separate thread that continuously captures and calls the process method of a 
    capture callback for each frame */
class CGV_API capture_thread : public threading::thread, public capture_base
{
protected:
	bool con_show;
	int con_w, con_h;
public:
	/// construct a capture device
	capture_thread(int w = 160, int h = 120, bool show = false, int i = -1);
	/// destructor
	~capture_thread();
	///
	void run();

	bool set_nr_buffers(uint32 nr);

	void set_duration(float seconds);

	/** perform capture */
	bool start();
	void stop();
};

		}
	}
}

#include <cgv/config/lib_end.h>
