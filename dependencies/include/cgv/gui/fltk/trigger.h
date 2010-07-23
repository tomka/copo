#pragma once

#include <cgv/config/global.h>
#include <cgv/gui/fltk/lib_begin.h>

namespace cgv {
	namespace gui {
		namespace fltk {

/** a trigger that calls its callback after a scheduled time period */
struct CGV_API trigger
{
	/** different trigger types: 
		ONCE  ... schedule trigger once after time seconds and stop afterwards
		COUNT ... schedule trigger count times after i*time seconds and stop afterwards
		RECURRENT ... schedule trigger recurrent after time seconds until stop or destructor is called
	*/
	enum mode_type { ONCE, COUNT, RECURRENT };
	/// number of times to be triggered in COUNT mode
	int count;
	/// currently used mode
	mode_type mode;
	/// time in second before trigger is executed
	float time;
	/// construct a trigger whose callback method is called after the given time either once, count times or recurrent, what is the default. If the time is initialized to <= 0, the trigger is not scheduled in the constructor
	trigger(float _time = -1, mode_type _mode = RECURRENT, int _count = 10);
	/// remove all pending trigger events in destructor
	~trigger();
	/// set the repetition time without scheduling the trigger
	void set_time(float _time);
	/// this method handles a trigger event, which can also be done by directly calling this method
	virtual void trigger_event();
	/// schedules the trigger according to the mode for the given time or if <= 0 the time stored in the trigger struct
	virtual void schedule(float _time = -1);
	/// stop a trigger
	virtual void stop();
	/// overload this to receive the trigger callbacks
	virtual void callback() = 0;
};

		}
	}
}