#include "include.h"

#if (MUSIC_REC_FILE_FILTER || REC_FAST_PLAY)
AT(.text.bsp.record)
bool is_mic_record_dir(char *sfn)
{
#if REC_DIR_LFN
    u8 fs_type = fs_get_type();
    if (fs_type == FS_EXFAT) {
        if (!memcmp(sfn, dir_mic_sfn_exfat, strlen(dir_mic_sfn_exfat))) {
            return true;
        }
    } else {
        if (!memcmp(sfn, dir_mic_sfn, strlen(dir_mic_sfn))) {
            return true;
        }
    }
#else
    if (!memcmp(sfn, dir_path_mic, strlen(dir_path_mic))) {
        return true;
    }
#endif // REC_DIR_LFN
    return false;
}

AT(.text.bsp.record)
bool is_fm_record_dir(char *sfn)
{
#if REC_DIR_LFN
    u8 fs_type = fs_get_type();
    if (fs_type == FS_EXFAT) {
        if (!memcmp(sfn, dir_fm_sfn_exfat, strlen(dir_fm_sfn_exfat))) {
            return true;
        }
    } else {
        if (!memcmp(sfn, dir_fm_sfn, strlen(dir_fm_sfn))) {
            return true;
        }
    }
#else
    if (!memcmp(sfn, dir_path_fm, strlen(dir_path_fm))) {
        return true;
    }
#endif // REC_DIR_LFN
    return false;
}

AT(.text.bsp.record)
bool is_aux_record_dir(char *sfn)
{
#if REC_DIR_LFN
    u8 fs_type = fs_get_type();
    if (fs_type == FS_EXFAT) {
        if (!memcmp(sfn, dir_aux_sfn_exfat, strlen(dir_aux_sfn_exfat))) {
            return true;
        }
    } else {
        if (!memcmp(sfn, dir_aux_sfn, strlen(dir_aux_sfn))) {
            return true;
        }
    }
#else
    if (!memcmp(sfn, dir_path_aux, strlen(dir_path_aux))) {
        return true;
    }
#endif // REC_DIR_LFN
    return false;
}

AT(.text.bsp.record)
bool is_bt_record_dir(char *sfn)
{
#if REC_DIR_LFN
    u8 fs_type = fs_get_type();
    if (fs_type == FS_EXFAT) {
        if (!memcmp(sfn, dir_bt_sfn_exfat, strlen(dir_bt_sfn_exfat))) {
            return true;
        }
    } else {
        if (!memcmp(sfn, dir_bt_sfn, strlen(dir_bt_sfn))) {
            return true;
        }
    }
#else
    if (!memcmp(sfn, dir_path_bt, strlen(dir_path_bt))) {
        return true;
    }
#endif // REC_DIR_LFN
    return false;
}

//ÅÐ¶ÏÊÇ·ñÂ¼ÒôÎÄ¼þ¼Ð
AT(.text.bsp.record)
bool is_record_dir(char *sfn)
{
#if REC_ONE_FOLDER_EN
    if (!memcmp(sfn, dir_path_rec, strlen(dir_path_rec))) {
        return true;
    }
    return false;
#else

#if REC_DIR_LFN
    //Â¼Òô¸ùÄ¿Â¼
    if (!memcmp(sfn, root_dir_path, strlen(root_dir_path))) {
        return true;
    }
#endif // REC_DIR_LFN

    //Â¼Òô×ÓÄ¿Â¼
    if (is_mic_record_dir(sfn) || is_fm_record_dir(sfn) || is_aux_record_dir(sfn) || is_bt_record_dir(sfn)) {
        return true;
    }
    return false;
#endif // REC_ONE_FOLDER_EN
}
#endif // MUSIC_REC_FILE_FILTER
