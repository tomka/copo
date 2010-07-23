#pragma once

#pragma warning(disable:4996)
#include <windows.h>
#include <tchar.h>
#include <cgv/media/video/ds_capture.h>
#include <cgv/media/video/lib_begin.h>

// {5E8FDF5C-9325-49f9-BBEC-47BBFDF8E4C9}
extern "C" const __declspec(selectany) GUID CLSID_dc_cb_filter;
// {40D27EE8-1365-43dd-A700-212F1A85B1A6}
extern "C" const __declspec(selectany) GUID CLSID_Callback;
// I420 isn't defined in any of the headers, but everyone seems to use it.... 
extern "C" const __declspec(selectany) GUID MEDIASUBTYPE_I420;	
// IYUV
extern "C" const __declspec(selectany) GUID MEDIASUBTYPE_IYUV;
// Y444
extern "C" const __declspec(selectany) GUID MEDIASUBTYPE_Y444;
// Y800
extern "C" const __declspec(selectany) GUID MEDIASUBTYPE_Y800;
// Y422
extern "C" const __declspec(selectany) GUID MEDIASUBTYPE_Y422;

namespace cgv {
	namespace media {
		namespace video {

// Application-defined message to notify app of filtergraph events
#define WM_GRAPHNOTIFY  WM_APP+1
#define CLASSNAME       TEXT("VidCapPreviewer\0")
#define NR_TABLE_ENTRIES 31

struct mstype_to_format_type {
	GUID ms_type;
	ds_capture::format_type ft;
};

extern CGV_API mstype_to_format_type lookup_table[NR_TABLE_ENTRIES];
extern CGV_API bool ensure_com();
extern CGV_API void run();
extern CGV_API ds_capture::format_type subtype_to_format_type(const GUID& subtype);
extern CGV_API GUID format_type_to_subtype(ds_capture::format_type ft);

		}
	}
}

#include <cgv/config/lib_end.h>
