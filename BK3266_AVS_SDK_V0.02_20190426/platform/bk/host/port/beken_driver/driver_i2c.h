#ifndef _DRIVER_I2C_H_
#define _DRIVER_I2C_H_


#ifdef CONFIG_DRIVER_I2C

typedef int (*i2c_reg_read_callback)(uint32 regId, uint8 *buff, uint8 len );
typedef int (*i2c_reg_write_callback)(uint32 regId, uint8 *buff, uint8 len );
typedef int (*i2c_get_reg_len)(uint32 regId );

void i2c_init(int freq, int m_s);
void i2c_attach_callback( i2c_reg_read_callback  reg_read, 
    i2c_reg_write_callback reg_write,
    i2c_get_reg_len get_len);
void i2c_isr(void);
int i2c_read( uint8 addr, uint8 *buff, uint8 len );
int  i2c_write( uint8 addr, uint8 *buff, uint8 len );
int i2c_reg_read( uint32 regId, uint8 *buff, uint8 len );
int i2c_reg_write( uint32 regId, uint8 *buff, uint8 len );
int  i2c_busy( void );
#endif

#endif

