#include "Ccd_inner.h"
#ifdef __PACFG339_H__

extern uint8 g_i2s_pa_status; //0表示POWER DOWN,1表示POWER ON
extern uint8 g_i2s_pa_delay_twi; //delay time
extern uint8 g_i2s_pa_drc_enable; //DRC使能
extern int pa_set_reg(uint8 reg_addr, uint8 *reg_data, uint8 set_len);

extern const gpio_init_cfg_t g_i2s_pa_gpio_twi[2];

void PAinit(void)
{
    uint8 val;
    uint32 flags;

//    flags = sys_local_irq_save(); 

    val = 0x63;
    Set_Device_sfr(CONFIG_A, &val, 1, (DEVICE_ADDR1|W_MODE));
//    Set_Device_sfr(CONFIG_A, &val, 1, (DEVICE_ADDR2|W_MODE));

    val = 0x80;
    Set_Device_sfr(CONFIG_B, &val, 1, (DEVICE_ADDR1|W_MODE));
//    Set_Device_sfr(CONFIG_B, &val, 1, (DEVICE_ADDR2|W_MODE));

    val = 0x9f;
    Set_Device_sfr(CONFIG_C, &val, 1, (DEVICE_ADDR1|W_MODE));
//    Set_Device_sfr(CONFIG_C, &val, 1, (DEVICE_ADDR2|W_MODE));

    val = 0x50;
    Set_Device_sfr(CONFIG_D, &val, 1, (DEVICE_ADDR1|W_MODE));
//    Set_Device_sfr(CONFIG_D, &val, 1, (DEVICE_ADDR2|W_MODE));

    val = 0xc2;
    Set_Device_sfr(CONFIG_E, &val, 1, (DEVICE_ADDR1|W_MODE));
//    Set_Device_sfr(CONFIG_E, &val, 1, (DEVICE_ADDR2|W_MODE));

    val = 0x00;
    Set_Device_sfr(CHN1_CFG, &val, 1, (DEVICE_ADDR1|W_MODE));
//    Set_Device_sfr(CHN1_CFG, &val, 1, (DEVICE_ADDR2|W_MODE));

    val = 0x40;
    Set_Device_sfr(CHN2_CFG, &val, 1, (DEVICE_ADDR1|W_MODE));
//    Set_Device_sfr(CHN2_CFG, &val, 1, (DEVICE_ADDR2|W_MODE));

    val = 0x80;
    Set_Device_sfr(CHN3_CFG, &val, 1, (DEVICE_ADDR1|W_MODE));
//    Set_Device_sfr(CHN3_CFG, &val, 1, (DEVICE_ADDR2|W_MODE));

    val = 0xc0;
    Set_Device_sfr(CONFIG_F, &val, 1, (DEVICE_ADDR1|W_MODE));
    
//    val = 0xc3;
//    Set_Device_sfr(CONFIG_F, &val, 1, (DEVICE_ADDR2|W_MODE));

//    val = 0x10;
//    Set_Device_sfr(MUTE_LOC, &val, 1, (DEVICE_ADDR1|W_MODE));
//    Set_Device_sfr(MUTE_LOC, &val, 1, (DEVICE_ADDR2|W_MODE));

    val = 0x50;
    Set_Device_sfr(MUTE_LOC, &val, 1, (DEVICE_ADDR1|W_MODE));
//    Set_Device_sfr(MUTE_LOC, &val, 1, (DEVICE_ADDR2|W_MODE));

    val = 0x80;
    Set_Device_sfr(AUTO1, &val, 1, (DEVICE_ADDR1|W_MODE));
//    Set_Device_sfr(AUTO1, &val, 1, (DEVICE_ADDR2|W_MODE));

//    val = 0x00;
//    Set_Device_sfr(AUTO2, &val, 1, (DEVICE_ADDR1|W_MODE));
//    Set_Device_sfr(AUTO2, &val, 1, (DEVICE_ADDR1|W_MODE));
    
    val = 0x00;
    Set_Device_sfr(AUTO3, &val, 1, (DEVICE_ADDR1|W_MODE));
//    Set_Device_sfr(AUTO3, &val, 1, (DEVICE_ADDR2|W_MODE));

    val = 0x77;
    Set_Device_sfr(TONE_BASS, &val, 1, (DEVICE_ADDR1|W_MODE));
//    Set_Device_sfr(TONE_BASS, &val, 1, (DEVICE_ADDR2|W_MODE));

    val = 0x6a;
    Set_Device_sfr(L1AR_RATE, &val, 1, (DEVICE_ADDR1|W_MODE));
//    Set_Device_sfr(L1AR_RATE, &val, 1, (DEVICE_ADDR2|W_MODE));
    
    Set_Device_sfr(L2AR_RATE, &val, 1, (DEVICE_ADDR1|W_MODE));
//    Set_Device_sfr(L2AR_RATE, &val, 1, (DEVICE_ADDR2|W_MODE));

    val = 0x69;
    Set_Device_sfr(L1AR_THLD, &val, 1, (DEVICE_ADDR1|W_MODE));
//    Set_Device_sfr(L1AR_THLD, &val, 1, (DEVICE_ADDR2|W_MODE));

    Set_Device_sfr(L2AR_THLD, &val, 1, (DEVICE_ADDR1|W_MODE));
//    Set_Device_sfr(L2AR_THLD, &val, 1, (DEVICE_ADDR2|W_MODE));
//    val = 0x42;
//    Set_Device_sfr(CHN1_VOL, &val, (DEVICE_ADDR1|W_MODE));
//    Set_Device_sfr(CHN2_VOL, &val, (DEVICE_ADDR1|W_MODE));

    val = 0x14;
    Set_Device_sfr(CHN1_VOL, &val, 1, (DEVICE_ADDR1|W_MODE));
//    Set_Device_sfr(CHN2_VOL, &val, 1, (DEVICE_ADDR2|W_MODE));
    Set_Device_sfr(CHN2_VOL, &val, 1, (DEVICE_ADDR1|W_MODE));
//    val = 0x00;
    Set_Device_sfr(CHN3_VOL, &val, 1, (DEVICE_ADDR1|W_MODE));

//    sys_local_irq_restore(flags);

}

#endif
