//
// 
//
// Mutual exclusion (often abbreviated to mutex) algorithms are used in concurrent 
// programming to avoid the simultaneous use of un-shareable resources by pieces of 
// computer code called critical sections.
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

#include <pthread.h>

namespace cgv {
	namespace threading {

	/**
	*A simple mutex (mutual exclusion) for solving thread synchronisation problems.
	*/
	struct mutex
	{
		pthread_mutex_t pmutex;
		
		///construct a mutex
		mutex()
		{
			pthread_mutex_init (&pmutex, NULL);
		}
		
		///destruct a mutex
		~mutex()
		{
			pthread_mutex_destroy (&pmutex);
		}
		
		///lock the mutex (if the mutex is still locked, the caller is blocked until the
		///mutex becomes available)
		inline void lock()
		{
			pthread_mutex_lock (&pmutex);	
		}

		///unlock the mutex
		inline void unlock()
		{
			pthread_mutex_unlock (&pmutex);	
		}

		///try to lock the mutex (return false if the mutex is still locked)
		inline bool try_lock()
		{
			return pthread_mutex_trylock(&pmutex) != EBUSY;
		}

		
	};
	}
}

