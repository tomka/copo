#pragma once

#include <fltk/run.h>
#include <cgv/gui/fltk/trigger.h>
#include <iostream>

namespace cgv {
	namespace gui {
		namespace fltk {

static void timeout_std_callback(void* trig)
{
	((trigger*) trig)->trigger_event();
}

/// construct a trigger whose callback method is called after the given time either once, count times or recurrent, what is the default. If the time is initialized to <= 0, the trigger is not scheduled in the constructor
trigger::trigger(float _time, mode_type _mode, int _count) :
	time(_time), mode(_mode), count(_count)
{
	if (time > 0)
		schedule();
}
/// remove all pending trigger events in destructor
trigger::~trigger()
{
	stop();
}

/// set the repetition time without scheduling the trigger
void trigger::set_time(float _time)
{
	time = _time;
}
/// this method handles a trigger event, which can also be done by directly calling this method
void trigger::trigger_event()
{
	if (mode == RECURRENT || mode == COUNT && --count > 0)
		::fltk::repeat_timeout(time, timeout_std_callback, this);
	callback();
}
/// schedules the trigger according to the mode for the given time or if <= 0 the time stored in the trigger struct
void trigger::schedule(float _time)
{
	if (_time > 0)
		time = _time;
	else
		if (time <= 0)
			std::cerr << "attemt to schedule trigger for a time interval of <= 0 seconds" << std::endl;
	::fltk::add_timeout(time, timeout_std_callback, this);
}
/// stop a trigger
void trigger::stop()
{
	::fltk::remove_timeout(timeout_std_callback, this);
}

		}
	}
}