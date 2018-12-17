/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：ID3相关接口
 * 作者：Gary Wang
 ********************************************************************************/

#ifndef _eh_id3_h
#define _eh_id3_h

#include "enhanced.h"
#include "eh_globle_data.h"

#define  FILTRATE_SPACE_SW   1

#define  UNICODE_FLAG  0xfeff
#define  UTF8_FLAG_0   0xef
#define  UTF8_FLAG_1   0xbb
#define  UTF8_FLAG_2   0xbf

#define FILE_SIZE_MIN    512
#define  SECTOR_SIZE   512   //读写扇区大小
#define KEY_BUF_LEN        64

typedef struct
{
    uint8 savelen; // FrameID保存的最长长度
    uint8* saveaddr; // FrameID保存地址
} id3_save_t;

extern HANDLE id3_handle;

extern uint8 *id3_temp_buf;
extern uint8 *key_buf;
extern uint8 check_count;
extern uint8 check_flag[8];     //查找ID3要素标记
extern uint16 id3_pos_buffer;     //当前TempBuf未处理字符的索引
extern uint32 id3_pos_file;       //当前文件的精确指针

extern id3_save_t *id3_save_p;     //FrameID存储位置
extern id3_info_t *id3_info_p;     //ap层提供的存储结构


extern bool get_init(void) __FAR__;
extern bool read_buf_data(uint8 *Tbuff, uint32 size) __FAR__;
extern char reset_file_pos(uint32 size, uint32 last_pos_file)__FAR__;
//int8 check_tag_list(uint8 *check_str,uint8 *tab_str,uint8 size_str,uint8 tab_count);
//int8 check_pic_flag(uint8 *buf);
extern uint8 str_to_capital(char *str, char end_char) __FAR__;
//uint32 get_frame_size(uint8 *data,uint8 byte_cn,uint8 bit7_flag);
extern uint32 id3_read_utf8_deal(id3_save_t* dest_p, uint32 read_size,uint8 *src_buf) __FAR__;

extern bool get_id3_aa(void) __FAR__;
extern bool get_id3_aac(void) __FAR__;
extern bool get_id3_aax(void) __FAR__;
extern bool get_id3_ape(void) __FAR__;
extern bool get_id3_flac(void) __FAR__;
extern bool get_id3_mp3(void) __FAR__;
extern bool get_id3_ogg(void) __FAR__;
extern bool get_id3_wma(void) __FAR__;
bool get_id3_aax_sub(uint32 end_pos_param);
extern void check_odd_char_in_str(uint8 *buf,uint8 length)__FAR__;

#endif//#ifndef _eh_id3_h
