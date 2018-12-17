/*******************************************************************************
 *                              US212A
 *                            Module: MainMenu
 *                 Copyright(c) 2003-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       zhangxs     2011-09-05 10:00     1.0             build this file
 *******************************************************************************/
/*!
 * \file     mainmenu_main.c
 * \brief    MainMenu的main函数模块
 * \author   zhangxs
 * \version  1.0
 * \date  2011/9/05
 *******************************************************************************/
#include "ap_upgrade.h"

typedef struct
{
    uint16 kt_file_size;
    uint16 kt_offset;
    uint16 kt_frame_size;
    uint32 kt_compact;
} kt_play_info_t;

kt_play_info_t kt_play_info;

uint8 *bitstream_buffer;

const uint8 keytone_pcm[] = "keytone.pcm";

static const uint32 kt_gain_comp_abs[VOLUME_VALUE_MAX + 1] =
{
    0x00000fec, 0x00000fec, 0x00000fec, 0x00000fec, 0x00000fec,
    0x00000fec, 0x00000fec, 0x00000fec, 0x00000fec, 0x00000fec,
    0x00000fec, 0x00000fec, 0x00000fec, 0x00000fec, 0x00000fec,
    0x00000fec, 0x00000fec, 0x00000fec, 0x00000fec, 0x00000e31,
    0x00000ca6, 0x00000b46, 0x00000a0c, 0x000008f4, 0x000007fb,
    0x0000071c, 0x00000656, 0x000005a6, 0x00000509, 0x0000047c,
    0x00000400, 0x00000400
};

/**
LED INIT
*/
void led_init_spio(void)
{
    act_writel(0x00000000,DEBUGSIOIE);
    act_writel(0x00000000,DEBUGSIOOE);
    act_writel(act_readl(MFP_CTL3) | (uint32) (1 << 4), MFP_CTL3);
    act_writel(act_readl(MFP_CTL3) | (uint32) (1 << 5), MFP_CTL3);
    act_writel(0x0c,SIO_DAT);
}

/**
LED ON
*/
void led_on_spio(void)
{
    act_writel(act_readl(SIO_OUTEN) | (uint32) (1 << 2), SIO_OUTEN);
    act_writel(act_readl(SIO_OUTEN) | (uint32) (1 << 3), SIO_OUTEN);
}


/**
LED OFF
*/
void led_off_spio(void)
{
    act_writel(act_readl(SIO_OUTEN) & (~(uint32) (1 << 2)), SIO_OUTEN);
    act_writel(act_readl(SIO_OUTEN) & (~(uint32) (1 << 3)), SIO_OUTEN);
}

void __section__(".bank") keytone_play_init(void)
{
    sd_handle kt_handle;
    uint32 file_size;
    uint8 cur_vol;
    
#if ((CASE_BOARD_TYPE == CASE_BOARD_ATS2823) || (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2823))
    led_init_spio();
    led_on_spio();
    return;
#endif     

    kt_handle = sys_sd_fopen(keytone_pcm);
    if (kt_handle == NULL)
    {
        //open fail
        return;
    }

    file_size = kt_handle->endaddr - kt_handle->startaddr + 1;

    if(file_size > UPGRADE_KEYTONE_LEN)
    {
        file_size = UPGRADE_KEYTONE_LEN;
    }

    sys_sd_fread(kt_handle, UPGRADE_KEYTONE_BUFFER, file_size);

    sys_sd_fclose(kt_handle);

    bitstream_buffer = (uint8 *)UPGRADE_KEYTONE_BUFFER;

    //真实的PCM数据长度
    kt_play_info.kt_file_size = *(uint32 *)bitstream_buffer;
    //前四个字节数据清0
    *(uint32 *)bitstream_buffer = 0;
    kt_play_info.kt_offset = 0;

    enable_dac(2, 0); //channel out0,asrc out

    config_asrc(K_OUT0_U0, 0);//URAM0 = 2KB

    set_asrc_rate(22, 0, 5000);

    cur_vol=31;

    set_pa_volume(cur_vol, 0xbf); //0xbf表示DA音量0db

    kt_play_info.kt_frame_size = 512;
    kt_play_info.kt_offset = 0;
    if(cur_vol < 15)
    {
        kt_play_info.kt_compact=kt_gain_comp_abs[31-cur_vol-6];
    }
    else
    {
        kt_play_info.kt_compact=0x400;
    }
    return;
}
/******************************************************************************/
/*!
 * \par  Description:
 * \void keytone_send_to_asrc(uint32 data_count)
 * \read decoder frame data
 * \param[in]    data_count send data number
 * \param[in]
 * \param[out]   none
 * \return
 * \ingroup
 * \note
 */
/*******************************************************************************/
static void keytone_send_to_asrc(kt_play_info_t *p_kt_play_info, uint8 keytone_mode)
{
    uint32 data_count = p_kt_play_info->kt_frame_size;
    //save data to fifo
    uint32 temp_data;
    //temp use
    long long shift_use;
    //position to buffer
    uint32 buf_ptr = p_kt_play_info->kt_offset - p_kt_play_info->kt_frame_size;
    int16 chanel_data;
    uint8 check_ret;

    while (data_count > 0)
    {
        if (keytone_mode == 0)
        {
            check_ret = act_readl(ASRC_OUT0_NUM);
        }
        else
        {
            check_ret = act_readl(ASRC_OUT1_NUM);
        }

        if (check_ret != 0)
        {
            temp_data = *(uint32 *) &bitstream_buffer[buf_ptr];
            chanel_data = (int16) temp_data;
            shift_use = (long long) chanel_data * p_kt_play_info->kt_compact;
            shift_use >>= 10;
            shift_use <<= 16;
            if (shift_use > 0x7fffffff)
            {
                shift_use = 0x7fffffff;
            }
            if (shift_use < -0x7fffffff)
            {
                shift_use = -0x7fffffff;
            }

            //out first sample data
            if (keytone_mode == 0)
            {
                *(uint32 *) ASRC_OUT0_WFIFO = (int32) shift_use;
            }
            else
            {
                *(uint32 *) ASRC_OUT1_WFIFO = (int32) shift_use;
            }

            chanel_data = (int16)(temp_data >> 16);
            shift_use = (long long) chanel_data * p_kt_play_info->kt_compact;
            shift_use >>= 10;
            shift_use <<= 16;
            if (shift_use > 0x7fffffff)
            {
                shift_use = 0x7fffffff;
            }
            if (shift_use < -0x7fffffff)
            {
                shift_use = -0x7fffffff;
            }

            //out second sample data

            //*(uint32 *)ASRC_OUT1_WFIFO = (int32)shift_use;
            if (keytone_mode == 0)
            {
                *(uint32 *) ASRC_OUT0_WFIFO = (int32) shift_use;
            }
            else
            {
                *(uint32 *) ASRC_OUT1_WFIFO = (int32) shift_use;
            }

            //position move
            buf_ptr += 4;
            //data count sub
            data_count -= 4;
        }
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 * \int32 keytone_play_frame(void* null0, void* null1, void* null2)
 * \按键音播放
 * \param[in]    none
 * \param[out]   none
 * \return       bool the result
 * \retval           1 sucess
 * \retval           0 failed
 * \ingroup
 * \note
 */
/*******************************************************************************/
static int32 keytone_play_frame(kt_play_info_t *p_kt_play_info, uint8 keytone_mode)
{
    //播放音频帧
    int32 play_ret;

    sys_os_sched_lock();

    if (p_kt_play_info->kt_offset < p_kt_play_info->kt_file_size)
    {
        if ((p_kt_play_info->kt_file_size - p_kt_play_info->kt_offset) < p_kt_play_info->kt_frame_size)
        {
            p_kt_play_info->kt_frame_size = (p_kt_play_info->kt_file_size - p_kt_play_info->kt_offset);
        }

        p_kt_play_info->kt_offset += p_kt_play_info->kt_frame_size;
        play_ret = p_kt_play_info->kt_frame_size;

        //send data to asrc
        keytone_send_to_asrc(p_kt_play_info, keytone_mode);
    }
    else
    {
        play_ret = -1;
    }

    sys_os_sched_unlock();

    return play_ret;
}

/******************************************************************************/
/*!
 * \par  Description:
 *	  设置按键音函数
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      music_test
 * \note
 *******************************************************************************/
void upgrade_start_key_tone(void)
{
    int32 play_ret;
    
#if ((CASE_BOARD_TYPE == CASE_BOARD_ATS2823) || (CASE_BOARD_TYPE == CASE_BOARD_DVB_ATS2823))
    if(g_led_state == 0)
    {
        led_off_spio();
        g_led_state = 1;    
    }
    else
    {
        led_on_spio();
        g_led_state = 0;            
    }
    return;
#endif    

    while (1)
    {
        play_ret = keytone_play_frame(&kt_play_info, 0);
        if (play_ret == -1)
        {
            //sys_os_time_dly(2);
            break;
        }
    }

    kt_play_info.kt_offset = 0;
    kt_play_info.kt_frame_size = 512;

    return;
}



