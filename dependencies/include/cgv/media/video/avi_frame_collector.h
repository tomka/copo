#pragma once

#pragma warning(disable:4275)
#pragma warning(disable:4251)

#include <cgv/threading/thread.h>
#include <cgv/threading/pc_queue.h>

#include <cgv/media/video/avi_file.h>

#include <cgv/media/video/lib_begin.h>

namespace cgv {
	namespace media {
		namespace video {


class CGV_API avi_frame_collector : public threading::thread, public avi_file
{
protected:
	/// keep n-1 frames in queue, what ensures that always one frame can be written to
	cgv::threading::pc_queue<char*> queue;
	/// number of buffered frames
	int nr_buffered_frames;
	/// allocate n frames
	char* frame_buffers;
	/// size of one frame
	int frame_size;
	/// next to be filled frame
	int next_write_frame;
	/// next to be read frame
	int next_read_frame;
public:
	/// 
	avi_frame_collector();
	///
	~avi_frame_collector();
	/// open with a buffer for given number of frames
	bool open(const char* fileName, int nrFrames);
	/// overload to compute something and return whether this was successful
	void run();
	/// return the current frame index
	int get_frame_index() const { return next_read_frame; }
	/// returns pointer to the next frame, if this is called, the collector assumes that the previously returned frame is not used anymore
	char* get_next_frame();
};

		}
	}
}

#include <cgv/config/lib_end.h>
