#pragma once
#include <cgv/scene/graph/node.h>
#include <vector>

namespace cgv{
	namespace scene{
		namespace graph{
/**
* A group node. It contains a vector of children. This node should be the direct or indirect 
* base class for all nodes which can be parents of other nodes.
*/
class group_node : public node
{
public:
	std::vector<node*> children;
	
	///call init of each child
	virtual void init()
	{
		std::vector<node*>::iterator it = children.begin();
		for(;it !=children.end();it++)
		{
			(*it)->init();
		}
	}

	///call render of each child
	virtual void render()
	{
		std::vector<node*>::iterator it = children.begin();
		for(;it !=children.end();it++)
		{
			(*it)->render();
		}
	}

};
		}
	}
}
