#pragma once

long randomSeed = 432876;
long remainingBits = 0;
char bitCount = 0;


#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define MASK 123459876
namespace cgv {
	namespace math {
		namespace random {

long ran0()
{
	// read save value
	long s = randomSeed;

	// process value
	s ^= MASK;
	long k=s/IQ;
	s=IA*(s-k*IQ)-IR*k;
	if (s < 0) s += IM;
	s ^= MASK;

	// set save value
	return randomSeed = s;
}

double ran1()
{
	return AM*ran0();
}

void set_random_seed(long s)
{
	randomSeed    = s;
	bitCount      = 0;
}

double random(double min, double max)
{
	return (max-min)*ran1()+min;
}

double random(double min, double max, long seed)
{
	set_random_seed(seed);
	return random(min,max);
}


bool random_bit()
{
	// read save values
	long rb = remainingBits;
	char bc = bitCount;

	// update values
	if (bc == 0) {
		rb = ran0();
		bc = 31;
	}
	bool res = (rb & 1) == 1;
	rb >>= 1;
	--bc;

	// set save values
	remainingBits = rb;
	bitCount      = bc;

	return res;
}

bool random_bit(long seed)
{
	set_random_seed(seed);
	return random_bit();
}

		}
	}
}