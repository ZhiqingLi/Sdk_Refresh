#ifndef _DRIVER_I2S_H
#define _DRIVER_I2S_H

void i2s_init( void );
void i2s_cfg( uint32 freq, uint16 datawidth );
void i2s_open( AUDIO_CTRL_BLK *ablk );
void i2s_close( void );
void i2s_isr( AUDIO_CTRL_BLK *ablk );
void i2s_rx_enable( uint8 enable );
void i2s_volume_mute( uint8 enable );
void i2s_volume_adjust( uint8 volume );
void i2s_rx_volume_adjust( uint8 volume );

#endif
