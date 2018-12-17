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
    KEY_VOL_NEXT_PREV,          //VOL/NEXT/PREV     短按VOL+,     长按NEXT,  双击PREV
};
#if KEY_MSG_REMAP_EN
AT(.com_text.key.table)
const u16 key_sta_table[] = {
    NO_KEY,
    KEY_SHORT,
    KEY_SHORT_UP,
    KEY_LONG,
    KEY_LONG_UP,
    KEY_HOLD,
    KEY_LHOLD,
    KEY_DOUBLE,
    KEY_THREE,
};

AT(.com_text.bsp.key)
void key_msg_remap(u16 *key)  //目前只支持一个按键重映射
{
    u16 key1_org_msg,key1_rmap_msg;
    if (!xcfg_cb.key_msg_remap_en){
        return;
    }
    key1_org_msg = (u16)key_config_table[xcfg_cb.key1_msg_org_val] | key_sta_table[xcfg_cb.key1_msg_org_sta];
    if (key1_org_msg == *key) {
        key1_rmap_msg = (u16)key_config_table[xcfg_cb.key1_msg_rmap_val] | key_sta_table[xcfg_cb.key1_msg_rmap_sta];
        *key = key1_rmap_msg;
    }
}
#endif

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

#if USER_ADKEY
///最多支持12个按键, 以0xff结束
AT(.com_text.adkey.table)
const adkey_tbl_t adkey_table[] = {
    {0x0A, KEY_PLAY},           //PLAY      S2
    {0x20, KEY_PREV},           //PREV      S4
    {0x35, KEY_NEXT},           //NEXT      S6
    {0x4A, KEY_MODE},           //MODE      S8
    {0x68, KEY_HSF},            //HSF       S10
    {0x78, KEY_VOL_DOWN},       //VOL-      S12
    {0x8E, KEY_VOL_UP},         //VOL+      S14
    {0xA8, KEY_EQ},             //EQ        S16
    {0xB8, KEY_REPEAT},         //REPEAT    S18
    {0xCB, KEY_VOL_DOWN_PREV},  //MINUS     S20
    {0xE4, KEY_VOL_UP_NEXT},    //PLUS      S22
    {0xF4, KEY_REC},            //REC       S24
    {0xFF, NO_KEY},             //          END
};


AT(.com_text.adkey)
u8 *get_adkey_configure(u8 num)
{
    if (xcfg_cb.adc_detect_linein) {
        if ((num > 1) && (num < 8)) {
            adc_linein_flag = 1;
        } else {
            adc_linein_flag = 0;
        }
        return NULL;
    } else {
        if (xcfg_cb.adkey_config_en) {
            return &xcfg_cb.adkey1_num0;
        } else {
            return NULL;
        }
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
    {0x0f, KEY_PLAY_POWER},     //P/P POWER     0
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
        return &xcfg_cb.pwrkey_num0;
    } else {
        return NULL;
    }
}
#endif // USER_PWRKEY

#if USER_IOKEY
gpio_t iokey0_gpio;
gpio_t iokey1_gpio;
gpio_t iokey2_gpio;
gpio_t iokey3_gpio;

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

    if (key_config_table[xcfg_cb.iokey_num4] == NO_KEY) {
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

        iokey_cfg_port_init(&iokey0_gpio);
        iokey_cfg_port_init(&iokey1_gpio);
        iokey_cfg_port_init(&iokey2_gpio);
        iokey_cfg_port_init(&iokey3_gpio);
        return;
    }

    //默认TSSOP24的IOKEY
    GPIOADE  |= BIT(6);
    GPIOADIR |= BIT(6);
    GPIOAPU  |= BIT(6);

    GPIOEDE  |= BIT(7);
    GPIOEDIR |= BIT(7);
    GPIOEPU  |= BIT(7);
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
        } else if (iokey_cfg_midkey_pressed(&iokey0_gpio, &iokey1_gpio)) {
            key_val = key_config_table[xcfg_cb.iokey_num4];
        }
        return key_val;
    }

    //IOKEY不配置时，默认TSSOP24的IOKEY
    if (!(GPIOA & BIT(6))) {
        //KEY0
        key_val = KEY_PREV_VOL_DOWN;
    } else if (!(GPIOE & BIT(7))) {
        //KEY1
        key_val = KEY_NEXT_VOL_UP;
    } else {
        GPIOACLR  = BIT(6);
        GPIOADIR &= ~BIT(6);
        delay_us(6);
        if (!(GPIOE & BIT(7))) {
            key_val = KEY_HSF;
        }
        GPIOADIR |= BIT(6);
    }
    return key_val;
}
#endif // USER_IOKEY

