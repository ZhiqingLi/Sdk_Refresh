
#ifndef _CEVA_LE_GAP_CONST_H__
#define _CEVA_LE_GAP_CONST_H__

/*********************************************************************
 * MODULE NAME:     le_gap.h
 * PROJECT CODE:    BlueStream
 * DESCRIPTION:
 * MAINTAINER:      Gary Fleming
 * DATE:            Jan-2012
 * SOURCE CONTROL:
 *
 * LICENSE:
 *     This source code is copyright (c) 1999-2012 Ceva Inc.
 *     All rights reserved.
 *
 *********************************************************************/
#include "config.h"
#ifdef BT_DUALMODE
/************************************
 *   GAP TIMERS
 ************************************/

#define TGAP_lim_adv_timeout    0xC000 // Slots // 30.72 s Maximum time to remain advertising when in the limited discoverable mode
#define TGAP_gen_disc_scan_min  0x4000 // 10.24 s Minimum time to perform scanning when performing the general discovery procedure
#define TGAP_gen_disc_scan_int  0x0012 // 11.25 ms Scan interval used in the general discovery procedure
#define TGAP_gen_disc_scan_wind 0x0012 // 11.25 ms Scan window used in the general discovery procedure
#define TGAP_lim_disc_scan_min  0x4000 // 10.24 s Minimum time to perform scanning when performing the limited discovery procedure
#define TGAP_lim_disc_scan_int  0x0012 // 11.25 ms Scan interval used in the limited discovery procedure
#define TGAP_lim_disc_scan_wind 0x0012 // 11.25 ms Scan window used in the limited discovery procedure
#define TGAP_conn_param_timeout 0xBB80 // 30 s Connection parameter update notification timer when performing the connection
                                // parameter update procedure
#define TGAP_conn_est_latency   0 // 0 Connection slave latency in any connection establishment procedure

// For Testing against the PTS we have shortened this to match the PIXIT value of 5000
#if (PRH_BS_CFG_SYS_TESTING_AGAINST_PTS4_5==1)
#define TGAP_private_addr_int   (1600*5)  // 5 Seconds
#else
#define TGAP_private_addr_int   0x15F900// 15 mins Minimum time interval between private address change
#endif

#define TGAP_lim_disc_adv_int_min 0x0190 //250 ms Minimum advertising interval when in the limited discoverable mode
#define TGAP_lim_disc_adv_int_max 0x0360// 500 ms Maximum advertising interval when in the limited discoverable mode

#if 0//charles modify 20131205// UPF44 - Changed to Smaller Intervals for Apple at 
#define TGAP_gen_disc_adv_int_min 0x0800 //1.28 s Minimum advertising interval when in the general discoverable mode
#define TGAP_gen_disc_adv_int_max 0x1000 // 2.56 s Maximum advertising interval when in the general discoverable mode
#else
#define TGAP_gen_disc_adv_int_min 160//160//480//160//0x0800 //160 * 625 us//1.28 s Minimum advertising interval when in the general discoverable mode
#define TGAP_gen_disc_adv_int_max 160//160//480//160//0x000 // 160 * 625 us//2.56 s Maximum advertising interval when in the general discoverable mode
#define TGAP_adv_AUX_value ((TGAP_gen_disc_adv_int_min*625 /1000)- 5)*10/25 ;
#endif

#define TPRTH_dir_conn_init_scan_interval 0x0200 // PROPIETRARY 0.32sec Initiating Scan interval used in the direct connection establishment procedure
#define TPRTH_dir_conn_init_scan_window   0x0024 // PROPIETRARY 22.5ms Initiating Scan Window used in the direct connection establishment procedure

#define TGAP_scan_fast_period     49152
#define TGAP_scan_fast_interval_max     96
#define TGAP_scan_fast_interval_min     48
#define TGAP_scan_fast_window       48

#define TGAP_scan_slow_interval1   2048
#define TGAP_scan_slow_window1      18

#define TGAP_scan_slow_interval2   4096
#define TGAP_scan_slow_window2      36

#define TGAP_adv_fast_period       (30*1600)
#define TGAP_adv_fast_interval_max  96
#define TGAP_adv_fast_interval_min  48

#define TGAP_adv_slow_interval_max     1600
#define TGAP_adv_slow_interval_min     1920

#define TGAP_initial_conn_interval_max   80
#define TGAP_initial_conn_interval_min   48

#define TGAP_conn_pause_central    1600
#define TGAP_conn_pause_peripheral 8000

#define TGAP_conn_param_timeout  0xBB80 // 30 s Timer used in connection parameter update procedure
/**********
 * FLAGS
 **********/

#define GAP_LE_LIMITED_DISCOVERABLE_MODE      0x01
#define GAP_LE_GENERAL_DISCOVERABLE_MODE      0x02
#define GAP_LE_BR_EDR_NOT_SUPPORTED           0x04
#define GAP_LE_SIMULTANEOUS_LE_BR_EDR         0x08
#define GAP_LE_SIMULTANEOUS_LE_BR_EDR_HOST    0x10

#define GAP_LE_AD_FLAGS        0x01


#define GAP_DIRECT_CONNECTABLE     0x01
#define GAP_UNDIRECTED_CONNECTABLE 0x02
#define GAP_NON_CONNECTABLE        0x03

#define GAP_NO_CONNECT             0x00
#define GAP_AUTO_CONNECT           0x01
#define GAP_DIRECTED_CONNECT       0x02
#define GAP_GENERAL_CONNECT        0x03
#define GAP_SELECTIVE_CONNECT      0x04
#define GAP_FIND_CONNECTABLE_DEVICES 0x05

// GAP Connect States
#define GAP_CONNECT_IDLE           0x01


// GAP Connect State Machine - States [ Optional ]
#define GAP_IDLE                                          0x00
#define GAP_W4_PRIVACY_FLAG_READ                          0x01
#define GAP_W4_PRIVACY_FLAG_READ_COMMAND_COMPLETE         0x02
#define GAP_W4_READ_RECONNECTION_ADDRESS_HANDLE_COMPLETE  0x03
#define GAP_W4_READ_RECONNECTION_ADDRESS_HANDLE           0x04

#define GAP_W4_RECONNECT_ADDRESS_WRITE                    0x05
#define GAP_W4_WRITE_RECONNECTION_ADDRESS_COMPLETE        0x06

#define GAP_W4_READ_RECONNECTION_ADDRESS                  0x07
#define GAP_W4_READ_RECONNECTION_ADDRESS_COMPLETE         0x08
#define GAP_W4_HCI_ENCRYPTION_CHANGE_EVENT                0x09
#define GAP_W4_PEER_GAP_HANDLE_RANGE                      0x0A
#define GAP_W4_WRITE_PRIVACY_FLAG_COMPLETE                0x0B
#define GAP_W4_READ_PRIVACY_FLAG_HANDLE                   0x0C
#define GAP_W4_READ_PRIVACY_FLAG_HANDLE_COMPLETE          0x0D

#endif

#endif
