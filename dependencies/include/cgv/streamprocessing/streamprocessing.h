#ifndef STREAMPROCESSING_H
#define STREAMPROCESSING_H

#include <vector>
#include <string>

#include <map>
#include <cgv/streamprocessing/queue.h>

#include <cgv/threading/thread.h>
#include <cgv/threading/mutex.h>



namespace cgv {
	namespace streamprocessing {

	class port_in_base
	{
	protected:
		//connected flag
		bool connected;
	public:
		///constructor
		port_in_base(){connected=false;}
	
		///destructor
		virtual ~port_in_base(){}

		
		///set the connected flag, should only be called by the connect method of port out
		void connect()
		{
			connected=true;
		}

		///disconnect the port if connected
		void disconnect()
		{
			connected= false;
		}

		///return true if port is connected
		bool is_connected()
		{
			return connected;
		}
	};

	class port_out_base
	{
		virtual bool connect(port_in_base *in) = 0;	
		virtual void disconnect()=0;
		virtual bool is_connected()=0;
	};


	template< typename input_t>
	class port_in : public port_in_base
	{
		///synchronisation queue
		queue<input_t> data;
		
				
	public:
		
		///constructor
		port_in(int buffer_num=1):port_in_base(),data(buffer_num)
		{
			connected=false;
			
		}
		
		
		///swap data out 
		void operator >>(input_t **target)
		{
			if(connected)
			{
				data.remove_via_swap(target);		
			}
			else
			{
				std::cerr<< "port is not connected"<<std::endl;
			}
		}

		///swap data in
		void operator <<(input_t **target)
		{
			if(connected)
			{
				data.add_via_swap(target);
			}else
			{
				std::cerr<< "port is not connected"<<std::endl;
				
			}

		}

	
	};


	template<typename output_t>
	class port_out : public port_out_base
	{
	private:
		port_in<output_t> * connected_port;

		
	public:
		///constructor
		port_out()
		{
			connected_port=NULL;
		}

		///try to connect with given port_in; return true if connection is established successful
		bool connect(port_in_base *pi)
		{
			port_in<output_t> *p = dynamic_cast< port_in<output_t>* >(pi);
			
			
			if(!is_connected()&& p!= NULL)
			{
				connected_port = p;
				p->connect();
				return true;
			}
			return false;
		}

		//disconnect the port
		void disconnect()
		{
			
			connected_port->disconnect();
			connected_port=NULL;
				

		}

		//return true if port is connected
		inline bool is_connected()
		{
			return connected_port!=NULL;
		}

		///swap data in
		void operator << (output_t **target)
		{
			if(is_connected())		
				*connected_port << target;
			
			
			
		}
	};



	/**
	* Base class for all processing nodes.
	*/
	class stream_node : public ::cgv::threading::thread
	{
		///map of in-ports
		std::map<std::string,port_in_base*> inputs;
		///map of out-ports
		std::map<std::string,port_out_base*> outputs;

	public:
		///standard constructor
		stream_node():thread(){}
		
		///destructor
		virtual ~stream_node(){}

		///register an in-port with an identifier string
		void register_port_in(const std::string identifier,port_in_base* in)
		{
			inputs[identifier]=in;
		}

		///register an out-port with an identifier string
		void register_port_out(const std::string identifier,port_out_base* out)
		{
			outputs[identifier]=out;
		}

		///return number of registered in-ports
		size_t get_port_in_num()
		{
			return inputs.size();
		}
		///return number of registered out-ports
		size_t get_port_out_num()
		{
			return outputs.size();
		}

		///template method to override with special node functionality
		virtual void run() =0;
	};



	}
}





#include <cgv/config/lib_end.h>






#endif
