#include <cgv/streamprocessing/sp_graph.h>
namespace cgv {
	namespace streamprocessing {

	void sp_graph::add_node(stream_node * node)
	{
		nodes.push_back(node);
	}
	
	void sp_graph::start()
	{
		for(std::vector<stream_node *>::iterator it = nodes.begin();it != nodes.end();it++)
		{
			(*it)->start();			
		}
	}
	void sp_graph::stop()
	{
		for(std::vector<stream_node *>::iterator it = nodes.begin();it != nodes.end();it++)
		{
			(*it)->stop();			
		}
	}
	
	void sp_graph::wait_for_completion()
	{
		for(std::vector<stream_node *>::iterator it = nodes.begin();it != nodes.end();it++)
		{
			(*it)->wait_for_completion();
		}
	}

	}
}