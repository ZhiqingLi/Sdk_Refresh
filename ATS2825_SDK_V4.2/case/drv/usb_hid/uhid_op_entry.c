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

uhid_operations_t uhid_ops = {
    (ud_op_i) uhid_inner_get_status,
    (ud_op_i) uhid_inner_module_start,
    (ud_op_i) uhid_inner_module_stop,
    (ud_op_i) uhid_inner_set_cfg,
    (ud_op_i) uhid_inner_hid_get,
    (ud_op_i) uhid_inner_hid_set,
    (ud_op_i) uhid_inner_set_cmd,
    (ud_op_i) uhid_inner_hid_deal,
};

/******************************************************************************/
/*!
 * \par  Description:get cur usb status if has been pull up or suspend etc
 * \ int usound_inner_get_status(uhid_status_t *pstatus)
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_op_entry.c
 * \par
 * \note
 *******************************************************************************/
int uhid_inner_get_status(uhid_status_t *pstatus)
{

    pstatus->line_sta = g_usb_state;

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
int uhid_inner_module_start(void)
{
    usb_hid_init();

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
int uhid_inner_module_stop(void)
{
    usb_hid_exit();

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
int uhid_inner_set_cfg(uint32 cfg_index, uint32 *pcfg, uint32 cfg_len)
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
int uhid_inner_hid_get(void)
{
    return 0;
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
int uhid_inner_hid_set(void)
{
    // uint32 last_data = g_record_data_count;

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
int uhid_inner_set_cmd(uint32 cmd, uint32 cmd_param)
{
    //clear some flag and do something
    switch(cmd) {

        case SET_HID_OPERS:
            //for data send to pc
            g_hid_in_buf[0] = (uint8) cmd_param;
            g_hid_change = 1;
            break;

        case SET_LINE_STATUS:
            g_usb_state = (uint8) cmd_param;
            break;

        //case SET_HID_CHANGE:
        //g_hid_change = 1;
        //break;

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
int uhid_inner_hid_deal(void)
{
    //hid_token_check();
    return 0;
}

