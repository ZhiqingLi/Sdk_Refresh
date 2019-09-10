#include "include.h"
#include "func.h"
#include "func_music.h"

func_msc_t f_msc AT(.buf.func.music);

#if FUNC_MUSIC_EN
char fname_buf[100] AT(.fname.buf);

void music_slot_kick(void);

AT(.text.func.music)
void func_music_mp3_res_play(u32 addr, u32 len)
{
    u32 cur_time;

    if (len == 0) {
        return;
    }

    msc_breakpiont_t brkpt;
    music_get_breakpiont(&brkpt);           //保存当前播放位置
    cur_time = music_get_cur_time();
    music_control(MUSIC_MSG_STOP);

    mp3_res_play(addr, len);

    music_decode_init();
    music_set_jump(&brkpt);                 //恢复播放位置
    music_set_cur_time(cur_time);
    if (f_msc.pause) {
        music_control(MUSIC_MSG_PAUSE);
    } else {
        music_control(MUSIC_MSG_PLAY);
    }
}

AT(.text.func.music)
static void func_music_get_curtime(void)
{
    u16 cur_sec;
    cur_sec = music_get_cur_time() / 10;
    f_msc.curtime.min = cur_sec / 60;
    f_msc.curtime.sec = cur_sec % 60;
}

AT(.text.func.music)
void func_music_remove_device(u8 dev)
{
    if (f_msc.cur_dev == dev) {
        bsp_clr_mute_sta();
        if (dev_is_online(DEV_SDCARD) || dev_is_online(DEV_UDISK) || dev_is_online(DEV_SDCARD1)) {
            f_msc.dev_change = 1;
            music_breakpoint_save();
        } else {
#if MUSIC_MODE_RETURN
            func_cb.sta = func_cb.last;
#else
            func_cb.sta = FUNC_NULL;
#endif
        }
#if BT_APP_EN
        bsp_app_msc_send_dev_info();
#endif
    }
}

AT(.text.func.music)
void func_music_insert_device(u8 dev)
{
    sys_cb.cur_dev = dev;
    bsp_clr_mute_sta();
    f_msc.dev_change = 2;
    music_breakpoint_save();
}

AT(.text.func.music)
bool func_music_switch_device(void)
{
    if (f_msc.cur_dev == DEV_SDCARD) {
        if (dev_is_online(DEV_UDISK)) {
            sys_cb.cur_dev = DEV_UDISK;
            return true;
        } else if (dev_is_online(DEV_SDCARD1)) {
            sys_cb.cur_dev = DEV_SDCARD1;
            return true;
        }
    } else if (f_msc.cur_dev == DEV_UDISK) {
        if (dev_is_online(DEV_SDCARD1)) {
            sys_cb.cur_dev = DEV_SDCARD1;
            return true;
        } else if (dev_is_online(DEV_SDCARD)) {
            sys_cb.cur_dev = DEV_SDCARD;
            return true;
        }
    } else if (f_msc.cur_dev == DEV_SDCARD1) {
        if (dev_is_online(DEV_SDCARD)) {
            sys_cb.cur_dev = DEV_SDCARD;
            return true;
        } else if (dev_is_online(DEV_UDISK)) {
            sys_cb.cur_dev = DEV_UDISK;
            return true;
        }
    }
    return false;
}

AT(.text.func.music)
bool func_music_auto_next_device(void)
{
    if (!func_music_switch_device()) {
        return false;
    }
    led_music_busy();
    fsdisk_callback_init(sys_cb.cur_dev);
    if ((!fs_mount()) || (!pf_scan_music(1))) {
        //挂载失败或无文件时还原到原设备
        fsdisk_callback_init(f_msc.cur_dev);
        if ((!fs_mount()) || (!pf_scan_music(1))) {
            func_cb.sta = FUNC_NULL;
            led_idle();
            return false;
        }
        return false;
    }
    led_idle();
    f_msc.cur_dev = sys_cb.cur_dev;
    return true;
}

//direction: 0->上一曲,    1->下一曲
AT(.text.func.music)
void func_music_switch_file(u8 direction)
{
    u16 dir_snum, dir_lnum;
    music_control(MUSIC_MSG_STOP);
    switch (sys_cb.play_mode) {
    case NORMAL_MODE:
    case SINGLE_MODE:
        if (direction) {
#if MUSIC_AUTO_SWITCH_DEVICE
            //auto switch to next device
            if (f_msc.file_num == f_msc.file_total) {
                if (func_music_auto_next_device()) {
                    f_msc.file_num = 1;
                    f_msc.file_change = 1;
                    return;
                }
            }
#endif // MUSIC_AUTO_SWITCH_DEVICE

            f_msc.file_num++;
            if (f_msc.file_num > f_msc.file_total) {
                f_msc.file_num = 1;
            }
        } else {
#if MUSIC_AUTO_SWITCH_DEVICE
            //auto switch to next device
            if (f_msc.file_num == 1) {
                if (func_music_auto_next_device()) {
                    f_msc.file_num = f_msc.file_total;
                    f_msc.file_change = 1;
                    return;
                }
            }
#endif // MUSIC_AUTO_SWITCH_DEVICE

            f_msc.file_num--;
            if ((f_msc.file_num < 1) || (f_msc.file_num > f_msc.file_total)) {
                f_msc.file_num = f_msc.file_total;
            }
        }
        break;

    case FLODER_MODE:
        dir_snum = fs_get_dir_fstart();                     //获取当前文件夹起始文件编号
        dir_lnum = dir_snum + fs_getdir_files() - 1;        //获取当前文件夹结束文件编号
        if (direction) {
            f_msc.file_num++;
            if (f_msc.file_num > dir_lnum) {
                f_msc.file_num = dir_snum;
            }
        } else {
            f_msc.file_num--;
            if ((f_msc.file_num < dir_snum) || (f_msc.file_num > dir_lnum)) {
                f_msc.file_num = dir_lnum;
            }
        }
        printf("Floder play mode: %d,%d\n", f_msc.file_num, dir_lnum);
        break;

    case RANDOM_MODE:
        f_msc.file_num = get_random(f_msc.file_total) + 1;
        break;
    }

    f_msc.file_change = 1;
    if (direction) {
        f_msc.prev_flag = 0;
    } else {
        f_msc.prev_flag = 1;
    }
}

AT(.text.func.music)
void func_music_auto_next_file(void)
{
    f_msc.file_change = 1;
    f_msc.prev_flag = 0;
    if (sys_cb.play_mode != SINGLE_MODE) {
        func_music_switch_file(1);
    }
}

#if MUSIC_REC_FILE_FILTER
//切换播放录音文件或不播放录音文件
AT(.text.func.music)
bool func_music_filter_switch(u8 rec_type)
{
    u16 file_num = f_msc.file_num;

    music_control(MUSIC_MSG_STOP);
    f_msc.file_change = 1;
    if (rec_type) {
        fs_scan_set(SCAN_SPEED|SCAN_SUB_FOLDER, music_only_record_file_filter, music_only_record_dir_filter);       //只播放录音文件
        if (!pf_scan_music(0)) {
            //无录音文件，还原到正常音乐
            fs_scan_set(SCAN_SPEED|SCAN_SUB_FOLDER, music_file_filter, music_rm_record_dir_filter);                 //不播放录音文件
            pf_scan_music(0);
            f_msc.file_num = file_num;
            return false;
        }
    } else {
        fs_scan_set(SCAN_SPEED|SCAN_SUB_FOLDER, music_file_filter, music_rm_record_dir_filter);                     //不播放录音文件
        if (!pf_scan_music(0)) {
            fs_scan_set(SCAN_SPEED|SCAN_SUB_FOLDER, music_only_record_file_filter, music_only_record_dir_filter);   //只播放录音文件
            pf_scan_music(0);
            f_msc.file_num = file_num;
            return false;
        }
    }
    f_msc.file_num = 1;
    return true;
}
#endif // MUSIC_REC_FILE_FILTER

#if MUSIC_FOLDER_SELECT_EN
//direction: 0->上一文件夹,    1->下一文件夹
AT(.text.func.music)
void func_music_switch_dir(u8 direction)
{
    u16 dir_file_num;
    if (f_msc.dir_total <= 1) {
        return;
    }
    if (direction) {
        f_msc.dir_num++;
    } else {
        f_msc.dir_num--;
    }
    if (f_msc.dir_num > f_msc.dir_total) {
        f_msc.dir_num = 1;
    } else if (f_msc.dir_num < 1) {
        f_msc.dir_num = f_msc.dir_total;
    }
    music_control(MUSIC_MSG_STOP);         //先结束当前播放
    dir_file_num = fs_open_dir_num(f_msc.dir_num);
    if (dir_file_num > 0) {
        f_msc.file_num = dir_file_num;
    }
    f_msc.file_change = 1;
    f_msc.prev_flag = 0;
    bsp_clr_mute_sta();
}

//选择指定编号文件夹播放
AT(.text.func.music)
void func_music_select_dir(u16 sel_num)
{
    u16 dir_file_num;
    if ((sel_num < 1) || (sel_num > f_msc.dir_total)) {
        return;
    }
    f_msc.dir_num = sel_num;
    music_control(MUSIC_MSG_STOP);         //先结束当前播放
    dir_file_num = fs_open_dir_num(f_msc.dir_num);
    if (dir_file_num > 0) {
        f_msc.file_num = dir_file_num;
    }
    f_msc.file_change = 1;
    f_msc.prev_flag = 0;
}

#endif // MUSIC_FOLDER_SELECT_EN

AT(.text.func.music)
void func_music_file_new(void)
{
    if (f_msc.file_change) {
        f_msc.file_change = 0;
        music_control(MUSIC_MSG_STOP);
        if (fs_open_num(f_msc.file_num)) {
            fs_get_short_fname(f_msc.fname, 0);
            f_msc.dir_num = fs_get_dirs_count();        //获取当前文件所在文件夹编号
            f_msc.alltime.min = 0xff;
            f_msc.alltime.sec = 0;
            f_msc.curtime.min = 0;
            f_msc.curtime.sec = 0;
            f_msc.pause = 0;
            f_msc.encrypt = 0;

#if MUSIC_ENCRYPT_EN
            if (!music_is_encrypt(MUSIC_ENCRYPT_KEY)) {
                f_msc.encrypt = 1;
            }
#endif // MUSIC_ENCRYPT_EN

#if MUSIC_ID3_TAG_EN
            if (strstr(f_msc.fname, ".mp3") != NULL) {
                get_mp3_id3_tag();
            }
#endif // MUSIC_ID3_TAG_EN
            if(music_decode_init()) {
#if MUSIC_ID3_TAG_EN
                if (strstr(f_msc.fname, ".wma") != NULL) {
                    get_wma_id3_tag();
                }
#endif // MUSIC_ID3_TAG_EN
                printf("music decoding [%s]\n", f_msc.fname);
                printf("new file number: %d, %d\n", f_msc.file_num, f_msc.file_total);
                param_msc_num_write();
                gui_box_show_num(f_msc.file_num);
                music_breakpoint_init();
                param_sync();
                led_music_play();
                music_control(MUSIC_MSG_PLAY);             //开始播放
                delay_5ms(1);
            }
        }
    }
    f_msc.brkpt_flag = 0;
}

AT(.text.func.music)
void func_music_device_new(void)
{
    u8 dev_change = f_msc.dev_change;
    if (f_msc.dev_change) {
        f_msc.dev_change = 0;
        music_control(MUSIC_MSG_STOP);
        if ((dev_change == 1) && (!func_music_switch_device())) {
            return;
        }
        #if SD_USB_MUX_IO_EN
        printf("sys_cb.cur_dev:%d\n",sys_cb.cur_dev);
        if(f_msc.cur_dev == DEV_UDISK){
            printf("SDmode\n");
            udisk_suspend();
        }if(sys_cb.cur_dev == DEV_UDISK){
            printf("USB mode\n");
            udisk_resume();
        }
        #endif
        //music_control(MUSIC_MSG_STOP);

        led_music_busy();
        fsdisk_callback_init(sys_cb.cur_dev);
        if ((!fs_mount()) || (!pf_scan_music(1))) {
            //还原到原设备
            sys_cb.cur_dev = f_msc.cur_dev;
            fsdisk_callback_init(sys_cb.cur_dev);
            if ((!fs_mount()) || (!pf_scan_music(0))) {
                func_cb.sta = FUNC_NULL;
                led_idle();
                return;
            }
        }
        led_idle();
        f_msc.cur_dev = sys_cb.cur_dev;
        param_msc_num_read();
        f_msc.brkpt_flag = 1;
        f_msc.file_change = 1;
#if SYS_MODE_BREAKPOINT_EN
        param_sys_mode_write(func_cb.sta);
        param_sync();
#endif // SYS_MODE_BREAKPOINT_EN
    }
}

#if IR_NUMKEY_EN
static void func_box_event(void)
{
    u16 ret;
    if (box_cb.event) {
        box_cb.event = 0;
        bsp_clr_mute_sta();
        if (sys_cb.play_mode != FLODER_MODE) {
            if ((box_cb.number > 0) && (box_cb.number <= f_msc.file_total)) {
                f_msc.file_num = box_cb.number;
                f_msc.file_change = 1;
            }
        } else {
            if ((box_cb.number > 0) && (box_cb.number <= f_msc.dir_total)) {
                music_control(MUSIC_MSG_STOP);
                ret = fs_open_dir_num(box_cb.number);           //打开目录，并返回起始文件编号
                printf("open_dir_num: %d, %d\n",box_cb.number, ret);
                if (ret != 0) {
                    f_msc.dir_num = box_cb.number;
                    f_msc.file_num = ret;
                }
                f_msc.file_change = 1;
            }
        }
    }
}
#endif // IR_NUMKEY_EN

AT(.text.func.music)
void func_music_process(void)
{
    u16 total_time;

    func_process();
#if IR_NUMKEY_EN
    func_box_event();
#endif // IR_NUMKEY_EN

    func_music_file_new();
    music_slot_kick();
    func_music_get_curtime();
    if ((get_music_dec_sta() == MUSIC_STOP) && (f_msc.dev_change == 0)) {
        if (dev_is_online(f_msc.cur_dev)) {          //设备拔出结束解码不自动切换下一曲
            music_breakpoint_clr();
            if ((f_msc.prev_flag) && ((f_msc.alltime.min == 0xff)
                || ((f_msc.curtime.min == 0) && (f_msc.curtime.sec == 0)))) {
                //错误文件或播放小于2S保持切换方向
                func_music_switch_file(0);
            } else {
                func_music_auto_next_file();
            }
        }
    }

    if (f_msc.alltime.min == 0xff) {
        total_time = music_get_total_time();
        if (total_time != 0xffff) {
            f_msc.alltime.min = total_time / 60;
            f_msc.alltime.sec = total_time % 60;
            printf("[%s] total time: %02d:%02d\n\n", f_msc.fname, f_msc.alltime.min, f_msc.alltime.sec);
        }
    }

    func_music_device_new();
}

void func_music_filter_set(void)
{
#if MUSIC_REC_FILE_FILTER
    if (f_msc.rec_type) {
        fs_scan_set(SCAN_SPEED|SCAN_SUB_FOLDER, music_only_record_file_filter, music_only_record_dir_filter);       //只播放录音文件
    } else {
        fs_scan_set(SCAN_SPEED|SCAN_SUB_FOLDER, music_file_filter, music_rm_record_dir_filter);                     //不播放录音文件
    }
#else
    fs_scan_set(SCAN_SPEED|SCAN_SUB_FOLDER, music_file_filter, music_dir_filter);                                   //播放全部文件
#endif // MUSIC_REC_FILE_FILTER
}

static void func_music_enter(void)
{
    msg_queue_clear();
    if ((!dev_is_online(DEV_UDISK)) && (!dev_is_online(DEV_SDCARD)) && (!dev_is_online(DEV_SDCARD1))) {
        func_cb.sta = FUNC_NULL;
        return;
    }

#if FMTX_EN
    adpll_freq_update(1);
#endif // FMTX_EN
#if BT_BACKSTAGE_EN
    if (dev_is_online(DEV_UDISK)) {
        udisk_resume();
    }
#endif
    func_music_filter_set();
    fsdisk_callback_init(sys_cb.cur_dev);
    memset(&f_msc, 0, sizeof(f_msc));
    f_msc.fname = &fname_buf[0];
    func_music_enter_display();
#if WARNING_FUNC_MUSIC
    mp3_res_play(RES_BUF_MUSIC_MODE_MP3, RES_LEN_MUSIC_MODE_MP3);
#endif
    led_music_busy();
    if ((!fs_mount()) || (!pf_scan_music(1))) {
        f_msc.cur_dev = sys_cb.cur_dev;
        if (!func_music_auto_next_device()) {
            f_msc.cur_dev = sys_cb.cur_dev;
//#if MUSIC_MODE_RETURN
//            func_cb.sta = func_cb.last;
//#else
            func_cb.sta = FUNC_NULL;
//#endif
            led_idle();
            return;
        }
    }
    led_idle();
    func_cb.mp3_res_play = func_music_mp3_res_play;
    f_msc.cur_dev = sys_cb.cur_dev;
    param_msc_num_read();
    f_msc.file_change = 1;
    f_msc.brkpt_flag = 1;
}

static void func_music_exit(void)
{
    func_music_exit_display();
    music_breakpoint_save();
    music_control(MUSIC_MSG_STOP);
#if SD_USB_MUX_IO_EN
    if (dev_is_online(DEV_UDISK)) {
        sys_cb.cur_dev = DEV_UDISK;
    } else if (dev_is_online(DEV_SDCARD1)) {
        sys_cb.cur_dev = DEV_SDCARD1;
    }
	udisk_invalid();
	sd_enable_user();
#endif
    if (dev_is_online(DEV_SDCARD) || dev_is_online(DEV_SDCARD1)) {
        sd0_stop(1);
#if I2C_MUX_SD_EN
        sd0_go_idle_state();
#endif
    }

#if BT_BACKSTAGE_EN
    if (dev_is_online(DEV_UDISK)) {
        udisk_suspend();
    }
#endif
#if FMTX_EN
    adpll_freq_update(0);
#endif
    func_cb.last = FUNC_MUSIC;
}

AT(.text.func.music)
void func_music(void)
{
    printf("%s\n", __func__);

    func_music_enter();

    while (func_cb.sta == FUNC_MUSIC) {
        func_music_process();
        func_music_message(msg_dequeue());
        func_music_display();
    }
    func_music_exit();
}
#endif // FUNC_MUSIC_EN
