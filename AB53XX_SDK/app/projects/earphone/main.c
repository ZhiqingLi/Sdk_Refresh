#include "include.h"

/********************************************************
RF参数说明：
tag0/1  ：标志，一般不修改
cap0    ：TX电容，影响TX功率，范围0~7
gain    ：TX增益，影响TX功率，范围2~5
dig     ：DIG增益，影响TX功率，范围30~57
v       ：TX电压，范围0~4
other   ：一般不修改
*********************************************************/
#define RF_PARAM_SIZE               10
static const uint8_t rf_param[][RF_PARAM_SIZE] = {
//   tag0, tag1, cap0,const, gain,  dig, b, v, m, udf
    {0x01, 0x00, 0x07, 0x07, 0x04,   55, 4, 4, 4, 0x00,},  //参数0，与setting对应
    {0x01, 0x00, 0x06, 0x07, 0x04,   59, 4, 4, 4, 0x00,},  //参数1，与setting对应，AB5317B-I7
    {0x01, 0x00, 0x07, 0x07, 0x04,   55, 4, 4, 4, 0x00,},  //参数2，与setting对应
    {0x01, 0x00, 0x07, 0x07, 0x07,   55, 4, 4, 4, 0x00,},  //参数3，与setting对应，单极Q13S
    {0x01, 0x00, 0x07, 0x07, 0x07,   55, 4, 4, 4, 0x00,},  //参数4，与setting对应，BT8812-I8
    {0x01, 0x00, 0x07, 0x07, 0x05,   55, 4, 4, 4, 0x00,},  //参数5，与setting对应，BT8832-I8
    {0x01, 0x00, 0x07, 0x07, 0x06,   53, 4, 4, 4, 0x00,},  //参数6，与setting对应，BT8832-I8
};

const uint8_t *bt_get_rf_param(void)
{
    if(xcfg_cb.bt_user_param_en) {
        return (const uint8_t *)&xcfg_cb.rf_tag0;       //#3, 使用自定义参数
    }

    if(xcfg_cb.bt_rf_user_param != 0 && xcfg_cb.bt_rf_user_param <= sizeof(rf_param)/RF_PARAM_SIZE) {
        return rf_param[xcfg_cb.bt_rf_user_param - 1];  //#2, 使用main.c自定义参数
    }
    return NULL;    //#1, 使用内部参数
}

bool bt_get_sco_param(u32 *mix_gain, u32 *dig_gain)
{
    if(xcfg_cb.rf_param_sco_en) {
        *mix_gain = xcfg_cb.rf_mix_gain_sco;
        *dig_gain = xcfg_cb.rf_dig_gain_sco;
        return true;
    }
    return false;
}

u32 get_test_sbc_addr(void)
{
    //return RES_BUF_TEST_SBC;
    return 0;
}

u32 get_test_sbc_len(void)
{
    //return RES_LEN_TEST_SBC;
    return 0;
}

void sys_error_hook(u8 err_no)
{
    WDT_RST();
}

//正常启动Main函数
int main(void)
{
    printf("Hello Bluetooth Earphone\n");

    bsp_sys_init();
    func_run();
    return 0;
}

//升级完成
void update_complete(int mode)
{
#if USB_SD_UPDATE_EN
    bsp_update_init();
    if (mode == 0) {
        WDT_DIS();
        while (1);
    }else if(mode == 1){
        WDT_RST();
    }else  //mode =2  OTA升级完成
#endif // USB_SD_UPDATE_EN
    {
        WDT_RST();
    }
}
