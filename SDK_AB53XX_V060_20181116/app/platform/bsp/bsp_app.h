#ifndef _BSP_APP_H
#define _BSP_APP_H

#define CMD_STA_TX_START    BIT(0)      //数据传输开始
#define CMD_STA_TX_ING      BIT(1)      //数据传输中
#define CMD_STA_TX_END      BIT(2)      //数据完成

enum {
    CMD_SYS_MODE = 0x05,    //切模式
};

enum {
    CMD_MSC_VOL,            //音乐音量
    CMD_MSC_BASS_VOL,       //音乐低音
    CMD_MSC_TREB_VOL,       //音乐高音
    CMD_MSC_EQ_TYPE,        //EQ音效类型
    CMD_MSC_EQ,             //自定义8段EQ
};

enum {
    CMD_MIC_VOL,            //MIC音量
    CMD_MIC_ECHO_LEVEL_GAIN,//混响衰减系数
    CMD_MIC_ECHO_DELEY,     //混响间隔
    CMD_MIC_FIRST_EN,       //MIC优先开关
    CMD_MIC_TB_EN,          //MIC高低音开关
    CMD_MIC_DSP_EN,         //MIC DSP音效开关
    CMD_MIC_EQ,             //自定义4/6段EQ
};

enum {
    CMD_FM_ALL_CH,          //频道总数
    CMD_FM_CUR_CH,          //当前频道
    CMD_FM_CUR_FREQ,        //当前频点
    CMD_FM_SEEK,            //搜台
    CMD_FM_LIST,            //电台列表
    CMD_FM_INFO,            //当前播放电台的信息
};

enum {
    CMD_MSC_LIST,           //根据曲目号获取歌曲列表
    CMD_MSC_DEV_STA,        //设备在线状态
    CMD_MSC_INFO,           //歌曲播放信息
    CMD_MSC_PLAY_STA,       //播放状态
    CMD_MSC_CIRCLE_MODE,    //循环模式
    CMD_MSC_PLAY_NUM,       //播放指定序列的歌曲
    CMD_MSC_CUR_TIME,       //当前播放时间
};

enum {
    CMD_REC_START,          //开始/停止录音
    CMD_REC_TIME,           //录音时间
    CMD_REC_PLAY_START,     //开始/停止播放录音文件
    CMD_REC_PLAY_TIME,      //录音文件播放时间
};

enum {
    CMD_AUX_PLAY_INFO,      //播放信息
};

enum {
    APP_CMD_SYS_PERIOD_STA = 0x04,  //系统定时状态
    APP_CMD_INIT_STA,               //系统初始化
    APP_CMD_SYS_CTL,                //系统控制
    APP_CMD_AUDIO_CTL,              //设备音效
    APP_CMD_FM_CTL,                 //电压/充电控制
    APP_CMD_MSC_CTL,                //音乐控制
    APP_CMD_REC_CTL,                //录音控制
    APP_CMD_AUX_CTL,                //AUX控制
};

enum {
    APP_FUNC_BT,
    APP_FUNC_AUX,
    APP_FUNC_FM,
    APP_FUNC_MSC_USB,
    APP_FUNC_MSC_SD,
};

struct app_cb_t{
    u8 tx_buf[20];

    u8 msc_list_sta;
    u16 msc_list_start;
    u16 msc_list_end;
    u16 msc_list_cnt;
};

extern struct app_cb_t app_cb;
void bsp_ble_process(void);
void bsp_app_process(u8 *ptr, u8 len);
void bsp_app_init(void);
void bsp_app_sys_init(void);
bool bsp_app_send_packet(u8 cmd, u8 *buf, u8 len);
void bsp_app_audio_ctl(u8 sub_cmd, u8 dat);
void bsp_app_audio_set_eq(u8 sub_cmd, u8 *gain, u8 len);
void bsp_app_func_send(u8 func);

void bsp_app_fm_seek(u8 en);
void bsp_app_fm_send_freq(u16 freq);
void bsp_app_fm_send_ch(u8 sub_cmd, u8 ch);
void bsp_app_fm_send_list(void);

void bsp_app_msc_send_dev_info(void);
void bsp_app_msc_get_name_for_num(u16 num);
void bsp_app_msc_num(void);
void bsp_app_msc_circle_mode(void);
void bsp_app_msc_play_sta(u8 paly, u8 vol);
void bsp_app_msc_cur_time(void);

void bsp_app_rec_start(u8 rec, u8 start);
void bsp_app_rec_play_time(u8 rec, u32 time);
#endif
