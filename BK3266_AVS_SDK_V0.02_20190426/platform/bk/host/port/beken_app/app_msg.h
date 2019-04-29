#ifndef _APP_MSG_H_
#define _APP_MSG_H_

#define AMSG_DEBUG

#ifdef AMSG_DEBUG
    #define AMSG_PRT      os_printf
#else
    #define AMSG_PRT 
#endif

extern uint16 input_number;

extern RAM_CODE void  BT_msg_handler(void);
extern RAM_CODE uint8 common_msg_handler(MSG_T *msg_ptr);
extern RAM_CODE uint8 pre_enter_mode_handle(uint32 mode);

#endif
