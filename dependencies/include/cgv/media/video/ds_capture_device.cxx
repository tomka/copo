#include <cgv/media/video/ds_common.h>
#include <cgv/media/video/ds_capture_device.h>
#include <iostream>
#include <winerror.h>

#define DEFAULT_VIDEO_WIDTH     640
#define DEFAULT_VIDEO_HEIGHT    480

#define APPLICATIONNAME TEXT("Video Capture Previewer (PlayCap)\0")

namespace cgv {
	namespace media {
		namespace video {

ds_capture_device::ds_capture_device(bool _use_preview)
{
	state = STOPPED;

	pre_win = 0;
	preview_attached = false;
	preview_shown = false;

	capture = NULL;
	gra_bui = NULL;
	vid_win = NULL;
	med_con = NULL;
	med_evt = NULL;

	source_filter = NULL;
	source_prop = NULL;
	stream_config = NULL;
	format_prop = NULL;

	cb_sink      = NULL;
	cb_filter     = NULL;

	use_preview = _use_preview;
}

ds_capture_device::~ds_capture_device()
{
	close_all();
}

void ds_capture_device::enable_preview()
{
	if (is_preview_enabled())
		return;
	use_preview = true;
	if (is_graph_built()) {
		clear_graph();
		build_graph();
	}
}

void ds_capture_device::disable_preview()
{
	if (!is_preview_enabled())
		return;
	destroy_preview();
	use_preview = false;
}


bool ds_capture_device::init_graph()
{
	if (is_initialized())
		return false;
	// Create the filter graph
	if (FAILED(CoCreateInstance (CLSID_FilterGraph, NULL, CLSCTX_INPROC,
		IID_IGraphBuilder, (void **) &gra_bui))) {
			std::cerr << "could not create graph builder" << std::endl;
			return false;
	}
	// Create the capture graph builder
	if (FAILED(CoCreateInstance (CLSID_CaptureGraphBuilder2 , NULL, CLSCTX_INPROC,
		IID_ICaptureGraphBuilder2, (void **) &capture))) {
			std::cerr << "could not create capture graph builder" << std::endl;
			return false;
	}
	// Attach the filter graph to the capture graph
	if (FAILED(capture->SetFiltergraph(gra_bui))) {
		std::cerr << "failed to set capture filter graph!" << std::endl;
		return false;
	}
	// Obtain interfaces for media control and Video Window
	if (FAILED(gra_bui->QueryInterface(IID_IMediaControl,(LPVOID *) &med_con)) ||
		FAILED(gra_bui->QueryInterface(IID_IVideoWindow, (LPVOID *) &vid_win)) ||
		FAILED(gra_bui->QueryInterface(IID_IMediaEvent, (LPVOID *) &med_evt))) {
			std::cerr << "could not query interfaces" << std::endl;
			return false;
	}
	return true;
}

void ds_capture_device::close_all()
{
	stop();
	detach_preview();
	clear_graph();
	detach_source();
	if (vid_win != NULL) { vid_win->Release(); vid_win = NULL; }
	if (med_con != NULL) { med_con->Release(); med_con = NULL; }
	if (med_evt != NULL) { med_evt->Release(); med_evt = NULL; }
	if (gra_bui != NULL) { gra_bui->Release(); gra_bui = NULL; }
	if (capture != NULL) { capture->Release(); capture = NULL; }
}


bool ds_capture_device::enum_format_caps(std::vector<ds_capture::format_caps>& formats) const
{
	if (!is_attached())
		return false;
	
	AM_MEDIA_TYPE *mt;
	VIDEO_STREAM_CONFIG_CAPS vscc;

	int count,size;
	stream_config->GetNumberOfCapabilities(&count, &size);
	for (int i=0; i<count; ++i) {
		stream_config->GetStreamCaps(i, &mt, (BYTE*) &vscc);
		ds_capture::format_caps fc;
		fc.w = vscc.InputSize.cx;
		fc.h = vscc.InputSize.cy;
		fc.max_fps = 10000000.0f/vscc.MinFrameInterval;
		fc.min_fps = 10000000.0f/vscc.MaxFrameInterval;
		fc.ft = subtype_to_format_type(mt->subtype);
		formats.push_back(fc);
	}
	return true;
}

ds_capture::format ds_capture_device::get_format() const
{
	if (!is_attached())
		return ds_capture::format();
	ds_capture::format f;
	AM_MEDIA_TYPE *mt;
	BITMAPINFOHEADER* bmiHeader = NULL;
	stream_config->GetFormat(&mt);
	f.ft = subtype_to_format_type(mt->subtype);
	if (mt->formattype == FORMAT_VideoInfo) {
		VIDEOINFOHEADER* vih = (VIDEOINFOHEADER*) mt->pbFormat;
		bmiHeader = &vih->bmiHeader;
		f.fps = 10000000.0f/vih->AvgTimePerFrame;
	}
	if (bmiHeader) {
		f.w = bmiHeader->biWidth;
		f.h = bmiHeader->biHeight;
	}
	return f;
}

void ds_capture_device::set_format(const ds_capture::format& fm)
{
	if (!is_attached())
		return;
	bool had_graph = is_graph_built();
	if (had_graph)
		clear_graph();

	// get current media type
	AM_MEDIA_TYPE *mt;
	stream_config->GetFormat(&mt);
	// copy media type
	AM_MEDIA_TYPE my_mt;
	std::copy((const char*)mt, (const char*)mt + sizeof(AM_MEDIA_TYPE), (char*)&my_mt);
	for (int i=0; i<NR_TABLE_ENTRIES; ++i) {
		if (lookup_table[i].ft == fm.ft) {
			my_mt.subtype = lookup_table[i].ms_type;
			break;
		}
	}
	//
	if (mt->formattype == FORMAT_VideoInfo) {
		// copy video header
		VIDEOINFOHEADER vih;
		my_mt.pbFormat = (BYTE*)&vih;
		std::copy((const char*)mt->pbFormat, (const char*)mt->pbFormat+sizeof(VIDEOINFOHEADER), (char*)&vih);
		// update video header
		vih.AvgTimePerFrame    = (REFERENCE_TIME)(10000000.0f/fm.fps);
		vih.bmiHeader.biWidth  = fm.w;
		vih.bmiHeader.biHeight = fm.h;
	}
	stream_config->SetFormat(&my_mt);

	if (had_graph)
		build_graph();
}

VideoProcAmpProperty prop_lookup[] = 
{
	VideoProcAmp_Brightness,
    VideoProcAmp_Contrast,
    VideoProcAmp_Hue,
    VideoProcAmp_Saturation,
    VideoProcAmp_Sharpness,
    VideoProcAmp_Gamma,
    VideoProcAmp_ColorEnable,
    VideoProcAmp_WhiteBalance,
    VideoProcAmp_BacklightCompensation,
    VideoProcAmp_Gain
};

/// check if a property is supported by the attached device
bool ds_capture_device::has_property(ds_capture::property_id pid) const
{
	if (!source_config)
		return false;
	long value, flags, min_val, max_val, delta, standard;
	return 
		S_OK == source_config->Get(prop_lookup[pid], &value, &flags) &&
		S_OK == source_config->GetRange(prop_lookup[pid], &min_val, &max_val, &delta, &standard, &flags) &&
		max_val > min_val;
}
/// query a property caps structure
ds_capture::property_caps ds_capture_device::get_property_caps(ds_capture::property_id pid) const
{
	ds_capture::property_caps pc;
	if (!source_config)
		return pc;
	long min_val, max_val, delta, standard, flags;
	source_config->GetRange(prop_lookup[pid], &min_val, &max_val, &delta, &standard, &flags);
	pc.min_value = min_val;
	pc.max_value = max_val;
	pc.default_value = standard;
	pc.value_step_size = delta;
	pc.allows_auto_adjustment = ((flags & VideoProcAmp_Flags_Auto) != 0);
	pc.allows_manual_adjustment = ((flags & VideoProcAmp_Flags_Manual) != 0);
	return pc;
}
/// get the property adjustment mode
ds_capture::property_adjustment ds_capture_device::get_property_adjustment(ds_capture::property_id pid) const
{
	if (!source_config)
		return ds_capture::UNDEF_ADJUSTMENT;
	long value, flags;
	source_config->Get(prop_lookup[pid], &value, &flags);
	if ((flags & VideoProcAmp_Flags_Auto) != 0)
		return ds_capture::AUTO;
	if ((flags & VideoProcAmp_Flags_Manual) != 0)
		return ds_capture::MANUAL;
	return ds_capture::UNDEF_ADJUSTMENT;
}
/// set the property adjustment mode
void ds_capture_device::set_property_adjustment(ds_capture::property_id pid, ds_capture::property_adjustment pa)
{
	if (!source_config)
		return;
	long value, flags;
	source_config->Get(prop_lookup[pid], &value, &flags);
	if (pa == ds_capture::AUTO) {
		flags |= VideoProcAmp_Flags_Auto;
		flags &= ~VideoProcAmp_Flags_Manual;
	}
	else if (pa == ds_capture::MANUAL) {
		flags &= ~VideoProcAmp_Flags_Auto;
		flags |= VideoProcAmp_Flags_Manual;
	}
	else {
		flags &= ~VideoProcAmp_Flags_Manual;
		flags &= ~VideoProcAmp_Flags_Auto;
	}	
	source_config->Set(prop_lookup[pid], value, flags);
}
/// get the value of a property
int ds_capture_device::get_property(ds_capture::property_id pid) const
{
	if (!source_config)
		return 0;
	long value, flags;
	source_config->Get(prop_lookup[pid], &value, &flags);
	return value;
}

/// set the value of a property
void ds_capture_device::set_property(ds_capture::property_id pid, int _value)
{
	if (!source_config)
		return;
	long value, flags;
	source_config->Get(prop_lookup[pid], &value, &flags);
	value = _value;
	source_config->Set(prop_lookup[pid], value, flags);
}

bool ds_capture_device::attach_source(IMoniker* pSrc)
{
	init_graph();
	detach_source();

	// bind Moniker to a source filter object
	if (FAILED(pSrc->BindToObject(0, 0, IID_IBaseFilter, (void**)&source_filter))) {
		std::cerr << "couldn't bind moniker to filter object!" << std::endl;
		return false;
	}
	// add source filter to our graph.
	if (FAILED(gra_bui->AddFilter(source_filter, L"Video Capture"))) {
		std::cerr << "Couldn't add the capture filter to the graph!" << std::endl;
		return false;
	}
	if (FAILED(source_filter->QueryInterface(IID_ISpecifyPropertyPages, (void **)&source_prop)))
		source_prop = NULL;

	if (FAILED(source_filter->QueryInterface(IID_IAMVideoProcAmp, (void **)&source_config)))
		source_config = NULL;

	if (FAILED(capture->FindInterface(&PIN_CATEGORY_CAPTURE,
               &MEDIATYPE_Interleaved,
			   source_filter, IID_IAMStreamConfig, (void **)&stream_config))) {
        if (FAILED(capture->FindInterface(&PIN_CATEGORY_CAPTURE,
                                      &MEDIATYPE_Video, source_filter,
                                      IID_IAMStreamConfig, (void **)&stream_config)))
			stream_config = NULL;
	}
	if (stream_config) {
		if (FAILED(stream_config->QueryInterface(IID_ISpecifyPropertyPages, (void **)&format_prop)))
			format_prop = NULL;
	}
	return true;
}

void ds_capture_device::detach_source()
{
	if (!is_attached())
		return;
	clear_graph();
	gra_bui->RemoveFilter(source_filter);
	if (source_prop != NULL) { source_prop->Release(); source_prop = NULL; }
	if (stream_config != NULL) { stream_config->Release(); stream_config = NULL; }
	if (format_prop != NULL) { format_prop->Release(); format_prop = NULL; }
	source_filter->Release(); source_filter = NULL;
}

void ds_capture_device::show_source_dlg()
{
	if (!has_source_dlg())
		return;
	// Get the filter's name and IUnknown pointer.
	FILTER_INFO FilterInfo;
	HRESULT hr = source_filter->QueryFilterInfo(&FilterInfo); 
	IUnknown *pFilterUnk;
	source_filter->QueryInterface(IID_IUnknown, (void **)&pFilterUnk);

	// Show the page. 
	CAUUID caGUID;
	source_prop->GetPages(&caGUID);
	OleCreatePropertyFrame(
		pre_win,                   // Parent window
		0, 0,                   // Reserved
		FilterInfo.achName,     // Caption for the dialog box
		1,                      // Number of objects (just the filter)
		&pFilterUnk,            // Array of object pointers. 
		caGUID.cElems,          // Number of property pages
		caGUID.pElems,          // Array of property page CLSIDs
		0,                      // Locale identifier
		0, NULL                 // Reserved
	);
	// Clean up.
	pFilterUnk->Release();
	FilterInfo.pGraph->Release(); 
	CoTaskMemFree(caGUID.pElems);

	if (cb_sink) 
		cb_sink->init();
}

void ds_capture_device::show_format_dlg()
{
	if (!has_format_dlg())
		return;
	clear_graph();
	IUnknown *pFilterUnk;
    stream_config->QueryInterface(IID_IUnknown, (void **)&pFilterUnk);
	CAUUID caGUID;
    if(SUCCEEDED(format_prop->GetPages(&caGUID)) && caGUID.cElems > 0)
    {
		OleCreatePropertyFrame(
			pre_win,                // Parent window
			0, 0,                   // Reserved
			NULL,
			1,                      // Number of objects (just the filter)
			&pFilterUnk,            // Array of object pointers. 
			caGUID.cElems,          // Number of property pages
			caGUID.pElems,          // Array of property page CLSIDs
			0,                      // Locale identifier
			0, NULL                 // Reserved
		);
        CoTaskMemFree(caGUID.pElems);
    }
	build_graph();
	start();
}

// {5E8FDF5C-9325-49f9-BBEC-47BBFDF8E4C9}
extern "C" const __declspec(selectany) GUID CLSID_dc_cb_filter = 
{0x5e8fdf5c, 0x9325, 0x49f9, {0xbb, 0xec, 0x47, 0xbb, 0xfd, 0xf8, 0xe4, 0xc9}};

ds_cb_filter* ds_capture_device::insert_transform(IPin* output_pin)
{
	HRESULT hr;
	IPin* input_pin;
	if (FAILED(output_pin->ConnectedTo(&input_pin))) {
		std::cerr << "no input pin connected" << std::endl;
		return 0;
	}
	if (FAILED(input_pin->Disconnect())) {
		std::cerr << "could not disconnect pin" << std::endl;
	}
	ds_cb_filter* cb_filter = new ds_cb_filter(NULL, &hr);
/*	if (hr = FAILED(CoCreateInstance(CLSID_dc_cb_filter, NULL, CLSCTX_INPROC,
		IID_IBaseFilter, (void **) &cb_filter))) {
			std::cerr << "couldn't create callback filter" << std::endl;
			return 0;
	}*/

	CBasePin* cbf_input = NULL, *cbf_output = NULL;
	for (int i=0; i<cb_filter->GetPinCount(); ++i) {
		CBasePin* pin = cb_filter->GetPin(i);
		PIN_DIRECTION dir;
		if (FAILED(pin->QueryDirection(&dir))) {
			std::cerr << "could not query pin direction of pin " << i << std::endl;
			delete cb_filter;
			return 0;
		}
		if (dir == PINDIR_OUTPUT) {
			if (cbf_output == NULL)
				cbf_output = pin;
			else
				std::cerr << "transform filter has several output pins" << std::endl;
		}
		else
			if (cbf_input == NULL)
				cbf_input = pin;
			else
				std::cerr << "transform filter has several input pins" << std::endl;
	}
	if (cbf_input == NULL || cbf_output == NULL) {
		std::cerr << "transform filter does not have two pins!!" << std::endl;
		delete cb_filter;
		return 0;
	}
	if (FAILED(gra_bui->AddFilter(cb_filter, L"Transform Callback"))) {
		std::cerr << "could not add cb_sink trans filter" << std::endl;
		delete cb_filter;
		return 0;
	}
	if (FAILED(hr = gra_bui->ConnectDirect(input_pin, cbf_output, NULL))) {
//	if (FAILED(hr = input_pin->CheckConnect(cbf_output))) {
		if (FAILED(hr = gra_bui->Connect(cbf_output, input_pin))) {
			std::cerr << "could not connect output" << std::endl;
			gra_bui->RemoveFilter(cb_filter);
			if (FAILED(hr = gra_bui->Connect(output_pin, input_pin))) {
				exit(-1);
			}
			delete cb_filter;
			return 0;
		}
	}
	if (FAILED(cbf_input->CheckConnect(output_pin))) {
		std::cerr << "check says: could not connect input" << std::endl;
		gra_bui->RemoveFilter(cb_filter);
		delete cb_filter;
		return 0;
	}

	//if (FAILED(cbf_input->Connect(output_pin, NULL))) {
	if (FAILED(hr = gra_bui->ConnectDirect(cbf_input, output_pin, NULL))) {
		std::cerr << "could not connect input" << std::endl;
		gra_bui->RemoveFilter(cb_filter);
		delete cb_filter;
		return 0;
	}
	return cb_filter;	
}

IPin* ds_capture_device::find_pin(IBaseFilter* istf, LPWSTR pin_name)
{
	IPin *pin;
	if (FAILED(istf->FindPin(pin_name, &pin))) {
		IEnumPins* eps;
		if (FAILED(istf->EnumPins(&eps))) {
			std::cerr << "couldn't enumerate pins" << std::endl;
			return NULL;
		}
		ULONG count;
		while (S_OK == eps->Next(1, &pin, &count)) {
			PIN_INFO pi;
			pin->QueryPinInfo(&pi);
			if (wcscmp(pi.achName, pin_name) == 0)
				return pin;
/*			USES_CONVERSION;
			char* s1 = W2A(pi.achName);
			char* s2 = W2A(pin_name);
			if (strcmp(s1,s2) == 0) 
				return pin;*/
		}
		return NULL;
	}
	else
		return pin;
}


IPin* ds_capture_device::find_pin(LPWSTR filter_name, LPWSTR pin_name)
{
	IBaseFilter* istf;
	if (FAILED(gra_bui->FindFilterByName(filter_name, &istf)))
		return NULL;
	return find_pin(istf, pin_name);
}

bool ds_capture_device::build_graph()
{
	if (is_graph_built())
		return true;
	if (!is_attached())
		return false;
	if (is_preview_enabled()) {
		// render the preview pin on the video capture filter
		if (FAILED(capture->RenderStream (&PIN_CATEGORY_PREVIEW, &MEDIATYPE_Video,
			source_filter, NULL, NULL))) {
			std::cerr << "couldn't render the preview pin!" << std::endl;
			return false;
		}
		attach_preview();
	}
	HRESULT hr;
	cb_sink = new ds_cb_sink(NULL, &hr);
	IBaseFilter* ibf;
	if (FAILED(cb_sink->QueryInterface(IID_IBaseFilter, &((void*&)ibf)))) {
		std::cerr << "couldn't convert callback to IBaseFilter" << std::endl;
		return false;
	}
	if (FAILED(gra_bui->AddFilter(ibf, L"callback"))) {
		std::cerr << "couldn't attach cdump" << std::endl;
		return false;
	}
	IPin* output_pin = find_pin(L"Smart Tee", L"Capture");
	if (output_pin == NULL)
		output_pin = find_pin(source_filter, L"Capture");
	if (output_pin == NULL) {
		std::cerr << "couldn't find a capture pin" << std::endl;
		return false;
	}
	if (FAILED(gra_bui->Connect(output_pin, cb_sink->pin))) {
		std::cerr << "couldn't connect pins" << std::endl;
		return false;
	}
	// insert cb_filter
/*	output_pin = find_pin(L"Smart Tee", L"Preview");
	if (output_pin == NULL)
		output_pin = find_pin(source_filter, L"Capture");		
	cb_filter = insert_transform(output_pin);*/
	return true;
}

void ds_capture_device::clear_graph()
{
	if (!is_graph_built())
		return;
	stop();
	IEnumFilters* filters;
	if (FAILED(gra_bui->EnumFilters(&filters)))
		return;
	ULONG nr_fetched;
	IBaseFilter** filter;
	filter = new IBaseFilter*[10];
	while (SUCCEEDED(filters->Next(10, filter, &nr_fetched))) {
		if (nr_fetched == 0) 
			break;

		for (unsigned int i=0; i<nr_fetched; ++i) {
			if (filter[i] != source_filter) {
				if (FAILED(gra_bui->RemoveFilter(filter[i]))) {
					std::cerr << "could not remove filter" << std::endl;
				}
				filter[i]->Release();
			}
			else {
				std::cout << "did not remove source filter!" << std::endl;
			}
		}
	}
	delete [] filter;

	if (cb_sink) {
		delete cb_sink;
		cb_sink = NULL;
	}
}

bool ds_capture_device::create_preview()
{
	if (is_preview_created())
		return true;
    // Create the main window.  The WS_CLIPCHILDREN style is required.
    pre_win = CreateWindow(CLASSNAME, APPLICATIONNAME,
                         WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_CLIPCHILDREN,
                         CW_USEDEFAULT, CW_USEDEFAULT,
                         DEFAULT_VIDEO_WIDTH, DEFAULT_VIDEO_HEIGHT,
                         0, 0, NULL, 0);
	if (pre_win) {
		void* ptr = this;
		SetWindowLongPtr(pre_win, GWLP_USERDATA, (LONG&)ptr);
        ShowWindow(pre_win, SW_SHOW);
		preview_shown = true;
	}
	return pre_win != NULL;
}

bool ds_capture_device::is_preview_shown() const
{
	return preview_shown;
}

void ds_capture_device::show_preview()
{
	if (preview_shown)
		return;
	if (!pre_win)
		create_preview();
	else {
        ShowWindow(pre_win, SW_SHOW);
		preview_shown = true;
	}
}
void ds_capture_device::hide_preview()
{
	if (!preview_shown)
		return;
	if (pre_win) {
        ShowWindow(pre_win, SW_HIDE);
		preview_shown = false;
	}
}

void ds_capture_device::destroy_preview()
{
	if (!is_preview_created())
		return;
	detach_preview();
    ShowWindow(pre_win, SW_HIDE);
	CloseWindow(pre_win);
//	hr = DestroyWindow(pre_win);
	pre_win = NULL;
}

bool ds_capture_device::attach_preview()
{
	if (is_preview_attached())
		return true;
	if (!is_attached())
		return false;
	if (!create_preview())
		return false;
	if (is_graph_built())
		clear_graph();
	// Set the window handle used to process graph events
	med_evt->SetNotifyWindow((OAHWND)pre_win, WM_GRAPHNOTIFY, 0);
	// Set the video window to be a child of the main window
	vid_win->put_Owner((OAHWND)pre_win);
	// Set video window style
	vid_win->put_WindowStyle(WS_CHILD | WS_CLIPCHILDREN);
	// Use helper function to position video window in client rect 
	resize_video_window();
	// Make the video window visible, now that it is properly positioned
	vid_win->put_Visible(OATRUE);
	return true;
}

void ds_capture_device::detach_preview()
{
	// Stop receiving events
	if (med_evt)
		med_evt->SetNotifyWindow(NULL, WM_GRAPHNOTIFY, 0);

	if(vid_win) {
		vid_win->put_Visible(OAFALSE);
		vid_win->put_Owner(NULL);
	}
	preview_attached = false;
}



bool ds_capture_device::stop()
{
	if (is_stopped())
		return true;
	if (!build_graph())
		return false;
	if (FAILED(med_con->Stop())) {
		std::cerr << "could not stop capture" << std::endl;
		return false;
	}
	std::cout << "stopped" << std::endl;
	// Remember current state
	state = STOPPED;
	return true;
}

bool ds_capture_device::start()
{
	if (is_running())
		return true;
	if (!build_graph())
		return false;

	if (FAILED(med_con->Run())) {
		std::cerr << "could not start capture" << std::endl;
		return false;
	}
	std::cout << "started" << std::endl;
	// Remember current state
	state = RUNNING;
	if (cb_sink) cb_sink->init();
	return true;
}
bool ds_capture_device::pause()
{
	if (is_paused())
		return true;
	if (!build_graph())
		return false;
	if (FAILED(med_con->Pause())) {
		std::cerr << "could not pause capture" << std::endl;
		return false;
	}
	std::cout << "paused" << std::endl;
	// Remember current state
	state = PAUSED;
	return true;
}

void ds_capture_device::run()
{
    MSG msg={0};
    while(GetMessage(&msg,NULL,0,0))
    {
		TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}
/*
void ds_capture_device::run(const std::vector<ds_capture_device*>& devices)
{
    MSG msg={0};
	do {
		for (unsigned int i=0; i<devices.size(); ++i) {
			long code;
			LONG_PTR param1, param2;
			HRESULT hr = devices[i]->med_evt->GetEvent(&code, &param1, &param2, 0);
			if (hr == S_OK) {
				std::cout << "received event for device " << i << std::endl;
//				if (code == EC_VIDEOFRAMEREADY) {
//					std::cout << "received video frame event" << std::endl;
//				}
			}
		}
		if (GetMessage(&msg,NULL,0,0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
			break;
	} while (true);
    // Release COM
    CoUninitialize();
}
*/

void ds_capture_device::hide()
{
	ShowWindow(pre_win, SW_HIDE);
}

void ds_capture_device::resize_video_window()
{
    // Resize the video preview window to match owner window size
	if (vid_win) {
        RECT rc;
        
        // Make the preview video fill our window
		GetClientRect(pre_win, &rc);
        vid_win->SetWindowPosition(0, 0, rc.right, rc.bottom);
    }
}

void ds_capture_device::change_preview_state(bool show)
{
	if (!is_preview_attached())
		return;
	if (show)
		start();
	else
		stop();
}

void ds_capture_device::handle_graph_event()
{
    LONG evCode;
	LONG_PTR evParam1, evParam2;
    HRESULT hr=S_OK;

	if (!med_evt)
        return;

    while(SUCCEEDED(med_evt->GetEvent(&evCode, &evParam1, &evParam2, 0)))
    {
        hr = med_evt->FreeEventParams(evCode, evParam1, evParam2);
        // Insert event processing code here, if desired
    }
}

		}
	}
}