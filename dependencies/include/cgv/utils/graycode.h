#pragma once

#include <bitset>

namespace cgv{
	namespace utils{

	
///convert binary code to graycode
unsigned long binarycode_to_graycode(unsigned long c)
{
	return c ^(c >>1);
}

///convert graycode to binary code
unsigned long graycode_to_binarycode(unsigned long c) 
{
	c ^= c >> 1;
	c ^= c >> 2;
	c ^= c >> 4;
	c ^= c >> 8;
	return c ^ (c >> 16);
}
	}
}




