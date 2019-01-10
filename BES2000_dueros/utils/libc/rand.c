#include "stdlib.h"

static unsigned int seed = 1;

void srand(unsigned int init)
{
    seed = init;
}

int rand()
{
    //Based on Knuth "The Art of Computer Programming"
    seed = seed * 1103515245 + 12345;
    return ( (unsigned int) (seed / 65536) % (RAND_MAX+1) );
}
