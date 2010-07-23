#pragma once
#include <cgv/scanner/patterns/pattern_sequence_creator.h>

namespace cgv {
	namespace scanner {
		namespace patterns {

/**
* Combined pattern sequence create. 
**/
template <typename T>
class combined_pattern_sequence_creator : public pattern_sequence_creator<T>
{
	//pattern sequence creators to combine
	std::vector<pattern_sequence_creator<T>* > creators;
public:
	
	combined_pattern_sequence_creator (int width, int height): pattern_sequence_creator(width,height)
	{

	}

	//add a pattern sequence create
	void add_creator(pattern_sequence_creator<T> *creator)
	{
		creators.push_back(creator);
	}
	
	pattern_sequence* construct_sequence()
	{
		pattern_sequence* sequence=NULL;
		std::vector<pattern_sequence_creator<T>* >::iterator it;
		
		for(it = creators.begin(); it != creators.end();it++)
		{
			if(sequence == NULL)
				sequence = (*it)->construct_sequence();
			else
			{

				pattern_sequence* temp = (*it)->construct_sequence();
				sequence->insert( sequence->end(), temp->begin(),temp->end() );
				delete temp;
				
			}
			
		}
		return sequence;
		
	}

};
		}
	}
}