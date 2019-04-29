//该头文件由软件自动生成，请勿随意修改！
#ifndef _XCFG_H
#define _XCFG_H

#define XCFG_EN             1

typedef struct __attribute__((packed)) _xcfg_cb_t {
    u8 lang_id;                                 //提示音语言选择: 0:英文, 1:中文, 2:中英文(出厂默认英文), 3:中英文(出厂默认中文)
    u32 func_music_en                    : 1;   //SD/U盘播放功能
    u32 func_aux_en                      : 1;   //AUX输入功能
    u32 func_fmrx_en                     : 1;   //FM收音功能
    u32 powkey_10s_reset                 : 1;   //POWKEY 10s复位系统
    u8 ext_power_io_sel;                        //外部POWER控制IO选择: 0:None, 1:PA0, 2:PA1, 3:PA2, 4:PA3, 5:PA4, 6:PA5, 7:PA6, 8:PA7, 9:PB0, 10:PB1, 11:PB2, 12:PB3, 13:PB4, 14:PE0, 15:PE1, 16:PE2, 17:PE3, 18:PE4, 19:PE5, 20:PE6, 21:PE7, 22:PF0, 23:PF1, 24:PF2, 25:PF3, 26:PF4
    u32 spk_mute_en                      : 1;   //功放MUTE功能
    u8 spk_mute_io_sel;                         //功放MUTE控制IO选择: 0:None, 1:PA0, 2:PA1, 3:PA2, 4:PA3, 5:PA4, 6:PA5, 7:PA6, 8:PA7, 9:PB0, 10:PB1, 11:PB2, 12:PB3, 13:PB4, 14:PE0, 15:PE1, 16:PE2, 17:PE3, 18:PE4, 19:PE5, 20:PE6, 21:PE7, 22:PF0, 23:PF1, 24:PF2, 25:PF3, 26:PF4
    u32 high_mute                        : 1;   //高MUTE
    u8 loudspeaker_unmute_delay;                //功放MUTE延时(单位5ms)
    u8 ampabd_type;                             //功放AB/D控制模式: 0:独立IO电平控制, 1:mute脉冲控制
    u8 ampabd_io_sel;                           //功放AB/D控制IO选择: 0:None, 1:PA0, 2:PA1, 3:PA2, 4:PA3, 5:PA4, 6:PA5, 7:PA6, 8:PA7, 9:PB0, 10:PB1, 11:PB2, 12:PB3, 13:PB4, 14:PE0, 15:PE1, 16:PE2, 17:PE3, 18:PE4, 19:PE5, 20:PE6, 21:PE7, 22:PF0, 23:PF1, 24:PF2, 25:PF3, 26:PF4
    u32 ampabd_level                     : 1;   //D类高电平
    u32 earphone_det_iosel               : 6;   //耳机检测IO选择: None: 0, PA0: 1, PA1: 2, PA2: 3, PA3: 4, PA4: 5, PA5: 6, PA6: 7, PA7: 8, PB0: 9, PB1: 10, PB2: 11, PB3: 12, PB4: 13, PE0: 14, PE1: 15, PE2: 16, PE3: 17, PE4: 18, PE5: 19, PE6: 20, PE7: 21, PF0: 22, PF1: 23, PF2: 24, PF3: 25, PF4: 26, PF5: 27, 复用SDCLK检测: 28, 复用SDCMD检测: 29, 复用PWRKEY检测: 30
    u16 sys_sleep_time;                         //自动休眠时间: 不休眠: 0, 10秒钟后: 10, 20秒钟后: 20, 30秒钟后: 30, 45秒钟后: 45, 1分钟后: 60, 2分钟后: 120, 3分钟后: 180, 4分钟后: 240, 5分钟后: 300, 6分钟后: 360, 7分钟后: 420, 8分钟后: 480, 9分钟后: 540, 10分钟后: 600, 15分钟后: 900, 20分钟后: 1200, 25分钟后: 1500, 30分钟后: 1800, 45分钟后: 2700, 1小时后: 3600
    u16 sys_off_time;                           //自动关机时间: 不关机: 0, 30秒钟后: 30, 1分钟后: 60, 2分钟后: 120, 3分钟后: 180, 4分钟后: 240, 5分钟后: 300, 6分钟后: 360, 7分钟后: 420, 8分钟后: 480, 9分钟后: 540, 10分钟后: 600, 15分钟后: 900, 20分钟后: 1200, 25分钟后: 1500, 30分钟后: 1800, 45分钟后: 2700, 1小时后: 3600
    u32 lowpower_warning_en              : 1;   //低电提示
    u8 lpwr_warning_vbat;                       //低电语音提示电压: 0:2.8V, 1:2.9V, 2:3.0V, 3:3.1V, 4:3.2V, 5:3.3V, 6:3.4V, 7:3.5V, 8:3.6V, 9:3.7V
    u8 lpwr_off_vbat;                           //低电关机电压: 0:不关机, 1:2.8V, 2:2.9V, 3:3.0V, 4:3.1V, 5:3.2V, 6:3.3V, 7:3.4V, 8:3.5V, 9:3.6V, 10:3.7V
    u8 lpwr_warning_period;                     //低电语音播报周期(秒)
    u8 lpwr_warning_count;                      //低电语音播报次数
    u8 vol_max;                                 //音量级数: 0:16级音量, 1:30级音量, 2:32级音量, 3:50级音量
    u8 sys_init_vol;                            //开机默认音量
    u8 warning_volume;                          //提示音播放音量
    u8 osc_both_cap;                            //OSC基础电容: 0:0PF, 1:6PF
    u8 osci_cap;                                //OSCI电容(0.25PF)
    u8 osco_cap;                                //OSCO电容(0.25PF)
    u32 vddio_sel                        : 4;   //VDDIO电压: None: 0, 2.6V: 3, 2.7V: 4, 2.8V: 5, 2.9V: 6, 3.0V: 7, 3.1V: 8, 3.2V: 9, 3.3V: 10, 3.4V: 11, 3.5V: 12, 3.6V: 13, 3.7V: 14
    u32 eq_dgb_spp_en                    : 1;   //EQ调试（蓝牙串口）
    u32 eq_dgb_uart_en                   : 1;   //EQ调试（UART）
    u32 eq_uart_sel                      : 2;   //EQ调试串口选择: PA7: 0, PB2: 1, PB3: 2
    u32 dac_sel                          : 4;   //DAC声道选择: 单声道: 0, 双声道: 1, VCMBUF单声道: 2, VCMBUF双声道: 3, MIX_VCMBUF单声道: 4
    u32 dac_ldoh_sel                     : 4;   //VDDDAC电压: 2.4V: 0, 2.5V: 1, 2.7V: 2, 2.9V: 3, 3.1V: 4, 3.2V: 5, 3.1V_VSEL2: 6, 3.2V_VSEL2: 7
    u32 dac_vcm_less_en                  : 1;   //省VCM方案
    u32 dacvdd_bypass_en                 : 1;   //省VDDDAC方案
    u8 dac_max_gain;                            //DAC最大音量: 0:-9DB, 1:-8DB, 2:-7DB, 3:-6DB, 4:-5DB, 5:-4DB, 6:-3DB, 7:-2DB, 8:-1DB, 9:0DB, 10:+1DB, 11:+2DB, 12:+3DB, 13:+4DB, 14:+5DB
    u8 bt_call_max_gain;                        //通话最大音量: 0:-9DB, 1:-8DB, 2:-7DB, 3:-6DB, 4:-5DB, 5:-4DB, 6:-3DB, 7:-2DB, 8:-1DB, 9:0DB, 10:+1DB, 11:+2DB, 12:+3DB, 13:+4DB, 14:+5DB
    u32 charge_en                        : 1;   //充电使能
    u32 charge_trick_en                  : 1;   //涓流充电使能
    u32 charge_dc_reset                  : 1;   //插入DC复位系统
    u32 charge_dc_not_pwron              : 1;   //插入DC禁止软开机
    u32 charge_warehouse_set             : 3;   //充电仓充电操作选项: 不进行任何操作: 0, 耳机充电完成自动关机: 1, 耳机从充电仓拿起自动开机: 2
    u8 charge_stop_curr;                        //充电截止电流: 0:2.5mA, 1:5mA, 2:10mA, 3:15mA, 4:20mA, 5:25mA, 6:30mA, 7:35mA
    u8 charge_constant_curr;                    //恒流充电电流: 0:30mA, 1:40mA, 2:50mA, 3:60mA, 4:70mA, 5:80mA, 6:90mA, 7:100mA, 8:110mA, 9:120mA, 10:140mA, 11:160mA, 12:180mA, 13:200mA
    u8 charge_trickle_curr;                     //涓流充电电流: 0:30mA, 1:40mA, 2:50mA
    char bt_name[32];                           //蓝牙名称
    u8 bt_addr[6];                              //蓝牙地址
    u8 bt_txpwr;                                //预置RF发射功率: 0:+4DBM, 1:+3DBM, 2:+2DBM, 3:+1DBM, 4:0DBM, 5:-1DBM, 6:-2DBM, 7:-3DBM, 8:-4DBM
    u32 bt_rf_param                      : 8;   //预置RF参数选择: LQFP48: 0, QFN32: 1, SOP28: 2, TSSOP24: 3, TSSOP20: 4, SOP16: 5, QFN32小耳机板: 129, SOP16小耳机板: 133, AUTO: 255
    u32 bt_user_param_en                 : 1;   //是否自定义RF参数
    u8 rf_tag0;                                 //TAG0
    u8 rf_tag1;                                 //TAG1
    u8 rf_captune;                              //CAPTUNE
    u8 rf_pa_gain;                              //PA_GAIN
    u8 rf_mix_gain;                             //MIX_GAIN
    u8 rf_dig_gain;                             //DIG_GAIN
    u8 rf_pa_bias;                              //PA_BIAS
    u8 rf_vbko_add;                             //VBKO_ADD
    u8 rf_txdbm;                                //DBM
    u8 rf_udf;                                  //UDF
    u8 bt_pwrkey_nsec_discover;                 //PWRKEY开机长按进入配对: 0:不支持, 1:1秒, 2:2秒, 3:3秒, 4:4秒, 5:5秒, 6:6秒, 7:7秒, 8:8秒
    u32 bt_dis_reconnect_en              : 1;   //蓝牙断开主动回连
    u8 bt_dis_reconnect_cnt;                    //主动回连时间(单位S)
    u32 bt_2acl_en                       : 1;   //连接两部手机功能
    u32 bt_a2dp_en                       : 1;   //音乐播放功能
    u32 bt_a2dp_vol_ctrl_en              : 1;   //音乐音量同步
    u32 bt_sco_en                        : 1;   //通话功能
    u32 bt_hfp_private_en                : 1;   //私密接听功能
    u32 bt_hfp_ring_number_en            : 1;   //来电报号功能
    u32 bt_spp_en                        : 1;   //串口功能
    u32 bt_hid_en                        : 1;   //拍照功能
    u32 bt_hid_menu_en                   : 1;   //按键HID连接/断开功能
    u32 bt_hid_discon_default_en         : 1;   //HID默认不连接
    u32 func_bthid_en                    : 1;   //HID独立自拍器模式
    char bthid_name[32];                        //HID蓝牙名称
    u32 bt_tws_en                        : 1;   //TWS功能
    u32 bt_tws_pair_mode                 : 3;   //TWS配对方式选择: 自动配对: 0, 双击PLAY键配对: 1, 长按MODE键配对: 2, 自定义配对<调用api>: 3
    u32 bt_tws_lr_mode                   : 4;   //TWS声道分配选择: 不分配，主副均双声道输出: 0, 自动分配，主右声道副左声道: 1, PWRKEY,有820K接地为左: 2, GPIOx有接地为左: 3, 自动分配，主左声道副右声道: 4
    u32 tws_sel_left_gpio_sel            : 6;   //TWS GPIOx有接地为左: None: 0, PA0: 1, PA1: 2, PA2: 3, PA3: 4, PA4: 5, PA5: 6, PA6: 7, PA7: 8, PB0: 9, PB1: 10, PB2: 11, PB3: 12, PB4: 13, PE0: 14, PE1: 15, PE2: 16, PE3: 17, PE4: 18, PE5: 19, PE6: 20, PE7: 21, PF0: 22, PF1: 23, PF2: 24, PF3: 25, PF4: 26, PF5: 27
    u32 ble_en                           : 1;   //BLE控制功能
    char le_name[32];                           //BLE名称
    u32 dac_off_for_conn                 : 1;   //蓝牙连接时关闭DAC
    u8 bt_ch_mic;                               //MIC通路选择: 0:MIC_PF2, 1:MIC_PF5
    u8 bt_anl_gain;                             //MIC模拟增益
    u8 bt_dig_gain;                             //MIC数字增益(0~3DB)
    u32 mic_det_iosel                    : 6;   //MIC插入检测配置: None: 0, PA0: 1, PA1: 2, PA2: 3, PA3: 4, PA4: 5, PA5: 6, PA6: 7, PA7: 8, PB0: 9, PB1: 10, PB2: 11, PB3: 12, PB4: 13, PE0: 14, PE1: 15, PE2: 16, PE3: 17, PE4: 18, PE5: 19, PE6: 20, PE7: 21, PF0: 22, PF1: 23, PF2: 24, PF3: 25, PF4: 26, PF5: 27, 复用SDCLK检测: 28, 复用SDCMD检测: 29, 复用PWRKEY检测: 30
    u32 bt_noise_threshoid;                     //环境噪声阈值
    u32 bt_aec_en                        : 1;   //AEC功能
    u8 bt_echo_level;                           //AEC回声消除级别
    u8 bt_far_offset;                           //AEC远端补偿值
    u32 bt_alc_en                        : 1;   //ALC功能
    u8 bt_alc_in_delay;                         //ALC淡入延时: 0:16ms, 1:64ms, 2:112ms, 3:160ms, 4:224ms, 5:288ms
    u8 bt_alc_in_step;                          //ALC淡入速度: 0:4ms, 1:8ms, 2:16ms, 3:32ms, 4:64ms, 5:128ms
    u8 bt_alc_out_delay;                        //ALC淡出延时: 0:16ms, 1:64ms, 2:112ms, 3:160ms, 4:224ms, 5:288ms
    u8 bt_alc_out_step;                         //ALC淡出速度: 0:4ms, 1:8ms, 2:16ms, 3:32ms, 4:64ms, 5:128ms
    u32 music_wav_support                : 1;   //WAV解码
    u32 music_wma_support                : 1;   //WMA解码
    u32 music_ape_support                : 1;   //APE解码
    u32 music_flac_support               : 1;   //FLAC解码
    u32 music_sdcard_en                  : 1;   //SDCARD播放功能
    u32 music_udisk_en                   : 1;   //UDISK播放功能
    u32 sddet_iosel                      : 6;   //SDCARD检测IO选择: None: 0, PA0: 1, PA1: 2, PA2: 3, PA3: 4, PA4: 5, PA5: 6, PA6: 7, PA7: 8, PB0: 9, PB1: 10, PB2: 11, PB3: 12, PB4: 13, PE0: 14, PE1: 15, PE2: 16, PE3: 17, PE4: 18, PE5: 19, PE6: 20, PE7: 21, PF0: 22, PF1: 23, PF2: 24, PF3: 25, PF4: 26, PF5: 27, 复用SDCLK检测: 28, 复用SDCMD检测: 29, 复用PWRKEY检测: 30
    u32 sd1det_iosel                     : 6;   //SDCARD1检测IO选择: None: 0, PA0: 1, PA1: 2, PA2: 3, PA3: 4, PA4: 5, PA5: 6, PA6: 7, PA7: 8, PB0: 9, PB1: 10, PB2: 11, PB3: 12, PB4: 13, PE0: 14, PE1: 15, PE2: 16, PE3: 17, PE4: 18, PE5: 19, PE6: 20, PE7: 21, PF0: 22, PF1: 23, PF2: 24, PF3: 25, PF4: 26, PF5: 27, 复用SDCLK检测: 28, 复用SDCMD检测: 29, 复用PWRKEY检测: 30
    u8 fmrx_rfcap_val;                          //FMRX电感匹配电容
    u8 fmrx_threshold_val;                      //FMRX搜台阈值
    u32 aux_2_sdadc_en                   : 1;   //AUX通路进ADC
    u32 linein_2_pwrdown_en              : 1;   //插入LINEIN关机
    u32 mode_2_aux_en                    : 1;   //模式切换进AUX模式
    u32 aux_anl_gain                     : 3;   //AUX模拟增益: -6DB: 0, -3DB: 1, 0DB: 2, +3DB: 3, +6DB: 4
    u32 aux_dig_gain                     : 5;   //AUX数字增益(0~3DB)
    u32 auxl_sel                         : 5;   //AUXL通路选择: 无AUXL输入: 0, AUXL_PA6: 1, AUXL_PB1: 2, AUXL_PE6: 3, AUXL_VCMBUF: 4, AUXL_MIC_PF2: 5, AUXL_MIC_PF5: 6, AUXL_VOUTRP: 7
    u32 auxr_sel                         : 5;   //AUXR通路选择: 无AUXR输入: 0, AUXR_PA7: 1, AUXR_PB2: 2, AUXR_PE7: 3, AUXL_VCMBUF: 4, AUXR_MIC_PF2: 5, AUXR_MIC_PF5: 6
    u32 linein_det_iosel                 : 6;   //LINEIN插入检测配置: None: 0, PA0: 1, PA1: 2, PA2: 3, PA3: 4, PA4: 5, PA5: 6, PA6: 7, PA7: 8, PB0: 9, PB1: 10, PB2: 11, PB3: 12, PB4: 13, PE0: 14, PE1: 15, PE2: 16, PE3: 17, PE4: 18, PE5: 19, PE6: 20, PE7: 21, PF0: 22, PF1: 23, PF2: 24, PF3: 25, PF4: 26, PF5: 27, 复用SDCLK检测: 28, 复用SDCMD检测: 29, 复用PWRKEY检测: 30
    u32 user_pwrkey_en                   : 1;   //PWRKEY功能
    u32 user_adkey_en                    : 1;   //ADKEY1功能
    u32 user_adkey2_en                   : 1;   //ADKEY2功能
    u32 user_iokey_en                    : 1;   //IOKEY功能
    u32 user_adkey_mux_sdclk_en          : 1;   //复用SDCLK的ADKEY
    u32 user_key_double_en               : 1;   //按键双击功能
    u32 double_key_time                  : 3;   //双击响应时间选择: 200ms: 0, 300ms: 1, 400ms: 2, 500ms: 3, 600ms: 4, 700ms: 5, 800ms: 6, 900ms: 7
    u8 adkey_ch;                                //ADKEY1通路选择: 0:ADCCH_PA5, 1:ADCCH_PA6, 2:ADCCH_PA7, 3:ADCCH_PB1, 4:ADCCH_PB2, 5:ADCCH_PB3, 6:ADCCH_PB4, 7:ADCCH_PE5, 8:ADCCH_PE6, 9:ADCCH_PE7, 10:ADCCH_PF2
    u8 adkey2_ch;                               //ADKEY2通路选择: 0:ADCCH_PA5, 1:ADCCH_PA6, 2:ADCCH_PA7, 3:ADCCH_PB1, 4:ADCCH_PB2, 5:ADCCH_PB3, 6:ADCCH_PB4, 7:ADCCH_PE5, 8:ADCCH_PE6, 9:ADCCH_PE7, 10:ADCCH_PF2
    u8 pwron_press_time;                        //软开机长按时间选择: 0:1.5秒, 1:2秒, 2:2.5秒, 3:3秒, 4:3.5秒, 5:4秒, 6:4.5秒, 7:5秒
    u8 pwroff_press_time;                       //软关机长按时间选择: 0:1.5秒, 1:2秒, 2:2.5秒, 3:3秒, 4:3.5秒, 5:4秒, 6:4.5秒, 7:5秒
    u32 key_double_config_en             : 1;   //支持双击的按键定制
    u8 dblkey_num0;                             //双击按键1选择: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 dblkey_num1;                             //双击按键2选择: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 dblkey_num2;                             //双击按键3选择: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 dblkey_num3;                             //双击按键4选择: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 dblkey_num4;                             //双击按键5选择: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u32 pwrkey_config_en                 : 1;   //PWRKEY按键定制
    u8 pwrkey_num0;                             //PWRKEY按键1功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 pwrkey_num1;                             //PWRKEY按键2功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 pwrkey_num2;                             //PWRKEY按键3功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 pwrkey_num3;                             //PWRKEY按键4功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 pwrkey_num4;                             //PWRKEY按键5功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u32 adkey_config_en                  : 1;   //ADKEY1按键定制
    u8 adkey1_num0;                             //ADKEY1按键1功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 adkey1_num1;                             //ADKEY1按键2功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 adkey1_num2;                             //ADKEY1按键3功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 adkey1_num3;                             //ADKEY1按键4功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 adkey1_num4;                             //ADKEY1按键5功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 adkey1_num5;                             //ADKEY1按键6功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 adkey1_num6;                             //ADKEY1按键7功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 adkey1_num7;                             //ADKEY1按键8功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 adkey1_num8;                             //ADKEY1按键9功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 adkey1_num9;                             //ADKEY1按键10功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 adkey1_num10;                            //ADKEY1按键11功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 adkey1_num11;                            //ADKEY1按键12功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u32 adkey2_config_en                 : 1;   //ADKEY2按键定制
    u8 adkey2_num0;                             //ADKEY2按键1功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 adkey2_num1;                             //ADKEY2按键2功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 adkey2_num2;                             //ADKEY2按键3功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 adkey2_num3;                             //ADKEY2按键4功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 adkey2_num4;                             //ADKEY2按键5功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 adkey2_num5;                             //ADKEY2按键6功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 adkey2_num6;                             //ADKEY2按键7功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 adkey2_num7;                             //ADKEY2按键8功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 adkey2_num8;                             //ADKEY2按键9功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 adkey2_num9;                             //ADKEY2按键10功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 adkey2_num10;                            //ADKEY2按键11功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 adkey2_num11;                            //ADKEY2按键12功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u32 iokey_config_en                  : 1;   //IOKEY按键定制
    u8 iokey_io0;                               //IOKEY按键1的IO: 0:None, 1:PA0, 2:PA1, 3:PA2, 4:PA3, 5:PA4, 6:PA5, 7:PA6, 8:PA7, 9:PB0, 10:PB1, 11:PB2, 12:PB3, 13:PB4, 14:PE0, 15:PE1, 16:PE2, 17:PE3, 18:PE4, 19:PE5, 20:PE6, 21:PE7, 22:PF0, 23:PF1, 24:PF2
    u8 iokey_io1;                               //IOKEY按键2的IO: 0:None, 1:PA0, 2:PA1, 3:PA2, 4:PA3, 5:PA4, 6:PA5, 7:PA6, 8:PA7, 9:PB0, 10:PB1, 11:PB2, 12:PB3, 13:PB4, 14:PE0, 15:PE1, 16:PE2, 17:PE3, 18:PE4, 19:PE5, 20:PE6, 21:PE7, 22:PF0, 23:PF1, 24:PF2
    u8 iokey_io2;                               //IOKEY按键3的IO: 0:None, 1:PA0, 2:PA1, 3:PA2, 4:PA3, 5:PA4, 6:PA5, 7:PA6, 8:PA7, 9:PB0, 10:PB1, 11:PB2, 12:PB3, 13:PB4, 14:PE0, 15:PE1, 16:PE2, 17:PE3, 18:PE4, 19:PE5, 20:PE6, 21:PE7, 22:PF0, 23:PF1, 24:PF2
    u8 iokey_io3;                               //IOKEY按键4的IO: 0:None, 1:PA0, 2:PA1, 3:PA2, 4:PA3, 5:PA4, 6:PA5, 7:PA6, 8:PA7, 9:PB0, 10:PB1, 11:PB2, 12:PB3, 13:PB4, 14:PE0, 15:PE1, 16:PE2, 17:PE3, 18:PE4, 19:PE5, 20:PE6, 21:PE7, 22:PF0, 23:PF1, 24:PF2
    u8 iokey_io4;                               //IOKEY按键5的IO: 0:None, 1:PA0, 2:PA1, 3:PA2, 4:PA3, 5:PA4, 6:PA5, 7:PA6, 8:PA7, 9:PB0, 10:PB1, 11:PB2, 12:PB3, 13:PB4, 14:PE0, 15:PE1, 16:PE2, 17:PE3, 18:PE4, 19:PE5, 20:PE6, 21:PE7, 22:PF0, 23:PF1, 24:PF2
    u8 iokey_num0;                              //IOKEY按键1功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 iokey_num1;                              //IOKEY按键2功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 iokey_num2;                              //IOKEY按键3功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 iokey_num3;                              //IOKEY按键4功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 iokey_num4;                              //IOKEY按键5功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u8 iokey_midkey_num;                        //两IO中间的按键功能: 0:None, 1:PP/POWER, 2:P/P, 3:POWER, 4:NEXT/VOL+, 5:VOL+/NEXT, 6:PREV/VOL-, 7:VOL-/PREV, 8:VOL+, 9:VOL-, 10:NEXT, 11:PREV, 12:MODE, 13:PP/MODE, 14:HSF, 15:EQ, 16:REPEAT, 17:MUTE, 18:MODE/POWER, 19:REC, 20:PLAY/HSF
    u32 led_disp_en                      : 1;   //系统指示灯(蓝灯)
    u32 led_pwr_en                       : 1;   //电源状态灯(红灯)
    u32 charge_full_bled                 : 1;   //充满电蓝灯亮
    u32 charge_full_bled_on_time;               //充满电蓝灯亮的时间(秒)
    u32 rled_lowbat_en                   : 1;   //电池低电闪灯
    u8 bled_io_sel;                             //蓝灯IO选择: 0:None, 1:PA0, 2:PA1, 3:PA2, 4:PA3, 5:PA4, 6:PA5, 7:PA6, 8:PA7, 9:PB0, 10:PB1, 11:PB2, 12:PB3, 13:PB4, 14:PE0, 15:PE1, 16:PE2, 17:PE3, 18:PE4, 19:PE5, 20:PE6, 21:PE7, 22:PF0, 23:PF1, 24:PF2, 25:PF3, 26:PF4, 27:PF5
    u8 rled_io_sel;                             //红灯IO选择: 0:None, 1:PA0, 2:PA1, 3:PA2, 4:PA3, 5:PA4, 6:PA5, 7:PA6, 8:PA7, 9:PB0, 10:PB1, 11:PB2, 12:PB3, 13:PB4, 14:PE0, 15:PE1, 16:PE2, 17:PE3, 18:PE4, 19:PE5, 20:PE6, 21:PE7, 22:PF0, 23:PF1, 24:PF2, 25:PF3, 26:PF4, 27:PF5
    u32 led_pwron_config_en              : 1;   //开机状态配置LED
    struct __attribute__((packed)) {
        u8 redpat;
        u8 bluepat;
        u8 unit;
        u8 cycle;
    } led_poweron;                              //开机闪灯控制
    u32 led_pwroff_config_en             : 1;   //关机状态配置LED
    struct __attribute__((packed)) {
        u8 redpat;
        u8 bluepat;
        u8 unit;
        u8 cycle;
    } led_poweroff;                             //关机闪灯控制
    u32 led_btinit_config_en             : 1;   //蓝牙初始化状态配置LED
    struct __attribute__((packed)) {
        u8 redpat;
        u8 bluepat;
        u8 unit;
        u8 cycle;
    } led_btinit;                               //初始化闪灯控制
    u32 led_reconnect_config_en          : 1;   //蓝牙回连状态配置LED
    struct __attribute__((packed)) {
        u8 redpat;
        u8 bluepat;
        u8 unit;
        u8 cycle;
    } led_reconnect;                            //回连闪灯控制
    u32 led_btpair_config_en             : 1;   //蓝牙配对状态配置LED
    struct __attribute__((packed)) {
        u8 redpat;
        u8 bluepat;
        u8 unit;
        u8 cycle;
    } led_pairing;                              //配对闪灯控制
    u32 led_tws_main_config_en           : 1;   //TWS主机连接状态配置LED
    struct __attribute__((packed)) {
        u8 redpat;
        u8 bluepat;
        u8 unit;
        u8 cycle;
    } led_tws_main_conn;                        //TWS主机连接闪灯控制
    u32 led_tws_slave_config_en          : 1;   //TWS副机连接状态配置LED
    struct __attribute__((packed)) {
        u8 redpat;
        u8 bluepat;
        u8 unit;
        u8 cycle;
    } led_tws_slave_conn;                       //TWS副机连接闪灯控制
    u32 led_btconn_config_en             : 1;   //蓝牙已连接状态配置LED
    struct __attribute__((packed)) {
        u8 redpat;
        u8 bluepat;
        u8 unit;
        u8 cycle;
    } led_connected;                            //已连接闪灯控制
    u32 led_btmusic_config_en            : 1;   //蓝牙音乐状态配置LED
    struct __attribute__((packed)) {
        u8 redpat;
        u8 bluepat;
        u8 unit;
        u8 cycle;
    } led_playing;                              //播放闪灯控制
    u32 led_btring_config_en             : 1;   //蓝牙来电状态配置LED
    struct __attribute__((packed)) {
        u8 redpat;
        u8 bluepat;
        u8 unit;
        u8 cycle;
    } led_ring;                                 //来电闪灯控制
    u32 led_btcall_config_en             : 1;   //蓝牙通话状态配置LED
    struct __attribute__((packed)) {
        u8 redpat;
        u8 bluepat;
        u8 unit;
        u8 cycle;
    } led_calling;                              //通话闪灯控制
    u32 led_lowbat_config_en             : 1;   //低电状态配置LED
    struct __attribute__((packed)) {
        u8 redpat;
        u8 bluepat;
        u8 unit;
        u8 cycle;
    } led_lowbat;                               //低电闪灯控制
} xcfg_cb_t;

extern xcfg_cb_t xcfg_cb;
#endif
