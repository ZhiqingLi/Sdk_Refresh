/*
*  (C) Copyright 2014-2016 Shenzhen South Silicon Valley microelectronics co.,limited
*
*  All Rights Reserved
*/


#ifndef _CLI_CMD_RF_TEST_H_
#define _CLI_CMD_RF_TEST_H_

typedef enum t_TAG_RADIOOP{
    CHANNEL 		= 0,
    WIFI_OPMODE,
    CONFIG11N,
    FIXDATARATE,
    AUTODATARATE,
    SECURITY,
    TX_POWER,
    AIFS,
    APSTA,
    SETMAC,
    FRAMETYPE,
    FRAMESIZE,
    FRAMEDATA,
    SEND_FRAME,
    BSSID,
    PEERMAC,
    SSID,
    RX_CNT_CLR,
    RX_DISABLE,
    START_TX,
    SET_COUNT,
    SET_DELAY,
    RF_START,
    RF_RATE,
    RF_BGAIN,
    RF_GNGAIN,
    RF_IQPHASE,
    RF_IQAMP,
    RF_STOP,
    RF_RESET,
    RF_COUNT,
} TAG_RADIOOP;

void cmd_rf_test(s32 argc, char *argv[]);
#endif //#ifndef _CLI_CMD_RF_TEST_H_
