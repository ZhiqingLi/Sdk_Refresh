#ifndef _API_MUSIC_H
#define _API_MUSIC_H

enum {
    MUSIC_STOP = 0,
    MUSIC_PAUSE,
    MUSIC_PLAYING,
};

enum {
    //decode msg
    MUSIC_MSG_STOP = 0,
    MUSIC_MSG_PAUSE,
    MUSIC_MSG_PLAY,

    //encode msg
    ENC_MSG_MP3 = 32,
    ENC_MSG_AEC,
    ENC_MSG_ALC,
    ENC_MSG_PLC,
    ENC_MSG_SBC,
    ENC_MSG_WAV,
    ENC_MSG_ADPCM,
};

enum {
    QSKIP_BACKWARD,
    QSKIP_FORWARD,
};

extern unsigned char avio_buf[556];

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
void music_set_eq_by_res(u32 *addr, u32 *len);
void music_set_eq_by_num(u8 num);
void music_eq_off(void);
void music_set_eq(u8 band_cnt, const u32 *eq_param);
void music_stream_var_init(void);
int music_is_encrypt(u16 key);                 //返回0表示为加密音乐
void music_stream_decrypt(void *buf, unsigned int len);
#endif // _API_MUSIC_H
