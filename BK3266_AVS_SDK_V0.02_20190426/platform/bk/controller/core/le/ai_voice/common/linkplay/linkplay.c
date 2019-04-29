/**
 ****************************************************************************************
 *
 * @file linkplay.c
 *
 * @brief Linkplay to the customer's interface.
 *
 * Copyright (C) RivieraWaves 2009-2018
 *
 *
 ****************************************************************************************
 */


/**
 ****************************************************************************************
 * @addtogroup LINKPLAY
 * @{
 ****************************************************************************************
 */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */
#include "driver_beken_includes.h"
#include "app_beken_includes.h"
#include "beken_external.h"
#include "linkplay.h"
#include "lp_type.h"

//The device information can be viewed on Alexa app
#define FIRWARE_VERSION     "0.1"        //The customer can define the version number by itself. len < 20
#define PROJECT_NAME        "BK3266"     //The customer can define the version number by itself. len < 20
#define DEVICE_ID           0x0021

char* get_customer_firmware_version(void)
{
    return FIRWARE_VERSION;
}

char* get_customer_project_name(void)
{
    return PROJECT_NAME;
}

int get_customer_device_id(LP_DEVICE_ID *device_id)
{
    if((device_id != NULL) && (DEVICE_ID>0 && DEVICE_ID<0xFFFF)){
        device_id->id[0]= (DEVICE_ID & 0xFF);
        device_id->id[1] = (DEVICE_ID & 0xFF00)>>8;
        return 0;
    }
    
    return -1;
}

void lp_beken_update_ui(uint16 ui_event)
{
    //os_printf("[UI] event:%d\r\n", ui_event);

    switch(ui_event)
    {
        case LP_ALEXA_UI_IDLE:
            break;

        case LP_ALEXA_UI_SPEECH_LISTENING:
            break;

        case LP_ALEXA_UI_SPEECH_THINKING:
            break;

        case LP_ALEXA_UI_SPEECH_SPEAKING:
            break;

        case LP_ALEXA_UI_SPEECH_ERROR:
            break;

        case LP_ALEXA_UI_NOTIFICATION_ACTIVED:
            break;

        case LP_ALEXA_UI_NOTIFICATION_WITH_MUTE:
            break;

        case LP_ALEXA_UI_ALARM:
            break;

        case LP_ALEXA_UI_MIC_MUTE:
            break;

        case LP_ALEXA_UI_LOST_CONNECTION:
            break;

        case LP_ALEXA_UI_LOST_NETWORK:
            break;

        case LP_ALEXA_UI_LOST_ACCOUNT:
            break;
    }
}
 
