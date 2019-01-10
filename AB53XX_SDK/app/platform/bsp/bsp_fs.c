#include "include.h"

#if FUNC_MUSIC_EN
extern u8 fname_buf[100];

//#define MUSIC_EXT_NUM 6                         //一共搜索3个扩展名
//const char tbl_music_ext[MUSIC_EXT_NUM][4] = {"mp3", "wav", "wma", "ape", "fla", "sbc"};

//转换为小写字符
AT(.text.fs.scan)
unsigned char char_tolower(unsigned char c)
{
	if (c >= 'A' && c <= 'Z') {
		c -= 'A'-'a';
	}
	return c;
}

//将字符串转换为小写
AT(.text.fs.scan)
void str_tolower(char *str, uint len)
{
    uint i;
    for (i = 0; i < len; i++) {
        str[i] = char_tolower(str[i]);
    }
}

AT(.text.fs.scan)
static bool file_is_music(char *ext_name)
{
//    u8 i;
//    for (i=0; i != MUSIC_EXT_NUM; i++) {
//        if (0 == memcmp(ext_name, (char *)(tbl_music_ext[i]), 3)) {
//            return true;
//        }
//    }
//    return false;

    if (0 == memcmp(ext_name, "mp3", 3)) {
        return true;
    }
#if MUSIC_WAV_SUPPORT
    if (xcfg_cb.music_wav_support) {
        if (0 == memcmp(ext_name, "wav", 3)) {
            return true;
        }
    }
#endif // MUSIC_WAV_SUPPORT

#if MUSIC_WMA_SUPPORT
    if (xcfg_cb.music_wma_support) {
        if (0 == memcmp(ext_name, "wma", 3)) {
            return true;
        }
    }
#endif // MUSIC_WMA_SUPPORT

#if MUSIC_APE_SUPPORT
    if (xcfg_cb.music_ape_support) {
        if (0 == memcmp(ext_name, "ape", 3)) {
            return true;
        }
    }
#endif // MUSIC_APE_SUPPORT

#if MUSIC_FLAC_SUPPORT
    if (xcfg_cb.music_flac_support) {
        if (0 == memcmp(ext_name, "fla", 3)) {
            return true;
        }
    }
#endif // MUSIC_FLAC_SUPPORT

#if MUSIC_SBC_SUPPORT
    if (xcfg_cb.music_sbc_support) {
        if (0 == memcmp(ext_name, "sbc", 3)) {
            return true;
        }
    }
#endif // MUSIC_SBC_SUPPORT
    return false;
}

///返回值： 0->不匹配的文件类型， 1->匹配的文件类型
AT(.text.fs.scan)
u8 music_file_filter(void)
{
    char extension[3];
    fs_get_fname_extension(extension);
    str_tolower(extension, 3);
    if (file_is_music(extension)) {
#if REC_FAST_PLAY
        bsp_update_final_rec_file_num();
#endif // REC_FAST_PLAY
        return 1;
    }
    return 0;
}

///返回值： 0->不匹配的目录类型， 1->匹配的目录类型
AT(.text.fs.scan)
u8 music_dir_filter(void)
{
#if REC_FAST_PLAY
    char sfn[13];
    fs_get_short_fname(sfn, 1);

    //录音目录
    if (is_record_dir(sfn)) {
        if (f_msc.rec_scan & BIT(0)) {
            f_msc.rec_scan |= BIT(1);
        }
    } else {
        f_msc.rec_scan &= ~BIT(1);
    }
#endif // REC_FAST_PLAY

    return 1;
}

#if MUSIC_REC_FILE_FILTER
///只播放录音文件歌曲
u8 music_only_record_dir_filter(void)
{
    char sfn[13];
    fs_get_short_fname(sfn, 1);

    //录音目录
    if (is_record_dir(sfn)) {
#if REC_FAST_PLAY
        if (f_msc.rec_scan & BIT(0)) {
            f_msc.rec_scan |= BIT(1);
        }
#endif // REC_FAST_PLAY
        return 1;
    }
    return 0;
}

///只播放录音文件歌曲
u8 music_only_record_file_filter(void)
{
    char extension[3];
    fs_get_fname_extension(extension);
    str_tolower(extension, 3);
    if (file_is_music(extension) && fs_get_dir_depth()) {   //去掉根目录正常歌曲
#if REC_FAST_PLAY
        bsp_update_final_rec_file_num();
#endif // REC_FAST_PLAY
        return 1;
    }
    return 0;
}

///去掉录音文件
u8 music_rm_record_dir_filter(void)
{
    char sfn[13];
    fs_get_short_fname(sfn, 1);

    //录音目录
    if (is_record_dir(sfn)) {
        return 0;
    }

    return 1;
}
#endif // MUSIC_REC_FILE_FILTER

#if REC_FAST_PLAY
///扫描指定文件名的编号
//u8 music_num_file_filter(void)
//{
//    char extension[3], sfn[13];
//    u8 sfn_len = 0;
//    fs_get_fname_extension(extension);
//    str_tolower(extension, 3);
//    if (file_is_music(extension)) {
//        fs_get_short_fname(sfn, 1);
//        sfn_len = strlen(sfn);
//        str_tolower(&sfn[sfn_len-3], 3);
//        if (strstr((char *)fname_buf, sfn) != NULL) {
//            return 2;
//        } else {
//            return 1;
//        }
//    }
//    return 0;
//}

void bsp_update_final_rec_file_num(void)
{
    if (fs_get_dir_depth() && (f_msc.rec_scan & BIT(1))) {
        if (fs_get_ftime() > sys_cb.ftime) {
            sys_cb.ftime = fs_get_ftime();
            sys_cb.rec_num = fs_get_file_count();
        }
    }
}
#endif // REC_FAST_PLAY

AT(.com_text.stream)
int stream_read(void *buf, unsigned int size)
{
    UINT len;
    u8 res = fs_read(buf, size, &len);
    if (res == FR_OK) {
#if MUSIC_ENCRYPT_EN
        if (f_msc.encrypt) {
            music_stream_decrypt(buf, len);
        }
#endif
        return len;
    } else {
        return -1;
    }
}

AT(.com_text.stream)
bool stream_seek(unsigned int ofs, int whence)
{
#if MUSIC_ENCRYPT_EN
    if (f_msc.encrypt) {
        if (whence == SEEK_SET) {
            ofs += 1;
        }
    }
#endif
    u8 res = fs_lseek(ofs, whence);
    if (res == FR_OK) {
        return true;
    }
    return false;
}

#endif // FUNC_MUSIC_EN
