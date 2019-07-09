#ifndef _LEDSEG_7P7S_HBS7642_H
#define _LEDSEG_7P7S_HBS7642_H



#define HBS7642_DISP_ADRESS		0x08


#define HBS7642_CMD_ADRESS		0x18
//bit7~bit4
#define HBS7642_DISP_CMD_OFF	0x00				
#define HBS7642_DUTY_CMD_1_16	0x80
#define HBS7642_DUTY_CMD_2_16	0x40
#define HBS7642_DUTY_CMD_3_16	0xC0
#define HBS7642_DUTY_CMD_4_16	0x20
#define HBS7642_DUTY_CMD_5_16	0xA0
#define HBS7642_DUTY_CMD_6_16	0x60
#define HBS7642_DUTY_CMD_7_16	0xE0
#define HBS7642_DUTY_CMD_8_16	0x10
#define HBS7642_DUTY_CMD_9_16	0x90
#define HBS7642_DUTY_CMD_10_16	0x50
#define HBS7642_DUTY_CMD_11_16	0xD0
#define HBS7642_DUTY_CMD_12_16	0x30
#define HBS7642_DUTY_CMD_13_16	0xB0
#define HBS7642_DUTY_CMD_14_16	0x70
#define HBS7642_DUTY_CMD_15_16	0xF0
//bit4~bit1
#define HBS7642_CURR_CMD_1_8	0x00
#define HBS7642_CURR_CMD_2_8	0x08
#define HBS7642_CURR_CMD_3_8	0x04
#define HBS7642_CURR_CMD_4_8	0x0C
#define HBS7642_CURR_CMD_5_8	0x02
#define HBS7642_CURR_CMD_6_8	0x0A
#define HBS7642_CURR_CMD_7_8	0x06
#define HBS7642_CURR_CMD_8_8	0x0E
//bit0
#define HBS7642_TYPE_CMD_8P5S	0x00
#define HBS7642_TYPE_CMD_7P6S	0x01





void ledseg_7p3s_HBS7642_init(void);
void ledseg_7p3s_HBS7642_off(void);
void ledseg_7p3s_HBS7642_scan(void);
void ledseg_7p3s_HBS7642_update_dispbuf(void);
void force_flush_ledseg(void);
void ledseg_7p3s_HBS7642_brightness_loop(void);
void ledseg_7p3s_HBS7642_on_off(void);
void ledseg_7p3s_HBS7642_on(void);

#endif //_LEDSEG_7P7S_H
