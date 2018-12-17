/********************************************************************************
 *        Copyright(c) 2014-2015 Actions (Zhuhai) Technology Co., Limited,
 *                            All Rights Reserved.
 *
 * 描述：APK/APP 闹铃加载接口，包括内置闹铃和自定义闹铃目录。
 * 作者：cailizhen
 ********************************************************************************/

#include "common_time_alarm.h"

#define GET_RING_ONE_TYPE      0
#define GET_RING_ALL_TYPE      1

uint8 ring_filename_type(uint8 *info)
{
    uint8 code_type = 0; //ANSI

    if ((info[0] == 0xff) && (info[1] == 0xfe))
    { //UTF16 LE
        code_type = 1;
    }
    else if ((info[0] == 0xfe) && (info[1] == 0xff))
    { //UTF16 BE
        code_type = 2;
    }
    else if ((info[0] == 0xef) && (info[1] == 0xbb) && (info[1] == 0xbf))
    { //UTF-8
        code_type = 3;
    }
    else
    {
        ; //do nothing
    }

    return code_type;
}

/*获取本地磁盘闹铃目录（类型）列表*/
app_result_e time_alarm_get_ring_folder(uint32 data1, uint32 data2)
{
    ring_folder_infor_t tmp_ring_folder;
    uint8 tmp_ring_folder_type[RCP_RING_FOLDER_MAX];
    int param, i;

    com_rcp_send_pkg(RCP_RING_FOLDER_MAX, 0, NULL, 0, sizeof(ring_folder_infor_t) * RCP_RING_FOLDER_MAX);

    com_load_share_data(COM_SHARE_DATA_RING_TYPE, tmp_ring_folder_type, RCP_RING_FOLDER_MAX, 0);

    for (i = 0; i < RCP_RING_FOLDER_MAX; i++)
    {
        tmp_ring_folder.ring_type = tmp_ring_folder_type[i];
        tmp_ring_folder.code_type = 0;
        param = (0 << 8) | i; //load short name
        com_load_share_data(COM_SHARE_DATA_RING_FOLDER_SUB, tmp_ring_folder.folder_name, 12, param);

        com_rcp_send_pkg_cont(&tmp_ring_folder, sizeof(ring_folder_infor_t));
    }

    return RESULT_NULL;
}

/*组织并发送内置闹铃列表*/
void time_alarm_builtin_ring_list(uint8 mode)
{
    ring_song_t tmp_ring_song;
    uint8 total_num;
    uint8 i;

    //cal all rings
    total_num = BUILDIN_RING_MAX;

    if (mode == GET_RING_ALL_TYPE)
    {
#if (SUPPORT_RING_FOLDER == 1)
        uint8 tmp_ring_foldernum[RCP_RING_FOLDER_MAX];

        time_alarm_record_list_operate(RECORD_LIST_READ_FOLDERNUM, tmp_ring_foldernum, 0);
        //闹铃目录
        for (i = 0; i < RCP_RING_FOLDER_MAX; i++)
        {
            total_num += tmp_ring_foldernum[i];
        }
#endif
    }

    com_rcp_send_pkg(total_num, 0, NULL, 0, sizeof(ring_song_t) * total_num);
    //先发送内置铃声，如果是 GET_RING_ALL_TYPE 模式，后面将接着发送其他类型铃声
    for (i = 0; i < BUILDIN_RING_MAX; i++)
    {
        tmp_ring_song.ring_type = RING_TYPE_BUILTIN;
        tmp_ring_song.code_type = 0;

        com_load_share_data(COM_SHARE_DATA_RING_BUILDIN, tmp_ring_song.song_name, 12, i);

        com_rcp_send_pkg_cont(&tmp_ring_song, sizeof(ring_song_t));
    }
}

/*组织并发送目录闹铃列表*/
void time_alarm_folder_ring_list(uint8 index, uint8 source_type, uint8 mode)
{
    ring_song_t tmp_ring_song;
    uint8 total_count, total_index, i;
    uint8 tmp_ring_foldernum[RCP_RING_FOLDER_MAX];
    uint32 vm_read_addr;
    int tmp_page, tmp_page_offset;

    time_alarm_record_list_operate(RECORD_LIST_READ_FOLDERNUM, tmp_ring_foldernum, 0);

    //song count
    total_count = tmp_ring_foldernum[index];
    total_index = 0;
    for (i = 0; i < index; i++)
    {
        total_index += tmp_ring_foldernum[i];
    }

    if (mode == GET_RING_ONE_TYPE)
    { //如果是 GET_RING_ALL_TYPE 模式，那么在 tools_builtin_ring_list 发送命令头
        com_rcp_send_pkg(total_count, 0, NULL, 0, sizeof(ring_song_t) * total_count);
    }

    for (i = 0; i < total_count; i++)
    {
        tmp_page = (total_index + i) / RING_FOLDER_FILENUM_PER_SEC;
        tmp_page_offset = (total_index + i) % RING_FOLDER_FILENUM_PER_SEC;

        vm_read_addr = (uint32) (VM_RING_FOLDER_BASE + tmp_page * 0x10000 + tmp_page_offset * RING_FOLDER_FILENAME_LEN);
        sys_vm_read(tmp_ring_song.song_name, vm_read_addr, RING_FOLDER_FILENAME_LEN);

        tmp_ring_song.ring_type = source_type;
        tmp_ring_song.code_type = ring_filename_type(tmp_ring_song.song_name);

        //sequencial packet
        com_rcp_send_pkg_cont(&tmp_ring_song, sizeof(ring_song_t));
    }
}

/*获取闹铃文件列表*/
app_result_e time_alarm_get_ring_list(uint32 data1, uint32 data2)
{
    ring_type_e ring_type = (ring_type_e) data1;
    uint8 i;

    //如果卡已拔出，那么将卡闹铃目录数目清0
#if (SUPPORT_RING_FOLDER == 1)
    if (get_card_state() == CARD_STATE_CARD_NULL)
    {
        uint8 tmp_ring_foldernum[RCP_RING_FOLDER_MAX];

        libc_memset(tmp_ring_foldernum, 0, RCP_RING_FOLDER_MAX);
        time_alarm_record_list_operate(RECORD_LIST_WRITE_FOLDERNUM, tmp_ring_foldernum, 0);
    }
#endif

    switch (ring_type)
    {
        case RING_TYPE_BUILTIN: //内置闹铃
        time_alarm_builtin_ring_list(GET_RING_ONE_TYPE);
        break;

#if (SUPPORT_RING_FOLDER == 1)
        case RING_TYPE_FOLDER1: //第一个目录
        case RING_TYPE_FOLDER2: //第二个目录
        case RING_TYPE_FOLDER3: //第三个目录
        case RING_TYPE_FOLDER4: //第四个目录
        {
            uint8 index = (uint8) (ring_type - RING_TYPE_FOLDER1);
            time_alarm_folder_ring_list(index, RING_TYPE_FOLDER1 + index, GET_RING_ONE_TYPE);
        }
        break;
#endif

        default:
        time_alarm_builtin_ring_list(GET_RING_ALL_TYPE);
#if (SUPPORT_RING_FOLDER == 1)
        for (i = 0; i < RCP_RING_FOLDER_MAX; i++)
        {
            time_alarm_folder_ring_list(i, RING_TYPE_FOLDER1 + i, GET_RING_ALL_TYPE);
        }
#endif
        break;
    }

    return RESULT_NULL;
}

app_result_e alarm_alarm_snooze(uint32 data1, uint32 data2)
{
    g_rcp_ctrl_quit_flag = TRUE;
    return RESULT_ALARM_SNOOZE;
}

app_result_e alarm_alarm_close(uint32 data1, uint32 data2)
{
    g_rcp_ctrl_quit_flag = TRUE;
    return RESULT_ALARM_CLOSE;
}

