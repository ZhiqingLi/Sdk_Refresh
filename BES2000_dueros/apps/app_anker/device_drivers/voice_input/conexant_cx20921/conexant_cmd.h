#ifndef _CONEXANT_CMDS_H_
#define _CONEXANT_CMDS_H_

#include "plat_types.h"
#include "stdarg.h"
#ifdef VOICE_INPUT_MODULE_CX20921
/* control_ex.h */
typedef enum {
    CONTROL_APP_VERSION               =  3,
    CONTROL_APP_EXEC_FILE             =  4,
    CONTROL_APP_FW_UPGD               = 33,
    SOS_RESOURCE                      = 47,
    CONTROL_MGR_TUNED_MODES           = 85,
    CONTROL_MGR_CREATE_FILE         = 117,
    CONTROL_MGR_OPEN_FILE           = 118,
    CONTROL_MGR_OPEN_FILE_AT_BLK    = 119,
    CONTROL_MGR_CLOSE_FILE          = 120,
    CONTROL_MGR_DELETE_FILE         = 121,
    CONTROL_MGR_RW_FILE             = 122,
    CONTROL_MGR_STAT_FILE           = 123,
    CONTROL_MGR_CREATE_FILE_AT_BLK  = 124,
    CONTROL_MGR_DELETE_FILE_AT_BLK  = 125,
} ControlAppCommandCode;

#define CONTROL_APP_GET_VERSION             	CMD_GET(CONTROL_APP_VERSION)
#define CONTROL_MGR_READ_FILE  CMD_GET(CONTROL_MGR_RW_FILE)
#define CONTROL_MGR_WRITE_FILE CMD_SET(CONTROL_MGR_RW_FILE)

typedef struct CommandHeaderTag {
    int32_t   num_32b_words:16;
    uint32_t  command_id   :15;
    uint32_t  reply        : 1;
    uint32_t  mgr_module_id   ;
} CommandHeader;

#define COMMAND_OF_SIZE(n)   \
struct {                     \
    int32_t   num_32b_words:16;\
    uint32_t  command_id:15;   \
    uint32_t  reply:1;         \
    uint32_t  app_module_id;   \
    uint32_t  data[n] ;        \
}

// The maximum number of 32-bit data elements that a command can contain
#define MAX_COMMAND_SIZE 13

#define CMD_SET(item)       ((item) & ~0x0100)
#define CMD_GET(item)       ((item) |  0x0100)
#define CMD_PARAM(item)     ((item) |  0x0200)
#define CMD_CLR_PARAM(item) ((item) & ~0x0200)

#define CMD_MASK            (~(CMD_SET(0)|CMD_GET(0)|CMD_PARAM(0)))
#define CMD_ITEM(cmd)       ((cmd) & CMD_MASK)

#define CMD_REPLY           1
#if !defined _ID
#define _ID(a,b,c,d)        ((uint32_t)((((a)-0x20)<<8)|(((b)-0x20)<<14)|(((c)-0x20)<<20)|(((d)-0x20)<<26)))
#endif
#define APP_ID(a,b,c,d)     _ID(a,b,c,d)

// Retrieve the app and module id from an mgr_module_id
#define GET_MGR_ID(mgr_module_id)    ((mgr_module_id)&~0xFF)
#define GET_APP_ID(mgr_module_id)    GET_MGR_ID(mgr_module_id)
#define GET_MODULE_ID(mgr_module_id) ((mgr_module_id)& 0xFF)

// Reserved App IDs
#define APP_ID_BROADCAST     0xFFFFFF00 // to broadcast commands to all apps
#define APP_ID_SEARCH_MOD    _ID('S','R','C','H') // to Search all module apps for the module to send the command to

// Reserved module IDs
#define MODULE_ID_APP        0    // to send commands to the app
#define MODULE_ID_BROADCAST  0xFF // to broadcast commands to all modules

// The Command type may be used to point to commands of arbitrary
// sizes, for example:
// COMMAND_OF_SIZE(5) cmd
// Command *ptr = (Command *)&cmd;
typedef COMMAND_OF_SIZE(MAX_COMMAND_SIZE) Command ;

#define SENDCMD_RC_SUCCESS           0
#define SENDCMD_RC_INVALID_ARG_NUM  -1 // Invalid number of arguments
#define SENDCMD_RC_FAILED_SEND      -2 // Failed to send command
#define SENDCMD_RC_REPLY_TIMEOUT    -3 // Timed out waiting for a reply
#define SENDCMD_RC_CMD_ERROR        -4 // Succeeded to send command, which returned an error

int32_t SendCmd (Command *cmd, uint32_t  app_module_id, uint32_t  command_id, uint32_t num_32b_words, ...);

#define NARGS(...) (sizeof((int[]){0,##__VA_ARGS__})/sizeof(int)-1)

#define SENDCMD(cmd, app_module_id, command_id, ...) \
    SendCmd(cmd, (uint32_t)(app_module_id), (uint32_t)(command_id), NARGS(__VA_ARGS__),##__VA_ARGS__)
#endif
#endif // CMDS_H_
