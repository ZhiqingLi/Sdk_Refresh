/*******************************************************************************
 *                              AS212A
 *                            Module: SD&MMC CARD DRIVERS
 *                 Copyright(c) 2012-2022 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>           <version >             <desc>
 *       gch      2012-3-20            1.0             build this file
 *******************************************************************************/

#include "ud_intern.h"
#include "ud_asrc.h"

usound_operations_t usound_ops = {
    (ud_op_i) usound_inner_get_status,
    (ud_op_i) usound_inner_module_start,
    (ud_op_i) usound_inner_module_stop,
    (ud_op_i) usound_inner_set_cfg,
    (ud_op_i) usound_inner_get_info,
    (ud_op_i) usound_inner_set_info,
    (ud_op_i) usound_inner_set_cmd,
    (ud_op_i) usound_inner_hid_deal,
};

/******************************************************************************/
/*!
 * \par  Description:get cur usb status if has been pull up or suspend etc
 * \ int usound_inner_get_status(void *status, uint32 sta_index)
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_op_entry.c
 * \par
 * \note
 *******************************************************************************/
int usound_inner_get_status(usound_status_t *pstatus)
{

    pstatus->line_sta = (uint16)g_usb_state;

    pstatus->set_sample = g_set_sample_flag;
    pstatus->set_adc_sample = g_set_adc_sample_flag;
    pstatus->set_dac_sample = g_set_dac_sample_flag;

    pstatus->sample_rate = (g_play_sample / 1000);//has not div 1000 yet
    pstatus->adc_sample_rate = (g_adc_sample / 1000);
    pstatus->dac_sample_rate = (g_dac_sample / 1000);
    if(tmp_play_status != 0) {
        pstatus->start_play = 0;//play flag
    } else {
        pstatus->start_play = g_start_play;
    }

#ifdef _USB_RECORD_
    pstatus->start_record = g_start_record;//record
#endif
    //{

    pstatus->volume_chg = g_volume_chg;

    pstatus->volume_value = g_current_volume_ch0;

    if(g_volume_chg == 1) {
        if(g_current_mute_ch0 == 1) {
            pstatus->volume_value = 0;
        } else {
            pstatus->volume_value = g_current_volume_ch0;
        }
    }

    pstatus->hid_idle_time = g_hid_idle_time;//ile time
    pstatus->data_in_flag = g_datain_flag;
    //}
    pstatus->switch_flag = g_pc_switch_flag;
    /*///检查USBfifo空的次数，用于判断ESD后，USB获取数据是否异常
        (现象:usbFIFO 数据正常，而小机暂停)*/
    if(g_no_empty_count == 50) {
        pstatus->no_empty_fla = 1;

    } else {
        pstatus->no_empty_fla = 0;
    }
    return 0;
}

/******************************************************************************/
/*!
 * \par  Description:start usb enumerate
 * \ int usound_inner_module_stop(void)
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_op_entry.c
 * \par
 * \note
 *******************************************************************************/
int usound_inner_module_start(isr_cbk *p_isr_cbk)
{
    usb_sound_init(p_isr_cbk);

    return 0;
}

/******************************************************************************/
/*!
 * \par  Description:stop usb trans and disconnect to pc
 * \ int usound_inner_module_stop(void)
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_op_entry.c
 * \par
 * \note
 *******************************************************************************/
int usound_inner_module_stop(void)
{
    usb_device_exit();
    return 0;
}

/******************************************************************************/
/*!
 * \par  Description:set some infomation to device
 * \ int usound_inner_set_cfg(uint32 cfg_index, uint32* pcfg, uint32 cfg_len)
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_op_entry.c
 * \par
 * \note
 *******************************************************************************/
int usound_inner_set_cfg(uint32 cfg_index, uint32 *pcfg, uint32 cfg_len)
{
    return set_usb_cfg_param(cfg_index, pcfg, cfg_len);
}

/******************************************************************************/
/*!
 * \par  Description:get usb some infomation
 * \ int usound_inner_get_info(uint32 info_index,uint32* pinfor,uint32* info_len)
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_op_entry.c
 * \par
 * \note
 *******************************************************************************/
int usound_inner_get_info(uint32 info_index, uint32 *pinfor, uint32 *info_len)
{
    uint32 read_len, len_pos;

    read_len = *info_len;

    //int_flag = sys_local_irq_save();

    if(g_play_data_count >= read_len) {

        g_no_data_count = 0;

        len_pos = (g_rd_play_ptr + read_len);

        if(len_pos > PLAY_BUFF_SIZE) {
            libc_memcpy((uint8 *) pinfor, (g_play_buf + g_rd_play_ptr), (PLAY_BUFF_SIZE - g_rd_play_ptr));

            libc_memcpy((uint8 *) pinfor, g_play_buf, (len_pos - PLAY_BUFF_SIZE));
        } else {
            libc_memcpy((uint8 *) pinfor, (g_play_buf + g_rd_play_ptr), read_len);
        }

        g_play_data_count -= read_len;

        g_rd_play_ptr += read_len;

        if(g_rd_play_ptr >= PLAY_BUFF_SIZE) {
            g_rd_play_ptr = 0;
        }
    } else {
        g_no_data_count++;
        read_len = 0;
    }

    if(g_no_data_count >= 100) {

        //libc_print("remain", g_play_data_count, 2);
        g_start_play = 0;

        g_no_data_count = 0;
    }

    //sys_local_irq_restore(int_flag);

    return read_len;
}

/******************************************************************************/
/*!
 * \par  Description:set some infomation to device
 * \ int usound_inner_set_info(uint32 info_index,uint32* pinfor,uint32 info_len)
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_op_entry.c
 * \par
 * \note
 *******************************************************************************/
int usound_inner_set_info(uint32 info_index, uint32 *pinfor, uint32 info_len)
{
    return 0;
}

/******************************************************************************/
/*!
 * \par  Description:set cmd to device for human handle
 * \ int usound_inner_set_cmd(uint32 cmd,uint32 cmd_param)
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_op_entry.c
 * \par
 * \note
 *******************************************************************************/
int usound_inner_set_cmd(uint32 cmd, uint32 cmd_param)
{
    //clear some flag and do something
    switch(cmd) {

        case SET_PLAY_FLAG:

            //g_start_play = (uint8) cmd_param;
            if(cmd_param == 3) {
                low_energy = 1;
                g_start_play = 0;
            } else if(cmd_param == 4) {
                low_energy = 0;
            } else {
                g_manul_stop = cmd_param;

                if(g_manul_stop != 0) {
                    g_start_play = 0;

                    reset_data_pos();
                }
            }

            break;

        case SET_HID_OPERS:
            //libc_print("next",g_hid_report,2);
            g_hid_report = (uint8) cmd_param;
            g_hid_change = 1;
            break;

        case SET_SAMPLE_RATE:

            //sample rate is 44100

            g_set_sample_flag = 0;
            g_set_dac_sample_flag = 0;
            g_set_adc_sample_flag = 0;

            break;

        case SET_ADJUST_TIMER:
            if(cmd_param == 1) {
                //start
                if(g_asrc_adjust_timer == -1) {
                    init_channel_record(&g_asrc_out1_record);
                    g_asrc_adjust_timer = sys_set_irq_timer1(deal_timer_process, 50);
                }
            } else {
                //stop
                //make sure timer over
                if(g_asrc_adjust_timer != -1) {
                    sys_del_irq_timer1(g_asrc_adjust_timer);
                    g_asrc_adjust_timer = -1;
                }

            }

            break;

        case SET_LINE_STATUS:
            g_usb_state = cmd_param;
            break;

        case SET_VOLUME_FLAG:
            g_volume_chg = 0;

            break;

        case SET_HID_CHANGE:
            g_hid_change = 1;
            break;

        case SET_TTS_FLAG:
            if(cmd_param == 1) {
                g_tts_start_flag = 1;
            } else {
                g_tts_start_flag = 0;
            }
            break;

        case SET_MUTE_FLAG:
            if(cmd_param == 1) {
                g_mute_flag = 1;
            } else {
                g_mute_flag = 0;
                g_mute_delay = 3;
            }
            break;

        case FIX_SAMPLE_RATE:
            g_fix_sample_flg = 1;
            break;

        case CLR_SWITCH_FLAG:
            g_pc_switch_flag = 0;
            break;

        case SET_VOLUME:
            g_current_volume_ch0 = (uint16)cmd_param;
            g_current_volume_ch1 = (uint16)cmd_param;
            g_current_volume_ch2 = (uint16)cmd_param;
            break;

        default:
            break;

    }
    return 0;
}

/******************************************************************************/
/*!
 * \par  Description:check if there has hid intoken,and send data to pc
 * \ int usound_inner_hid_deal(void)
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_op_entry.c
 * \par
 * \note
 *******************************************************************************/
int usound_inner_hid_deal(void)
{
    hid_token_check();
    return 0;
}

