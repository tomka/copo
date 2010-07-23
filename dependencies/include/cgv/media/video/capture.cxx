#include <cgv/media/video/capture.h>
#include <windows.h>
#include <Vfw.h>

namespace cgv {
	namespace media {
		namespace video {

storage::collection::vector<std::string>& capture_base::device_names()
{
	static storage::collection::vector<std::string> names;
	return names;
}

storage::collection::vector<std::string>& capture_base::device_versions()
{
	static storage::collection::vector<std::string> devices;
	return devices;
}

void capture_base::scan_devices()
{
	__wchar_t w_name[80];
	__wchar_t w_version[80];
	char name[100];
	char version[100];
	device_names().clear();
	device_versions().clear();
	int i = 0;
	size_t s;
	for (i = 0; i < 10; ++i) {
		if (capGetDriverDescription(i, w_name, 80, w_version, 80)) {
			if (wcstombs_s(&s, name, 100, w_name, 100) == 0 &&
				wcstombs_s(&s, version, 100, w_version, 100) == 0) {
				device_names().push_back(std::string(name));
				device_versions().push_back(std::string(version));
			}
		}
	}
}

capture_base::capture_base()
{
	handle = 0;
	capabilities = 0;
	callback = 0;
	overlayed = false;
}

/// construction method
void capture_base::construct(int w, int h, bool show)
{
	HWND hWndC = capCreateCaptureWindow (L"", show? WS_VISIBLE : WS_DISABLED, 0, 0, w, h, (HWND) 0, (int) 0);
	capture_base* t = this;
	SetWindowLong(hWndC, GWL_USERDATA, (LONG&) t);
	handle = (void*&)hWndC;
}

capture::capture(int w, int h, bool show, int i)
{
	construct(w,h,show);
	if (i > -1)
		attach(i);
}

capture_avi::capture_avi(int w, int h, bool show, int i)
{
	construct(w,h,show);
	if (i > -1)
		attach(i);
}

capture_thread::capture_thread(int w, int h, bool show, int i)
{
	con_w = w;
	con_h = h;
	con_show = show;
	thread::start();
	while (handle == 0) {
		Sleep(10);
	}
	if (i > -1)
		attach(i);
}

capture_base::~capture_base()
{
	if (callback)
		set_callback(0);
	detach();
	if (handle)
		std::cout << "destroy = " << DestroyWindow((HWND&) handle) << std::endl;
	handle = 0;
}

capture_thread::~capture_thread()
{
	thread::stop();
	wait_for_completion();
}

bool capture_base::attach(int i)
{
	if (capDriverConnect((HWND&) handle, i)) {
		if (capabilities == 0)
			capabilities = new CAPDRIVERCAPS;
		std::cout << "get caps = " << capDriverGetCaps((HWND&) handle, (CAPDRIVERCAPS*) capabilities, sizeof(CAPDRIVERCAPS)) << std::endl;
	}
	overlayed = false;
	return is_attached();
}

void capture_base::detach()
{
	if (is_attached())
		std::cout << "detach = " << capDriverDisconnect((HWND&) handle) << std::endl;
	if (capabilities) {
		delete (CAPDRIVERCAPS*) capabilities;
		capabilities = 0;
	}
	overlayed = false;
}

bool capture_base::has_overlay() const
{
	if (is_attached())
		return ((CAPDRIVERCAPS*) capabilities)->fHasOverlay == TRUE;
	return false;
}
bool capture_base::has_source_dlg() const
{
	if (is_attached())
		return ((CAPDRIVERCAPS*) capabilities)->fHasDlgVideoSource == TRUE;
	return false;
}
bool capture_base::has_format_dlg() const
{
	if (is_attached())
		return ((CAPDRIVERCAPS*) capabilities)->fHasDlgVideoFormat == TRUE;
	return false;
}
bool capture_base::has_display_dlg() const
{
	if (is_attached())
		return ((CAPDRIVERCAPS*) capabilities)->fHasDlgVideoDisplay == TRUE;
	return false;
}
bool capture_base::is_initialized() const
{
	if (is_attached())
		return ((CAPDRIVERCAPS*) capabilities)->fCaptureInitialized == TRUE;
	return false;
}

void capture_base::repaint()
{
	SendMessage((HWND&) handle, WM_PAINT, 0, 0);
}

///
void capture_base::enable_preview(float fps)
{
	capPreviewRate((HWND&) handle, 1000/fps);
	capPreviewScale((HWND&) handle, TRUE);
	capPreview((HWND&) handle, TRUE);
}
void capture_base::disable_preview()
{
	capPreview((HWND&) handle, FALSE);
}

bool capture_base::enable_overlay()
{
	if (capOverlay((HWND&) handle, TRUE) == TRUE)
		overlayed = true;
	return overlayed;
}
void capture_base::disable_overlay()
{
	if (capOverlay((HWND&) handle, FALSE) == TRUE)
		overlayed = false;
}

bool capture_base::is_overlayed() const
{
	return overlayed;
}

void capture_base::show_format_dlg()
{
	capDlgVideoFormat((HWND&) handle);
}
void capture_base::show_source_dlg()
{
	capDlgVideoSource((HWND&) handle);
}
void capture_base::show_display_dlg()
{
	capDlgVideoDisplay((HWND&) handle);
}

#define EXTRACT_MEMBER(MEMBER,TYPE) \
	LPBITMAPINFO lpbi; \
	DWORD dwSize; \
	dwSize = capGetVideoFormatSize((HWND&) handle); \
	lpbi = (LPBITMAPINFO) GlobalAlloc(GHND, dwSize); \
	capGetVideoFormat((HWND&) handle, lpbi, dwSize); \
	TYPE tmp = (TYPE)lpbi->bmiHeader.MEMBER;\
	GlobalFree(lpbi);\
	return tmp;

/// query video format
uint32 capture_base::w() const
{
	EXTRACT_MEMBER(biWidth, uint32);
}

uint32 capture_base::h() const
{
	EXTRACT_MEMBER(biHeight, uint32);
}

uint32 capture_base::nr_planes() const
{
	EXTRACT_MEMBER(biPlanes, uint32);
}

uint32 capture_base::nr_bits() const
{
	EXTRACT_MEMBER(biBitCount, uint32);
}

size_type capture_base::size() const
{
	EXTRACT_MEMBER(biSizeImage, size_type);
}

/// set video format. If a parameter is 0, keep previous value. Return if this was possible.
bool capture_base::set_format(uint32 w, uint32 h, uint32 nr_planes, uint32 nr_bits)
{
	LPBITMAPINFO lpbi;
	DWORD dwSize;
	dwSize = capGetVideoFormatSize((HWND&) handle);
	lpbi = (LPBITMAPINFO) GlobalAlloc(GHND, dwSize);
	capGetVideoFormat((HWND&) handle, lpbi, dwSize);
	if (w > 0)
		lpbi->bmiHeader.biWidth = w;
	if (h > 0)
		lpbi->bmiHeader.biHeight = h;
	if (nr_planes > 0)
		lpbi->bmiHeader.biPlanes = nr_planes;
	if (nr_bits > 0)
		lpbi->bmiHeader.biBitCount = nr_bits;

	bool tmp = capSetVideoFormat((HWND&) handle, lpbi, dwSize) == TRUE;
	GlobalFree(lpbi);
	return tmp;
}

/** controll of capture_base parameters */
float capture_base::get_fps() const
{
	CAPTUREPARMS params;
	capCaptureGetSetup((HWND&) handle, &params, sizeof(CAPTUREPARMS));
	return 1000000.0f/params.dwRequestMicroSecPerFrame;
}

void capture_base::set_fps(float fps)
{
	CAPTUREPARMS params;
	capCaptureGetSetup((HWND&) handle, &params, sizeof(CAPTUREPARMS));
	params.dwRequestMicroSecPerFrame = DWORD(1000000/fps);
	capCaptureSetSetup((HWND&) handle, &params, sizeof(CAPTUREPARMS));
}

void capture_base::enable_start_dlg()
{
	CAPTUREPARMS params;
	capCaptureGetSetup((HWND&) handle, &params, sizeof(CAPTUREPARMS));
	params.fMakeUserHitOKToCapture = TRUE;
	capCaptureSetSetup((HWND&) handle, &params, sizeof(CAPTUREPARMS));
}

void capture_base::disable_start_dlg()
{
	CAPTUREPARMS params;
	capCaptureGetSetup((HWND&) handle, &params, sizeof(CAPTUREPARMS));
	params.fMakeUserHitOKToCapture = FALSE;
	capCaptureSetSetup((HWND&) handle, &params, sizeof(CAPTUREPARMS));
}

void capture_base::set_maximal_drop_fraction(float frac)
{
	CAPTUREPARMS params;
	capCaptureGetSetup((HWND&) handle, &params, sizeof(CAPTUREPARMS));
	params.wPercentDropForError = (UINT) (100*frac);
	capCaptureSetSetup((HWND&) handle, &params, sizeof(CAPTUREPARMS));
}

void capture_base::enable_threading()
{
	CAPTUREPARMS params;
	capCaptureGetSetup((HWND&) handle, &params, sizeof(CAPTUREPARMS));
	params.fYield = TRUE;
	capCaptureSetSetup((HWND&) handle, &params, sizeof(CAPTUREPARMS));
}

void capture_base::disable_threading()
{
	CAPTUREPARMS params;
	capCaptureGetSetup((HWND&) handle, &params, sizeof(CAPTUREPARMS));
	params.fYield = FALSE;
	capCaptureSetSetup((HWND&) handle, &params, sizeof(CAPTUREPARMS));
}

bool capture_base::is_threading() const
{
	CAPTUREPARMS params;
	capCaptureGetSetup((HWND&) handle, &params, sizeof(CAPTUREPARMS));
	return params.fYield == TRUE;
}

bool capture_thread::set_nr_buffers(uint32 nr)
{
	CAPTUREPARMS params;
	capCaptureGetSetup((HWND&) handle, &params, sizeof(CAPTUREPARMS));
	params.dwIndexSize = nr;
	return capCaptureSetSetup((HWND&) handle, &params, sizeof(CAPTUREPARMS)) == TRUE;
}

void capture_base::enable_audio()
{
	CAPTUREPARMS params;
	capCaptureGetSetup((HWND&) handle, &params, sizeof(CAPTUREPARMS));
	params.fCaptureAudio = TRUE;
	capCaptureSetSetup((HWND&) handle, &params, sizeof(CAPTUREPARMS));
}

void capture_base::disable_audio()
{
	CAPTUREPARMS params;
	capCaptureGetSetup((HWND&) handle, &params, sizeof(CAPTUREPARMS));
	params.fCaptureAudio = FALSE;
	capCaptureSetSetup((HWND&) handle, &params, sizeof(CAPTUREPARMS));
}

void capture_avi::set_duration(float seconds)
{
	CAPTUREPARMS params;
	capCaptureGetSetup((HWND&) handle, &params, sizeof(CAPTUREPARMS));
	params.fLimitEnabled = TRUE;
	params.wTimeLimit = UINT(seconds);
	capCaptureSetSetup((HWND&) handle, &params, sizeof(CAPTUREPARMS));
}

LRESULT PASCAL frame_callback(HWND hWnd, LPVIDEOHDR lpVHdr)
{ 
    if (!hWnd) 
        return FALSE; 

	LONG user_data = GetWindowLong(hWnd, GWL_USERDATA);
	capture_base* c = (capture_base*&) user_data;
	if (c->get_callback()) {
		capture_callback::Info info;
		info.data_ptr = (char*)lpVHdr->lpData;
		info.buffer_size = lpVHdr->dwBufferLength;
		info.nr_bytes = lpVHdr->dwBytesUsed;
		info.ellapsed_time = lpVHdr->dwTimeCaptured;
		info.width = c->w();
		info.height = c->h();
		c->get_callback()->view(info);
	}
	return TRUE ; 
} 

LRESULT PASCAL stream_callback(HWND hWnd, LPVIDEOHDR lpVHdr)
{ 
    if (!hWnd) 
        return FALSE; 

	LONG user_data = GetWindowLong(hWnd, GWL_USERDATA);
	capture_base* c = (capture_base*&) user_data;
	if (c->get_callback()) {
		capture_callback::Info info;
		info.data_ptr = (char*)lpVHdr->lpData;
		info.buffer_size = lpVHdr->dwBufferLength;
		info.nr_bytes = lpVHdr->dwBytesUsed;
		info.ellapsed_time = lpVHdr->dwTimeCaptured;
		info.width = c->w();
		info.height = c->h();
		c->get_callback()->stream(info);
	}
	return TRUE ; 
} 

bool capture_base::set_callback(capture_callback* c)
{
	callback = c;
	if (c == 0) {
		bool r1 = capSetCallbackOnFrame((HWND&) handle, NULL) == TRUE;
		bool r2 = capSetCallbackOnVideoStream((HWND&) handle, NULL) == TRUE;
		return r1 && r2;
	}
	else {
		bool r1 = capSetCallbackOnFrame((HWND&) handle, frame_callback) == TRUE;
		bool r2 = capSetCallbackOnVideoStream((HWND&) handle, stream_callback) == TRUE;
		return r1 && r2;
	}
}

/** perform capture_base */
bool capture_avi::start(const std::string& file_name)
{
	size_t l1, l0 = file_name.size();
	wchar_t *file_name_w = new wchar_t[l0+1];
	if (mbstowcs_s(&l1, file_name_w, l0+1, file_name.c_str(), l0) == 0) {
		BOOL r1 = capFileSetCaptureFile((HWND&) handle, file_name_w);
		BOOL r2 = capCaptureSequence((HWND&) handle);
		return r1 && r2;
	}
	return false;
}

bool capture_avi::start()
{
	return capCaptureSequenceNoFile((HWND&) handle) == TRUE;
}

///
void capture_thread::run()
{
	HWND hWndC = capCreateCaptureWindow (L"", con_show? WS_VISIBLE : WS_DISABLED, 0, 0, con_w, con_h, (HWND) 0, (int) 0);
//	std::cout << "handle = " << hWndC << " of size " << sizeof(hWndC) << std::endl;
	capture_base* t = this;
	SetWindowLong(hWndC, GWL_USERDATA, (LONG&) t);
	handle = (void*&)hWndC;
	MSG msg;
	std::cout << " started!!! " << std::endl;
	while (!stop_request) {
		BOOL bRet = PeekMessage( &msg, (HWND&) handle, 0, 0, PM_NOREMOVE);
		if (bRet) {
			bRet = GetMessage( &msg, (HWND&) handle, 0, 0);
			if (bRet == -1)
			{
				std::cout << "ERROR" << std::endl;
				return;
			}
			else
			{
	//			std::cout << "dispatch message " << msg.message << " time = " << msg.time << std::endl;
				TranslateMessage(&msg); 
				DispatchMessage(&msg); 
			}
		}
	}
	detach();
	if (handle)
		std::cout << "destroy = " << DestroyWindow((HWND&) handle) << std::endl;
	handle = 0;
}

bool capture_thread::start()
{
	return capCaptureSequenceNoFile((HWND&) handle) == TRUE;
}

bool capture::start()
{
	return capCaptureSingleFrameOpen((HWND&) handle) == TRUE;
}

bool capture::capture_frame()
{
	return capCaptureSingleFrame((HWND&) handle) == TRUE;
}

void capture::stop()
{
	capCaptureSingleFrameClose((HWND&) handle);
}

void capture_thread::stop()
{
	thread::stop();
	capCaptureStop((HWND&) handle);
}

		}
	}
}
