#include "include.h"

//与配置工具下拉选项一一对应
AT(.com_text.key.table)
const u8 key_config_table[] = {
    NO_KEY,                     //None
    KEY_PLAY_POWER,             //PP/POWER
    KEY_PLAY,                   //P/P
    KEY_POWER,                  //POWER
    KEY_NEXT_VOL_UP,            //NEXT/VOL+
    KEY_VOL_UP_NEXT,            //VOL+/NEXT
    KEY_PREV_VOL_DOWN,          //PREV/VOL-
    KEY_VOL_DOWN_PREV,          //VOL-/PREV
    KEY_VOL_UP,                 //VOL+
    KEY_VOL_DOWN,               //VOL-
    KEY_NEXT,                   //NEXT
    KEY_PREV,                   //PREV
    KEY_MODE,                   //MODE
    KEY_PLAY_MODE,              //PP/MODE
    KEY_HSF,                    //HSF
    KEY_EQ,                     //EQ
    KEY_REPEAT,                 //REPEAT
    KEY_MUTE,                   //MUTE
    KEY_MODE_POWER,             //MODE/POWER
    KEY_REC,                    //REC
    KEY_VOICE_RM,               //VOICE_RM
};

#if USER_KEY_DOUBLE_EN
AT(.com_text.key)
u8 check_key_double_configure(u16 key_val)
{
    if (xcfg_cb.key_double_config_en) {
        if (key_val == ((u16)key_config_table[xcfg_cb.dblkey_num0] | KEY_SHORT_UP)) {
            return 2;
        }
        if (key_val == ((u16)key_config_table[xcfg_cb.dblkey_num1] | KEY_SHORT_UP)) {
            return 2;
        }
        if (key_val == ((u16)key_config_table[xcfg_cb.dblkey_num2] | KEY_SHORT_UP)) {
            return 2;
        }
        if (key_val == ((u16)key_config_table[xcfg_cb.dblkey_num3] | KEY_SHORT_UP)) {
            return 2;
        }
        if (key_val == ((u16)key_config_table[xcfg_cb.dblkey_num4] | KEY_SHORT_UP)) {
            return 2;
        }
        return 1;
    }
    return 0;
}
#endif // USER_KEY_DOUBLE_EN

#if (USER_ADKEY || USER_ADKEY_MUX_SDCLK)
#if ADKEY_PU10K_EN
/******************************************************************
*                   内部10K上拉的table表
* 1、不复用SDCLK时，最多支持7个按键，按键电阻0R, 2K, 5.1K, 10K, 22K, 56K, 150K
* 2、复用SDCLK时, 需要从2K电阻的按键开始，最多6个按键
* 3、150K电阻也可以用于插入检测
*******************************************************************/
AT(.com_text.adkey.table)
const adkey_tbl_t adkey_table[] = {
    {0x0A, KEY_PLAY},           //PLAY          0R
    {0x3F, KEY_PREV_VOL_DOWN},  //PREV/VOL-     2K
    {0x6C, KEY_NEXT_VOL_UP},    //NEXT/VOL+     5.1K,
    {0x97, KEY_MODE},           //MODE          10K,
    {0xC1, KEY_HSF},            //HSF           22K,
    {0xE7, KEY_REPEAT},         //REPEAT        56K,
    {0xF7, KEY_REC},            //REC           150K
    {0xFF, NO_KEY},             //              END
};
#else

/******************************************************************
*                   外部10K上拉的table表
* 1、最多支持12个按键
* 2、复用SDCLK时, 需要从2K电阻的按键开始或ADKEY先串个10K电阻到IO
* 3、100K电阻也可以用于插入检测
*******************************************************************/
AT(.com_text.adkey.table)
const adkey_tbl_t adkey_table[] = {
    {0x0A, KEY_PLAY},           //PLAY          0R
    {0x20, KEY_PREV},           //PREV          1K
    {0x35, KEY_NEXT},           //NEXT          2K
    {0x4A, KEY_MODE},           //MODE          3.3K
    {0x62, KEY_HSF},            //HSF           5.1K,
    {0x76, KEY_VOL_DOWN},       //VOL-          7.5K,
    {0x8C, KEY_VOL_UP},         //VOL+          10K,
    {0xA8, KEY_EQ},             //EQ            15K,
    {0xB8, KEY_REPEAT},         //REPEAT        20K,
    {0xCB, KEY_VOL_DOWN_PREV},  //MINUS         33K,
    {0xE4, KEY_VOL_UP_NEXT},    //PLUS          56K
    {0xF4, KEY_REC},            //REC           100K,
    {0xFF, NO_KEY},             //              END
};
#endif // ADKEY_PU10K_EN

AT(.com_text.adkey)
u8 *get_adkey_configure(u8 num)
{
    if (xcfg_cb.adkey_config_en) {
        //ADKEY 10K上拉，用100K电阻复用耳机检测。
        if ((num > 9) && (num <= 11)) {
            adkey_detect_flag = 1;
        } else if (num > 11) {
            adkey_detect_flag = 0;
        }
        return &xcfg_cb.adkey1_num0;
    } else {
		adkey_detect_flag = 0;
        return NULL;
    }
}
#endif // USER_ADKEY

#if USER_ADKEY2
///最多支持12个按键, 以0xff结束
AT(.com_text.adkey2.table)
const adkey_tbl_t adkey2_table[] = {
    {0x0A, KEY_NUM_0},
    {0x20, KEY_NUM_1},
    {0x35, KEY_NUM_2},
    {0x4A, KEY_NUM_3},
    {0x68, KEY_NUM_4},
    {0x78, KEY_NUM_5},
    {0x8E, KEY_NUM_6},
    {0xA8, KEY_NUM_7},
    {0xB8, KEY_NUM_8},
    {0xCB, KEY_NUM_9},
    {0xE4, NO_KEY},
    {0xF4, NO_KEY},
    {0xFF, NO_KEY},
};

AT(.com_text.adkey2)
u8 *get_adkey2_configure(u8 num)
{
    if (xcfg_cb.adkey2_config_en) {
        return &xcfg_cb.adkey2_num0;
    } else {
        return NULL;
    }
}
#endif // USER_ADKEY2

#if USER_PWRKEY
///最多支持5个按键。数组元数总数请保持不变。不需要的按键改为NO_KEY
AT(.com_text.pwrkey.table)
const adkey_tbl_t pwrkey_table[6] = {
    {0x08, KEY_PLAY_POWER},     //P/P POWER     0
    {0x3A, KEY_PREV_VOL_DOWN},  //PREV/VOL-     64
    {0x7B, KEY_NEXT_VOL_UP},    //NEXT/VOL+     129
    {0xC0, KEY_MODE},           //MODE          192
    {0xF8, KEY_HSF},            //HSF           238
    {0xFF, NO_KEY},
};

AT(.com_text.pwrkey)
u8 *get_pwrkey_configure(u8 num)
{
    if (xcfg_cb.pwrkey_config_en) {
        if (num == 4) {
            pwrkey_detect_flag = 1;
        } else if (num > 4) {
            pwrkey_detect_flag = 0;
        }
        return &xcfg_cb.pwrkey_num0;
    } else {
        pwrkey_detect_flag = 0;
        return NULL;
    }
}
#endif // USER_PWRKEY

#if USER_IOKEY
//工具配置的IO初始化
gpio_t iokey0_gpio;
gpio_t iokey1_gpio;
gpio_t iokey2_gpio;
gpio_t iokey3_gpio;
gpio_t iokey4_gpio;

//工具配置的IO初始化
AT(.text.key.init)
void iokey_cfg_port_init(gpio_t *g)
{
    if ((g == NULL) || (g->sfr == NULL)) {
        return;
    }
    g->sfr[GPIOxDE] |= BIT(g->num);
    g->sfr[GPIOxPU] |= BIT(g->num);
    g->sfr[GPIOxDIR] |= BIT(g->num);
}

AT(.com_text.port.key)
bool iokey_cfg_port_pressed(gpio_t *g)
{
    if ((g == NULL) || (g->sfr == NULL)) {
        return false;
    }
    return (!(g->sfr[GPIOx] & BIT(g->num)));
}

AT(.com_text.port.key)
void iokey_cfg_port_out_low(gpio_t *g)
{
    if ((g == NULL) || (g->sfr == NULL)) {
        return;
    }
    g->sfr[GPIOxDE] |= BIT(g->num);
    g->sfr[GPIOxDIR] &= ~BIT(g->num);
    g->sfr[GPIOxCLR] = BIT(g->num);
}

AT(.com_text.port.key)
bool iokey_cfg_midkey_pressed(gpio_t *g0, gpio_t *g1)
{
    bool pressed_flag = false;

    if (key_config_table[xcfg_cb.iokey_midkey_num] == NO_KEY) {
        return false;
    }
    iokey_cfg_port_out_low(g0);
    delay_us(6);
    if (iokey_cfg_port_pressed(g1)) {
        pressed_flag = true;
    }
    iokey_cfg_port_init(g0);
    return pressed_flag;
}

AT(.text.key.init)
void io_key_init(void)
{
    //配置工具是否使能了IOKEY？
    if (!xcfg_cb.user_iokey_en) {
        return;
    }

    //工具配置了IOKEY的按键定义？
    if (xcfg_cb.iokey_config_en) {
        bsp_gpio_cfg_init(&iokey0_gpio, xcfg_cb.iokey_io0);
        bsp_gpio_cfg_init(&iokey1_gpio, xcfg_cb.iokey_io1);
        bsp_gpio_cfg_init(&iokey2_gpio, xcfg_cb.iokey_io2);
        bsp_gpio_cfg_init(&iokey3_gpio, xcfg_cb.iokey_io3);
        bsp_gpio_cfg_init(&iokey4_gpio, xcfg_cb.iokey_io4);

        iokey_cfg_port_init(&iokey0_gpio);
        iokey_cfg_port_init(&iokey1_gpio);
        iokey_cfg_port_init(&iokey2_gpio);
        iokey_cfg_port_init(&iokey3_gpio);
        iokey_cfg_port_init(&iokey4_gpio);
        return;
    }

    GPIOFDE  |= BIT(0) | BIT(1) | BIT(3);
    GPIOFDIR |= BIT(0) | BIT(1) | BIT(3);
    GPIOFPU  |= BIT(0) | BIT(1) | BIT(3);
}

AT(.com_text.port.key)
u8 get_iokey(void)
{
    u8 key_val = NO_KEY;

    //配置工具是否使能了IOKEY？
    if (!xcfg_cb.user_iokey_en) {
        return NO_KEY;
    }

    //工具配置了IOKEY的按键定义？
    if (xcfg_cb.iokey_config_en) {
        if (iokey_cfg_port_pressed(&iokey0_gpio)) {
            key_val = key_config_table[xcfg_cb.iokey_num0];
        } else if (iokey_cfg_port_pressed(&iokey1_gpio)) {
            key_val = key_config_table[xcfg_cb.iokey_num1];
        } else if (iokey_cfg_port_pressed(&iokey2_gpio)) {
            key_val = key_config_table[xcfg_cb.iokey_num2];
        } else if (iokey_cfg_port_pressed(&iokey3_gpio)) {
            key_val = key_config_table[xcfg_cb.iokey_num3];
        } else if (iokey_cfg_port_pressed(&iokey4_gpio)) {
            key_val = key_config_table[xcfg_cb.iokey_num4];
        } else if (iokey_cfg_midkey_pressed(&iokey0_gpio, &iokey1_gpio)) {
            key_val = key_config_table[xcfg_cb.iokey_midkey_num];
        }
        return key_val;
    }

    if (!(GPIOF & BIT(0))) {
        //KEY0
        key_val = KEY_PLAY;
    } else if (!(GPIOF & BIT(1))) {
        //KEY1
        key_val = KEY_PREV;
    } else if (!(GPIOF & BIT(3))) {
        //KEY2
        key_val = KEY_NEXT;
    }
    return key_val;
}
#endif // USER_IOKEY

