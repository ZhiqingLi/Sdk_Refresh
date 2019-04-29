/******************************************************************************
 * MODULE NAME:    hw_serial_ports.h
 * PROJECT CODE:   BlueStream
 * DESCRIPTION:    None
 * MAINTAINER:     Ivan Griffin
 * CREATION DATE:  7 August 1999
 *
 * SOURCE CONTROL: $Id: hw_serial_ports.h,v 1.4 2004/07/07 14:21:21 namarad Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2004 Ceva Inc.
 *     All rights reserved.
 *
 ******************************************************************************/

#ifndef _PARTHUS_HW_SERIAL_PORTS_
#define _PARTHUS_HW_SERIAL_PORTS_

#define SER_MAX_PKT_LEN     31
#define SER_ADDR_LEN_SHFT   0
#define SER_ADDR_LEN_MASK   0x000001f
#define SER_PKT_LEN_SHFT    5
#define SER_PKT_LEN_MASK    0x00003e0
#define SER_CLK_POL_SHFT    10
#define SER_CLK_POL_MASK    0x0000400
#define SER_DAT_POL_SHFT    11
#define SER_DAT_POL_MASK    0x0000800
#define SER_DEV_ENB_SHFT    12
#define SER_DEV_ENB_MASK    0x000f000
#define SER_CLK_LOW_SHFT    16
#define SER_CLK_LOW_MASK    0x00f0000
#define SER_CLK_HI_SHFT     20
#define SER_CLK_HI_MASK     0x0700000
#define SER_CLK_BYP_SHFT    23
#define SER_CLK_BYP_MASK    0x0800000
#define SER_LE_INV_SHFT     24
#define SER_LE_INV_MASK     0x1000000
#define SER_LE_SRC_SHFT     25
#define SER_LE_SRC_MASK     0x2000000

typedef struct ser_device_configuration {
    u_int32 addr_len      :5;
    u_int32 pkt_len       :5;
    u_int32 clk_poll      :1;
    u_int32 data_poll     :1;
    u_int32 dev_enable    :4;

    u_int32 clk_low       :4;
    u_int32 clk_high      :3;
    u_int32 clk_byp       :1;
    u_int32 le_inv        :1;
    u_int32 le_src        :1;
    u_int32               :6;
    } t_serial_device_config;

typedef struct ser_device_struct {
    union {
     t_serial_device_config bitfield;
     u_int32                word32;
     } config; 
/* 
 * JN: These MAY always sequential TBC
 */
    u_intHW *ser_cfg_addr;
    u_intHW *ser_data_addr;
  } t_serial_device;

void HWsp_Config_Serial_Port(t_serial_device *p_ser_device);
void HWsp_Set_Serial_Port(t_serial_device *p_ser_device, 
          u_int32 address, u_int32 data);
void HWsp_Get_Serial_Port(t_serial_device *p_ser_device, 
          u_int32 address, u_int32 *data);

#endif
