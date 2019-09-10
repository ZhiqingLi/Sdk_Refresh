#ifndef _BSP_I2C_H
#define _BSP_I2C_H

void bsp_i2c_init(void);
void bsp_i2c_start(void);
void bsp_i2c_stop(void);
void bsp_i2c_tx_byte(uint8_t dat);
uint8_t bsp_i2c_rx_byte(void);
bool bsp_i2c_rx_ack(void);
void bsp_i2c_tx_ack(void);
void bsp_i2c_tx_nack(void);

#endif
