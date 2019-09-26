#include "include.h"

///配置工具选择MUTE及AB/D控制的GPIO
static gpio_t mute_gpio;
static gpio_t amp_gpio;
static u8 amp_conf_type = 0;

void loudspeaker_mute_init(void)
{
    if (!xcfg_cb.spk_mute_en) {
        xcfg_cb.loudspeaker_unmute_delay = 0;
        return;
    }
    u8 io_num = xcfg_cb.spk_mute_io_sel;
    gpio_t *p = &mute_gpio;
    bsp_gpio_cfg_init(p, io_num);
    if (p->sfr == NULL) {
        return;
    }
    p->sfr[GPIOxDE] |= BIT(p->num);
    p->sfr[GPIOxDIR] &= ~BIT(p->num);
    if (xcfg_cb.high_mute) {
        p->sfr[GPIOxSET] = BIT(p->num);
    } else {
        p->sfr[GPIOxCLR] = BIT(p->num);
    }
}

AT(.com_text.mute)
void loudspeaker_mute(void)
{
    if (!xcfg_cb.spk_mute_en) {
        return;
    }
    gpio_t *p = &mute_gpio;
    if (p->sfr == NULL) {
        return;
    }

	p->sfr[GPIOxDE] |= BIT(p->num);
	p->sfr[GPIOxDIR] &= ~BIT(p->num);	//输出
    if (xcfg_cb.high_mute) {
        p->sfr[GPIOxSET] = BIT(p->num);
    } else {
        p->sfr[GPIOxCLR] = BIT(p->num);
    }
}

AT(.com_text.mute)
void loudspeaker_unmute(void)
{
    if (!xcfg_cb.spk_mute_en) {
        return;
    }
    gpio_t *p = &mute_gpio;
    if (p->sfr == NULL) {
        return;
    }
    
	p->sfr[GPIOxDE] |= BIT(p->num);
	p->sfr[GPIOxDIR] &= ~BIT(p->num);	//输出
	if (xcfg_cb.high_mute) {
		p->sfr[GPIOxCLR] = BIT(p->num);
	} else {
		p->sfr[GPIOxSET] = BIT(p->num);
	}
	
    if ((xcfg_cb.ampabd_type == 1) && !xcfg_cb.high_mute) {
        delay_5ms(4);           //先拉高20ms
        if (amp_conf_type) {    //AB类脉冲控制
            for (u32 i = 0; i < 4; i++) {
                p->sfr[GPIOxCLR] = BIT(p->num);
                delay_us(75);
                p->sfr[GPIOxSET] = BIT(p->num);
                delay_us(75);
            }
        } else {                //暂时只支持D类防破音关闭
            for (u32 i = 0; i < 3; i++) {
                p->sfr[GPIOxCLR] = BIT(p->num);
                delay_us(75);
                p->sfr[GPIOxSET] = BIT(p->num);
                delay_us(75);
            }
        }
    } else if (xcfg_cb.ampabd_type == 2) {
    	 if (amp_conf_type) {    //AB类设置为输入10K上拉
			p->sfr[GPIOxDE] |= BIT(p->num);
			p->sfr[GPIOxDIR] |= BIT(p->num);	//输入
			if (xcfg_cb.high_mute) {
				p->sfr[GPIOxPU] &= ~BIT(p->num);
				p->sfr[GPIOxPD] |= BIT(p->num);
			} else {
				p->sfr[GPIOxPU] |= BIT(p->num);
				p->sfr[GPIOxPD] &= ~BIT(p->num);
			}
        } else {                //D类设置为输出10K上拉
            p->sfr[GPIOxDE] |= BIT(p->num);
			p->sfr[GPIOxDIR] &= ~BIT(p->num);	//输出
			p->sfr[GPIOxPU] |= BIT(p->num);
			if (xcfg_cb.high_mute) {
				p->sfr[GPIOxCLR] = BIT(p->num);
			} else {
				p->sfr[GPIOxSET] |= BIT(p->num);
			}
        }
    }
}

AT(.text.mute)
void loudspeaker_disable(void)
{
    if (!xcfg_cb.spk_mute_en) {
        return;
    }
    gpio_t *p = &mute_gpio;
    if (p->sfr == NULL) {
        return;
    }
    p->sfr[GPIOxDIR] |= BIT(p->num);
}

AT(.text.amplifier)
void amp_sel_cfg_d(void)
{
    amp_conf_type = 0;
    gpio_t *p = &amp_gpio;
    if (p->sfr == NULL) {
        return;
    }

    if (0 == xcfg_cb.ampabd_type) {				//只实现IO控制模式切换，其他方式在解MUTE时设置
	    p->sfr[GPIOxDE] |= BIT(p->num);
	    p->sfr[GPIOxDIR] &= ~BIT(p->num);
		if (xcfg_cb.ampabd_level){				//20190224
	    	p->sfr[GPIOxSET] = BIT(p->num);
	    }else {
			p->sfr[GPIOxCLR] = BIT(p->num);
		}
	}
}

AT(.text.amplifier)
void amp_sel_cfg_init(u8 io_num)
{
    gpio_t *p = &amp_gpio;
    bsp_gpio_cfg_init(p, io_num);
    amp_sel_cfg_d();
}

AT(.text.amplifier)
void amp_sel_cfg_ab(void)
{
    amp_conf_type = 1;
    gpio_t *p = &amp_gpio;
    if (p->sfr == NULL) {
        return;
    }

    if (0 == xcfg_cb.ampabd_type) {				//只实现IO控制模式切换，其他方式在解MUTE时设置
		p->sfr[GPIOxDE] |= BIT(p->num);
		p->sfr[GPIOxDIR] &= ~BIT(p->num);
		if (xcfg_cb.ampabd_level){				//20190224
	    	p->sfr[GPIOxCLR] = BIT(p->num);
	    }else {
			p->sfr[GPIOxSET] = BIT(p->num);
		}
	}
}

AT(.text.amplifier)
void amp_sel_cfg_dis(void)
{
    gpio_t *p = &amp_gpio;
    if (p->sfr == NULL) {
        return;
    }
    p->sfr[GPIOxDE] |= BIT(p->num);
    p->sfr[GPIOxDIR] |= BIT(p->num);
}
