#ifndef _PARTHUS_LM_FEATURES_
#define _PARTHUS_LM_FEATURES_

/**************************************************************************
 * MODULE NAME:    lm_features.h
 * PROJECT CODE:   BlueStream
 * DESCRIPTION:    Manipulatation of the features
 * MAINTAINER:     Tom Kerwick
 * DATE:           22-12-1999
 *
 * SOURCE CONTROL: $Id: lmp_features.h,v 1.18 2012/09/28 11:47:20 tomk Exp $
 *
 * LICENSE:
 *   This source code is copyright (c) 2000-2007 Ceva Inc.
 *   All rights reserved.
 *
 *************************************************************************/

#include "sys_types.h"
#include "lmp_types.h"

typedef u_int8 t_features[8];

/* BYTE0 FEATURES MASK */
#define SLOT_3_PACKETS_MASK            0x01
#define SLOT_5_PACKETS_MASK            0x02
#define ENCRYPTION_MASK                0x04
#define SLOT_OFFSET_MASK               0x08
#define TIMING_ACCURACY_MASK           0x10
#define SWITCH_ROLE_MASK               0x20
#define HOLD_MODE_MASK                 0x40
#define SNIFF_MODE_MASK                0x80

/* BYTE1 FEATURES MASK */
#define PARK_MODE_MASK                 0x01
#define RSSI_MASK                      0x02
#define RATE_CHANGE_MASK               0x04
#define SCO_LINK_MASK                  0x08
#define HV2_PACKETS_MASK               0x10
#define HV3_PACKETS_MASK               0x20
#define U_LAW_MASK                     0x40
#define A_LAW_MASK                     0x80

/* BYTE2 FEATURES MASK */
#define CVSD_MASK                      0x01
#define PAGING_SCHEME_MASK             0x02
#define POWER_CONTROL_MASK             0x04
#define TRANSPARENT_SYN_DATA_MASK      0x08
/* skip flow control lag               0x10*/
/* skip flow control lag               0x20*/
/* skip flow control lag               0x40*/
#define BROADCAST_ENCRYPTION_MASK      0x80

/* BYTE3 FEATURES MASK */

#define SCATTER_MODE_MASK              0x01 /* note: still officially reserved in BT2.1+EDR */
#define EDR_ACL_2MBPS_MODE_MASK        0x02
#define EDR_ACL_3MBPS_MODE_MASK        0x04
#define ENHANCED_INQ_SCAN_MASK         0x08
#define INTERLACED_INQ_SCAN_MASK       0x10
#define INTERLACED_PAGE_SCAN_MASK      0x20
#define RSSI_WITH_INQ_RESULTS_MASK     0x40
#define ESCO_LINK_MASK                 0x80

/* BYTE4 FEATURES MASK */
#define EV4_PACKETS_MASK               0x01
#define EV5_PACKETS_MASK               0x02
/* skip reserved                       0x04*/
#define AFH_CAPABLE_SLAVE_MASK         0x08
#define AFH_CLASSIFICATION_SLAVE_MASK  0x10
#define NO_CLASSIC_BT_MASK             0x20
#define LE_CONTROLLER_MASK             0x40
#define SLOT_3_EDR_ACL_PACKETS_MASK	   0x80

/* BYTE5 FEATURES MASK */
#define SLOT_5_EDR_ACL_PACKETS_MASK	   0x01
#define SNIFF_SUBRATING_MASK           0x02
#define PAUSE_ENCRYPTION               0x04
#define AFH_CAPABLE_MASTER_MASK        0x08
#define AFH_CLASSIFICATION_MASTER_MASK 0x10
#define EDR_ESCO_2MBPS_MODE_MASK       0x20
#define EDR_ESCO_3MBPS_MODE_MASK       0x40
#define SLOT_3_EDR_ESCO_PACKETS_MASK   0x80

/* BYTE6 FEATURES MASK */
#define EXTENDED_INQUIRY_RESPONSE_MASK 0x01
#define SIMULTANEOUS_LE_BR_EDR_MASK    0x02
/* skip reserved                       0x04*/
#define SECURE_SIMPLE_PAIRING_MASK     0x08
#define ENCAPSULATED_PDU_MASK          0x10
#define ERRONOUS_DATA_REPORTING_MASK   0x20
#define NONFLUSHABLE_PBF_MASK          0x40
/* skip reserved                       0x80*/

/* BYTE7 FEATURES MASK */
#define LSTO_CHANGED_EVENT_MASK        0x01
#define INQUIRY_RESPONSE_TX_PWR_MASK   0x02
#define ENHANCED_POWER_CONTROL_MASK    0x04
/* skip reserved                       0x04*/
/* skip reserved                       0x08*/
/* skip reserved                       0x10*/
/* skip reserved                       0x20*/
/* skip reserved                       0x40*/
#define EXTENDED_FEATURES_MASK         0x80


#define mFeat_Check_3_Slot(features)                    ( features[0] & SLOT_3_PACKETS_MASK )
#define mFeat_Check_5_Slot(features)                    ( features[0] & SLOT_5_PACKETS_MASK )
#define mFeat_Check_Encryption(features)                ( features[0] & ENCRYPTION_MASK     )
#define mFeat_Check_Slot_Offset(features)               ( features[0] & SLOT_OFFSET_MASK    )
#define mFeat_Check_Timing_Accuracy(features)           ( features[0] & TIMING_ACCURACY_MASK)
#define mFeat_Check_Role_Switch(features)               ( features[0] & SWITCH_ROLE_MASK    )
#define mFeat_Check_Hold_Mode(features)                 ( features[0] & HOLD_MODE_MASK      )
#define mFeat_Check_Sniff_Mode(features)                ( features[0] & SNIFF_MODE_MASK     )

#define mFeat_Check_Park_State(features)                ( features[1] & PARK_MODE_MASK      )
#define mFeat_Check_RSSI(features)                      ( features[1] & RSSI_MASK           )
#define mFeat_Check_CQDDR(features)                     ( features[1] & RATE_CHANGE_MASK    )
#define mFeat_Check_Sco_Link(features)                  ( features[1] & SCO_LINK_MASK       )
#define mFeat_Check_HV2_Packets(features)               ( features[1] & HV2_PACKETS_MASK    )
#define mFeat_Check_HV3_Packets(features)               ( features[1] & HV3_PACKETS_MASK    )
#define mFeat_Check_U_Law(features)                     ( features[1] & U_LAW_MASK          )
#define mFeat_Check_A_Law(features)                     ( features[1] & A_LAW_MASK          )

#define mFeat_Check_CVSD(features)                      ( features[2] & CVSD_MASK           )
#define mFeat_Check_Paging_System(features)             ( features[2] & PAGING_SCHEME_MASK  )
#define mFeat_Check_Power_Control(features)             ( features[2] & POWER_CONTROL_MASK  )
#define mFeat_Check_Transparent_Sync_Data(features)     ( features[2] & TRANSPARENT_SYN_DATA_MASK )
#define mFeat_Check_Broadcast_Encryption(features)      ( features[2] & BROADCAST_ENCRYPTION_MASK )

#define mFeat_Check_Scatter_Mode(features)              ( features[3] & SCATTER_MODE_MASK   )
#define mFeat_Check_EDR_ACL_2MBPS_Mode(features)        ( features[3] & EDR_ACL_2MBPS_MODE_MASK )
#define mFeat_Check_EDR_ACL_3MBPS_Mode(features)        ( features[3] & EDR_ACL_3MBPS_MODE_MASK )
#define mFeat_Check_Enhanced_Inquiry_Scan(features)     ( features[3] & ENHANCED_INQ_SCAN_MASK )
#define mFeat_Check_Interlaced_Inquiry_Scan(features)   ( features[3] & INTERLACED_INQ_SCAN_MASK )
#define mFeat_Check_Interlaced_Page_Scan(features)      ( features[3] & INTERLACED_PAGE_SCAN_MASK )
#define mFeat_Check_RSSI_With_Inquiry_Results(features) ( features[3] & RSSI_WITH_INQ_RESULTS_MASK )
#define mFeat_Check_eSco_Link(features)                 ( features[3] & ESCO_LINK_MASK      )

#define mFeat_Check_EV4_Packets(features)               ( features[4] & EV4_PACKETS_MASK    )
#define mFeat_Check_EV5_Packets(features)               ( features[4] & EV5_PACKETS_MASK    )
#define mFeat_Check_AFH_Capable_Slave(features)         ( features[4] & AFH_CAPABLE_SLAVE_MASK )
#define mFeat_Check_AFH_Classification_Slave(features)  ( features[4] & AFH_CLASSIFICATION_SLAVE_MASK )
#define mFeat_Check_No_Classic_BT(features)				( features[4] & NO_CLASSIC_BT_MASK )
#define mFeat_Check_LE_Controller(features)             ( features[4] & LE_CONTROLLER_MASK )
#define mFeat_Check_3_Slot_EDR_ACL_Packets(features)    ( features[4] & SLOT_3_EDR_ACL_PACKETS_MASK )

#define mFeat_Check_5_Slot_EDR_ACL_Packets(features)    ( features[5] & SLOT_5_EDR_ACL_PACKETS_MASK )
#define mFeat_Check_Sniff_Subrating(features)           ( features[5] & SNIFF_SUBRATING_MASK )
#define mFeat_Check_Pause_Encryption(features)          ( features[5] & PAUSE_ENCRYPTION )
#define mFeat_Check_AFH_Capable_Master(features)         ( features[5] & AFH_CAPABLE_MASTER_MASK )
#define mFeat_Check_AFH_Classification_Master(features)  ( features[5] & AFH_CLASSIFICATION_MASTER_MASK )
#define mFeat_Check_EDR_eSco_2MBPS_Mode(features)       ( features[5] & EDR_ESCO_2MBPS_MODE_MASK )
#define mFeat_Check_EDR_eSco_3MBPS_Mode(features)       ( features[5] & EDR_ESCO_3MBPS_MODE_MASK )
#define mFeat_Check_3_Slot_EDR_eSco_Packets(features)   ( features[5] & SLOT_3_EDR_ESCO_PACKETS_MASK )

#define mFeat_Check_Extended_Inquiry_Response(features) ( features[6] & EXTENDED_INQUIRY_RESPONSE_MASK )
#define mFeat_Check_Simultaneous_LE_BR_EDR(features)    ( features[6] & SIMULTANEOUS_LE_BR_EDR_MASK )
#define mFeat_Check_Secure_Simple_Pairing(features)     ( features[6] & SECURE_SIMPLE_PAIRING_MASK )
#define mFeat_Check_Encapsulated_PDU(features)          ( features[6] & ENCAPSULATED_PDU_MASK )
#define mFeat_Check_Erronous_Data_Reporting(features)   ( features[6] & ERRONOUS_DATA_REPORTING_MASK )
#define mFeat_Check_NonFlushable_PBF(features)          ( features[6] & NONFLUSHABLE_PBF_MASK )

#define mFeat_Check_LSTO_Changed_Event(features)        ( features[7] & LSTO_CHANGED_EVENT_MASK )
#define mFeat_Check_Inquiry_Response_Tx_Pwr(features)   ( features[7] & INQUIRY_RESPONSE_TX_PWR_MASK )
#define mFeat_Check_Extended_Features(features)         ( features[7] & EXTENDED_FEATURES_MASK )
#define mLMPfeat_Is_Remote_Feature_Supported(feature_set, lmp_feature_id) \
        ((feature_set)[(lmp_feature_id)>>8] & (u_int8) (lmp_feature_id&0xFF))

/* 
 * CM: May 2009
 * To allow the dymanic enabling/disabling of this feature during testing
 */
//#define mFeat_Check_Enhanced_Power_Control(features)    ( features[7] & ENHANCED_POWER_CONTROL_MASK)
boolean  mFeat_Check_Enhanced_Power_Control(u_int8 *features);

#endif

