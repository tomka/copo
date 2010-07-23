#ifndef STOPWATCH_H
#define STOPWATCH_H

#include <iostream>
#include <ctime>
#ifdef WIN32
#include <windows.h>
#endif

namespace cgv {
	namespace utils {

	/**
	* A trivial stop watch class for time measurement.
	* On Win32 the perfomance counter with a resolution of 0,313 microseconds is used, if available.
	* If the performance counter is not available and on other systems the std::clock() function with 
	* a resolution of 0,055 milliseconds is used.
	*
	* TODO: Better implementation for Linux (with higher resolution)
	* 
	* Example 1:
	*
	* {
	*	stopwatch s; 
	*	dosomethingtimeconsuming();
	* }
	* 
	* The duration is written into cout.
	*
	* Example 2:
	* double d = 0.0;
	*
	* {
	*	stopwatch s(&d); 
	*	dosomethingtimeconsuming();
	* }
	*
	* See also \link Profiler \endlink
	*/

	class stopwatch
	{
	public:
		//standard constructor starts time measurement
		stopwatch()
		{
			
			resultd = NULL;
#ifdef WIN32
			if(!QueryPerformanceFrequency((LARGE_INTEGER*) &frequencyW))
			{
				perfcounter_available=false;
#endif			
				start = std::clock();
#ifdef WIN32
			}
			else
			{
				perfcounter_available=true;
				QueryPerformanceCounter((LARGE_INTEGER*) &startW);
			}
#endif

		} 
		
		

		//start counting time
		stopwatch(double *result)
		{

			this->resultd = result;
#ifdef WIN32
			if(!QueryPerformanceFrequency((LARGE_INTEGER*) &frequencyW))
			{
				perfcounter_available=false;
#endif			
				start = std::clock();
#ifdef WIN32
			}
			else
			{
				perfcounter_available=true;
				QueryPerformanceCounter((LARGE_INTEGER*) &startW);
			}
#endif
			
		}
		// add_time adds the time ellapsed thus far
		void add_time()
		{
			double time;
#ifdef WIN32
			if(perfcounter_available)
			{
				QueryPerformanceCounter((LARGE_INTEGER*) &endW);
				time =(endW-startW)/ (double)frequencyW;
				startW = endW;				
			}
			else
			{
#endif
				std::clock_t end = clock();
				clock_t total = end - start; //get elapsed time
				time =  double(total)/CLOCKS_PER_SEC;
				start = end;
#ifdef WIN32
			}
#endif
			
			if(resultd)
			{
				*resultd += time;	
				
			}
			else
			{
				std::cout<<"elapsed time in seconds: "<<time<<std::endl;
			}
		}
		//destructor stops time measurement and puts the result into cout
		~stopwatch()
		{
			add_time();
		}
	private:

#ifdef WIN32
		LONGLONG startW,endW;
		LONGLONG frequencyW;

		bool perfcounter_available;
#endif
		std::clock_t start;

		double *resultd;
		
	};

	}
}

#endif //STOPWATCH_H

