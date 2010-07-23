#ifndef _TIMER_H
#define _TIMER_H

#include <iostream>

#ifdef _WIN32
	#include <windows.h>
#else
	#include <sys/time.h>
    #include <time.h>
#endif

#include <cmath>

namespace SnowSim {
namespace Utilities {

	class Timer {

#ifdef _WIN32

	public:
		LONGLONG freq;
		LONGLONG time1;
		LONGLONG time2;

	public:
		Timer() {
			QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
		}

		void start() {
			QueryPerformanceCounter((LARGE_INTEGER*)&time1);
		}

		void stop() {
			QueryPerformanceCounter((LARGE_INTEGER*)&time2);
		}

		double runningTimeInMsec() {
			LONGLONG current;
			QueryPerformanceCounter((LARGE_INTEGER*)&current);

			return (((double)(current-time1))/((double)freq)) * 1000;
		}

		double runningTimeInSec() {
			LONGLONG current;
			QueryPerformanceCounter((LARGE_INTEGER*)&current);

			return (((double)(current-time1))/((double)freq));
		}

		double diffTimeInSec() {
			return (((double)(time2-time1))/((double)freq));
		}

		double diffTimeInMSec() {
			return (((double)(time2-time1))/((double)freq)) * 1000;
		}

		int sec() {
			double secs;
			modf(diffTimeInSec(), &secs);
			return (int)secs;
		}

		long msec() {
			double secs;
			return (int)(modf(diffTimeInSec(), &secs) * 1000.0f) % 1000;
		}

		int usec() {
			double secs;
			return (int)(modf(diffTimeInSec(), &secs) * 1000000.0f) % 1000;
		}

		void printDifference(char* what = "Time taken") {
			std::cout << what << " : " << diffTimeInSec() << " s" << std::endl;
		}
#else
	private:
		 timeval _tstart, _tend;

	public:

		Timer()
		{
				//QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
		}

		void start()
		{
			gettimeofday(&_tstart, NULL);
		}

		void stop()
		{
			gettimeofday(&_tend, NULL);
		}

		double runningTimeInMsec()
		{
			timeval l_tend;
			gettimeofday(&l_tend, NULL);

			double t1, t2;

			t1 =  (double)(_tstart.tv_sec * 1000.0) + (double)_tstart.tv_usec*0.001;

			t2 =  (double)(l_tend.tv_sec * 1000.0) + (double)l_tend.tv_usec*0.001;

			return t2-t1;
		}

		double runningTimeInSec() {
			timeval l_tend;
			gettimeofday(&l_tend, NULL);

			double t1, t2;

			t1 =  (double)_tstart.tv_sec + (double)_tstart.tv_usec*0.000001;

			t2 =  (double)l_tend.tv_sec + (double)l_tend.tv_usec*0.000001;

			return t2-t1;
		}

		double diffTimeInMSec() {
			double t1, t2;

			t1 =  (double)(_tstart.tv_sec * 1000.0) + (double)_tstart.tv_usec*0.001;

			t2 =  (double)(_tend.tv_sec * 1000.0) + (double)_tend.tv_usec*0.001;

			return t2-t1;
		}

		double diffTimeInSec() {
			double t1, t2;

			t1 =  (double)_tstart.tv_sec + (double)_tstart.tv_usec*0.000001;

			t2 =  (double)_tend.tv_sec + (double)_tend.tv_usec*0.000001;

			return t2-t1;
		}

		int sec() {
			double secs;
			modf(diffTimeInSec(), &secs);
			return (int)secs;
		}

		long msec() {
			double secs;
			return (int)(modf(diffTimeInSec(), &secs) * 1000.0f) % 1000;
		}

		int usec() {
			double secs;
			return (int)(modf(diffTimeInSec(), &secs) * 1000000.0f) % 1000;
		}

		void printDifference(char* what = "Time taken") {
			std::cout << what << " : " << diffTimeInSec() << " s" << std::endl;
		}


#endif
	};
}
}
#endif
