#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4995)
#include <windows.h>
#include <tchar.h>
#include <dshow.h>
#include <string>

#include <cgv/media/video/lib_begin.h>

namespace cgv {
	namespace media {
		namespace video {

struct CGV_API ds_video_device_iterator
{
	HRESULT hr;
	ICreateDevEnum* pDevEnum;
	IEnumMoniker* pClassEnum;
	IMoniker* pMoniker;
	ds_video_device_iterator();
	~ds_video_device_iterator();
	void operator ++();
	bool at_end() const;
	IMoniker* operator -> ();
	std::string get_name();
};

		}
	}
}

#include <cgv/config/lib_end.h>
