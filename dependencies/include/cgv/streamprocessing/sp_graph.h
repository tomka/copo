#ifndef SP_GRAPH_H
#define SP_GRAPH_H

#include <vector>
#include <map>
#include <cgv/config/global.h>
#include <cgv/streamprocessing/streamprocessing.h>

#include <cgv/streamprocessing/lib_begin.h>
#pragma warning (disable : 4251)

namespace cgv {
	namespace streamprocessing{

	class CGV_API sp_graph
	{
		 
		 std::vector<stream_node *> nodes;

	public:
		sp_graph(){}
		virtual ~sp_graph(){}

		void add_node(stream_node * node);
		void start();
		void stop();
		void wait_for_completion();
				

	};

	}
}



#include <cgv/config/lib_end.h>

#endif