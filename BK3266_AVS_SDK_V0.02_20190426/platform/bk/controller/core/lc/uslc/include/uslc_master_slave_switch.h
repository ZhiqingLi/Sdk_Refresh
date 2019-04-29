#ifndef _PARTHUS_USLC_MASTER_SLAVE_SWITCH_
#define _PARTHUS_USLC_MASTER_SLAVE_SWITCH_

/***********************************************************************
 *
 * HEADER NAME:    uslc_master_slave_switch.h
 * PROJECT CODE:   BlueStream
 * DESCRIPTION:    LC Portion of Master Slave Switch Procedure
 * MAINTAINER:     Ivan Griffin
 * CREATION DATE:  17 August 2000
 *
 * SOURCE CONTROL: $Id: uslc_master_slave_switch.h,v 1.8 2004/07/07 14:21:59 namarad Exp $
 *
 * LICENSE:
 *     This source code is copyright (c) 2002-2004 Ceva Inc.
 *     All rights reserved.
 *    
 ***********************************************************************/

void USLCmss_Initialise(void);
t_error USLCmss_Request(t_deviceIndex device_index);
t_error USLCmss_Cancel(void);
DRAM_CODE void USLCmss_State_Dispatcher(boolean);

#endif
