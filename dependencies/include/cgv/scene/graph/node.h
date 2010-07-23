#pragma once



namespace cgv{
	namespace scene{
		namespace graph{

		
/**
* Interface for all scene graph nodes
*/
class node
{
	
public:
		
	virtual void init(){}
	virtual void render(){}
	/*virtual box get_bbox(){}
	virtual float intersect(ray* r){}*/

	
};
		}
	}
}

