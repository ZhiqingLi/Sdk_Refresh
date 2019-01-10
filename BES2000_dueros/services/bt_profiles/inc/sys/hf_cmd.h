/****************************************************************************
 *
 * File:
 *     $Id: hf_cmd.h 1378 2009-04-14 23:22:21Z brentk $
 *     $Product: BES Hands-Free SDK version 2.x $
 *     $Revision: 1378 $
 *
 * Description: This file contains the header file for the command queuing 
 *     functions of the Hands-free SDK.  
 *
 * Created:     February 11, 2005
 *
 * Copyright 2000-2005 Extended Systems, Inc.  
 * Portions copyright BES.  
 * All rights reserved.  All unpublished rights reserved.  
 *
 * Unpublished Confidential Information of BES.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of 
 * BES. which is protected by copyright, trade secret, 
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#ifndef __HF_CMD_H_
#define __HF_CMD_H_

#include "sys/hfi.h"

BtStatus   HfAddCommand(        HfChannel *Chan, HfCommand *Command);
BtStatus   HfCompleteCommand(   HfChannel *Chan, HfCommand *Command);
BtStatus   HfExecuteNextCommand(HfChannel *Chan);
BtStatus   HfRemoveCommand(     HfChannel *Chan, HfCommand *Command);
HfCommand *HfEnumCommands(      HfChannel *Chan, HfCommand *Command);
HfCommand *HfGetCurrentCommand( HfChannel *Chan);
void       HfFlushCommands(     HfChannel *Chan, BtStatus Status);

#endif /* __HF_CMD_H_ */ 

