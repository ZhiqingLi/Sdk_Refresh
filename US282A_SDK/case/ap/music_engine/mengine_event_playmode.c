#include "app_mengine.h"

//for id3 use
#ifdef MENGINE_ID3_SUPPORT

#define MusicExtNum 7

//定义所有后缀名
const uint8 support_ext[MusicExtNum][4] =
{
    "MP1", //mp3
    "MP2",//mp3
    "MP3",//mp3
    "WMA",//wma
    "APE",//ape
    "FLA",//flac
    "OGG",//ogg
};

const id3_type_e support_id3_type[] =
{
    ID3_TYPE_MP3, //mp1
    ID3_TYPE_MP3,//mp2
    ID3_TYPE_MP3,//mp3
    ID3_TYPE_WMA,//wma
    ID3_TYPE_APE,//ape
    ID3_TYPE_FLAC,//fla
    ID3_TYPE_OGG,//ogg
};

//得到ID3的类型
id3_type_e mengine_check_music_type(uint8 * extname)
{
    uint8 cnt;
    id3_type_e ret = ID3_TYPE_END;
    //转换当前后缀名(转成大写)
    extname[0] = ((extname[0] >= 'a') && (extname[0] <= 'z')) ? (extname[0] - 'a' + 'A') : extname[0];
    extname[1] = ((extname[1] >= 'a') && (extname[1] <= 'z')) ? (extname[1] - 'a' + 'A') : extname[1];
    extname[2] = ((extname[2] >= 'a') && (extname[2] <= 'z')) ? (extname[2] - 'a' + 'A') : extname[2];
    //比较后缀
    for (cnt = 0; cnt < MusicExtNum; cnt++)
    {
        if (libc_strncmp(extname, support_ext[cnt], sizeof(support_ext[cnt])) == 0)
        {
            ret = support_id3_type[cnt];
            break;
        }
    }
    return ret;
}

//初始化ID3的相关buffer, 并将这些buffer的指针存到g_id3_info里面
void mengine_init_id3buf(void)
{
    //标题buffer
    libc_memset(g_title_buf, 0, sizeof(g_title_buf));

    g_id3_info.tit2_buffer = g_title_buf;
    //歌手buffer和长度
    libc_memset(g_artist_buf, 0, sizeof(g_artist_buf));

    g_id3_info.tpe1_buffer = g_artist_buf;
    //专辑buffer
    libc_memset(g_album_buf, 0, sizeof(g_album_buf));

    g_id3_info.talb_buffer = g_album_buf;
    //流派buffer
    g_id3_info.genre_buffer = NULL;
    //drm的buffer
    g_id3_info.drm_buffer = NULL;
    //音轨buffer
    g_id3_info.track_buffer = NULL;

    //文件标题长度
    g_id3_info.tit2_length = Id3BufSIZE;
    //作者长度
    g_id3_info.tpe1_length = ArtBufSIZE;
    //专辑长度
    g_id3_info.talb_length = ArtBufSIZE;
    //流派长度
    g_id3_info.genre_length = 0;
    //drm长度
    g_id3_info.drm_length = 0;
    //音轨长度
    g_id3_info.track_length = 0;
}

//获取id3信息(结果存到g_id3_info)
//path_info: 文件的路径信息
void mengine_getid3_done(musfile_location_u* path_info)
{
    id3_type_e music_type;
    //bool have_no_id3 = FALSE;
    uint8* nameptr = path_info->dirlocation.file_info.file_extend_info.file_ext;

    //初始化变量
    mengine_init_id3buf();

    //获取当前文件类型
    music_type = mengine_check_music_type(nameptr);

    //获取id3信息
    fsel_get_id3_info(&g_id3_info, NULL, music_type);

    //处理id3超长等
    //play_scene_deal_id3();
}

//回复客户端：id3信息
app_result_e mengine_getid3_info(void* msg_ptr)
{
    //设置播放类型的指针
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;

    id3_info_t* dest_ptr = (id3_info_t*)data_ptr->reply->content;

    //copy //标题 info
    libc_memcpy(dest_ptr->tit2_buffer, g_title_buf,
            (unsigned int)dest_ptr->tit2_length);
    //copy //artist info
    libc_memcpy((uint8*)(dest_ptr->tpe1_buffer), g_artist_buf,
            (unsigned int)dest_ptr->tpe1_length);
    //copy //album info
    libc_memcpy((uint8*)(dest_ptr->talb_buffer), g_album_buf,
            (unsigned int)dest_ptr->talb_length);
    //返回成功
    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}

#endif //#ifdef MENGINE_ID3_SUPPORT

#ifdef DIR_NAME_SUPPORT

//获取目录名
app_result_e mengine_get_dirname(void* msg_ptr)
{
    //设置播放类型的指针
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;

    uint8* dest_ptr = (uint8*)data_ptr->msg.content.addr;
    //dir name info
    libc_memcpy(dest_ptr,g_dir_name_buf,sizeof(g_dir_name_buf));

    //返回成功
    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}

#endif

//设置快进退的步长
app_result_e mengine_fastplay_step(void *msg_ptr)
{
    //消息指针
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;
    //速度参数 毫秒为单位
    uint32 *speed_ptr = (uint32 *) (data_ptr->msg.content.addr);

    //是正常播放 or 快进
    if ((g_eg_status_p->play_status == PlaySta) || (g_eg_status_p->play_status == PlayFast))
    {
        mmm_mp_cmd(g_mp_handle, MMM_MP_FFB_STEP, (unsigned int) (*speed_ptr)); //快进
    }

    //返回成功
    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}

//快进
app_result_e mengine_fast_forward(void* msg_ptr)
{
    //是正常播放 or 快进
    if ((g_eg_status_p->play_status == PlaySta) || (g_eg_status_p->play_status == PlayFast))
    {
        mmm_mp_cmd(g_mp_handle, MMM_MP_SET_FFB, (unsigned int) 4); //快进
        g_eg_status_p->play_status = PlayFast; //修改状态
        g_eg_status_p->fast_status = FFPlaySta;
    }
    //返回成功
    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}

//快退
app_result_e mengine_fast_backward(void* msg_ptr)
{
    //当前是正常播放 or 快退
    if ((g_eg_status_p->play_status == PlaySta) || (g_eg_status_p->play_status == PlayFast))
    {
        mmm_mp_cmd(g_mp_handle, MMM_MP_SET_FFB, (unsigned int) -4); //快退
        g_eg_status_p->play_status = PlayFast; //修改状态
        g_eg_status_p->fast_status = FBPlaySta;
    }

    //返回成功
    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}

//取消快进快退
app_result_e mengine_cancel_ffb(void* msg_ptr)
{
    //当前是快进退状态
    if ((g_eg_status_p->play_status == PlayFast) && (g_eg_status_p->fast_status != FBPlay_Null))
    {
        mmm_mp_cmd(g_mp_handle, MMM_MP_SET_FFB, (unsigned int) 0); //取消快进退
        g_eg_status_p->play_status = PlaySta; //修改状态
        g_eg_status_p->fast_status = FBPlay_Null;
    }

    //返回成功
    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}

#ifdef AB_SUPPORT

//启动ab复读,等待设置a点
app_result_e mengine_start_ab(void* msg_ptr)
{
    //mengine_status_t *eg_status = &mengine_info.eg_status;
    if (g_eg_status_p->play_status == PlaySta)
    {
        g_eg_status_p->ab_status = PlayAB_A; //等待设置A点
        g_eg_status_p->play_status = PlayAB;

        //设置复读次数到解码器
        //mmm_mp_cmd(g_mp_handle, MMM_MP_AB_COUNT, (unsigned int)(g_eg_cfg_p->replaytimes));

        //设置复读间隔到解码器  10ms为单位
        //mmm_mp_cmd(g_mp_handle, MMM_MP_AB_WAIT_TIME, (unsigned int) (g_eg_cfg_p->replaygap * 100));
    }

    //返回成功
    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}

//设置a点
app_result_e mengine_set_apoint(void* msg_ptr)
{
    //mengine_status_t *eg_status = &mengine_info.eg_status;
    g_eg_status_p->ab_status = PlayAB_B; //等待设置B点
    g_eg_status_p->play_status = PlayAB;
    mmm_mp_cmd(g_mp_handle, MMM_MP_SET_A, (unsigned int) NULL); //发送设置A点命令

    //返回成功
    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}

//设置b点,开始复读
app_result_e mengine_set_bpoint(void* msg_ptr)
{
    //mengine_status_t *eg_status = &mengine_info.eg_status;
    g_eg_status_p->ab_status = PlayAB_Do; //开始ab复读
    g_eg_status_p->play_status = PlayAB; //设置播放状态为AB复读
    mmm_mp_cmd(g_mp_handle, MMM_MP_SET_B, (unsigned int) NULL); //发送设置B点命令

    //返回成功
    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}

//清除ab复读
app_result_e mengine_clear_ab(void* msg_ptr)
{
    //mengine_status_t *eg_status = &mengine_info.eg_status;
    g_eg_status_p->ab_status = PlayAB_Null; //清除AB复读状态
    g_eg_status_p->play_status = PlaySta; //恢复正常播放状态
    mmm_mp_cmd(g_mp_handle, MMM_MP_CLEAR_AB, (unsigned int) NULL); //发送清除AB复读命令

    //返回成功
    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}

//设置ab复读次数
app_result_e mengine_set_ab_count(void* msg_ptr)
{
    //设置复读次数的指针
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;
    uint16 rep_count;
    //获取当前复读次数
    rep_count = *(uint16*) (data_ptr->msg.content.addr);
    //mengine_info.eg_config.replaytimes = rep_count;//保存复读次数
    //设置复读次数到解码器
    mmm_mp_cmd(g_mp_handle, MMM_MP_AB_COUNT, (unsigned int) rep_count);

    //返回成功
    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}

//设置ab复读间隔时间
app_result_e mengine_set_ab_gap(void* msg_ptr)
{
    //设置复读间隔的指针
    private_msg_t* data_ptr = (private_msg_t*) msg_ptr;
    uint8 ab_gap;
    ab_gap = *(uint8*) (data_ptr->msg.content.addr); //获取当前复读间隔
    //mengine_info.eg_config.replaygap = ab_gap;///保存复读间隔到引擎变量
    //设置复读间隔到解码器  10ms为单位
    mmm_mp_cmd(g_mp_handle, MMM_MP_AB_WAIT_TIME, (unsigned int) ab_gap * 100);

    //返回成功
    mengine_reply_msg(msg_ptr, TRUE);

    return RESULT_IGNORE;
}
#endif //#ifdef AB_SUPPORT
