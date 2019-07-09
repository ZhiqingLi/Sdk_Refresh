#include "include.h"
#include "func.h"
#include "func_idle.h"

#if FUNC_IDLE_EN

AT(.text.func.idle)
void func_idle_process(void)
{
	GPIOBDE |= BIT(1);
	GPIOBDIR |= BIT(1);
	GPIOBPU &= ~BIT(1);
	GPIOBPD |= BIT(1);
	sys_cb.vbat_charge_sta = GPIOB & BIT(1);

	if (sys_cb.vbat_charge_sta == 0)
	{
		sys_cb.power_off_play_tone = 0;
		func_cb.sta = FUNC_PWROFF;
	}

    func_process();
}

static void func_idle_enter(void)
{
#if PLUGIN_FUNC_IDLE_ENTER_CHECK
    if (!plugin_func_idle_enter_check()) {  //可以处理开机, 判断是否要停在idle模式, 等待设备插入的方案.
        return;
    }
#endif

	if (sys_cb.vbat_charge_sta)
	{
		gui_box_poweroff();
		mp3_res_play(RES_BUF_POWEROFF_MP3, RES_LEN_POWEROFF_MP3);
	}

    led_idle();
}

static void func_idle_exit(void)
{

}

AT(.text.func.idle)
void func_idle(void)
{
	printf("%s\n", __func__);

	func_idle_enter();

	sys_cb.bright_lv--;
	ledseg_7p3s_HBS7642_brightness_loop();

    while (func_cb.sta == FUNC_IDLE) {
        func_idle_process();
        func_idle_message(msg_dequeue());
        func_idle_display();
    }

    func_idle_exit();
}

#endif
