#ifndef _PARTHUS_HW_CODEC_
#define _PARTHUS_HW_CODEC_

/******************************************************************************
 * MODULE NAME:    hw_codec.h
 * PROJECT CODE:   BlueStream
 * DESCRIPTION:    Generic Codec hardware abstraction layer
 * MAINTAINER:     Daire McNamara
 * DATE:           10 October 2001
 *
 * SOURCE CONTROL: $Id: hw_codec.h,v 1.10 2005/05/25 14:21:20 namarad Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2001-2004 Ceva Inc.
 *     All rights reserved.
 *
 * REVISION HISTORY:
 *    10 October 2001 - DMN - Initial Version
 *
 * NOTES TO USERS:
 ******************************************************************************/

#include "sys_config.h"
#include "lc_types.h"

void HWcodec_Initialise(void);
boolean HWcodec_Is_Valid_SCO_Conversion(u_int16 voice_setting);
void HWcodec_Set_Voice_Setting(u_int16 new_voice_setting);
u_int16 HWcodec_Get_Voice_Setting(void);
void HWcodec_Enable(void);
void HWcodec_Disable(void);
u_int8 HWcodec_Get_VCI_Clk_Sel(void);
u_int8 HWcodec_Get_VCI_Clk_Sel_Map(void);
void HWcodec_Increase_Volume(void);
void HWcodec_Decrease_Volume(void);

#if defined(SYS_HAL_VCI_CLK_OVERRIDE_SUPPORTED)
typedef enum {
    HW_CODEC_VCI_CLK_DEFAULT=0,
    HW_CODEC_VCI_CLK_64kHz=1,
    HW_CODEC_VCI_CLK_200kHz=2,
    HW_CODEC_VCI_CLK_256kHz=3,
    HW_CODEC_VCI_CLK_512kHz=4,
    HW_CODEC_VCI_CLK_1000kHz=5,
    HW_CODEC_VCI_CLK_1024kHz=6
} t_VCI_SCO_CLK_Freq;

u_int8 HWcodec_VCI_CLK_Override(t_VCI_SCO_CLK_Freq clk_freq);
boolean HWcodec_Is_VCI_CLK_Override_Enabled();
void HWcodec_VCI_CLK_Override_Enable(u_int8 vci_clk_sel, u_int8 vci_clk_sel_map, u_int8 codec_type, u_int8 sco_cfg0);
void HWcodec_VCI_CLK_Override_Disable();
#endif

#endif
