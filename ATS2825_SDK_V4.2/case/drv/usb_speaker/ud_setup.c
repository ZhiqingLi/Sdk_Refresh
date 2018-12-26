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

/******************************************************************************/
/*!
 * \par  Description:
 *  set endpoint0 no NAK.
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_setup.c
 * \par
 * \note
 *******************************************************************************/
void no_data_control(void)
{
    act_writeb(0x02 | act_readb(Ep0cs_hcep0cs), Ep0cs_hcep0cs);
}

/******************************************************************************/
/*!
 * \par  Description:
 *   updata by endpoint0.
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_setup.c
 * \par
 * \note
 *******************************************************************************/
void control_write(uint8 *data_address, uint8 data_length, uint8 request_length)
{
    uint8 transfer_length, temp_count;
    uint8 *p_temp;

    /* if length of data to be received is more than PC sent, only send data length that pc sent.  */
    if(data_length <= request_length) {
        transfer_length = data_length;
    } else {
        transfer_length = request_length;
    }
    p_temp = data_address;

    /* clear OUT0BC to receive data from PC.  */
    act_writeb(0x00, Out0bc_hcin0bc);

    /* wait transfer complete. */
    while(1) {
        /* check if busy.   */
        if((act_readb(Ep0cs_hcep0cs) & 0x08) == 0x00) {
            break;
        }

        if(USB_CONNECT_STATUS != 0x00) {
            g_usb_state = 0x01;
            break;
        }
    }/* while(1) */

    for(temp_count = 0; temp_count < transfer_length; temp_count++) {
        *(p_temp + temp_count) = act_readb(Ep0outdata + temp_count);
    }
    act_writeb(0x01, Out0bc_hcin0bc);
    act_writeb(act_readb(Ep0cs_hcep0cs) | 0x02, Ep0cs_hcep0cs);

}

/******************************************************************************/
/*!
 * \par  Description:
 *   download data by endpoint0.
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_setup.c
 * \par
 * \note
 *******************************************************************************/
void control_read(uint8 *d_addr, uint8 actul_dlen, uint8 rqst_dlen)
{
    uint8 translen_sum;
    uint8 translen_once;
    uint32 time_out_cnt = 0;
    uint8 i;
    //calucate data length of this control read
    if(rqst_dlen == 0x00) {
        translen_sum = actul_dlen;
    } else if(rqst_dlen > actul_dlen) {
        translen_sum = actul_dlen;
    } else {
        translen_sum = rqst_dlen;
    }

    //transferring
    while(translen_sum != 0) {
        if(translen_sum > 0x40) {
            translen_once = 0x40;
            translen_sum = translen_sum - 0x40;
        } else {
            translen_once = translen_sum;
            translen_sum = 0x00;
        }
        for(i = 0; i < translen_once; i++) {
            //write data to ep0infofo
            act_writeb(*d_addr, (Ep0indata + i));
            d_addr++;
        }
        //wtite in0 conter,it will start packet transfer and set in0busy
        act_writeb((uint8) translen_once, In0bc_hcout0bc);
        //wait transfer end,Inbsy_hcoutbsy = b'0'
        while((act_readb(Ep0cs_hcep0cs) & 0x04) != 0x00) {
            //5s timeout delay
            if(time_out_cnt < 100000) {
                time_out_cnt++;
                sys_udelay(10);
            } else {
                break;
            }
        }

        if(USB_CONNECT_STATUS != 0x00) {
            g_usb_state = 0x01;
            break;
        }
    }
    act_writeb(0x02 | act_readb(Ep0cs_hcep0cs), Ep0cs_hcep0cs);
}

/******************************************************************************/
/*!
 * \par  Description:
 *   stall endpoint.
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_setup.c
 * \par
 * \note
 *******************************************************************************/
void stall_endpoint(uint8 ep_num)
{
    switch(ep_num) {
        case EP0:
            //stall ep0
            act_writeb(0x01 | act_readb(Ep0cs_hcep0cs), Ep0cs_hcep0cs);
            break;
        case EP1OUT:
            //stall ep1out
            act_writeb(0x40 | act_readb(Out1ctrl_hcin1ctrl), Out1ctrl_hcin1ctrl);
            break;
        case EP2OUT:
            //stall ep2out
            act_writeb(0x40 | act_readb(Out2ctrl_hcin2ctrl), Out2ctrl_hcin2ctrl);
            break;
        case EP1IN:
            //stall ep1in
            act_writeb(0x40 | act_readb(In1ctrl_hcout1ctrl), In1ctrl_hcout1ctrl);
            break;
        case EP2IN:
            //stall ep2in
            act_writeb(0x40 | act_readb(In2ctrl_hcout2ctrl), In2ctrl_hcout2ctrl);
            break;
        default:
            break;
    }
    return;
}

/******************************************************************************/
/*!
 * \par  Description:
 *   recover enpoint.
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_setup.c
 * \par
 * \note
 *******************************************************************************/
void recover_stall_endpoint(uint8 ep_num)
{
    switch(ep_num) {
        case EP0:
            //claer stall bit for ep0
            act_writeb((uint8) 0xfe & act_readb(Ep0cs_hcep0cs), Ep0cs_hcep0cs);
            break;
        case EP1OUT:
            //claer stall bit for ep1out
            act_writeb((uint8) 0xbf & act_readb(Out1ctrl_hcin1ctrl), Out1ctrl_hcin1ctrl);
            break;
        case EP2OUT:
            //claer stall bit for ep2out
            act_writeb((uint8) 0xbf & act_readb(Out2ctrl_hcin2ctrl), Out2ctrl_hcin2ctrl);
            break;
        case EP1IN:
            //claer stall bit for ep1in
            act_writeb((uint8) 0xbf & act_readb(In1ctrl_hcout1ctrl), In1ctrl_hcout1ctrl);
            break;
        case EP2IN:
            //claer stall bit for ep2in
            act_writeb((uint8) 0xbf & act_readb(In2ctrl_hcout2ctrl), In2ctrl_hcout2ctrl);
            break;
        default:
            break;
    }
}

/******************************************************************************/
/*!
 * \par  Description:
 *   set bulk out endpoint.
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_setup.c
 * \par
 * \note
 *******************************************************************************/
void set_out_iso_ep(void)
{
    act_writeb(0x80, Hcin1maxpckl);
    act_writeb(0x01, Hcin1maxpckh);

    act_writeb(0x84, Out1ctrl_hcin1ctrl);
    //act_writel(0x00000680, OUT1STADDR);
    act_writel(0x00000000, OUT1STADDR);
    act_writeb(0x01, EPRST);
    act_writeb(0x21, EPRST);
    act_writeb(0x41, EPRST);
    act_writeb(0x21, Fifoctrl); //set inep1 autorun
}

/******************************************************************************/
/*!
 * \par  Description:
 *   set interrupt in endpoint.
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_setup.c
 * \par
 * \note
 *******************************************************************************/
void set_in_int_ep(void)
{
    act_writeb(0x04, Hcout2maxpckl);
    act_writeb(0x00, Hcout2maxpckh);

    act_writeb(0x8c, In2ctrl_hcout2ctrl);//single buff、interrupt ep
    //act_writel(0x00000c80, IN2STADDR);
    act_writel(0x00000600, IN2STADDR);
    act_writeb(0x12, EPRST);
    act_writeb(0x32, EPRST);
    act_writeb(0x52, EPRST);
    act_writeb(0x32, Fifoctrl); //set inep1 autorun
}

/******************************************************************************/
/*!
 * \par  Description:
 *   deal endpoint0 data.
 * \param[in]   none
 * \return  none
 * \retval  none
 * \ingroup ud_setup.c
 * \par
 * \note
 *******************************************************************************/
void deal_setup(void)
{
    uint8 temp_count;
    uint8 *p_temp;
    uint32 new_sample;

    p_temp = &g_setup_data.bmRequestType;

    /* get setup data.*/
    g_setup_data.bmRequestType = *(volatile uint8 *)(Setupdat0);
    g_setup_data.bRequest = *(volatile uint8 *)(Setupdat1);
    g_setup_data.wValue = *(volatile uint16 *)(Setupdat2);
    g_setup_data.wIndex = *(volatile uint16 *)(Setupdat4);
    g_setup_data.wLength = *(volatile uint16 *)(Setupdat6);

    /* judge type of device request.   */
    switch(g_setup_data.bmRequestType) {
        case RD_STD_DEV:
            if(g_setup_data.bRequest == GET_DESCRIPTOR) {
                /* get device descriptor. */
                if(g_setup_data.wValue == VALUE_DEVICE) {
                    control_read(&Device_Descriptor.bLength, Device_Descriptor.bLength, (uint8) g_setup_data.wLength);
                }

                /* get device configuration. */
                else if(g_setup_data.wValue == VALUE_CONFIG) {
                    //prints("config\n");
                    control_read((uint8 *) &All_Descriptors_of_Configuration/*.Configuration_Descriptor.bLength*/,
                                 (uint8)(All_Descriptors_of_Configuration.Configuration_Descriptor.wTotalLength),
                                 (uint8) g_setup_data.wLength);
                }

                /* get string of language ID. */
                else if(g_setup_data.wValue == VALUE_STRING_LANGID) {
                    control_read(&language_string.bLength, language_string.bLength, (uint8) g_setup_data.wLength);
                }

                /* get string of manufaturer. */
                else if(g_setup_data.wValue == VALUE_STRING_MANUFACTURER) {
                    control_read(&manufacure_string.bLength, manufacure_string.bLength, (uint8) g_setup_data.wLength);
                }

                /* get string of product. */
                else if(g_setup_data.wValue == VALUE_STRING_PRODUCT) {
                    control_read(&device_name_string.bLength, device_name_string.bLength, (uint8) g_setup_data.wLength);
                }

                /* get string of serial number. */
                else if(g_setup_data.wValue == VALUE_STRING_SERIAL_NUMBER) {
                    control_read(&SerialNumber.bLength, SerialNumber.bLength, (uint8) g_setup_data.wLength);
                }

                /* others error.  */
                else {
                    ;//do nothing
                }
            } else {
                stall_endpoint(EP0);
            }

            break;

        case RD_STD_IF:
            /* get Descriptor. */
            if(g_setup_data.bRequest == GET_DESCRIPTOR) {
                //HID请求
                if(g_setup_data.wIndex == INDEX_INTERFACE_HID) {
                    control_read(g_hid_Descriptor, sizeof(g_hid_Descriptor), (uint8) g_setup_data.wLength);
                }
            }
            break;

        case WR_STD_DEV:
            /* set address. */
            if(g_setup_data.bRequest == SET_ADDRESS) {
                no_data_control();
            }

            /* set configuration. */
            else if(g_setup_data.bRequest == SET_CONFIGURATION) {
                //prints("sconfig\n");
                set_out_iso_ep();
                //hid device
                set_in_int_ep();

                no_data_control();
                //prints("e\n");
            } else {
                stall_endpoint(EP0);
            }
            break;

        case WR_STD_IF:
            /* set interface. */
            if(g_setup_data.bRequest == SET_INTERFACE) {
                if(g_setup_data.wIndex == INDEX_INTERFACE_PLAY) {
                    /* setting 0 means that zero-bandwidth. */
                    //setup_data.wValue == VALUE_ALTERNATE_SETTING0
                    /* setting 1\2\3 means that pcm data will be sent to device. */
                    if(g_setup_data.wValue == VALUE_ALTERNATE_SETTING1) {
                        g_wr_play_ptr = 0;

                        g_rd_play_ptr = 0;

                        g_play_data_count = 0;

                        g_start_play = 0;//need stop play
                    }
                }

                no_data_control(); /* set interface. */
            } else {
                stall_endpoint(EP0);
            }
            break;

        case SPECIFIC_REQUEST_OUT:
            /* set feature unit. */
            if((g_setup_data.wIndex == FEATURE_UNIT_INDEX1) || (g_setup_data.wIndex == FEATURE_UNIT_INDEX2)) {
                /* set current mute or volume of main channel 0,1,2.   */
                if(g_setup_data.bRequest == SET_CUR) {
                    //prints("set_cur\n");
                    temp_count = VOLUME_LENGTH;
                    if(g_setup_data.wValue == ((MUTE_CONTROL << 8) | MAIN_CHANNEL_NUMBER0)) {
                        //prints("set_mute\n");
                        p_temp = &g_current_mute_ch0;
                        temp_count = MUTE_LENGTH;
                        g_volume_chg = 1;
                    } else if(g_setup_data.wValue == ((VOLUME_CONTROL << 8) | MAIN_CHANNEL_NUMBER0)) {
                        //prints("set_volume0\n");
                        p_temp = (uint8 *) &g_current_volume_ch0;
                        //flag
                        g_volume_chg = 1;
                    } else if(g_setup_data.wValue == ((VOLUME_CONTROL << 8) | MAIN_CHANNEL_NUMBER1)) {
                        //prints("set_volume1\n");
                        p_temp = (uint8 *) &g_current_volume_ch1;
                    } else if(g_setup_data.wValue == ((VOLUME_CONTROL << 8) | MAIN_CHANNEL_NUMBER2)) {
                        //prints("set_volume2\n");
                        p_temp = (uint8 *) &g_current_volume_ch2;
                    } else {
                        ;//do nothing
                    }

                    //在控制传输中接收参数前先判断是否有同步传输请求

                    control_write(p_temp, temp_count, (uint8) g_setup_data.wLength);

                    no_data_control();

                    break;
                } else if(g_setup_data.bRequest == SET_RES) {
                    temp_count = 0;
                    control_write(p_temp, temp_count, (uint8) g_setup_data.wLength);
                    stall_endpoint(EP0);
                    break;
                } else {
                    ;//nothing
                }
            } else if(g_setup_data.wIndex == SELECTOR_UNIT_INDEX) {
                if(g_setup_data.bRequest == SET_CUR) {
                    temp_count = 0x01;
                    p_temp = (uint8 *) &g_cur_selector;

                    control_write(p_temp, temp_count, (uint8) g_setup_data.wLength);

                }
            } else if(g_setup_data.wIndex == INDEX_INTERFACE_HID) { //处理HID设备特殊请求
                if(g_setup_data.bRequest == SET_IDLE) { //get_idle
                    g_hid_idle_time = g_setup_data.wValue >> 6; //setup_data.wValue先左移8位再乘以4
                }
            } else {
                //;//do nothing
                //在控制传输中接收参数前先判断是否有同步传输请求
                uint32 actions_flag;
                control_write(&actions_flag, 4, (uint8) g_setup_data.wLength);
                if(actions_flag == 0x22cb21cb) {
                    g_pc_switch_flag = 1;//switch to stub
                    libc_print("swtich", 0, 0);
                }
            }
            no_data_control();
            break;

        case SPECIFIC_REQUEST_IN:
            /* get feature unit. */
            p_temp = (uint8 *) &g_volume_value;
            temp_count = VOLUME_LENGTH;
            if((g_setup_data.wIndex == FEATURE_UNIT_INDEX1) || (g_setup_data.wIndex == FEATURE_UNIT_INDEX2)) {
                temp_count = VOLUME_LENGTH;

                /* get current mute or volume of main channel 0,1,2.   */
                if(g_setup_data.bRequest == GET_CUR) {
                    if(g_setup_data.wValue == ((MUTE_CONTROL << 8) | MAIN_CHANNEL_NUMBER0)) {
                        p_temp = &g_current_mute_ch0;
                        temp_count = MUTE_LENGTH;
                    } else if(g_setup_data.wValue == ((VOLUME_CONTROL << 8) | MAIN_CHANNEL_NUMBER0)) {
                        p_temp = (uint8 *) &g_current_volume_ch0;
                    } else if(g_setup_data.wValue == ((VOLUME_CONTROL << 8) | MAIN_CHANNEL_NUMBER1)) {
                        p_temp = (uint8 *) &g_current_volume_ch1;
                    } else if(g_setup_data.wValue == ((VOLUME_CONTROL << 8) | MAIN_CHANNEL_NUMBER2)) {
                        p_temp = (uint8 *) &g_current_volume_ch2;
                    } else {
                        ;//for qac
                    }
                }

                /* get minimum volume of main channel 0,1,2.   */
                else if(g_setup_data.bRequest == GET_MIN) {
                    if(g_setup_data.wValue == ((VOLUME_CONTROL << 8) | MAIN_CHANNEL_NUMBER0)) {
                        ;//for qac
                    } else if(g_setup_data.wValue == ((VOLUME_CONTROL << 8) | MAIN_CHANNEL_NUMBER1)) {
                        ;//for qac
                    } else if(g_setup_data.wValue == ((VOLUME_CONTROL << 8) | MAIN_CHANNEL_NUMBER2)) {
                        ;//for qac
                    } else {
                        ;//for qac
                    }
                    g_volume_value = MINIMUM_VOLUME;
                }

                /* get maximum volume of main channel 0,1,2.   */
                else if(g_setup_data.bRequest == GET_MAX) {
                    if(g_setup_data.wValue == ((VOLUME_CONTROL << 8) | MAIN_CHANNEL_NUMBER0)) {
                        ;//for qac
                    } else if(g_setup_data.wValue == ((VOLUME_CONTROL << 8) | MAIN_CHANNEL_NUMBER1)) {
                        ;//for qac
                    } else if(g_setup_data.wValue == ((VOLUME_CONTROL << 8) | MAIN_CHANNEL_NUMBER2)) {
                        ;//for qac
                    } else {
                        ;//for qac
                    }
                    g_volume_value = MAXIMUM_VOLUME;
                }

                /* get resolution volume of main channel 0,1,2.   */
                else if(g_setup_data.bRequest == GET_RES) {
                    if(g_setup_data.wValue == ((VOLUME_CONTROL << 8) | MAIN_CHANNEL_NUMBER0)) {
                        ;//for qac
                    } else if(g_setup_data.wValue == ((VOLUME_CONTROL << 8) | MAIN_CHANNEL_NUMBER1)) {
                        ;//for qac
                    } else if(g_setup_data.wValue == ((VOLUME_CONTROL << 8) | MAIN_CHANNEL_NUMBER2)) {
                        ;//for qac
                    } else {
                        ;//for qac
                    }
                    g_volume_value = RESOTION_VOLUME;
                } else {
                    ;//do nothing
                }

                control_read(p_temp, temp_count, (uint8) g_setup_data.wLength);

            } else if(g_setup_data.wIndex == SELECTOR_UNIT_INDEX) {
                if(g_setup_data.bRequest == GET_CUR) {
                    temp_count = 0x01;
                    p_temp = (uint8 *) &g_cur_selector;
                    control_read(p_temp, temp_count, (uint8) g_setup_data.wLength);
                }
            } else if(g_setup_data.wIndex == INDEX_INTERFACE_HID) { //处理HID设备特殊请求
                if(g_setup_data.bRequest == GET_REPORT) { //get_report
                    control_read((uint8 *) &g_hid_report, sizeof(g_hid_report), (uint8) g_setup_data.wLength);
                } else if(g_setup_data.bRequest == GET_IDLE) { //get_idle
                    temp_count = (uint8)(g_hid_idle_time >> 2);
                    control_read((uint8 *) &temp_count, 0x01, (uint8) g_setup_data.wLength);
                } else {
                    ;//for qac
                }
            } else {
                ;//for qac
            }
            break;

        case SET_ENDPOINT_CONTROL:
            /* set endpoint control request. */
            if(g_setup_data.wValue == SAMPLING_FREQ_CONTROL) {
                /* set sample frequency.   */
                if(g_setup_data.bRequest == SET_CUR) {
                    new_sample = 0x00000000;
                    temp_count = SAMPLE_FREQUENCY_LENGTH;
                    p_temp = (uint8 *) &new_sample;

                    control_write(p_temp, temp_count, (uint8) g_setup_data.wLength);
                    no_data_control();

                    if(g_setup_data.wIndex == 0x01) {
                        /* if new sample is different from current sample, initialize dec again.  */
                        g_start_play = 0;

                        g_wr_play_ptr = 0;

                        g_rd_play_ptr = 0;

                        g_dac_sample = new_sample;

                        g_set_dac_sample_flag = 1;
                    }

                    g_play_sample = new_sample;

                    g_set_sample_flag = 1;

                }
            }
            break;

        case GET_SAMPLERATE:
            if((g_setup_data.bRequest == GET_CUR) && (g_setup_data.wValue == 0x0100)) {
                if(g_play_sample == 44100) {
                    control_read(All_Descriptors_of_Configuration.Audio_Class_Format_Setting1.tSamFreq2, 0x03, \
                                 (uint8) g_setup_data.wLength);
                } else if(g_play_sample == 48000) {
                    control_read(All_Descriptors_of_Configuration.Audio_Class_Format_Setting1.tSamFreq1, 0x03, \
                                 (uint8) g_setup_data.wLength);
                } else {
                    ;//qac
                }
            }
            no_data_control();
            break;

        default:
            no_data_control();//可能会发set clearfeature到en2in
            break;
    }

}

