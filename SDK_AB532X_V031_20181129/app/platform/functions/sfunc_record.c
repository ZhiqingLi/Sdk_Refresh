#include "include.h"
#include "func.h"
#include "sfunc_record.h"


#if FUNC_REC_EN
rec_src_t rec_src AT(.buf.record);
rec_cb_t  rec_cb AT(.buf.record);
rec_enc_t rec_enc_cb AT(.buf.record);
u8 rec_obuf[REC_OBUF_SIZE] AT(.rec.obuf);
u8 rec_encbuf[REC_ENC_SIZE] AT(.recenc.obuf);
u8 rec_sco_obuf[REC_OBUF_SIZE] AT(.wavenc.sco);

extern u8 fname_buf[100];
void music_enc_control(u8 msg);
void mp3enc_kick_start(void);
bool bt_rec_status_process(void);
void fs_create_time_inc(void);
void mpaen_var_init(void);
void record_enter(void);
void record_exit(void);
void bt_sco_rec_exit(void);

#if ((REC_TYPE_SEL == REC_WAV) || (REC_TYPE_SEL == REC_ADPCM) || BT_HFP_REC_EN)
u8 wav_header_buf[512] AT(.wavenc.aec);

AT(.text.record.table)
const u8 wav_header_tbl[52] = {
    0x52, 0x49, 0x46, 0x46, 0x00, 0x00, 0x00, 0x00, 0x57, 0x41, 0x56, 0x45, 0x66, 0x6D, 0x74, 0x20,
	0x14, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00, 0x44, 0xAC, 0x00, 0x00, 0x10, 0xB1, 0x02, 0x00,
	0x04, 0x00, 0x10, 0x00, 0x02, 0x00, 0x00, 0x00, 0x66, 0x61, 0x63, 0x74, 0xC8, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};
#endif

#if REC_DIR_LFN
//录音根目录
AT(.text.record.table)
const char root_dir_path[7] = "RECORD";

//录音子目录短文件名
AT(.text.record.table)
const char dir_fm_sfn[7]  = {0x46, 0x4D, 0xC2, 0xBC, 0xD2, 0xF4, '\0'};

AT(.text.record.table)
const char dir_aux_sfn[8] = {0x41, 0x55, 0x58, 0xC2, 0xBC, 0xD2, 0xF4, '\0'};

AT(.text.record.table)
const char dir_mic_sfn[8] = {0x4D, 0x49, 0x43, 0xC2, 0xBC, 0xD2, 0xF4, '\0'};

AT(.text.record.table)
const char dir_bt_sfn[7]  = {0x42, 0x54, 0xC2, 0xBC, 0xD2, 0xF4, '\0'};

AT(.text.record.table)
const char dir_fm_sfn_exfat[7]  = {0x46, 0x4D, 0x55, 0x5F, 0xF3, 0x97, '\0'};

AT(.text.record.table)
const char dir_aux_sfn_exfat[8] = {0x41, 0x55, 0x58, 0x55, 0x5F, 0xF3, 0x97, '\0'};

AT(.text.record.table)
const char dir_mic_sfn_exfat[8] = {0x4D, 0x49, 0x43, 0x55, 0x5F, 0xF3, 0x97, '\0'};

AT(.text.record.table)
const char dir_bt_sfn_exfat[7]  = {0x42, 0x54, 0x55, 0x5F, 0xF3, 0x97, '\0'};

//录音子目录长文件名(unicode UTF-16)
AT(.text.record.table)
const char dir_fm_lfn[] = {
    0x04, 0x00,         //unicode characters counter
    0x46, 0x00, 0x4D, 0x00, 0x55, 0x5F, 0xF3, 0x97,                 //"FM录音" unicode
};

AT(.text.record.table)
const char dir_aux_lfn[] = {
    0x05, 0x00,         //unicode characters counter
    0x41, 0x00, 0x55, 0x00, 0x58, 0x00, 0x55, 0x5F, 0xF3, 0x97,     //"AUX录音" unicode
};
const char dir_mic_lfn[] = {
    0x05, 0x00,         //unicode characters counter
    0x4D, 0x00, 0x49, 0x00, 0x43, 0x00, 0x55, 0x5F, 0xF3, 0x97,     //"MIC录音" unicode
};

AT(.text.record.table)
const char dir_bt_lfn[] = {
    0x04, 0x00,         //unicode characters counter
    0x42, 0x00, 0x54, 0x00, 0x55, 0x5F, 0xF3, 0x97,                 //"BT录音" unicode
};

#else
AT(.text.record.table)
const char dir_path_mic[8] = "RECORDM";

AT(.text.record.table)
const char dir_path_aux[8] = "RECORDL";

AT(.text.record.table)
const char dir_path_fm[8]  = "RECORDF";

AT(.text.record.table)
const char dir_path_bt[8]  = "RECORDB";
#endif // REC_DIR_LFN

#if REC_ONE_FOLDER_EN
AT(.text.record.table)
const char dir_path_rec[7]  = "AB_REC";
#endif // REC_ONE_FOLDER_EN

#if BT_HFP_REC_EN
const char file_name_hfp[]  = "HFP0001.WAV";
#endif

#if ((REC_TYPE_SEL == REC_WAV) || (REC_TYPE_SEL == REC_ADPCM))
AT(.text.record.table)
const char file_name_mic[] = "MIC0001.WAV";

AT(.text.record.table)
const char file_name_aux[] = "AUX0001.WAV";

AT(.text.record.table)
const char file_name_fm[]  = "FM0001.WAV";

AT(.text.record.table)
const char file_name_bt[]  = "BT0001.WAV";
#elif (REC_TYPE_SEL == REC_MP3)
AT(.text.record.table)
const char file_name_mic[] = "MIC0001.mp3";

AT(.text.record.table)
const char file_name_aux[] = "AUX0001.mp3";

AT(.text.record.table)
const char file_name_fm[]  = "FM0001.mp3";

AT(.text.record.table)
const char file_name_bt[]  = "BT0001.mp3";
#endif

#define SFN_NUM_LEN               4
#define MIC_SFN_NUM_OFS           3
#define AUX_SFN_NUM_OFS           3
#define FM_SFN_NUM_OFS            2
#define BT_SFN_NUM_OFS            2
#define HFP_SFN_NUM_OFS           3

void mp3en_get_pcm(u8 *buf, u16 len);
bool sbc_encode_init(u8 spr, u8 nch);
bool msbc_encode_init(void);
bool rec_mp3_init(u8 spr, u8 channel);
void rec_mp3_exit(void);
bool adpcm_encode_init(u8 nch);

AT(.text.func.record)
bool sfunc_rec_mkdir(void)
{
    FRESULT res;
    u8 len;

#if REC_ONE_FOLDER_EN      //各模式录音不区分文件夹
    const char *path;
    memset(fname_buf, 0 ,sizeof(fname_buf));
	path = dir_path_rec;
    len = strlen(dir_path_rec);
    res = fs_mkdir(path);
    if ((res == FR_OK) || (res == FR_EXIST)) {
        memcpy(fname_buf, path, len);
        fname_buf[len++] = '/';
        fname_buf[99] = len;            //保存目录PATH的位置，生成文件PATH时使用
        return true;
    }
    return false;
#else

#if REC_DIR_LFN
    const char *lfn;
    u8 fs_type = fs_get_type();

    res = fs_mkdir(root_dir_path);                          //创建"RECORD"根目录
    if ((res != FR_OK) && (res != FR_EXIST)) {
        return false;
    }

    memset(fname_buf, 0 ,sizeof(fname_buf));
    len = strlen(root_dir_path);
    memcpy(fname_buf, root_dir_path, len);
    fname_buf[len++] = '/';
    if (func_cb.sta == FUNC_FMRX) {
        //”FM录音“ 目录
        if (fs_type == FS_EXFAT) {
            memcpy(&fname_buf[len], dir_fm_sfn_exfat, sizeof(dir_fm_sfn_exfat));
            len += sizeof(dir_fm_sfn_exfat);
        } else {
            memcpy(&fname_buf[len], dir_fm_sfn, sizeof(dir_fm_sfn));
            len += sizeof(dir_fm_sfn);
        }
        lfn = dir_fm_lfn;
    } else if (func_cb.sta == FUNC_AUX) {
        //”AUX录音“ 目录
        if (fs_type == FS_EXFAT) {
            memcpy(&fname_buf[len], dir_aux_sfn_exfat, sizeof(dir_aux_sfn_exfat));
            len += sizeof(dir_aux_sfn_exfat);
        } else {
            memcpy(&fname_buf[len], dir_aux_sfn, sizeof(dir_aux_sfn));
            len += sizeof(dir_aux_sfn);
        }
        lfn = dir_aux_lfn;
    } else if (func_cb.sta == FUNC_BT) {
        //”BT录音“ 目录
        if (fs_type == FS_EXFAT) {
            memcpy(&fname_buf[len], dir_bt_sfn_exfat, sizeof(dir_bt_sfn_exfat));
            len += sizeof(dir_bt_sfn_exfat);
        } else {
            memcpy(&fname_buf[len], dir_bt_sfn, sizeof(dir_bt_sfn));
            len += sizeof(dir_bt_sfn);
        }
        lfn = dir_bt_lfn;
    } else {
        //”MIC录音“ 目录
        if (fs_type == FS_EXFAT) {
            memcpy(&fname_buf[len], dir_mic_sfn_exfat, sizeof(dir_mic_sfn_exfat));
            len += sizeof(dir_mic_sfn_exfat);
        } else {
            memcpy(&fname_buf[len], dir_mic_sfn, sizeof(dir_mic_sfn));
            len += sizeof(dir_mic_sfn);
        }
        lfn = dir_mic_lfn;
    }
    res = fs_mkdir_lfn((const char *)fname_buf, lfn);    //创建录音子目录(长文件名)
    if ((res == FR_OK) || (res == FR_EXIST)) {
        fname_buf[len-1] = '/';
        fname_buf[99] = len;                //保存目录PATH的位置，生成文件PATH时使用
        return true;
    }
    return false;
#else

    const char *path;
    memset(fname_buf, 0 ,sizeof(fname_buf));

    if (func_cb.sta == FUNC_FMRX) {
        path = dir_path_fm;
        len = strlen(dir_path_fm);
    } else if (func_cb.sta == FUNC_AUX) {
        path = dir_path_aux;
        len = strlen(dir_path_aux);
    } else if (func_cb.sta == FUNC_BT) {
        path = dir_path_bt;
        len = strlen(dir_path_bt);
    } else {
        path = dir_path_mic;
        len = strlen(dir_path_mic);
    }
    res = fs_mkdir(path);
    if ((res == FR_OK) || (res == FR_EXIST)) {
        memcpy(fname_buf, path, len);
        fname_buf[len++] = '/';
        fname_buf[99] = len;            //保存目录PATH的位置，生成文件PATH时使用
        return true;
    }
    return false;

#endif // REC_DIR_LFN

#endif  // REC_ONE_FOLDER_EN
}

static bool rec_file_sfn_inc(u8 *fn)
{
    u8 *ptr = fn + SFN_NUM_LEN - 1;

    *ptr += 1;
    for (u8 i = 0; i < (SFN_NUM_LEN - 1); i++) {
        if(*ptr > '9') {
            *ptr-- = '0';
            *ptr += 1;
        } else {
            return true;
        }
    }

    return (*ptr > '9')? false : true;
}

AT(.text.func.record)
bool sfunc_rec_create_file(void)
{
    FRESULT res;
    u8 pos = fname_buf[99];

    if (func_cb.sta == FUNC_FMRX) {
        strcpy((char *)&fname_buf[pos], file_name_fm);
        pos += FM_SFN_NUM_OFS;
    } else if (func_cb.sta == FUNC_AUX) {
        strcpy((char *)&fname_buf[pos], file_name_aux);
        pos += AUX_SFN_NUM_OFS;
    } else if (func_cb.sta == FUNC_BT) {
#if BT_HFP_REC_EN
        uint status = bt_get_status();
        if (status > BT_STA_PLAYING) {
            strcpy((char *)&fname_buf[pos], file_name_hfp);
            pos += HFP_SFN_NUM_OFS;
        } else
#endif
        {
            strcpy((char *)&fname_buf[pos], file_name_bt);
            pos += BT_SFN_NUM_OFS;
        }
    } else {
        strcpy((char *)&fname_buf[pos], file_name_mic);
        pos += MIC_SFN_NUM_OFS;
    }
    fs_create_time_inc();

    while (1) {
        res = fs_open((const char *)fname_buf, FA_WRITE|FA_CREATE_NEW);
        if (res == FR_OK) {
            break;
        } else if (res == FR_EXIST) {
            if (!rec_file_sfn_inc(&fname_buf[pos])) {
                return false;
            }
        } else {
            return false;
        }
    }
    printf("%s: %s\n", __func__, fname_buf);
    return true;
}

//可能在DMA中断调用，必须放公共区。缓存ADC数据
AT(.com_rec.func)
void puts_rec_obuf(u8 *inbuf, u16 len)
{
    u16 clen, clen2, rest;
    rec_cb_t *rec = &rec_cb;
    if ((!rec->src) || (rec->len + len) > REC_OBUF_SIZE) {
        //uart_putchar('+');
    } else {
        rest = rec->obuf + REC_OBUF_SIZE - rec->wptr;
        clen = (rest < len) ? rest : len;
        clen2 = len - clen;
        if (clen) {
            memcpy(rec->wptr, inbuf, clen);
        }
        if (clen2) {
            memcpy(rec->obuf, inbuf + clen, clen2);
            rec->wptr = rec->obuf + clen2;
        } else {
            rec->wptr += clen;
        }
        GLOBAL_INT_DISABLE();
        rec->len += len;
        GLOBAL_INT_RESTORE();
    }
#if BT_HFP_REC_EN
    if (rec->sco_flag) {
        music_enc_control(ENC_MSG_ADPCM);
        return;
    }
#endif
#if (REC_TYPE_SEL == REC_WAV)
    music_enc_control(ENC_MSG_WAV);
#elif (REC_TYPE_SEL == REC_ADPCM)
    music_enc_control(ENC_MSG_ADPCM);
#elif (REC_TYPE_SEL == REC_MP3)
    if (rec->len >= rec->trigger_len) {
        mp3enc_kick_start();
    }
#endif
}
#endif // FUNC_REC_EN

//读取len的ADC数据
AT(.com_rec.func)
bool gets_rec_obuf(u8 *buf, u16 len)
{
#if FUNC_REC_EN
    u16 clen, clen2, rest;
    rec_cb_t *rec = &rec_cb;

    if (rec->len < len) {
        return false;
    }

    rest = rec->obuf + REC_OBUF_SIZE - rec->rptr;
    clen = (rest < len) ? rest : len;
    clen2 = len - clen;
    if (clen) {
        memcpy(buf, rec->rptr, clen);
    }
    if (clen2) {
        memcpy(buf + clen, rec->obuf, clen2);
        rec->rptr = rec->obuf + clen2;
    } else {
        rec->rptr += clen;
    }
    GLOBAL_INT_DISABLE();
    rec->len -= len;
    GLOBAL_INT_RESTORE();
    return true;
#else
    return false;
#endif // FUNC_REC_EN
}

//缓存压缩好的数据
AT(.com_rec.func)
bool puts_rec_encbuf(u8 *buf, u16 len)
{
#if FUNC_REC_EN
    u16 clen, clen2, rest;
    rec_enc_t *enc = rec_cb.enc;
    if ((!enc) || (enc->len + len) > REC_ENC_SIZE) {
        //uart_putchar('-');
        return false;             //record enc obuf full
    }
    rest = enc->buf + REC_ENC_SIZE - enc->wptr;
    clen = (rest < len) ? rest : len;
    clen2 = len - clen;
    if (clen) {
        memcpy(enc->wptr, buf, clen);
    }
    if (clen2) {
        memcpy(enc->buf, buf + clen, clen2);
        enc->wptr = enc->buf + clen2;
    } else {
        enc->wptr += clen;
    }
    GLOBAL_INT_DISABLE();
    enc->len += len;
    GLOBAL_INT_RESTORE();

    return true;
#else
    return false;
#endif // FUNC_REC_EN
}

//wave只是将数据从pcmbuf搬到encbuf
void rec_wave_process(void)
{
#if FUNC_REC_EN
    u8 *rbuf = &avio_buf[0];

    if (!gets_rec_obuf(rbuf, 512)) {
        return;
    }
    puts_rec_encbuf(rbuf, 512);
#endif // FUNC_REC_EN
}

#if FUNC_REC_EN
//取len压缩好的数据
AT(.com_rec.func)
bool gets_rec_encbuf(u8 *buf, u16 len)
{
    u16 clen, clen2, rest;
    rec_cb_t *rec = &rec_cb;
    rec_enc_t *enc = rec->enc;

    if (enc->len < len) {
        return false;
    }

    rest = enc->buf + REC_ENC_SIZE - enc->rptr;
    clen = (rest < len) ? rest : len;
    clen2 = len - clen;
    if (clen) {
        memcpy(buf, enc->rptr, clen);
    }
    if (clen2) {
        memcpy(buf + clen, enc->buf, clen2);
        enc->rptr = enc->buf + clen2;
    } else {
        enc->rptr += clen;
    }
    GLOBAL_INT_DISABLE();
    enc->len -= len;
    GLOBAL_INT_RESTORE();
    return true;
}

AT(.text.func.record)
bool sfunc_rec_write_file(u8 *buf)
{
    FRESULT res;

    res = fs_write(buf, 512);
    if (res != FR_OK) {
        if (res == FR_NOT_ENOUGH_CORE) {
            printf("record disk full\n");
        } else {
            printf("record disk failed: %d\n", res);
        }
        sfunc_record_stop();
        rec_cb.flag_play = 1;
        return false;
    }

    return true;
}

#if ((REC_TYPE_SEL == REC_WAV) || (REC_TYPE_SEL == REC_ADPCM) || BT_HFP_REC_EN)
AT(.text.func.record)
bool rec_wave_init(rec_cb_t *rec)
{
    u8 *wbuf = avio_buf;
    wav_header_t *wavhead = (wav_header_t *)wav_header_buf;

    memset(wav_header_buf, 0, 512);
    memcpy(wav_header_buf, wav_header_tbl, sizeof(wav_header_tbl));
    wavhead->wFormatTag = WAVE_FORMAT_PCM;
    wavhead->nChannels  = rec->src->nchannel & 0x03;
    wavhead->nSamplesPerSec  = tbl_sample_rate[rec->src->spr];
    wavhead->nAvgBytesPerSec = (wavhead->nSamplesPerSec * wavhead->nChannels * PCM_BITS) / 8;
    wavhead->nBlockAlign = (wavhead->nChannels * PCM_BITS) / 8;
    wavhead->wBitsPerSample = PCM_BITS;
    wavhead->data_id = DATA_CKID;
#if (REC_TYPE_SEL == REC_ADPCM || BT_HFP_REC_EN)
    wavhead->wFormatTag = WAVE_FORMAT_DVI_ADPCM;
    wavhead->nAvgBytesPerSec = (wavhead->nSamplesPerSec * wavhead->nChannels * 4) / 8;
    wavhead->wValidBitsPerSample = (128 - 4 * wavhead->nChannels) * 2 / wavhead->nChannels + 1;		//adpcm nSamplesPerBlock
    wavhead->wBitsPerSample = 4;
    wavhead->nBlockAlign = 128;
#endif

    memcpy(wbuf, wav_header_buf, 512);
    if (fs_write(wbuf, 512) != FR_OK) {
        return false;
    }
    rec->fssect = fs_get_file_ssect();
    return true;
}

AT(.text.func.record)
static bool rec_wav_header_sync(rec_cb_t *rec)
{
    u8 *wbuf = avio_buf;
    wav_header_t *wavhead = (wav_header_t *)wav_header_buf;
    u32 fsize = fs_get_file_size();
    wavhead->riff_size = fsize - 8;
    wavhead->data_size = fsize - 512;
    wavhead->dwSampleLength = (fsize - 512)/wavhead->nBlockAlign;  //Number of samples
    memcpy(wbuf, wav_header_buf, 512);
    if (disk_writep(wbuf, rec->fssect)) {
        return false;
    }
    return true;
}
#endif

AT(.text.func.record)
void sfunc_rec_proc(void)
{
    u8 *rbuf = &avio_buf[0];

    if (!sfunc_is_recording()) {
        return;
    }

    if (!gets_rec_encbuf(rbuf, 512)) {
        return;
    }

    if (!sfunc_rec_write_file(rbuf)) {
        printf("write err!\n");
        return;
    }
}

AT(.text.func.record)
static bool rec_file_close(rec_cb_t *rec)
{
#if ((REC_TYPE_SEL == REC_WAV) || (REC_TYPE_SEL == REC_ADPCM) || BT_HFP_REC_EN)
     if (rec->sco_flag || (REC_TYPE_SEL == REC_WAV) || (REC_TYPE_SEL == REC_ADPCM)) {
        if (!rec_wav_header_sync(rec)) {
            return false;
        }
     }
#endif
    if (fs_close() != FR_OK) {
        return false;
    }
    return true;
}

AT(.text.func.record)
bool sfunc_is_recording(void)
{
    if (rec_cb.sta == REC_RECORDING) {
        return true;
    }
    return false;
}

AT(.text.func.record)
bool sfunc_record_is_pause(void)
{
    if (rec_cb.sta == REC_PAUSE) {
        return true;
    }
    return false;
}

AT(.text.func.record)
void sfunc_record_stop(void)
{
    rec_cb_t *rec = &rec_cb;
    if (rec->sta == REC_STOP) {
        return;
    }
    printf("record stop\n");
    if (rec->src) {
        rec->src->source_stop();
    }
    record_exit();
    if (rec->flag_file) {
        rec_file_close(rec);
    }
#if BT_HFP_REC_EN
    if (rec->sco_flag) {
        bt_sco_rec_exit();
    } else
#endif
    {
#if (REC_TYPE_SEL == REC_MP3)
        rec_mp3_exit();
#endif
    }
    rec->flag_file = 0;
    rec->flag_dir = 0;
    rec->tm_sec = 0;
    rec->src = 0;
    rec->enc = 0;
    rec->sta = REC_STOP;
    if (dev_is_online(DEV_SDCARD) || dev_is_online(DEV_SDCARD1)) {
        sd0_stop(1);
#if I2C_MUX_SD_EN
        sd0_go_idle_state();
#endif
    }
}

AT(.text.func.record)
void sfunc_record_pause(void)
{
    rec_cb_t *rec = &rec_cb;

    if (rec->sta == REC_RECORDING) {
        rec->src->source_stop();
        record_exit();
#if (REC_TYPE_SEL == REC_MP3)
        rec_mp3_exit();
#endif
        if (dev_is_online(DEV_SDCARD) || dev_is_online(DEV_SDCARD1)) {
            sd0_stop(1);
#if I2C_MUX_SD_EN
            sd0_go_idle_state();
#endif
        }
        rec->src = 0;
        rec->enc = 0;
        rec->sta = REC_PAUSE;
        printf("record pause\n");
    }
}

AT(.text.func.record)
void sfunc_record_continue(void)
{
    if (rec_cb.sta == REC_PAUSE) {
        printf("record continue\n");
        sfunc_record_start();
    }
}

AT(.text.func.record)
bool sfunc_fwrite_sync(void)
{
    rec_cb_t *rec = &rec_cb;

    rec->tm_sec++;
    if (rec->tm_sec % REC_SYNC_TIMES) {
        return true;
    }
#if ((REC_TYPE_SEL == REC_WAV) || (REC_TYPE_SEL == REC_ADPCM) || BT_HFP_REC_EN)
    if (rec->sco_flag || (REC_TYPE_SEL == REC_WAV) || (REC_TYPE_SEL == REC_ADPCM)) {
        if (!rec_wav_header_sync(rec)) {
            return false;
        }
    }
#endif
    if (fs_sync() != FR_OK) {
        return false;
    }
    return true;
}

AT(.text.func.record)
void sfunc_record_process(void)
{
    func_process();
    sfunc_rec_proc();

#if BT_REC_EN
    if(!bt_rec_status_process()) {
        sfunc_record_stop();
    }
#endif // BT_REC_EN
}

AT(.text.func.record)
bool sfunc_record_switch_device(void)
{
    if (sys_cb.cur_dev == DEV_SDCARD) {
        if (dev_is_online(DEV_UDISK)) {
            sys_cb.cur_dev = DEV_UDISK;
            return true;
        } else if (dev_is_online(DEV_SDCARD1)) {
            sys_cb.cur_dev = DEV_SDCARD1;
            return true;
        }
    } else if (sys_cb.cur_dev == DEV_UDISK) {
        if (dev_is_online(DEV_SDCARD1)) {
            sys_cb.cur_dev = DEV_SDCARD1;
            return true;
        } else if (dev_is_online(DEV_SDCARD)) {
            sys_cb.cur_dev = DEV_SDCARD;
            return true;
        }
    } else if (sys_cb.cur_dev == DEV_SDCARD1) {
        if (dev_is_online(DEV_SDCARD)) {
            sys_cb.cur_dev = DEV_SDCARD;
            return true;
        } else if (dev_is_online(DEV_UDISK)) {
            sys_cb.cur_dev = DEV_UDISK;
            return true;
        }
    }
    return false;
}

AT(.text.func.record)
bool sfunc_record_start(void)
{
    rec_cb_t *rec = &rec_cb;

    printf("record start\n");
    rec->sta = REC_STARTING;

    fsdisk_callback_init(sys_cb.cur_dev);
    if (!rec->flag_file) {
        if (!fs_mount()) {
            return false;
        }
    }

#if MUSIC_REC_FILE_FILTER
    if (rec->first_flag) {
        rec->first_flag = 0;
        fs_scan_set(SCAN_SPEED|SCAN_SUB_FOLDER, music_file_filter, music_dir_filter);
        pf_scan_music(0);
    }
#endif // MUSIC_REC_FILE_FILTER

    //创建录音文件夹
    if (!rec->flag_dir) {
        if (!sfunc_rec_mkdir()) {
            return false;
        }
        rec->flag_dir = 1;
    }

    //创建录音文件
    if (!rec->flag_file) {
        if (!sfunc_rec_create_file()) {
            return false;
        }
        rec->flag_file = 1;
    }
    rec->src = &rec_src;
    do {
#if (REC_TYPE_SEL == REC_WAV)
        if (!rec_wave_init(rec)) {
            rec->src = 0;
            return false;
        }
#endif

#if ((REC_TYPE_SEL == REC_ADPCM) || BT_HFP_REC_EN)
        if ((REC_TYPE_SEL == REC_ADPCM) || rec->sco_flag) {
            if (!rec_wave_init(rec)) {
                rec->src = 0;
                return false;
            }
            adpcm_encode_init(rec->src->nchannel & 0x03);
            break;
        }
#endif

#if (REC_TYPE_SEL == REC_MP3)
        rec->trigger_len = 384 << (rec->src->nchannel & 0x03);
        if (!rec_mp3_init(rec->src->spr, rec->src->nchannel)) {
            rec->src = 0;
            return false;
        }
#endif
    } while(0);

    record_enter();
    memset(&rec_enc_cb, 0, sizeof(rec_enc_cb));
    rec->enc = &rec_enc_cb;
#if BT_HFP_REC_EN
    if (rec->sco_flag) {
        rec->enc->buf = rec->enc->rptr = rec->enc->wptr = (u8*)0x60000;
        rec->obuf = rec->wptr = rec->rptr = rec_sco_obuf;
    } else
#endif
    {
        rec->enc->buf = rec->enc->rptr = rec->enc->wptr = rec_encbuf;
        rec->obuf = rec->wptr = rec->rptr = rec_obuf;
    }
    rec->len = 0;
    rec->src->source_start();
    rec->sta = REC_RECORDING;
    led_record();
    return true;
}

void record_var_init(void)
{
    memset(&rec_cb, 0, sizeof(rec_cb));
    memset(&rec_src, 0, sizeof(rec_src));
    rec_cb.first_flag = 1;
#if (REC_TYPE_SEL == REC_MP3)
    mpaen_var_init();
#endif
}

AT(.text.func.record)
static void sfunc_record_enter(void)
{
    if (dev_is_online(DEV_UDISK)) {
        sys_cb.cur_dev = DEV_UDISK;
    } else if (dev_is_online(DEV_SDCARD1)) {
        sys_cb.cur_dev = DEV_SDCARD1;
    } else if (dev_is_online(DEV_SDCARD)) {
        sys_cb.cur_dev = DEV_SDCARD;
    } else {
        return;
    }
    if (!sfunc_record_start()) {
        if (sfunc_record_switch_device()) {
            sfunc_record_start();
        }
    }
}

AT(.text.func.record)
static void sfunc_record_exit(void)
{
    if (rec_cb.sta != REC_PAUSE) {
        sfunc_record_stop();
        rec_cb.flag_play = 1;
    }
    led_idle();

#if REC_STOP_MUTE_1S
    if (!sys_cb.mute) {  //有客户需要在退出录音时, 静音1S.
         bsp_sys_mute();
         delay_5ms(200);
         bsp_sys_unmute();
    }
#endif
}

AT(.text.func.record)
void sfunc_record(void)
{
//    printf("%s\n", __func__);
    sfunc_record_enter();

    while (rec_cb.sta == REC_RECORDING) {
        sfunc_record_process();
        sfunc_record_message(msg_dequeue());
        sfunc_record_display();
    }

    sfunc_record_exit();
}
#endif //FUNC_REC_EN
