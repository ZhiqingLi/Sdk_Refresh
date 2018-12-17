#include "music_rcp_dir_get.h"

//这里的定义要跟xn里面的一样
#define MUSIC_RCP_DEAL_GET_XDATA_LEN    (512 + 16)
#define PAY_LOAD_LEN    (MUSIC_RCP_DEAL_GET_XDATA_LEN - RCP_COMMAND_HEAD_SIZE)
static uint8 music_rcp_dir_buffer[PAY_LOAD_LEN + RCP_COMMAND_HEAD_SIZE] _BANK_DATA_ATTR_;
uint8 g_dirname_buf[71];   //目录名字buff

static int8 s_timer_id_rcp_getdlist = -1;
const char root_dir_name[4] = {'R','O','O','T'};	//根目录名字
music_dir_info_t g_cur_dir_info;    //目录信息info

void tmp_print_dir_name(void)
{
    uint8 i;
    libc_print("dir name = ",0,0);
    libc_print("dir name len",g_cur_dir_info.name_len,2);
    for(i=0;i<g_cur_dir_info.name_len;i++)
    {
        libc_print("  -",g_dirname_buf[i],2);
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 *    初始化目录信息info
 * \param[in]    none
 * \param[out]   nonne
 * \return       none
 * \ingroup      rcp_dir_get 
 * \note
 *******************************************************************************/
void init_dir_info(void)
{
    libc_memset(g_dirname_buf, 0, sizeof(g_dirname_buf));
    g_cur_dir_info.pdir_buf= g_dirname_buf;
    g_cur_dir_info.name_len = sizeof(g_dirname_buf);
}

/******************************************************************************/
/*!
 * \par  Description:
 *    获取目录的编码类型
 * \param[in]    info 目录名字
 * \param[out]   none
 * \return       返回类型 看注释
 * \ingroup      rcp_dir_get 
 * \note
 *******************************************************************************/
static uint8 _music_dir_enctype(uint8 *info)
{
    uint8 code_type = 0; //ANSI
    if ((info[0] == 0xff) && (info[1] == 0xfe))//UTF16 LE
        {code_type = 1;}
    else if ((info[0] == 0xfe) && (info[1] == 0xff))//UTF16 BE
        {code_type = 2;}
    else if ((info[0] == 0xef) && (info[1] == 0xbb) && (info[1] == 0xbf))//UTF-8
        {code_type = 3;}
    else//do nothing
        {}
    return code_type;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    通过目录序号定位到目录项,并返回信息rcp_dir_info
 * \param[in]    dir_index 需要获取的目录序号(从1开始编号)
 * \param[out]   *rcp_dir_info 目录die info
 * \return       bool 成功返回TRUE 失败 FALSE
 * \ingroup      rcp_dir_get 
 * \note
 *******************************************************************************/
bool music_rcp_get_dir_list_info(music_dir_info_t *rcp_dir_info, uint16 dir_index)
{
    list_dir_info_t tmp_dir;
    list_dir_info_t entry;
    
//libc_print("get dir index = ",dir_index,2);
            
    if (!get_dir_by_dirno(dir_index , &g_cur_dir))//找到这个目录信息 dir_index
    {
        PRINT_ERR_STR("!!vram have not dir!!");//这个目录超出了记录(vram里面没有记录它) 
        return FALSE;
    }

    if(g_last_dir_no == 0)//music scan刚结束,第一次获取，当前目录在第一个目录
    {
        libc_print("first scan get",0,0);
        g_cur_dir_no = g_last_dir_no = 1;//当前和上一次目录都在第一个目录
    }

    if(g_cur_dir.cur_layer == 0)//当前目录在根目录ROOT
    {
        libc_memcpy(g_cur_dir_info.pdir_buf, &root_dir_name,sizeof(root_dir_name));
    }
    else
    {
        libc_memcpy(&entry, &g_cur_dir, sizeof(entry));
        //跳到目录
        if(dir_index != g_cur_dir_no)//在当前目录，不需要跳转
        {
            if(!vfs_jump_to_direntry(eh_vfs_mount,
                    0, &g_cur_dir, &entry))//((prev_flag == TRUE) ? PREV_FLAG : 0)
            {
                PRINT_ERR_STR("!!vfs jump FALSE!!");
                return FALSE;
            }    
        }
        g_cur_dir_no = g_last_dir_no = dir_index;
        //CD到父目录
        if (vfs_cd(eh_vfs_mount, CD_BACK, NULL) == FALSE)
        {
            PRINT_ERR_STR("!!vfs CD BACK FALSE!!");//回不到父目录，肯定是出错了！
            return FALSE;
        }
        //获取目录名
        if(vfs_get_name(eh_vfs_mount, rcp_dir_info->pdir_buf, rcp_dir_info->name_len / 2)==0)
        {
        	  PRINT_ERR_STR("!!vfs name FALSE!!");//出错了！
            return FALSE; 
        }
        //tmp_print_dir_name();
        vfs_cd(eh_vfs_mount, CD_SUB, NULL);//不CD回原来目录有可能出错
    }
    rcp_dir_info->begin_index = g_cur_dir.begin_index;  //起始曲目号
//libc_print("begin_file = ",g_cur_dir.begin_index,2);
    if( g_vmdir_total >= (dir_index+1) ) //下个目录小于目录总数
    {
        get_dir_by_dirno(dir_index+1 , &tmp_dir);  //获取下个目录项表
        rcp_dir_info->end_index = tmp_dir.begin_index-1;    //结束曲目号
    }
    else
    {
        rcp_dir_info->end_index = g_vmfile_total;
    }
//libc_print("end_file = ",rcp_dir_info->end_index,2);   

    return TRUE;
}

/******************************************************************************/
/*!
 * \par  Description:
 *    更新list随机数
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      rcp_dir_get 
 * \note
 *******************************************************************************/
void update_dir_list_random(void)
{
    uint32 tmp_random_no;
    uint8 sys_random_no[4];
    libc_print("update dlist random",0,0);
    tmp_random_no = sys_random();//获取随机数
    sys_random_no[0] = (uint8)tmp_random_no;
    sys_random_no[1] = (uint8)(tmp_random_no>>8);
    sys_random_no[2] = (uint8)(tmp_random_no>>16);
    sys_random_no[3] = (uint8)(tmp_random_no>>24);
    sys_vm_write(&sys_random_no,VM_DIR_LIST,4);
}

/******************************************************************************/
/*!
 * \par  Description:
 *    获取小机卡上文件目录表状态
 * \param[in]    none
 * \param[out]   list_show_way 显示方式
 * \param[out]   dir_amount 目录总数
 * \param[out]   error_num 错误标志
 * \param[out]   random_number 随机数
 * \return       app_result_e RESULT_NULL
 * \ingroup      rcp_dir_get 
 * \note
 *******************************************************************************/
app_result_e music_get_dir_list_status(uint32 data1, uint32 data2)//app_result_e  uint32 data1, uint32 data2
{
    music_rcp_dir_list_status_t dir_list_staus;
    uint8 random_buf[sizeof(music_rcp_dlist_random_t) + RCP_COMMAND_HEAD_SIZE];
    uint8 *p_dir_list_staus = (uint8*)(random_buf + RCP_COMMAND_HEAD_SIZE);
    uint8 i;
    uint32 send_data1;

    /*if( (g_scan_handle != NULL) ||
        ( (g_app_info_state.card_state == 0) && (g_app_info_state.uhost_state == 0) ) )//没卡且没有U盘
    {
        //mus_scan在占用内存，id3没有内存执行
        //按照约定，app/apk不会在没有scan完时，进入这里的
        dir_list_staus.error_num = 0;
        libc_print("rcp error",0,0);
        com_rcp_send_pkg(0,  dir_list_staus.error_num, 0, 0, 0);
        return RESULT_NULL;
    }*/

    dir_list_staus.list_show_way = DIR_LIST_VIEW;//获取支持显示方式
    dir_list_staus.error_num = 1;//错误标志
    dir_list_staus.dir_amount = (uint8)g_vmdir_total;//获取目录总数

    sys_vm_read(&random_buf , VM_BTSTACK , 8);//读蓝牙地址
    for(i=0;i<6;i++)
    {
        random_buf[RCP_COMMAND_HEAD_SIZE + i] = random_buf[7-i];
    }
    sys_vm_read(p_dir_list_staus+6, VM_DIR_LIST , 4);//获取系统随机数
    
    if( (g_scan_handle != NULL) ||
        ( (g_app_info_state.card_state == 0) && (g_app_info_state.uhost_state == 0) ) )//没卡且没有U盘
    {
        //mus_scan在占用内存，id3没有内存执行
        //按照约定，app/apk不会在没有scan完时，进入这里的
        dir_list_staus.error_num = 0;
        libc_print("rcp error",0,0);
        //com_rcp_send_pkg(0,  dir_list_staus.error_num, 0, 0, 0);
        //return RESULT_NULL;
    }
    
    send_data1 =    (((uint32)dir_list_staus.list_show_way) <<24) |
                    (((uint32)dir_list_staus.dir_amount) <<16) |
                    (((uint32)dir_list_staus.error_num) <<8);
    libc_print("music get dir list status =",send_data1,2);
//libc_print("data len",sizeof(tmp_random_buf),2);
    //发送回复包
    com_rcp_send_pkg(send_data1,0,random_buf,sizeof(music_rcp_dlist_random_t),sizeof(music_rcp_dlist_random_t));
    return RESULT_NULL;
}

MUS_STATIC __section__(".text.bank2") void _get_dlist_over(void)
{
    adjust_freq_set_level(AP_FRONT_LOW_PRIO, FREQ_LEVEL7, FREQ_NULL);
    change_ui_status(UI_STATUS_IDLE);
    s_timer_id_rcp_getdlist = -1;
}

void reset_dir_list(void)
{
    vfs_cd(eh_vfs_mount, CD_ROOT, 0);       
    music_rcp_dir_first();
    music_rcp_first_get();

    if (g_file_path_info.file_path.dirlocation.disk == DISK_U)
    {
        g_app_info_state.need_update_playlist &= (uint8)(~UHOST_NEED_UPLOAD);
    }
    else
    {
        g_app_info_state.need_update_playlist &= (uint8)(~CARD_NEED_UPLOAD);
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 *    获取小机卡目录表信息
 * \param[in]    data1 app获取目录起始序号
 * \param[in]    data2 app获取目录个数
 * \param[out]   目录list包，包构成请查看具体设计文档
 * \return       app_result_e RESULT_NULL
 * \ingroup      rcp_dir_get 
 * \note
 *******************************************************************************/
__section__(".text.bank2") app_result_e music_get_music_dir_list(uint32 data1, uint32 data2)
{
    music_rcp_dir_list_info_t *p_rcp_dlist_info;
    uint8 *p_data_base = music_rcp_dir_buffer + RCP_COMMAND_HEAD_SIZE;

    uint32 tmp_dir_seq = data1;         //目录序号(从1开始)
    uint32 tmp_dir_count = data2;     //目录个数
    uint32 dlist_info_len, count_one_time, i; //数据包长度，数据包个数

//    libc_print("App get dir list dirseq",tmp_dir_seq,2);
//    libc_print("App get dir list dircount",tmp_dir_count,2);
    if(tmp_dir_seq>g_vmdir_total)//目录序号超出总数
    {return FALSE;}

    if(tmp_dir_seq == 0)    //错误编号处理,例:手机发dir_count=5，dir_seq=0，小机回目录表（1234）
    {
        tmp_dir_seq=1;
        tmp_dir_count-=1;
    }
    if( (tmp_dir_seq+tmp_dir_count-1)>g_vmdir_total )//获取的目录超出总数
    {
        tmp_dir_count = g_vmdir_total - tmp_dir_seq+1;
    }

#if ((SUPPORT_PLIST_TITLE == 1) || (SUPPORT_PLIST_ARTIST == 1)\
    || (SUPPORT_PLIST_ALBUM == 1) || (SUPPORT_PLIST_GENRE == 1))
    if(g_scan_handle != NULL)
    {
        //mus_scan在占用内存，id3没有内存执行
        //按照约定，app/apk不会在没有scan完时，进入这里的
        return RESULT_NULL;
    }
#endif

    if (eh_vfs_mount == -1)
    {return RESULT_NULL;}

    if(s_timer_id_rcp_getdlist == -1)
    {
        s_timer_id_rcp_getdlist = set_single_shot_app_timer(APP_TIMER_ATTRB_CONTROL | (APP_TIMER_TAG_SYS << 8),
                500, (timer_proc) _get_dlist_over);
        if(s_timer_id_rcp_getdlist == -1)
        {
            PRINT_ERR("s_timer_id_rcp_getplist err1!");
        }
        adjust_freq_set_level(AP_FRONT_LOW_PRIO, FREQ_LEVEL11, FREQ_NULL);
        change_ui_status(UI_STATUS_BUSY);
    }
    else
    {
        restart_app_timer(s_timer_id_rcp_getdlist);
    }

    //APK已经处理好更新播放列表标志，可以清除更新播放列表标志了
    if(tmp_dir_seq == 1)//从1开始,从根目录开始
    {
        reset_dir_list();//重新刷新播放列表，默认进入根目录
    }
    
    //起始曲目号+结束曲目号一共4个BYTE 目录名65+编码类型1 =66个Byte
    dlist_info_len = sizeof(music_rcp_dir_list_info_t);
    count_one_time = (sizeof(music_rcp_dir_buffer) - RCP_COMMAND_HEAD_SIZE) / dlist_info_len;//每次能发送的包个数 7个

    //循环获取/发送 目录list信息   
    for (i = 0; i < tmp_dir_count; i++)
    {
        p_rcp_dlist_info = (music_rcp_dir_list_info_t*) (p_data_base + dlist_info_len * (i % count_one_time));
  
        //获取目录list信息
        init_dir_info();        //初始化info
        if(!music_rcp_get_dir_list_info(&g_cur_dir_info, (uint16) tmp_dir_seq))//获取目录info
        {
            return RESULT_NULL;
        }
 
        //目录list信息组包
        libc_memcpy(p_rcp_dlist_info->dir_buffer, g_cur_dir_info.pdir_buf,
            MIN(g_cur_dir_info.name_len, sizeof(p_rcp_dlist_info->dir_buffer)));//目录名拷贝
        p_rcp_dlist_info->enctype = _music_dir_enctype(p_rcp_dlist_info->dir_buffer);    //获取编码类型
        libc_memcpy(&(p_rcp_dlist_info->begin_index), &(g_cur_dir_info.begin_index), sizeof(uint16));//起始曲目号
        libc_memcpy(&(p_rcp_dlist_info->end_index), &(g_cur_dir_info.end_index), sizeof(uint16));//结束曲目号
        

        //发送应答命令
        if (((i + 1) % count_one_time) == 0) //空间已满包个数，必须发送
        {
            if (((i + 1) / count_one_time) == 1)    //第一个包，发送应答
            {
                com_rcp_send_pkg((tmp_dir_seq-count_one_time), count_one_time,
                        music_rcp_dir_buffer, (uint16) (dlist_info_len * count_one_time),
                        (uint16) (dlist_info_len * tmp_dir_count));
            }
            else
            {
                com_rcp_send_pkg_cont(p_data_base, (uint16) (dlist_info_len * count_one_time));
            }
        }

        tmp_dir_seq++;
    }

    if ((tmp_dir_count % count_one_time) != 0)  //不满包个数
    {
        if (tmp_dir_count < count_one_time)    //请求目录个数小于包个数
        {
            com_rcp_send_pkg((tmp_dir_seq-tmp_dir_count), tmp_dir_count, music_rcp_dir_buffer,
                               (uint16)(dlist_info_len*tmp_dir_count),(uint16)(dlist_info_len * tmp_dir_count));
        }
        else    //最后一个包不满包个数
        {
            com_rcp_send_pkg_cont(p_data_base, (uint16) (dlist_info_len * (tmp_dir_count % count_one_time)));
        }
    }

    //本函数执行时间：随意选择40首歌, 81+78+120+120+92+112+80+81=764毫秒

    return RESULT_NULL;
}

