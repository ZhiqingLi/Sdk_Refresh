#include "include.h"
#include "ledseg_common.h"
#include "ledseg_7p7s.h"

#if (GUI_SELECT & DISPLAY_LEDSEG)

u8 ledseg_buf[6] AT(.buf.ledseg);
u8 ledseg_disp_num AT(.buf.ledseg);

typedef void (*PFUNC) (void);
extern const PFUNC ledseg_disp_pfunc[];

AT(.rodata.ledseg)
const u8 ledseg_num_table[10] =
{
    T_LEDSEG_0, T_LEDSEG_1, T_LEDSEG_2, T_LEDSEG_3, T_LEDSEG_4,
    T_LEDSEG_5, T_LEDSEG_6, T_LEDSEG_7, T_LEDSEG_8, T_LEDSEG_9,
};

AT(.text.ledseg)
void ledseg_disp_number(u16 num)
{
    if (num > 9999) {
        num = 9999;
    }
    ledseg_buf[0] = ledseg_num_table[num / 1000];
    ledseg_buf[1] = ledseg_num_table[(num % 1000) / 100];
    ledseg_buf[2] = ledseg_num_table[(num % 100) / 10];
    ledseg_buf[3] = ledseg_num_table[num % 10];
}


AT(.text.ledseg)
void ledseg_disp_bat(void)
{
	switch(sys_cb.vbat_icon_lv){
		case 0:
			ledseg_buf[5] |= ICON_BAT1;
			ledseg_buf[5] |= ICON_BAT2;
			ledseg_buf[5] |= ICON_BAT3;
			ledseg_buf[5] |= ICON_BAT4;
			break;
		case 1:
			ledseg_buf[5] |= ICON_BAT2;
			ledseg_buf[5] |= ICON_BAT3;
			ledseg_buf[5] |= ICON_BAT4;
			break;
		case 2:
			ledseg_buf[5] |= ICON_BAT3;
			ledseg_buf[5] |= ICON_BAT4;
			break;
		case 3:
			ledseg_buf[5] |= ICON_BAT4;
			break;
	}


	if ((sys_cb.vbat_charge_sta) ||(sys_cb.power_off_dc_in == 1))
	{
		ledseg_buf[4] |= ICON_CHARGE;
	}
}


AT(.text.ledseg)
void ledseg_display(u8 disp_num)
{
	u8 buf_bak[6];
	void (*pfunc)(void);
	memcpy(buf_bak, ledseg_buf, 6);
	memset(ledseg_buf, 0, sizeof(ledseg_buf));

	if(sys_cb.lowbat_flag == 0)
	{
	    pfunc = ledseg_disp_pfunc[disp_num];
	    (*pfunc)();
		ledseg_disp_bat();
	}

	if (memcmp(buf_bak, ledseg_buf, 6) == 0)
	{
		return;
	}
	ledseg_disp_num = disp_num;
	ledseg_update_dispbuf();
}

#endif
