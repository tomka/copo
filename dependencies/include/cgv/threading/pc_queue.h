#pragma once

#include <pthread.h>
#include <cgv/type/size_type.h>
#include <cgv/storage/collection/array.h>

namespace cgv {
	namespace threading {

/**
* Fixed size producer-consumer-queue. 
*/
template<typename T, cgv::type::uint32 N = 0> 
class pc_queue
{
protected:

	cgv::storage::collection::array<T,N> buffer;
		
	int counter;
	int lo;
	int hi;
	
	pthread_mutex_t counter_mutex;
	pthread_cond_t not_empty_anymore, not_full_anymore;	
	bool completing;
	
			


public:
	///standard constructor
	pc_queue()
	{
		init();
	}
	///standard constructor
	pc_queue(cgv::type::uint32 n) : buffer(n)
	{
		init();
	}

	void init()
	{
		pthread_mutex_init (&counter_mutex, NULL);
		pthread_cond_init (&not_empty_anymore, NULL);
		pthread_cond_init (&not_full_anymore, NULL);
		counter=0;
		hi=lo=0;
		completing=false;
	}
	/// resize the queue
	void resize(cgv::type::uint32 n)
	{
		buffer.resize(n);
	}

	
	///standard destructor
	virtual ~pc_queue()
	{
		pthread_mutex_destroy (&counter_mutex);
		pthread_cond_destroy (&not_empty_anymore);
		pthread_cond_destroy (&not_full_anymore);
	
		
	}

	
	///The producer thread has to call add to insert an element into the queue. The method returns
	///until the element was successfully added.
	void add(T& in)
	{

		
		pthread_mutex_lock (&counter_mutex);
		completing=false;
			
		while (counter == buffer.size()) 
		{
			pthread_cond_wait (&not_full_anymore,&counter_mutex);
		}
		
							
		buffer[hi] = in;
		hi=(hi+1)%buffer.size();
		counter++;

		if(counter == 1)
			pthread_cond_signal(&not_empty_anymore);
			
		pthread_mutex_unlock (&counter_mutex);
		
		
			
			
	}
	///The producer thread has to call complete, when the last element was added into the queue.
	void complete()
	{
		
		pthread_mutex_lock (&counter_mutex);
		completing=true;
		if(counter == 0) 
			pthread_cond_signal(&not_empty_anymore);
		
		
		pthread_mutex_unlock (&counter_mutex);
		
	}

	
	///The consumer thread has to call this method to remove an object from the queue.
	///If the return value is false remove was not able to get a value from the queue because the queue
	///is empty and the producer has completed its work. 
	bool remove(T& value)
	{
		
		pthread_mutex_lock (&counter_mutex);
		
		while (counter == 0) 
		{
			if(completing)
				return false;
			pthread_cond_wait (&not_empty_anymore,&counter_mutex);
		
			
		}
				

			

		 value= buffer[lo];
			
		 lo=(lo+1)%buffer.size();
		 counter--;

		
		 if(counter == (buffer.size()-1))
			 pthread_cond_signal(&not_full_anymore);
		
		 pthread_mutex_unlock (&counter_mutex);
		return true;
		
	}

		
};

	}
}


