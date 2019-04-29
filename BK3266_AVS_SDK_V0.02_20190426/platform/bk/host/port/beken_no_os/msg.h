#ifndef _BEKEN_MSG_H_
#define _BEKEN_MSG_H_

#include "types.h"
#include "os_port.h"

#define MSG_DEBUG

#ifdef MSG_DEBUG
    #define MSG_PRT                 INFO_PRT
#else
    #define MSG_PRT 
#endif

#define MSG_COUNT                   (32)
#define MSG_SECURITY_BOUNDARY_NUM   (4)
#define MSG_HDR_MAGIC_WORD          (0x67676767)
#define MSG_TAIL_MAGIC_WORD         (0x78787878)

#define MSG_POOL_INIT               (1)
#define MSG_POOL_UNINIT             (2)
#define MSG_POOL_FULL               (3)
#define MSG_POOL_ADSUM              (4)
#define MSG_POOL_EMPTY              (5)

typedef struct
{
    MSG_T *borderline;
    MSG_T *pool;
    uint32 count;

    uint32 status;

    volatile uint32 out_id;
    volatile uint32 in_id;

    #ifdef DELAY_MSG_PUT
    jtask_h msg_task;
    MSG_T   arg;
    #endif
}MESSAGE_ENTITY_T;

int msg_lush_put(MSG_T *msg_ptr);
RAM_CODE int msg_get(MSG_T *msg_ptr);
#endif
