#pragma once

#include <string>
#include <cgv/media/video/lib_begin.h>

namespace cgv {
	namespace media {
		namespace video {

/** the avi_file allows to read and write the image content
    of avi files using the Win32 API. */
class CGV_API avi_file
{
protected:
	/// support structure
	void* avi;
	/// frame rate
	int  fps;
	/// width
	int width;
	/// height
	int height;
	/// current frame index
	int fi;
	/// total number of frames
	int nr_frames;
public:
	/// different modes for opening files
	enum open_mode { READ, WRITE };
	/// whether to show debug information
	bool show_debug_info;
	/// create avi_file structure
	avi_file(int w = 320, int h = 200);
	/// close if not done so
	~avi_file();
	/// set the image dimensions
	void set_size(int w, int h);
	/// return whether the avi file has been opened
	bool is_open() const { return avi != 0; }
	/// return the width
	int get_width() const { return width; }
	/// return the height
	int get_height() const { return height; }
	/// return number of frames
	int get_nr_frames() const { return nr_frames; }
	/// return the number of frames per second
	int get_fps() const { return fps; }
	/// return the current frame index
	int get_frame_index() const { return fi; }
	/// jump to a given frame
	bool select_frame(int i);
	/// open file for reading or writing
	bool open(const std::string& file_name, open_mode mode);
	/// write a frame in rgb format, where the frame_ptr must point to memory of size 3*width*height
	bool write_rgb_frame(char* frame_ptr);
	/// read a frame in rgb format into the given buffer of size 3*width*height
	bool read_rgb_frame(char* frame_ptr);
	/// close avi_file, only necessary after writing
	void close();
};

		}
	}
}

#include <cgv/config/lib_end.h>
