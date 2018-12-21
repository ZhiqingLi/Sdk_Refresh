#ifndef _SFUNC_RECORD_H
#define _SFUNC_RECORD_H

#define REC_OBUF_SIZE                0x1000               //ADC PCM缓存BUF SIZE
#define REC_ENC_SIZE                 0x2400              //录音压缩数据缓存BUF SIZE
#define REC_SYNC_TIMES               5                   //间隔5S录音时间同步一次

#define WAVE_FORMAT_PCM              0x0001
#define WAVE_FORMAT_DVI_ADPCM        0x0011
#define PCM_BITS                     16
#define DATA_CKID                    0x61746164

enum {
    REC_STOP,
    REC_PAUSE,
    REC_RECORDING,
    REC_STARTING,
};

typedef struct {
    u8 spr;                         //采样率索引
    u8 nchannel;                    //channel number
    void (*source_start)(void);
    void (*source_stop)(void);
} rec_src_t;
extern rec_src_t rec_src;

//record压缩数据BUF管理
typedef struct {
volatile u16 len;                   //record enc buf valid data bytes
    u8 *buf;                        //record enc buf start address
    u8 *wptr;                       //record enc buf write point
    u8 *rptr;                       //record enc buf read point
} rec_enc_t;
extern rec_enc_t rec_enc_cb;

typedef struct {
    u8 sta;                         //record state machine
    u8 flag_dir     : 1,            //目录已创建标志
       flag_file    : 1,            //文件已创建并打开标志
       flag_play    : 1,            //录音完是否跳到播放
       pause        : 1,            //是否录音暂停
       first_flag   : 1,
       sco_flag     : 1,            //通话录音
       reserved     : 2;
volatile u16 len;                   //record pcm buf valid data bytes
    u8 *obuf;                       //record pcm buf start address
    u8 *wptr;                       //record pcm buf write point
    u8 *rptr;                       //record pcm buf read point
    u16 bps;
    u32 tm_sec;                     //录音时间(second)
    u32 fssect;                     //录音文件起始sector地址
    rec_src_t *src;                 //录音输入源
    rec_enc_t *enc;                 //压缩数据BUF管理
} rec_cb_t;
extern rec_cb_t rec_cb;

typedef struct {
    u8 disp_draw    : 1,
       disp_sta     : 1,
       time_update  : 1;
    u16 bps;
    u32 tm_sec;
} rec_lcd_t;
extern rec_lcd_t rec_lcd;

typedef struct {
    ///RIFF WAVE Chunk
    u32 riff_id;                    //Chunk ID: “RIFF”
    u32 riff_size;                  //文件实际长度-8
    u32 wave_id;                    //WAVE ID: “WAVE”

    ///fmt Chunk
    u32 fmt_id;                     //Chunk ID: “fmt ”
    u32 fmt_size;                   //Chunk size: 16
    u16 wFormatTag;                 //Format code:  WAVE_FORMAT_PCM(0x0001),  WAVE_FORMAT_DVI_ADPCM(0x0011)
    u16 nChannels;                  //Number of interleaved channels
    u32 nSamplesPerSec;             //Sampling rate
    u32 nAvgBytesPerSec;            //Data rate:   (nSamplesPerSec * wBitsPerSample * nChannels)/8
    u16 nBlockAlign;                //Data block size (bytes) :  (wBitsPerSample * nChannels)/8
    u16 wBitsPerSample;             //Bits per sample (16bits)
    u16 cbSize;                     //Size of the extension (2bytes)
    u16 wValidBitsPerSample;        //Number of valid bits

    ///fact Chunk
    u32 fact_id;                    //Chunk ID: “fact”
    u32 fact_size;                  //0x01C8, skip to data Chunk begin
    u32 dwSampleLength;             //Number of samples (per channel)

    u8 reserved[452];

    ///data Chunk
    u32 data_id;                    //Chunk ID: “data”
    u32 data_size;                  //实际PCM数据长度： 文件长度-512
} wav_header_t;

extern const char dir_path_rec[7];
extern const char root_dir_path[7];
extern const char dir_fm_sfn[7];
extern const char dir_aux_sfn[8];
extern const char dir_mic_sfn[8];
extern const char dir_bt_sfn[7];

extern const char dir_fm_sfn_exfat[7];
extern const char dir_aux_sfn_exfat[8];
extern const char dir_mic_sfn_exfat[8];
extern const char dir_bt_sfn_exfat[7];

extern const char dir_path_mic[8];
extern const char dir_path_aux[8];
extern const char dir_path_fm[8];
extern const char dir_path_bt[8];

void sfunc_record_message(u16 msg);
void sfunc_record(void);
void puts_rec_obuf(u8 *inbuf, u16 len);
void sfunc_record_stop(void);
bool sfunc_fwrite_sync(void);
bool sfunc_record_switch_device(void);
bool sfunc_record_start(void);
void sfunc_record_pause(void);
void sfunc_record_continue(void);
bool sfunc_is_recording(void);
void record_var_init(void);
u16 get_rec_mp3_bps(void);
extern bool spifalsh_rec_play_last_file(void);
#if (GUI_SELECT != GUI_NO)
void sfunc_record_display(void);
void sfunc_record_display_enter(void);
#else
#define sfunc_record_display()
#define sfunc_record_display_enter()
#endif

#endif // _FUNC_IDLE_H
