//
// 
//
// Simple thread class. This class is an abstract base class for a thread.
// Derive your concrete thread from this class and implement the void run() method.
// To start the thread execution call start().
//
// Authored by Sören König
//
// This library is free software, without any warrenty
// you can redistribute it and/or modify it under the 
// terms of the GNU Library General Public License 2 or
// later as published by the Free Software Foundation
//
// Please report all bugs and problems to "cgv@inf.tu-dresden.de".
//

#pragma once

#include <cgv/threading/mutex.h>
#include <iostream>


namespace cgv {
	namespace threading {

	///simple thread class
	class thread
	{
	protected:
		pthread_t *pthread;
		
		bool stop_request;
		bool running;
		
		static inline void* execute_s(void* args) {((thread*)args)->execute(); return NULL; }

	public:
		

		///create the thread
		thread()
		{ 
			pthread = new pthread_t();
			
			
			stop_request = false;
			running = false;
			
		}

		

		
		///start the implemented run() method (asynchronly)
		void start()
		{
			if(!running)
			{
				stop_request=false;
				if(pthread_create(pthread,NULL,execute_s,this))
					std::cerr << "error: starting thread" <<std::endl;
				running=true;
					
				
			}
		}

		void execute()
		{
			run(); 
			running=false; 
		}

		///thread function to override
		virtual void run()=0;
		
		/// try to stop the thread execution via indicating a stop request. The existence of a stop request
		/// can be recognized by the no_stop_request() method. This test should be done periodically within
		/// the implementation of the run() method, e.g. to leave the execution loop in a clean way.
		void stop()
		{
			if(running)
			{
				stop_request=true;
				pthread_join(*pthread,NULL);
				stop_request=false;
			}
		}

		
		///kill a running thread
		void kill()
		{
			if(running)
			{
				pthread_cancel(*pthread);
				stop_request=false;
				running=false;
			}
		}

		///join the current thread
		void wait_for_completion()
		{
			if(running)
				pthread_join(*pthread,NULL);
			
		}

		///return true if thread is running
		inline bool is_running()
		{
			return running;
		}



		///check if there is a stop request (helper to implement a concrete run() method 
		inline bool no_stop_request()
		{
			return !stop_request;
		}

		///standard destructor (a running thread will be killed)
		virtual ~thread()
		{
			if(running)
				kill();
			delete pthread;
			
			
		}

		

	};
	
	}
}

/*

how to create and run a thread?


class mythread : thread
{
	
	void run()
	{
		//no external stop request? 
		while(no_stop_request())
		{
			std::cout << "abc" << std::endl;
		}
	}
	
};

mythread t1;

t1.start();
...
t1.stop();






*/