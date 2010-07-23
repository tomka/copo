#include "utilities.h"
#include <math.h>
#include <stdlib.h>

double gen_rand()
{
	const double rand_scale = 2.0/RAND_MAX;
	return rand_scale*rand()-1.0;
}

double gen_rand_high_res()
{
	const double rand_scale = (2.0/RAND_MAX)/RAND_MAX;
	return rand_scale*((double)rand()*(RAND_MAX+1)+rand())-1.0;
}
