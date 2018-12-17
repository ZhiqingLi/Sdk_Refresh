#ifndef _TTS_INTERFACE_H
#define _TTS_INTERFACE_H

#include <typeext.h>
#include <api.h>

//tts语音片段信息
typedef struct
{
    uint32 offset; //语音片段起始地址
    uint32 length; //语音片段数据长度
} tts_section_t;

//TTS索引表头数据结构,占据32byte,reserved 24byte
typedef struct
{
    uint16 wordcnt; //TTS字库汉字的个数                 //完整库6768
    uint16 bytes1; //每个汉字信息所占字节数            // 8
    uint16 multi_cnt; //多音字其他发音个数                //当前15个
    uint16 bytes2; //占用字节数                        //8
    uint16 citiaocnt; //扩展词条的个数                    //31
    uint16 bytes3; //每个扩展词条信息所占字节数        // 6
} tts_header_t;

typedef struct
{
    uint32 start_offset; //
    uint16 str_length; //
    uint16 multi_offset; //      多音字链表偏移，0XFFFF代表没有多音字
} tts_pos_t;

typedef struct
{
    uint8* str_buf; //存储字符信息地址，字符以内码的方式存储
    uint8 reserve[2]; //保留
} tts_info_t;

typedef struct
{
    char multitone_tag; //
    char qu; //
    char wei; //      多音字链表偏移，0XFFFF代表没有多音字
} tts_multitone_t;

/* tts 驱动命令枚举类型*/
typedef enum
{
    /*tts初始化*/
    TTS_PLAY_INIT = 0,
    /*tts退出*/
    TTS_PLAY_EXIT,
    /*TTS帧播放*/
    TTS_PLAY_FRAME
} tts_cmd_e;

//驱动入口
extern void* tts_op_entry(void *param1, void *param2, void *param3, tts_cmd_e cmd)__FAR__;

/*int32 tts_play_init(uint8 *section_ids, uint8 section_cnt, tts_info_t* pb_info)*/
/*设置tts播放参数*/
/*section_ids:播放的词条的id类表;section_cnt:词条个数;pb_info:电话本语音字符串(如果为NULL则为词条播报)*/
#define tts_status_play_init(a,b,c)    (int)tts_op_entry((void*)(a), (void*)(uint32)(b), (void*)(c), TTS_PLAY_INIT)

/*int32 tts_play_exit(void* null0, void* null1, void* null2)*/
/*关闭tts资源文件,卸载dsp库等释放tts播放资源等操作*/
#define tts_status_play_exit()         (int)tts_op_entry((void*)(0), (void*)(0), (void*)(0), TTS_PLAY_EXIT)

/*int32 tts_play_frame(void* null0, void* null1, void* null2)*/
/*处理每一帧播报,以及播报过程中与dsp的交互工作*/
#define tts_status_play_frame()        (int)tts_op_entry((void*)(0), (void*)(0), (void*)(0), TTS_PLAY_FRAME)

#endif //_TTS_INTERFACE_H
