#include <cgv/media/video/ds_common.h>
#include <cgv/media/video/ds_callback_filter.h>
#include <cgv/media/video/ds_capture_device.h>
#include <dshow.h>

// {5E8FDF5C-9325-49f9-BBEC-47BBFDF8E4C9}
extern "C" const __declspec(selectany) GUID CLSID_ds_cb_filter = 
{0x5e8fdf5c, 0x9325, 0x49f9, {0xbb, 0xec, 0x47, 0xbb, 0xfd, 0xf8, 0xe4, 0xc9}};

// {40D27EE8-1365-43dd-A700-212F1A85B1A6}
extern "C" const __declspec(selectany) GUID CLSID_ds_cb_sink = 
{0x40d27ee8, 0x1365, 0x43dd, {0xa7, 0x0, 0x21, 0x2f, 0x1a, 0x85, 0xb1, 0xa6}};

// I420 isn't defined in any of the headers, but everyone seems to use it.... 
extern "C" const __declspec(selectany) GUID MEDIASUBTYPE_I420 =
	{0x30323449,0x0000,0x0010, {0x80,0x00,0x00,0xAA,0x00,0x38,0x9B,0x71}};	
// IYUV
extern "C" const __declspec(selectany) GUID MEDIASUBTYPE_IYUV =
	   {0x56555949,0x0000,0x0010, {0x80,0x00,0x00,0xAA,0x00,0x38,0x9B,0x71}};
// Y444
extern "C" const __declspec(selectany) GUID MEDIASUBTYPE_Y444 = 
   {0x34343459, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71}};
// Y800
extern "C" const __declspec(selectany) GUID MEDIASUBTYPE_Y800 = 
   {0x30303859, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71}};
// Y422
extern "C" const __declspec(selectany) GUID MEDIASUBTYPE_Y422 = 
   {0x32323459, 0x0000, 0x0010, {0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71}};

const AMOVIESETUP_FILTER sud_ds_cb_filter =
{
    &CLSID_ds_cb_filter,   // Filter CLSID
    L"ds_cb_filter",       // String name
    MERIT_DO_NOT_USE,    // Filter merit
    0,                   // Number pins
    NULL                 // Pin details
};

const AMOVIESETUP_FILTER sud_ds_cb_sink =
{
    &CLSID_ds_cb_sink,   // Filter CLSID
    L"ds_cb_sink",       // String name
    MERIT_DO_NOT_USE,    // Filter merit
    0,                   // Number pins
    NULL                 // Pin details
};
/*
#ifdef _DEBUG
CFactoryTemplate g_Templates[] = 
{
	{ L"ds_cb_filter", &CLSID_ds_cb_filter, cgv::media::video::ds_cb_filter::CreateInstance, NULL, &sud_ds_cb_filter },
    { L"ds_cb_sink",   &CLSID_ds_cb_sink,   cgv::media::video::ds_cb_sink::CreateInstance,   NULL, &sud_ds_cb_sink }
};
int g_cTemplates = sizeof(g_Templates) / sizeof(g_Templates[0]);
#endif*/
//
// DllRegisterServer
//
STDAPI DllRegisterServer()
{
    return AMovieDllRegisterServer2( TRUE );

} // DllRegisterServer


//
// DllUnregisterServer
//
STDAPI DllUnregisterServer()
{
    return AMovieDllRegisterServer2( FALSE );

} // DllUnregisterServer


namespace cgv {
	namespace media {
		namespace video {

mstype_to_format_type lookup_table[NR_TABLE_ENTRIES] = {
	{ MEDIASUBTYPE_YUY2,        cgv::media::video::ds_capture::YUY2        },  // 
	{ MEDIASUBTYPE_IYUV,        cgv::media::video::ds_capture::IYUV        },  //
	{ MEDIASUBTYPE_I420,        cgv::media::video::ds_capture::I420        },  //
	{ MEDIASUBTYPE_UYVY,        cgv::media::video::ds_capture::UYVY        },  // UYVY 	4:2:2	Packed	8
	{ MEDIASUBTYPE_IMC1,        cgv::media::video::ds_capture::IMC1        },  // IMC1	4:2:0	Planar	8
	{ MEDIASUBTYPE_IMC3,        cgv::media::video::ds_capture::IMC3        },  // IMC2	4:2:0	Planar	8
	{ MEDIASUBTYPE_IMC2,        cgv::media::video::ds_capture::IMC2        },  // IMC3	4:2:0	Planar	8
	{ MEDIASUBTYPE_IMC4,        cgv::media::video::ds_capture::IMC4        },  // IMC4	4:2:0	Planar	8
	{ MEDIASUBTYPE_YV12,        cgv::media::video::ds_capture::YV12        },  // YV12	4:2:0	Planar	8
	{ MEDIASUBTYPE_NV12,        cgv::media::video::ds_capture::NV12        },  // NV12	4:2:0	Planar	8
	{ MEDIASUBTYPE_Y411,        cgv::media::video::ds_capture::Y411        },  // Y411	4:1:1	Packed	8
	{ MEDIASUBTYPE_Y422,        cgv::media::video::ds_capture::Y422        },  // Y422	4:2:2	Packed	8
	{ MEDIASUBTYPE_Y41P,        cgv::media::video::ds_capture::Y41P        },  // Y41P	4:1:1	Packed	8
	{ MEDIASUBTYPE_Y800,        cgv::media::video::ds_capture::Y800        },  // Y800
	{ MEDIASUBTYPE_Y211,        cgv::media::video::ds_capture::Y211        },  // Y211	See remarks.	Packed	8
	{ MEDIASUBTYPE_YVYU,        cgv::media::video::ds_capture::YVYU        },  // YVYU	4:2:2	Packed	8
	{ MEDIASUBTYPE_YVU9,        cgv::media::video::ds_capture::YVU9        },  // YVU9	See remarks.	Planar	8
	{ MEDIASUBTYPE_IF09,        cgv::media::video::ds_capture::IF09        },  // Indeo YVU9
	{ MEDIASUBTYPE_RGB1,        cgv::media::video::ds_capture::RGB1        },  // RGB, 1 bit per pixel (bpp), palettized					     
	{ MEDIASUBTYPE_RGB4,        cgv::media::video::ds_capture::RGB4	},  // RGB, 4 bpp, palettized							     
	{ MEDIASUBTYPE_RGB8,        cgv::media::video::ds_capture::RGB8	},  // RGB, 8 bpp								     
	{ MEDIASUBTYPE_RGB555,      cgv::media::video::ds_capture::RGB555      },  // RGB 555, 16 bpp								     
	{ MEDIASUBTYPE_RGB565,      cgv::media::video::ds_capture::RGB565      },  // RGB 565, 16 bpp								     
	{ MEDIASUBTYPE_RGB24,       cgv::media::video::ds_capture::RGB24	},  // RGB, 24 bpp								     
	{ MEDIASUBTYPE_RGB32,       cgv::media::video::ds_capture::RGB32	},  // RGB, 32 bpp								     
	{ MEDIASUBTYPE_ARGB1555,    cgv::media::video::ds_capture::ARGB1555    },  // RGB 555 with alpha channel						     
	{ MEDIASUBTYPE_ARGB32,      cgv::media::video::ds_capture::ARGB32      },  // RGB 32 with alpha channel						     
	{ MEDIASUBTYPE_ARGB4444,    cgv::media::video::ds_capture::ARGB4444    },  // 16-bit RGB with alpha channel; 4 bits per channel			     
	{ MEDIASUBTYPE_A2R10G10B10, cgv::media::video::ds_capture::A2R10G10B10 },  // 32-bit RGB with alpha channel; 10 bits per RGB channel plus 2 bits for alpha.
	{ MEDIASUBTYPE_A2B10G10R10, cgv::media::video::ds_capture::A2B10G10R10 },  // 32-bit BGR with alpha channel; 10 bits per BGR channel plus 2 bits for alpha.
};

ds_capture::format_type subtype_to_format_type(const GUID& subtype)
{
	for (int i=0; i<NR_TABLE_ENTRIES; ++i) {
		if (subtype == lookup_table[i].ms_type)
			return lookup_table[i].ft;
	}
	return ds_capture::UNDEFINED_FORMAT_TYPE;
}

GUID format_type_to_subtype(ds_capture::format_type ft)
{
	for (int i=0; i<NR_TABLE_ENTRIES; ++i) {
		if (ft == lookup_table[i].ft)
			return lookup_table[i].ms_type;
	}
	return GUID();
}


void run()
{
    MSG msg={0};
    while(GetMessage(&msg,NULL,0,0))
    {
		std::cout << "msg" << std::endl;
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

LRESULT CALLBACK WndMainProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LONG ud = GetWindowLongPtr(hwnd, GWLP_USERDATA);
	void* ptr = (void*&) ud;
	ds_capture_device* device = (ds_capture_device*) ptr;
	if (device) {
		switch (message)
		{
			case WM_GRAPHNOTIFY:
				device->handle_graph_event();
				break;
			case WM_SIZE:
				device->resize_video_window();
				break;
			case WM_WINDOWPOSCHANGED:
				device->change_preview_state(!(IsIconic(hwnd)));
				break;
			case WM_CLOSE:            
				// Hide the main window while the graph is destroyed
				device->hide();
				device->stop();
				break;
			case WM_DESTROY:
				PostQuitMessage(0);
				return 0;
		}

		// Pass this message to the video window for notification of system changes
		if (device->is_initialized())
			device->ref_video_window()->NotifyOwnerMessage((LONG_PTR) hwnd, message, wParam, lParam);
	}
    return DefWindowProc (hwnd , message, wParam, lParam);
}



bool ensure_com()
{
	static bool initialized = false;
	if (initialized)
		return true;
    // Initialize COM
	HRESULT hr;
    if(FAILED(hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED)))
    {
		std::cerr << "CoInitialize Failed!";
        exit(1);
    }
	if (hr == S_OK) {
		WNDCLASS wc;
		// Register the window class
		ZeroMemory(&wc, sizeof wc);
		wc.lpfnWndProc   = WndMainProc;
		wc.hInstance     = NULL;
		wc.lpszClassName = CLASSNAME;
		wc.lpszMenuName  = NULL;
		wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
		wc.hCursor       = LoadCursor(NULL, IDC_CROSS);
		if(!RegisterClass(&wc))
		{
			CoUninitialize();
			exit(1);
		}
	}
	initialized = true;
	return true;
}

		}
	}
}