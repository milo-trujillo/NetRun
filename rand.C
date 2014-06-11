#include <time.h>
#include <stdlib.h>
#include "rand.h"

void seed_random()
{
	//srand48( time(NULL) ); // Seed RNG
	srand( time(NULL) ); // Seed RNG
}

int random( int lower, int upper )
{
	if( upper <= lower )
		return lower;
	//return lower + drand48() * (upper - lower);
	return lower + (rand() % (upper - lower));
}
