#include <cgv/media/video/avi_file.h>
#include <iostream>

namespace cgv {
	namespace media {
		namespace video {

#ifdef WIN32
#undef UNICODE
#define  STRICT
#define  INC_OLE2
#include <windows.h>
#include <windowsx.h>
#include <memory.h>
#include <mmsystem.h>
#include <vfw.h>
#include <wingdi.h>

struct AVIVideoInfo
{
	PAVIFILE pfile;
	PAVISTREAM ps;
	PAVISTREAM psCompressed;
	PGETFRAME  getFrame;
	AVIVideoInfo() : pfile(0), ps(0), psCompressed(0) {}
};

bool avi_file::open(const std::string& fileName, open_mode mode)
{
	AVISTREAMINFO strhdr;
	AVICOMPRESSOPTIONS opts;
	AVICOMPRESSOPTIONS FAR * aopts[1] = {&opts};
	AVIVideoInfo* info = new AVIVideoInfo();
	avi = info;
	fi = 0;
	AVIFileInit();
	if (mode == WRITE) {
		// open movie file
		HRESULT hr = AVIFileOpen(&info->pfile, fileName.c_str(), OF_WRITE | OF_CREATE,NULL);
		if (hr != AVIERR_OK) return false;
		info->getFrame = 0;
		// fill header for the video stream....
		_fmemset(&strhdr, 0, sizeof(strhdr));
		strhdr.fccType                = streamtypeVIDEO;// stream type
		strhdr.fccHandler             = 0;
		strhdr.dwScale                = 1;
		strhdr.dwRate                 = fps;		    // 15 fps
		strhdr.dwSuggestedBufferSize  = 3*width*height;
		SetRect(&strhdr.rcFrame, 0, 0,	width, height);

		// create the stream;
		hr = AVIFileCreateStream(info->pfile,	&info->ps, &strhdr);
		if (hr != AVIERR_OK) goto error;

		// ask for options
		_fmemset(&opts, 0, sizeof(opts));
		if (!AVISaveOptions(NULL, 0, 1, &info->ps, (LPAVICOMPRESSOPTIONS FAR *) &aopts)) goto error;

		// make compressed stream
		hr = AVIMakeCompressedStream(&info->psCompressed, info->ps, &opts, NULL);
		if (hr != AVIERR_OK) goto error;

		BITMAPINFOHEADER bhi;
		bhi.biSize = sizeof(BITMAPINFOHEADER);
		bhi.biWidth = width;
		bhi.biHeight = height;
		bhi.biPlanes = 1;
		bhi.biBitCount = 24;
		bhi.biCompression = BI_RGB;
		bhi.biSizeImage = 0;
		bhi.biXPelsPerMeter = 0 ;
		bhi.biYPelsPerMeter = 0 ;
		bhi.biClrUsed = 0;
		bhi.biClrImportant = 0;

		hr = AVIStreamSetFormat(info->psCompressed, 0, &bhi, bhi.biSize);
		if (hr == AVIERR_OK) return true;
	}
	else {
		// read avi
		HRESULT hr = AVIFileOpen(&info->pfile, fileName.c_str(), OF_READ | OF_SHARE_DENY_WRITE,NULL);
		if (hr != AVIERR_OK) {
			info->getFrame = 0;
			std::cerr << "couldn't open avi file " << fileName << std::endl;
			return false;
		}
		info->psCompressed = 0;
		// get the file info
		AVIFILEINFO fileInfo;
		hr = AVIFileInfo(info->pfile, &fileInfo, sizeof(fileInfo));
		if (hr != AVIERR_OK) {
			std::cerr << "no file info available" << std::endl;
			goto error;
		}
		width = fileInfo.dwWidth;
		height = fileInfo.dwHeight;
		fps = int(float(fileInfo.dwRate)/fileInfo.dwScale);
		if (show_debug_info)
			std::cout << "avi width = " << width << ", height = " << height << ", fps = " << fps << ", nr streams = " << fileInfo.dwStreams << std::endl;
		// get the stream
		hr = AVIFileGetStream(info->pfile, &info->ps, streamtypeVIDEO, 0);
		if (hr != AVIERR_OK) {
			std::cerr << "no video stream found" << std::endl;
			goto error;
		}
		// get the stream info
		hr = AVIStreamInfo(info->ps, &strhdr, sizeof(strhdr));
		if (hr != AVIERR_OK) {
			std::cerr << "no stream info provided" << std::endl;
			goto error;
		}
		width = strhdr.rcFrame.right-strhdr.rcFrame.left;
		height = strhdr.rcFrame.bottom-strhdr.rcFrame.top;
		nr_frames = AVIStreamLength(info->ps);
		if (show_debug_info)
			std::cout << "stream width = " << width << ", height = " << height << "nr_frames = " << nr_frames << ", starting frame = " << strhdr.dwStart << std::endl;
		if (nr_frames == -1) {
			std::cerr << "no stream length provided" << std::endl;
			goto error;
		}
		if (show_debug_info)
			std::cout << "nr_frames = " << nr_frames << std::endl;
		char fmt[5];
		fmt[4] = 0;
		((DWORD&) fmt[0]) = strhdr.fccHandler;
		if (show_debug_info)
			std::cout << "format: " << fmt << std::endl;

		hr = AVIStreamBeginStreaming(info->ps, 0, nr_frames, 1000);
		if (hr != AVIERR_OK) {
			std::cerr << "start of streaming not possible" << std::endl;
			goto error;
		}
		info->getFrame = AVIStreamGetFrameOpen(info->ps, NULL);
		if (info->getFrame == NULL) {
			std::cerr << "no decompressor found" << std::endl;
			goto error;
		}
		fi = 0;
		return true;
	}
error:
	close();
	return false;
}

/// write a frame
bool avi_file::write_rgb_frame(char* frame)
{
	if (avi == 0) 
		return false;
	AVIVideoInfo *info = (AVIVideoInfo*) avi;
	// toggle red and blue component in frame
	int i, s = width*height;
	char* ptr = frame;
	for (i=0; i<s; ++i, ptr += 3) 
		std::swap(ptr[0],ptr[2]);
	// write frame
	HRESULT hr = AVIStreamWrite(info->psCompressed, fi, 1, frame, 3*s, AVIIF_KEYFRAME, 0, 0);
	// toggle red and blue components back
	ptr = frame;
	for (i=0; i<s; ++i, ptr += 3) std::swap(ptr[0],ptr[2]);
	if (hr != AVIERR_OK) return false;
	++fi;
	return true;
}

/// read a frame into the given buffer
bool avi_file::read_rgb_frame(char* frame)
{
	if (!avi) return false;
	AVIVideoInfo *info = (AVIVideoInfo*) avi;
	// read frame
	int i, s = width*height;
	if (info->getFrame != NULL) {
		char* buffer = (char*) AVIStreamGetFrame(info->getFrame, fi);
		if (buffer == 0) return false;
		// toggle red and blue components back
		buffer += sizeof(BITMAPINFOHEADER);
		char* ptr = frame;
		for (i=0; i<s; ++i, ptr += 3, buffer += 3) {
			ptr[0] = buffer[2];
			ptr[1] = buffer[1];
			ptr[2] = buffer[0];
		}
	}
	else {
		return false;
	}
	++fi;
	return true;
}
/// jump to a given frame
bool avi_file::select_frame(int i)
{
	if (!avi) return false;
	AVIVideoInfo *info = (AVIVideoInfo*) avi;
	// close get frame 
	if (info->getFrame) {
		AVIStreamGetFrameClose(info->getFrame);
		info->getFrame = 0;
	}
	HRESULT hr = AVIStreamBeginStreaming(info->ps, i, nr_frames, 1000);
	if (hr != AVIERR_OK) {
		std::cerr << "streaming from " << i << " not possible" << std::endl;
		return false;
	}
	info->getFrame = AVIStreamGetFrameOpen(info->ps, NULL);
	if (info->getFrame == NULL) {
		std::cerr << "no decompressor found" << std::endl;
		return false;
	}
	fi = i;
	return true;
}

/// close after writing
void avi_file::close()
{
	if (avi) {
		AVIVideoInfo *info = (AVIVideoInfo*) avi;
		if (info->ps) AVIStreamClose(info->ps);
		if (info->psCompressed) AVIStreamClose(info->psCompressed);
		if (info->pfile) AVIFileClose(info->pfile);
		if (info->getFrame) AVIStreamGetFrameClose(info->getFrame);
		AVIFileExit();
		delete info;
		avi = 0;
	}
}
#else
bool avi_file::open(const std::string& fileName, open_mode mode)
{
	std::cerr << "avi_file::open not implemented" << std::endl;
	return false;
}
bool avi_file::write_rgb_frame(char* frame)
{
	std::cerr << "avi_file::write_rgb_frame not implemented" << std::endl;
	return false;
}
bool avi_file::read_rgb_frame(char* frame)
{
	std::cerr << "avi_file::read_rgb_frame not implemented" << std::endl;
	return false;
}
bool avi_file::select_frame(int i)
{
	std::cerr << "avi_file::select_frame not implemented" << std::endl;
	return false;
}
void avi_file::close()
{
	std::cerr << "avi_file::close not implemented" << std::endl;
}

#endif

avi_file::avi_file(int w, int h)
{
	avi = 0;
	show_debug_info = false;
	fps = 25;
	width = w;
	height = h;
	fi = 0;
	nr_frames = 0;
}


/// set the image dimensions
void avi_file::set_size(int w, int h)
{
	width = w;
	height = h;
}

/// close if not done so
avi_file::~avi_file()
{
	close();
}

		}
	}
}
