#ifndef _API_FMRX_H
#define _API_FMRX_H

#define FM_FREQ_MIN                         8750
#define FM_FREQ_MAX                         10800

#define FM_0DB_VAL          8192
#define FM_DIG_P18DB       (FM_0DB_VAL / 0.125893)      //65071    //7.943282
#define FM_DIG_P17DB       (FM_0DB_VAL / 0.141254)      //57994    //7.079458
#define FM_DIG_P16DB       (FM_0DB_VAL / 0.158489)      //51688    //6.309574
#define FM_DIG_P15DB       (FM_0DB_VAL / 0.177828)      //46067    //5.623413
#define FM_DIG_P14DB       (FM_0DB_VAL / 0.199526)      //41057    //5.011872
#define FM_DIG_P13DB       (FM_0DB_VAL / 0.223872)      //36592    //4.466835
#define FM_DIG_P12DB       (FM_0DB_VAL / 0.251189)      //32612    //3.981072
#define FM_DIG_P11DB       (FM_0DB_VAL / 0.281838)      //29066    //3.548134
#define FM_DIG_P10DB       (FM_0DB_VAL / 0.316228)      //25905    //3.162278
#define FM_DIG_P9DB       (FM_0DB_VAL / 0.354813)      //23088    //2.818383
#define FM_DIG_P8DB       (FM_0DB_VAL / 0.398107)      //20577    //2.511886
#define FM_DIG_P7DB       (FM_0DB_VAL / 0.446684)      //18339    //2.238721
#define FM_DIG_P6DB       (FM_0DB_VAL / 0.501187)      //16345    //1.995262
#define FM_DIG_P5DB       (FM_0DB_VAL / 0.562341)      //14567    //1.778279
#define FM_DIG_P4DB       (FM_0DB_VAL / 0.630957)      //12983    //1.584893
#define FM_DIG_P3DB       (FM_0DB_VAL / 0.707946)      //11571    //1.412538
#define FM_DIG_P2DB       (FM_0DB_VAL / 0.794328)      //10313    //1.258925
#define FM_DIG_P1DB       (FM_0DB_VAL / 0.891251)      //9191    //1.122018
#define FM_DIG_N0DB       (FM_0DB_VAL / 1.000000)      //8192    //1.000000
#define FM_DIG_N1DB       (FM_0DB_VAL / 1.122018)      //7301    //0.891251
#define FM_DIG_N2DB       (FM_0DB_VAL / 1.258925)      //6507    //0.794328
#define FM_DIG_N3DB       (FM_0DB_VAL / 1.412538)      //5799    //0.707946
#define FM_DIG_N4DB       (FM_0DB_VAL / 1.584893)      //5168    //0.630957
#define FM_DIG_N5DB       (FM_0DB_VAL / 1.778279)      //4606    //0.562341
#define FM_DIG_N6DB       (FM_0DB_VAL / 1.995262)      //4105    //0.501187
#define FM_DIG_N7DB       (FM_0DB_VAL / 2.238721)      //3659    //0.446684
#define FM_DIG_N8DB       (FM_0DB_VAL / 2.511886)      //3261    //0.398107
#define FM_DIG_N9DB       (FM_0DB_VAL / 2.818383)      //2906    //0.354813
#define FM_DIG_N10DB       (FM_0DB_VAL / 3.162278)      //2590    //0.316228
#define FM_DIG_N11DB       (FM_0DB_VAL / 3.548134)      //2308    //0.281838
#define FM_DIG_N12DB       (FM_0DB_VAL / 3.981072)      //2057    //0.251189
#define FM_DIG_N13DB       (FM_0DB_VAL / 4.466836)      //1833    //0.223872
#define FM_DIG_N14DB       (FM_0DB_VAL / 5.011872)      //1634    //0.199526
#define FM_DIG_N15DB       (FM_0DB_VAL / 5.623413)      //1456    //0.177828
#define FM_DIG_N16DB       (FM_0DB_VAL / 6.309574)      //1298    //0.158489
#define FM_DIG_N17DB       (FM_0DB_VAL / 7.079459)      //1157    //0.141254
#define FM_DIG_N18DB       (FM_0DB_VAL / 7.943282)      //1031    //0.125893
#define FM_DIG_N19DB       (FM_0DB_VAL / 8.912509)      //919    //0.112202
#define FM_DIG_N20DB       (FM_0DB_VAL / 10.000000)      //819    //0.100000
#define FM_DIG_N21DB       (FM_0DB_VAL / 11.220183)      //730    //0.089125
#define FM_DIG_N22DB       (FM_0DB_VAL / 12.589254)      //650    //0.079433
#define FM_DIG_N23DB       (FM_0DB_VAL / 14.125375)      //579    //0.070795
#define FM_DIG_N24DB       (FM_0DB_VAL / 15.848934)      //516    //0.063096
#define FM_DIG_N25DB       (FM_0DB_VAL / 17.782794)      //460    //0.056234
#define FM_DIG_N26DB       (FM_0DB_VAL / 19.952621)      //410    //0.050119
#define FM_DIG_N27DB       (FM_0DB_VAL / 22.387212)      //365    //0.044668
#define FM_DIG_N28DB       (FM_0DB_VAL / 25.118862)      //326    //0.039811
#define FM_DIG_N29DB       (FM_0DB_VAL / 28.183833)      //290    //0.035481
#define FM_DIG_N30DB       (FM_0DB_VAL / 31.622778)      //259    //0.031623
#define FM_DIG_N31DB       (FM_0DB_VAL / 35.481335)      //230    //0.028184
#define FM_DIG_N32DB       (FM_0DB_VAL / 39.810719)      //205    //0.025119
#define FM_DIG_N33DB       (FM_0DB_VAL / 44.668358)      //183    //0.022387
#define FM_DIG_N34DB       (FM_0DB_VAL / 50.118728)      //163    //0.019953
#define FM_DIG_N35DB       (FM_0DB_VAL / 56.234132)      //145    //0.017783
#define FM_DIG_N36DB       (FM_0DB_VAL / 63.095731)      //129    //0.015849
#define FM_DIG_N37DB       (FM_0DB_VAL / 70.794580)      //115    //0.014125
#define FM_DIG_N38DB       (FM_0DB_VAL / 79.432822)      //103    //0.012589
#define FM_DIG_N39DB       (FM_0DB_VAL / 89.125104)      //91     //0.011220

void fmrx_manual_gain_init(u16 gain);  //8192(0DB)
void fmrx_pga_gain_set(u8 gain);
s8 fmrx_rssi_dbu_get(void);   //位于段 AT(.fmrxcom2.*)
s16 fmrx_rssi_sum_get(void);   //信噪比统计
s8 fmrx_rssi_pow_get(void);    //信号强度  //10dbu~49dbu  基本一致 //小于 10dbu 或大于49dbu误差会变大  //等同以前的 fmrx_rssi_dbu_get
//noise特征连续大于noise_power, 且持续时间为noise_time毫秒，则启动淡出, 淡出到gian
//noise特征连续小于voice_power，且持续时间为voice_time毫秒，则启动入,恢复正常的gain
//淡入淡出速度由fade_step控制
void fmrx_noise_dnr_init(u16 noise_time, u16 noise_power, u16 voice_time, u16 voice_power, u16 gain, u16 fade_step);
void fmrx_noise_dnr_cfg(u32 cfg); //1 换台时先高增益，中途有强台变弱台后才低增益 //2 换台时先低增益，判断到有强台才高增益


void fmrx_dac_sync(void);
void fmrx_power_on(u32 val);
void fmrx_power_off(void);
void fmrx_analog_init(void);
void fmrx_digital_stop(void);
void fmrx_digital_start(void);
void fmrx_set_freq(u16 freq);
u8 fmrx_check_freq(u16 freq);
void fmrx_logger_out(void);
void fmrx_set_rf_cap(u8 val);
void fmrx_set_audio_channel(u8 mono_flag);  //设置立体声或单声道解调输出  1:mono, 0:stereo
bool fmrx_cur_freq_is_stereo(void);         //检测当前收听台是不是立体声, 用于车机的ST标志显示(判断需要100ms)

void fmrx_rec_enable(void);
void fmrx_rec_start(void);
void fmrx_rec_stop(void);

#endif // _API_FMRX_H
