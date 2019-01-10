#ifndef __APP_THIRDPARTY_H__
#define __APP_THIRDPARTY_H__

#define INVALID_THIRDPARTY_ENTRY_INDEX		0xFFFF

extern uint32_t __thirdparty_handler_table_start[];
extern uint32_t __thirdparty_handler_table_end[];


typedef enum
{
	THIRDPARTY_ID_DEMO,//Each library has a unique id
    THIRDPARTY_ID_NO1, 
    THIRDPARTY_ID_NO2,
    THIRDPARTY_ID_NUMBER,   
}THIRDPARTY_LIB_ID;


typedef enum
{
    THIRDPARTY_INIT = 0x0,
    THIRDPARTY_START,
    THIRDPARTY_STOP,
    THIRDPARTY_DEINIT,
    THIRDPARTY_BT_CONNECTABLE,
    THIRDPARTY_BT_DISCOVERABLE,
    THIRDPARTY_BT_CONNECTED,
    THIRDPARTY_A2DP_STREAMING,
    THIRDPARTY_HFP_SETUP,
    THIRDPARTY_MIC_OPEN,
    THIRDPARTY_MIC_CLOSE,
    THIRDPARTY_GET_PAGE_E_EFUSE,
    THIRDPARTY_GET_PAGE_F_EFUSE,
    THIRDPARTY_OTHER_EVENT,
    THIRDPARTY_EVENT_NUMBER,   
}THIRDPARTY_EVENT_TYPE;

typedef int (*APP_THIRDPARTY_HANDLE_CB_T)(unsigned char, void *param);


typedef struct {
    THIRDPARTY_LIB_ID id;
    THIRDPARTY_EVENT_TYPE event;
}APP_THIRDPARTY_SIGN;



typedef struct {
    APP_THIRDPARTY_SIGN thirdparty_sign;
    APP_THIRDPARTY_HANDLE_CB_T function;
	unsigned char status;
    void *param;
} APP_THIRDPARTY_HANDLE;

/*
//#define THIRDPARTY_EVENT_HANDLER_TAB_ADD(id_name,event_num)   const APP_THIRDPARTY_HANDLE id_name##_event_handler_tab[event_num] =

#define THIRDPARTY_EVENT_HANDLER_TAB_ADD(id_name,event_type,event_handle, status, param)	\
	static APP_THIRDPARTY_HANDLE id_name##event_type##_entry __attribute__((used, section(".thirdparty_handler_table"))) = 	\
		{{id_name,event_type},event_handle,status,param};
*/

#define THIRDPARTY_EVENT_HANDLER_TAB_ADD(id_name,id_NUM)	\
	static APP_THIRDPARTY_HANDLE id_name##_entry[id_NUM]  __attribute__((used, section(".thirdparty_handler_table"))) =



#define THIRDPARTY_EVENT_TYPE_PTR_FROM_ENTRY_INDEX(index)	\
	((APP_THIRDPARTY_HANDLE *)((uint32_t)__thirdparty_handler_table_start + (index)*sizeof(APP_THIRDPARTY_HANDLE)))


#ifdef __cplusplus
extern "C" {
#endif

int app_thirdparty_specific_lib_event_handle(THIRDPARTY_LIB_ID libId,THIRDPARTY_EVENT_TYPE event_type);


#ifdef __cplusplus
	}
#endif

#endif
