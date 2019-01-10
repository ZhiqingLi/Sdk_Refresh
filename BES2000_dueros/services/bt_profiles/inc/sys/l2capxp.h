/***************************************************************************
 *
 * File:
 *     $Id: l2capxp.h 588 2009-01-20 23:25:48Z gladed $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 588 $
 *
 * Description:
 *     Funtion exports for lower layer (ME).
 *
 * Created:
 *     July 15, 1999
 *
 * Copyright 1999-2005 Extended Systems, Inc.
 * Portions copyright BES.
 * All rights reserved. All unpublished rights reserved.
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

#ifndef __L2CAPXP_H
#define __L2CAPXP_H

#include "bttypes.h"

/*---------------------------------------------------------------------------
 * Prototype:     BtStatus L2CAP_Init()
 *
 * Descriptions:  Initialize the L2CAP Protocol.
 *
 * Parameters:    void
 *
 * Returns:       BT_STATUS_SUCCESS - L2CAP is initialized.
 *
 *                BT_STATUS_FAILED - L2CAP initialization failed.
 */
BtStatus L2CAP_Init(void);


/*---------------------------------------------------------------------------
 * Prototype:     void L2CAP_Deinit()
 *
 * Description:   Denitialize the L2CAP Protocol.
 *
 * Parameters:    void
 *
 * Returns:       void
 */
void L2CAP_Deinit(void);


/*---------------------------------------------------------------------------
 * Prototype:     void L2CAP_HciReceive()
 *
 * Description:   Exported for ME Only.
 *
 * Parameters:    
 *
 * Returns:       
 */
void L2CAP_HciReceive(HciBuffer *BuffDesc, HciHandle HciHndl);


/*---------------------------------------------------------------------------
 * Prototype:     BtPacket *L2CAP_HciTxReady()
 *
 * Description:   Exported for ME Only.
 *
 * Parameters:    
 *
 * Returns:       
 */
BtPacket *L2CAP_HciTxReady(HciHandle HciHndl);


/*---------------------------------------------------------------------------
 * Prototype:     void L2CAP_HciTxDone()
 *
 * Description:   Exported for ME Only.
 *
 * Parameters:    
 *
 * Returns:       
 */
void L2CAP_HciTxDone(BtPacket *Packet, BtStatus Status, HciHandle HciHndl);


/*---------------------------------------------------------------------------
 * Prototype:     void L2CAP_LinkDisconnect()
 *
 * Description:   Exported for ME Only.
 *
 * Parameters:    
 *
 * Returns:       
 */
void L2CAP_LinkDisconnect(BtRemoteDevice* remDev);

#endif

