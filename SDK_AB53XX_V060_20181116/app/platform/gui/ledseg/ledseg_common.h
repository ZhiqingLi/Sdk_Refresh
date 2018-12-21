#ifndef _LEDSEG_COMMON_H
#define _LEDSEG_COMMON_H

//ASCII Display Definition
#define T_LEDSEG_0        (SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F)
#define T_LEDSEG_1        (SEG_B | SEG_C)
#define T_LEDSEG_2        (SEG_A | SEG_B | SEG_G | SEG_E | SEG_D)
#define T_LEDSEG_3        (SEG_A | SEG_B | SEG_G | SEG_C | SEG_D)
#define T_LEDSEG_4        (SEG_B | SEG_C | SEG_F | SEG_G)
#define T_LEDSEG_5        (SEG_A | SEG_C | SEG_D | SEG_F | SEG_G)
#define T_LEDSEG_6        (SEG_A | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G)
#define T_LEDSEG_7        (SEG_A | SEG_B | SEG_C)
#define T_LEDSEG_8        (SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F | SEG_G)
#define T_LEDSEG_9        (SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G)
#define T_LEDSEG_A        (SEG_A | SEG_B | SEG_C | SEG_E | SEG_F | SEG_G)
#define T_LEDSEG_B        (SEG_C | SEG_D | SEG_E | SEG_F | SEG_G)
#define T_LEDSEG_C        (SEG_A | SEG_D | SEG_E | SEG_F)
#define T_LEDSEG_D        (SEG_B | SEG_C | SEG_D | SEG_E | SEG_G)
#define T_LEDSEG_E        (SEG_A | SEG_D | SEG_E | SEG_F | SEG_G)
#define T_LEDSEG_F        (SEG_A | SEG_E | SEG_F | SEG_G)
#define T_LEDSEG_G        (SEG_A | SEG_B | SEG_C | SEG_D | SEG_F | SEG_G)
#define T_LEDSEG_H        (SEG_B | SEG_C | SEG_E | SEG_F | SEG_G)
#define T_LEDSEG_I        (SEG_B | SEG_C)
#define T_LEDSEG_J        (SEG_B | SEG_C | SEG_D | SEG_E)
#define T_LEDSEG_K        (SEG_E | SEG_F | SEG_G)
#define T_LEDSEG_L        (SEG_D | SEG_E | SEG_F)
#define T_LEDSEG_M        (SEG_C | SEG_E | SEG_G)
#define T_LEDSEG_N        (SEG_A | SEG_B | SEG_C | SEG_E | SEG_F)
#define T_LEDSEG_O        (SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F)
#define T_LEDSEG_P        (SEG_A | SEG_B | SEG_E | SEG_F | SEG_G)
#define T_LEDSEG_Q        (SEG_A | SEG_B | SEG_C | SEG_F | SEG_G)
#define T_LEDSEG_R        (SEG_E | SEG_G)
#define T_LEDSEG_S        (SEG_A | SEG_C | SEG_D | SEG_F | SEG_G)
#define T_LEDSEG_T        (SEG_D | SEG_E | SEG_F | SEG_G)
#define T_LEDSEG_U        (SEG_B | SEG_C | SEG_D | SEG_E | SEG_F)
#define T_LEDSEG_V        (SEG_B | SEG_C | SEG_D | SEG_F | SEG_E)
#define T_LEDSEG_W        (SEG_C | SEG_D | SEG_E)
#define T_LEDSEG_X        (SEG_B | SEG_C | SEG_E | SEG_F | SEG_G)
#define T_LEDSEG_Y        (SEG_B | SEG_C | SEG_D | SEG_F | SEG_G)
#define T_LEDSEG_Z        (SEG_A | SEG_B | SEG_D | SEG_E | SEG_G)
#define T_LEDSEG_lo       (SEG_C | SEG_D | SEG_E | SEG_G)
#define T_LEDSEG_M1       (SEG_A | SEG_C | SEG_E | SEG_G)

#define T_LEDSEG__        (SEG_G)
#define T_LEDSEG_ON       (LEDSEG_8)
#define T_LEDSEG_OFF      (0x00)

extern u8 ledseg_buf[5];
extern u8 ledseg_disp_num;

void ledseg_disp_number(u16 num);
void ledseg_init(void);
void ledseg_display(u8 disp_num);

#endif //_LEDSEG_COMMON_H
