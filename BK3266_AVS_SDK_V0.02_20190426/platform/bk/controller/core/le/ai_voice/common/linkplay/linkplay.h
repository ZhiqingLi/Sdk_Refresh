#ifndef __LINK_PLAY_H__
#define __LINK_PLAY_H__


typedef struct _LP_DEVICE_ID {
    uint8_t    id[2];
} LP_DEVICE_ID;


/*
 * 
 * Function description:The callback function of the current state of APP;
 * Parameter description: appState
 * 0x00 IDLE
 * 0x01 LISTENING
 * 0x02 PROCESSING
 * 0x03 SPEAKING
 * 0x04 ERROR
*/
void App_To_Device_UpdateAlexaState(uint8_t e_type, uint8_t e_state);

char* get_customer_firmware_version(void);
char* get_customer_project_name(void);
int   get_customer_device_id(LP_DEVICE_ID *device_id);

#endif //__LINK_PLAY_H__

