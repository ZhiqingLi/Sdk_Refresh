#ifndef _FM_INTERFACE_H         
#define _FM_INTERFACE_H

#include "psp_includes.h"
#include "case_independent.h"

typedef struct
{
    uint32 freq; //当前电台的频率。单位为1k (最高位恒为1)，取最低2 字节
    uint8 station; //当前电台对应的电台表项号，from 1 to station_count，当前电台未保存时为0
    uint8 station_count; //电台表中的保存电台数。
} fm_play_status_t;

/* 电台波段模式分为播放模式和菜单选择模式
 ** 如果在非用户电台播放模式下，播放的波段和
 ** 菜单中选择的波段一致。
 ** 用户电台播放时，播放模式的波段为Band_MAX
 ** 菜单中的波段维持不变
 ** 此时如果选择清除预设或保存到预设，都是将
 ** 用户电台的频点保存到当前菜单波段指定的
 ** 预设列表或者从菜单波段指定的列表选择频点删除
 ** 播放用户电台时，如果在菜单中改变菜单波段
 ** 的选择值，会进行波段切换，并更改为预设播放模式*/
typedef enum
{
    Band_China_US = 0, //china/USA band   87500---108000,  step 100kHz
    Band_Japan, //Japan band   76000---90000, step 100kHz
    Band_Europe, //Europe band  87500---108000, step 50kHz
    Band_MAX //播放用户电台时，播放模式可以为Band_MAX
} radio_band_e;

typedef enum
{
    releaseMUTE = 0, //解除静音
    SetMUTE, //静音
} FM_MUTE_e;

typedef enum
{
    HARDSEEK = 0, //采用硬件seek 模式
    SOFTSEEK, //采用软件seek 模式
} FM_SeekMode_e;

typedef enum
{
    DIR_DOWN = 0, //向下搜台
    DIR_UP //向上搜台
} FM_SeekDir_e;

/* FM 驱动命令枚举类型*/
typedef enum
{
    /*FM模组初始化*/
    FM_INIT = 0,
    /*FM模组进入standby*/
    FM_STANDBY,
    /*设置频点开始播放*/
    FM_SETFREQ,
    /*获取当前状态信息*/
    FM_GETSTATUS,
    /*静音与解除静音*/
    FM_MUTE_VOL,
    /*开启软件搜台过程*/
    FM_SEARCH,
    /*开启硬件搜台过程*/
    FM_HARDSEEK,
    /* 设置电台频段*/
    FM_SETBAND,
    /* 设置搜台门限*/
    FM_SETTHROD,
    /* 手动退出硬件seek 过程*/
    FM_BREAKSEEK,
    /* 获取硬件seek 是否完成标识*/
    FM_GET_FLAG,
    /* 获取当前频段信息*/
    FM_GET_BAND,
    /* 获取当前频点信息*/
    FM_GET_FREQ,
    /* 获取当前电台信号强度*/
    FM_GET_INTENTY,
    /* 获取天线状态*/
    FM_GET_ANTEN,
    /* 获取电台立体声状态*/
    FM_GET_STEREO,
    //调试接口
    FM_DEBUG,
} fm_cmd_e;

/* FM 驱动向上层提供的统一入口定义*/
typedef void * (*fm_op_func)(void *, void *, void *);

/* 驱动内部实现的具体接口函数声明*/
typedef struct
{
    fm_op_func Init;
    fm_op_func Standby;
    fm_op_func SetFreq;
    fm_op_func GetStatus;
    fm_op_func Mute;
    fm_op_func Search;
    fm_op_func HardSeek;
    fm_op_func SetBand;
    fm_op_func SetThrod;
    fm_op_func SeekBreak;
    fm_op_func GetSeekOver;
    fm_op_func GetBand;
    fm_op_func GetFreq;
    fm_op_func GetIntsity;
    fm_op_func GetAntenna;
    fm_op_func GetStereo;
    fm_op_func Debug;
} fm_driver_operations;

extern void * fm_op_entry(void *param1, void *param2, void *param3, fm_cmd_e cmd)__FAR__;

//初始化
//uint8_band: 波段，radio_band_e类型
//uint8_level:门限
//uint32_freq:频率(单位kHz)
//返回值:-1表示失败，0表示成功
#define fm_open(uint8_band, uint8_level, uint32_freq)\
    (int)fm_op_entry((void *)(uint32)(uint8_band),\
    (void *)(uint32)(uint8_level),\
    (void *)(uint32_freq), FM_INIT)

//关闭
//返回值:-1表示失败，0表示成功
#define fm_close()\
    (int)fm_op_entry((void *)(0), (void *)(0), (void *)(0), FM_STANDBY)

//设置频率
//uint32_freq:要设置的频率(单位:kHz)
//返回值:-1表示失败，0表示成功
#define fm_set_freq(uint32_freq)\
    (int)fm_op_entry((void *)(uint32_freq), (void *)(0), (void *)(0), FM_SETFREQ)

//得到fm的状态
//p_status: Engine_Status_t型指针
//uint8_mode: 1表示在SEEK的过程中取信息, 0表示正常播放的过程中取信息
//返回值:-1表示失败，0表示成功
#define fm_get_status(p_status, uint8_mode)\
    (int)fm_op_entry((void *)(p_status),\
    (void *)(uint32)(uint8_mode),\
    (void *)(0), FM_GETSTATUS)

//设置静音开关
//uint8_mode:FM_MUTE_e型
//返回值:-1表示失败，0表示成功
#define fm_mute(uint8_mode)\
    (int)fm_op_entry((void *)(uint32)(uint8_mode),\
    (void *)(0), (void *)(0), FM_MUTE_VOL)

//软件搜台
//uint32_freq: 频率(单位:kHz)
//uint8_direct: FM_SeekDir_e型
//返回值:-1表示失败，0表示成功， 1表示成功，并且是立体声
#define fm_soft_seek(uint32_freq, uint8_direct)\
    (int)fm_op_entry((void *)(uint32_freq),\
    (void *)(uint32)(uint8_direct),\
    (void *)(0), FM_SEARCH)

//硬件搜台
//uint32_freq: 频率(单位:kHz)
//uint8_direct: FM_SeekDir_e型
//返回值:-1表示失败，0表示成功
#define fm_hard_seek(uint32_freq, uint8_direct)\
    (int)fm_op_entry((void *)(uint32_freq),\
    (void *)(uint32)(uint8_direct),\
    (void *)(0), FM_HARDSEEK)

//设置波段
//uint8_band: radio_band_e型
//返回值:-1表示失败，0表示成功
#define fm_set_band(uint8_band)\
    (int)fm_op_entry((void *)(uint32)(uint8_band),\
    (void *)(0), (void *)(0), FM_SETBAND)

//设置搜台门限
//uint8_level: 门限的级别，跟FM驱动有关。
//  如果FM驱动中有10级，这里传进去的参数就可以是0~9.
//返回值:-1表示失败，0表示成功
#define fm_set_throd(uint8_level)\
    (int)fm_op_entry((void *)(uint32)(uint8_level),\
    (void *)(0), (void *)(0), FM_SETTHROD)

//中断搜台过程
//返回值:-1表示失败，0表示成功
#define fm_set_breakseek()\
    (int)fm_op_entry((void *)(0), (void *)(0), (void *)(0), FM_BREAKSEEK)

//查询是否还在搜台
//p_flag:指向uint8型的指针
//  *p_flag的
//  bit0表示搜台是否结束    1:已结束    0:未结束
//  bit1找到的是否有效台    1:有效台
//返回值:-1表示失败，0表示成功
#define fm_get_seekflag(p_flag)\
    (int)fm_op_entry((void *)(p_flag),\
    (void *)(0), (void *)(0), FM_GET_FLAG)

//查询波段
//p_band: 指向radio_band_e型的指针, *p_band就是波段
//返回值:-1表示失败，0表示成功
#define fm_get_band(p_band)\
    (int)fm_op_entry((void *)(p_band),\
    (void *)(0), (void *)(0), FM_GET_BAND)

//查询频率
//p_freq: 指向uint32型的指针, *p_freq就是频率(单位kHz)
//返回值:-1表示失败，0表示成功
#define fm_get_freq(p_freq)\
    (int)fm_op_entry((void *)(p_freq),\
    (void *)(0), (void *)(0), FM_GET_FREQ)

//查询信号强度
//p_intensity: 指向uint8型的指针, *p_intensity就是信号强度
//返回值:-1表示失败，0表示成功
#define fm_get_intensity(p_intensity)\
    (int)fm_op_entry((void *)(p_intensity),\
    (void *)(0), (void *)(0), FM_GET_INTENTY)

//查询是否有天线
//p_antena:指向uint8型的指针 *p_antena为1时表示有天线，为0时表示没有
//返回值:-1表示失败，0表示成功
#define fm_get_antena(p_antena)\
    (int)fm_op_entry((void *)(p_antena),\
    (void *)(0), (void *)(0), FM_GET_ANTEN)

//查询是否立体声
//p_stereo:指向uint8型的指针 *p_stereo为1时表示立体声，为0时表示不是
//返回值:-1表示失败，0表示成功
#define fm_get_stereo(p_stereo)\
    (int)fm_op_entry((void *)(p_stereo),\
    (void *)(0), (void *)(0), FM_GET_STEREO)

//调试驱动的接口
//返回值:-1表示失败，0表示成功
#define fm_debug()\
    (int)fm_op_entry((void *)(0), (void *)(0), (void *)(0), FM_DEBUG)

#endif

