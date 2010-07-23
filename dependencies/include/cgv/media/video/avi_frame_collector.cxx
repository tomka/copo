#include <cgv/media/video/avi_frame_collector.h>

namespace cgv {
	namespace media {
		namespace video {

/// 
avi_frame_collector::avi_frame_collector() 
{
	frame_buffers = 0; 
}
///
avi_frame_collector::~avi_frame_collector() 
{
	if (frame_buffers) delete [] frame_buffers;
}
///
bool avi_frame_collector::open(const char* fileName, int n)
{
	queue.resize(n);
	if (!avi_file::open(fileName, READ)) 
		return false;
	frame_size = 3*get_width()*get_height();
	if (frame_buffers) 
		delete [] frame_buffers;
	nr_buffered_frames = n+1;
	frame_buffers = new char[frame_size*nr_buffered_frames];
	next_write_frame = 0;
	next_read_frame = 0;
	return true;
}

/// overload to compute something and return whether this was successful
void avi_frame_collector::run()
{
	while (no_stop_request()) {
		char* frame_pointer = frame_buffers + frame_size*(next_write_frame%nr_buffered_frames);
		if (!read_rgb_frame(frame_pointer)) {
			if (show_debug_info)
				std::cout << "closing avi after " << fi << " frames with " << next_write_frame << " frames used." << std::endl;
			break;
		}
		queue.add(frame_pointer);
		if (show_debug_info)
			std::cout << "add " << next_write_frame << " : " << (void*)frame_pointer << std::endl;
		++next_write_frame;
	}
	queue.complete();
	close();
}

/// returns pointer to the next frame or 0 if no next frame available. If this is called, the collector assumes that the previously returned frame is not used anymore
char* avi_frame_collector::get_next_frame()
{
	char* frame_pointer;
	if (!queue.remove(frame_pointer))
		return 0;
	if (show_debug_info)
		std::cout << "got " << next_read_frame << " : " << (void*)frame_pointer << std::endl;
	++next_read_frame;
	return frame_pointer;
}

		}
	}
}
