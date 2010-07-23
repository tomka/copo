#pragma once
#include <cgv/scanner/patterns/pattern_sequence_creator.h>

namespace cgv {
	namespace scanner {
		namespace patterns {

/**
* Repeated pattern sequence creator. 
**/
template <typename T>
class repeated_pattern_sequence_creator : public pattern_sequence_creator<T>
{
	//pattern sequence creator to repeat
	pattern_sequence_creator<T>*  creator;
	//number of repeatations
	int n;

public:
	
	///constructor
	repeated_pattern_sequence_creator (int width, int height,int n, pattern_sequence_creator<T>*  creator): pattern_sequence_creator(width,height)
	{
		this->creator = creator;
		this->n = n;
	}

	///create repeated patternsequence
	pattern_sequence* construct_sequence()
	{
		pattern_sequence* sequence=NULL;
				
		for(int i=0; i < n; i++)
		{
			if(sequence == NULL)
				sequence = creator->construct_sequence();
			else
			{

				pattern_sequence* temp = creator->construct_sequence();
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