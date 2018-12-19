#include "include.h"
#include "func.h"
#include "func_usbdev.h"

typedef struct {
    u8 vol;
} f_ude_t;
f_ude_t f_ude;

#if FUNC_USBDEV_EN
AT(.text.func.usbdev)
void ude_mic_start(void)
{
    audio_path_init(AUDIO_PATH_USBMIC);
    audio_path_start(AUDIO_PATH_USBMIC);
    dac_spr_set(SPR_48000);
}

AT(.text.func.usbdev)
void ude_mic_stop(void)
{
    audio_path_exit(AUDIO_PATH_USBMIC);
}

AT(.usbdev.com)
u8 ude_get_sys_volume(void)
{
    return sys_cb.vol;
}

AT(.usbdev.com)
u8 ude_get_sys_vol_level(void)
{
    return 256 / VOL_MAX;
}

AT(.usbdev.com)
void ude_set_sys_volume(u8 vol)
{
    bsp_set_volume(vol);
}

AT(.text.func.usbdev)
void func_usbdev_process(void)
{
    func_process();
    usb_device_process();
}

static void func_usbdev_enter(void)
{
    f_ude.vol = sys_cb.vol;         //restore system volume
    if (!dev_is_online(DEV_USBPC)) {
        func_cb.sta = FUNC_NULL;
        return;
    }

    func_usbdev_enter_display();
    led_idle();
#if WARNING_FUNC_USBDEV
    mp3_res_play(RES_BUF_PC_MODE_MP3, RES_LEN_PC_MODE_MP3);
#endif // WARNING_FUNC_USBDEV

#if UDE_SPEAKER_EN
    adpll_spr_set(1);
    dac_spr_set(0);                 //samplerate 48K
    DACDIGCON0 |= BIT(6);           //Src0 Sample Rate Synchronization Enable
    dac_fade_in();
#endif // UDE_SPEAKER_EN
#if UDE_STORAGE_EN
    if (dev_is_online(DEV_SDCARD)) {
        fsdisk_callback_init(sys_cb.cur_dev);
        fs_mount();
    }
#endif // UDE_STORAGE_EN

    usb_device_enter(UDE_ENUM_TYPE);
}

static void func_usbdev_exit(void)
{
    usb_device_exit();
    func_usbdev_exit_display();
#if UDE_SPEAKER_EN
    DACDIGCON0 &= ~BIT(6);
    adpll_spr_set(DAC_OUT_SPR);
    sys_cb.vol = f_ude.vol;         //recover system volume
    bsp_change_volume(sys_cb.vol);
    dac_fade_out();
#endif // UDE_SPEAKER_EN
#if UDE_STORAGE_EN
    if (dev_is_online(DEV_SDCARD)) {
        sd0_stop(1);
    #if I2C_MUX_SD_EN
        sd0_go_idle_state();
    #endif
    }
#endif // UDE_STORAGE_EN

    func_cb.last = FUNC_USBDEV;
}

AT(.text.func.usbdev)
void func_usbdev(void)
{
    printf("%s\n", __func__);

    func_usbdev_enter();

    while (func_cb.sta == FUNC_USBDEV) {
        func_usbdev_process();
        func_usbdev_message(msg_dequeue());
        func_usbdev_display();
    }

    func_usbdev_exit();
}
#endif // FUNC_USBDEV_EN
