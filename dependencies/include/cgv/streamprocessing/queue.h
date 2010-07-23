#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>

namespace cgv {
	namespace streamprocessing {

/**
* Producer-Consumer Queue
* This is a thread-safe fixed size queue (fifo).
* It is working fine with multiple producers and multiple consumers.
* A producer is calling add_via_swap() to insert data and a consumer is
* calling remove_via_swap() to remove data from the queue.
* queue full -> producer is blocked until one element is removed
* queue empty -> consumer is blocked until one element is added
*/



	template<typename T> 
	class queue
	{
	private:
		

		T** buffer;	
		
		int counter;
		int lo;
		int hi;
		pthread_mutex_t counter_mutex;
		pthread_cond_t not_empty_anymore, not_full_anymore;	
		int N;
		


	public:
		queue(int buffer_size )
		{
			N=buffer_size;		
			pthread_mutex_init (&counter_mutex, NULL);
			pthread_cond_init (&not_empty_anymore, NULL);
			pthread_cond_init (&not_full_anymore, NULL);
			counter=0;
			hi=lo=0;
			
			buffer = new T*[N];
			for(int i = 0; i < N; i++)
			{
				buffer[i]=new T;
			}
			
			
			
			
		}

		void reset()
		{
			counter=0;
			hi=lo=0;
			
		}
		virtual ~queue()
		{
			pthread_mutex_destroy (&counter_mutex);
			pthread_cond_destroy (&not_empty_anymore);
			pthread_cond_destroy (&not_full_anymore);
			for(int i = 0; i < N; i++)
			{
				delete buffer[i];
			}
			delete[] buffer;

		}

		void add_via_swap(T** in)
		{
			
			
			pthread_mutex_lock (&counter_mutex);
			
			while (counter == N) 
			{

				pthread_cond_wait (&not_full_anymore,&counter_mutex);
						
				
			}
			
			T* swap = buffer[hi];
			buffer[hi] = *in;
			*in = swap;

			hi=(hi+1)%N;
			counter++;

			if(counter == 1)
			{
				pthread_cond_signal(&not_empty_anymore);
			}
			
			pthread_mutex_unlock (&counter_mutex);
			
			
		}

		void remove_via_swap(T** out)
		{
			
			
			

			pthread_mutex_lock (&counter_mutex);
			
			while (counter == 0) 
			{
				pthread_cond_wait (&not_empty_anymore,&counter_mutex);
			}

			 T* swap = buffer[lo];
			 buffer[lo] = *out;
			 *out = swap;
			

			 lo=(lo+1)%N;
			 counter--;

			 if(counter == (N-1))
			 {
				 pthread_cond_signal(&not_full_anymore);
			 }

			
			pthread_mutex_unlock (&counter_mutex);
					
		}

		
		
	};

	}
}

#endif //QUEUE_H
