#ifndef _API_MUSIC_H
#define _API_MUSIC_H

enum {
    MUSIC_STOP = 0,
    MUSIC_PAUSE,
    MUSIC_PLAYING,
};

enum {
    NONE_TYPE = 0,
    WAV_TYPE,
    MP3_TYPE,
    WMA_TYPE,
    APE_TYPE,
    FLAC_TYPE,
};

enum {
    //decode msg
    MUSIC_MSG_STOP = 0,
    MUSIC_MSG_PAUSE,
    MUSIC_MSG_PLAY,

    //encode msg
    ENC_MSG_MP3 = 32,
    ENC_MSG_MSBC,
    ENC_MSG_AEC,
    ENC_MSG_ALC,
    ENC_MSG_PLC,
    ENC_MSG_SBC,
    ENC_MSG_WAV,
    ENC_MSG_ADPCM,
    ENC_MSG_SPDIF,
};

enum {
    QSKIP_BACKWARD,
    QSKIP_FORWARD,
};

void music_control(u8 msg);
u8 get_music_dec_sta(void);
int music_decode_init(void);
int wav_decode_init(void);
int mp3_decode_init(void);
int wma_decode_init(void);
int ape_decode_init(void);
int flac_decode_init(void);
int sbcio_decode_init(void);
void sbc_decode_init(void);
u16 music_get_total_time(void);
u16 music_get_cur_time(void);
void music_set_cur_time(u32 cur_time);
void music_qskip(bool direct, u8 second);      //快进快退控制 direct = 1为快退，0为快进
void music_qskip_end(void);
void music_set_jump(void *brkpt);
void music_get_breakpiont(void *brkpt);
u32 get_music_bitrate(void);
void music_stream_var_init(void);
int music_is_encrypt(u16 key);                 //返回0表示为加密音乐
void music_stream_decrypt(void *buf, unsigned int len);

void res_play_loop_back(u8 enable);             //提示音循环播放开关
#endif // _API_MUSIC_H
