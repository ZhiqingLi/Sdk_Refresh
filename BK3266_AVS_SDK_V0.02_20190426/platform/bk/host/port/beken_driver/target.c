#include "driver_beken_includes.h"
#include "beken_external.h"

void Delay(int num)
{
    volatile int i,j;
    for(i=0;i<num;i++)
        for(j=0;j<100;j++);

}

