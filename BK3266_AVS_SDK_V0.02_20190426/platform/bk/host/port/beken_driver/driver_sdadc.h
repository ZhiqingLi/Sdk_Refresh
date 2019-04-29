#ifndef _DRIVER_SDADC_H_
#define _DRIVER_SDADC_H_

#define SDADC_VOLUME_MAX        124 //95    // all 48db, 0.5db per step

void sdadc_init( void );
void sdadc_enable( uint8 enable );
void sdadc_isr( void );
void sdadc_volume_adjust( uint8 volume );

#endif

// EOF
