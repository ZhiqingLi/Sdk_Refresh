#ifndef __BSP_PARAM_H
#define __BSP_PARAM_H

#define PARAM_MSC_NUM_SD            0
#define PARAM_MSC_NUM_USB           2
#define PARAM_SYS_VOL               4
#define PARAM_HSF_VOL               5
#define PARAM_LANG_ID               6
#define PARAM_RANDOM_KEY            7           //4Byte随机数
#define PARAM_BT_XOSC_CAP           0x0b        //1Byte xosc value
#define PARAM_SYS_MODE              0x0c        //系统模式记忆
#define PARAM_MSC_NUM_SD1           0x0d        //2BYTE

#define PARAM_MSC_BRKPT_SD          0x10        //10BYTE = frame_count(4byte) +  file_ptr(4byte) + fname_crc(2byte)
#define PARAM_MSC_BRKPT_USB         0x1a        //10BYTE = frame_count(4byte) +  file_ptr(4byte) + fname_crc(2byte)
#define PARAM_MSC_BRKPT_SD1         0x24        //10BYTE = frame_count(4byte) +  file_ptr(4byte) + fname_crc(2byte)

#define PARAM_FMRX_CHCUR            0x30
#define PARAM_FMRX_CHCNT            0x31
#define PARAM_FMRX_CHBUF            0x32        //26Byte
#define PARAM_FMTX_FREQ             0x4C        //FM TX freq 2 Byte
#define PARAM_ECHO_LEVEL            0x4E        //echo level 1 Byte
#define PARAM_ECHO_DELAY            0x4F        //echo delay 1 Byte

#define PARAM_EXIT_STATE            0x50        //20190711,外部MCU状态，16Byte

#define RTCRAM_PWROFF_FLAG          63         //软关机的标识放在RTCRAM的最后一BYTE



void param_init(bool reset);
void param_sync(void);
void param_msc_num_write(void);
void param_msc_num_read(void);
void param_msc_breakpoint_write(void);
void param_msc_breakpoint_read(void);
void param_fmrx_chcur_write(void);
void param_fmrx_chcur_read(void);
void param_fmrx_chcnt_write(void);
void param_fmrx_chcnt_read(void);
void param_fmrx_chbuf_write(void);
void param_fmrx_chbuf_read(void);
void param_sys_vol_write(void);
void param_sys_vol_read(void);
void param_lang_id_write(void);
void param_lang_id_read(void);
void param_random_key_write(void);
void param_random_key_read(u8 *key);
u8 param_sys_mode_read(void);
void param_sys_mode_write(u8 mode);
//20190711
void param_exit_state_write(bool reboot);
bool param_exit_state_read(void);

#endif // __BSP_PARAM_H

