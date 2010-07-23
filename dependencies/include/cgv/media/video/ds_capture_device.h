#pragma once

#pragma warning(disable:4996)
#include <cgv/media/video/ds_capture.h>
#include <cgv/media/video/ds_callback_filter.h>
#include <dshow.h>
#include <cgv/media/video/lib_begin.h>

namespace cgv {
	namespace media {
		namespace video {

class ds_capture_device
{
public:
	/// current state
	enum State { RUNNING, PAUSED, STOPPED } state;

	/// preview window
	HWND            pre_win;
	bool            preview_attached;
	bool            preview_shown;
	
	/// capture graph builder
	ICaptureGraphBuilder2 * capture;
	IGraphBuilder * gra_bui;
	IVideoWindow  * vid_win;
	IMediaControl * med_con;
	IMediaEventEx * med_evt;

	/// capture source
	IBaseFilter* source_filter;
	ISpecifyPropertyPages *source_prop;
	IAMVideoProcAmp *source_config;
	IAMStreamConfig *stream_config;
	ISpecifyPropertyPages *format_prop;

	/// callback filters
	ds_cb_sink* cb_sink;
	ds_cb_filter* cb_filter;
	ds_cb_filter* insert_transform(IPin* output_pin);
	IPin* find_pin(IBaseFilter* istf, LPWSTR pin_name);
	IPin* find_pin(LPWSTR filter_name, LPWSTR pin_name);

	bool use_preview;
	bool is_preview_attached() const { return preview_attached; } 
	bool attach_preview();
	void detach_preview();

	static void run();

public:
	ds_capture_device(bool _use_preview);
	~ds_capture_device();

	bool enum_format_caps(std::vector<ds_capture::format_caps>& formats) const;
	ds_capture::format get_format() const;
	void set_format(const ds_capture::format& fm);

	/// check if a property is supported by the attached device
	bool has_property(ds_capture::property_id pid) const;
	/// query a property caps structure
	ds_capture::property_caps get_property_caps(ds_capture::property_id pid) const;
	/// get the property adjustment mode
	ds_capture::property_adjustment get_property_adjustment(ds_capture::property_id pid) const;
	/// set the property adjustment mode
	void set_property_adjustment(ds_capture::property_id pid, ds_capture::property_adjustment pa);
	/// get the value of a property
	int get_property(ds_capture::property_id pid) const;
	/// set the value of a property
	void set_property(ds_capture::property_id pid, int value);

	IVideoWindow* ref_video_window() { return vid_win; }

	bool is_preview_enabled() const { return use_preview; }
	void enable_preview();
	void disable_preview();

	bool is_preview_shown() const;
	void show_preview();
	void hide_preview();

	bool is_initialized() const { return capture != NULL; }
	bool init_graph();
	void close_all();

	bool is_attached() const { return source_filter != NULL; }
	bool attach_source(IMoniker* pSrc);
	void detach_source();

	bool has_source_dlg() const { return source_prop != NULL; }
	bool has_format_dlg() const { return format_prop != NULL; }
	void show_source_dlg();
	void show_format_dlg();
	
	bool is_graph_built() const { return cb_sink != NULL; }
	bool build_graph();
	void clear_graph();

	bool is_preview_created() const { return pre_win != NULL; }
	bool create_preview();
	void destroy_preview();

	bool is_running() const { return state == RUNNING; }
	bool is_paused() const { return state == PAUSED; }
	bool is_stopped() const { return state == STOPPED; }
	bool stop();
	bool start();
	bool pause();

	void hide();
	void resize_video_window();
	void change_preview_state(bool show);
	void handle_graph_event();
};

		}
	}
}

#include <cgv/config/lib_end.h>
