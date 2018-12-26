/********************************************************************************
 *                              US280A
 *                            Module: USB device
 *                 Copyright(c) 2001-2012 Actions Semiconductor,
 *                            All Rights Reserved.
 *
 * History:
 *      <author>    <time>            <version >             <desc>
 *      dengtaiping 2013-01-18 16:40     1.0                build this file
 ********************************************************************************/
#include "ud_setup.h"
#include "ud_intern.h"
#include "sys_info.h"

/******************************************************************************/
/*!
 * \par  Description:
 *    get the current status the udisk be in.
 * \param[in]   none
 * \return  the reust
 * \retval  value -> the state
 * \ingroup msc_isr.c
 * \par
 * \note
 *******************************************************************************/
int set_usb_cfg_param(uint32 index, uint32 *cfg_info, uint32 cfg_len)
{
    uint32 *set_p = NULL;
    int32 ret_val = -1;
    switch(index) {
        case 0:
            set_p = (uint32 *) g_usb_vid;
            break;
        case 1:
            set_p = (uint32 *) g_usb_pid;
            break;
        case 2:
            set_p = (uint32 *) &manufacure_string.wManufacturerName;
            break;
        case 3:
            set_p = (uint32 *) &device_name_string.wProductName;
            break;
        case 4:
            set_p = &g_volume_flag;
            break;
        case 5:
            set_p = (uint32 *) g_usb_bcd;
            break;
        case 6:
            set_p = (uint32 *) &SerialNumber.wManufacturerName;
            break;
        default:
            break;
    }

    if(set_p != NULL) {
        libc_memcpy((uint8 *) set_p, (uint8 *) cfg_info, cfg_len);
        ret_val = 0;
    }

    return ret_val;
}

/******************************************************************************/
/*!
 * \par  Description:
 *   count string lenth.
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_setup.c
 * \par
 * \note
 *******************************************************************************/
uint8 count_string_charlngth(uint8 *pstraddr, uint8 strlgth)
{
    unsigned char charcounter = 0x00;
    unsigned char i;
    for(i = 0; i < strlgth; i++) {
        if(*pstraddr == 0) {
            break;
        }
        charcounter++;
        pstraddr++;
    }
    return charcounter;
}

/******************************************************************************/
/*!
 * \par  Description:
 *   ASCII code to unique code.
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_setup.c
 * \par
 * \note
 *******************************************************************************/
void cnvrt_ascii_to_uni16(uint8 *pstraddr, uint8 strlgth)
{
    uint8 *data_p1;
    uint8 *data_p2;

    //数组的下标从0开始
    data_p1 = pstraddr + (strlgth - 1); //The last byte address
    data_p2 = pstraddr + (strlgth - 1) * 2 + 1; //the data address to be load

    //当ASCII码长度为0或1时作下面的转换会覆盖地址指向memory之前的数据
    while(strlgth != 0x00) {
        *data_p2 = 0x00;
        data_p2--;
        *data_p2 = *data_p1; //Original ASCII char

        data_p1--;
        data_p2--;
        strlgth--;
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 *   string to int
 * \param[in]
 * \param[in]
 * \return
 * \retval
 * \ingroup enumerate.c
 * \par
 * \note
 *******************************************************************************/
uint32 atoi(const char *src, int lenth)
{
    int total = 0;
    int temp_value;

    while(lenth != 0) {
        if((*src >= 'a') && (*src <= 'f')) {
            temp_value = 10 + *src - 'a';
        } else if((*src >= 'A') && (*src <= 'F')) {
            temp_value = 10 + *src - 'A';
        } else {
            temp_value = *src - '0';
        }
        total = total * 16 + temp_value;
        src++;
        lenth--;
    }

    return total;
}

/******************************************************************************/
/*!
 * \par  Description:
 *   deal descriptor.
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_setup.c
 * \par
 * \note
 *******************************************************************************/
void descriptor_init(void)
{
    uint8 string_len;
    uint8 ret_type;

    if(0 != g_usb_vid[2]) {
        Device_Descriptor.idVendor = (unsigned short) atoi((char *) &g_usb_vid[2], 4);
    }

    if(0 != g_usb_pid[2]) {
        Device_Descriptor.idProduct = (unsigned short) atoi((char *) &g_usb_pid[2], 4);
    }

    if(0 != g_usb_bcd[2]) {
        Device_Descriptor.bcdDevice = (unsigned short) atoi((char *) &g_usb_bcd[2], 4);
    }

    string_len = count_string_charlngth((uint8 *) &manufacure_string.wManufacturerName, 0x08);
    manufacure_string.bLength = 0x02 + string_len * 2;
    manufacure_string.bDescriptorType = 0x03;
    cnvrt_ascii_to_uni16((uint8 *) &manufacure_string.wManufacturerName, string_len);

    string_len = count_string_charlngth((uint8 *) &device_name_string.wProductName, 0x10);
    device_name_string.bLength = 0x02 + string_len * 2;
    device_name_string.bDescriptorType = 0x03;
    cnvrt_ascii_to_uni16((uint8 *) &device_name_string.wProductName, string_len);

    //sys_get_fw_info((uint8 *) &ret_type, FW_INFO_USB_SN_TYPE_ADDR, 1);
    //if ((2 == ret_type) || (1 == ret_type))
    {
        //sys_get_fw_info((uint8 *) &SerialNumber.wManufacturerName, FW_INFO_USB_ID_ADDR, 16);
        string_len = count_string_charlngth((uint8 *) &SerialNumber.wManufacturerName, 0x30);
        if(string_len != 0) {
            SerialNumber.bLength = 0x02 + string_len * 2;
            SerialNumber.bDescriptorType = 0x03;
            cnvrt_ascii_to_uni16((uint8 *) &SerialNumber.wManufacturerName, string_len);
        } else {
            Device_Descriptor.iSerialNumber = 0x00;
        }
    }
    //序列号一致且同为UNICODE编码时，连两台小机到同一PC会死机，所以这里不转换成unicode编码
    //    string_len = count_string_charlngth((uint8 *) &SerialNumber.wManufacturerName, 0x10);
    //    SerialNumber.bLength = 0x02 + string_len * 2;
    //    SerialNumber.bDescriptorType = 0x03;
    //    cnvrt_ascii_to_uni16((uint8 *) &SerialNumber.wManufacturerName, string_len);
    if((g_volume_flag & 0x02) != 0) {
        All_Descriptors_of_Configuration.Audio_Class_Feature1.bmaControls0 = 0x03;
    } else {
        All_Descriptors_of_Configuration.Audio_Class_Feature1.bmaControls0 = 0x01;
    }
    libc_print("pcctrl", g_volume_flag, 2);
}

