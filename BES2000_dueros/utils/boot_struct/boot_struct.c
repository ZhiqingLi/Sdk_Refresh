#include "tool_msg.h"
#include "reboot_param.h"
#include "norflash_cfg.h"

extern const char sys_build_info[];

#ifdef USER_SECURE_BOOT
extern const unsigned int system_info;
#endif

// -----------------------------------------------------------
// Boot struct and code sig struct
// -----------------------------------------------------------

#define BOOT_STRUCT_LOC __attribute((section(".boot_struct")))

#if defined(SECURE_BOOT) || defined(PROGRAMMER)

struct combined_boot_struct_t {
    struct boot_struct_t boot_struct;
    struct code_sig_struct_t code_sig_struct;
#ifndef PROGRAMMER
    struct norflash_cfg_struct_t norflash_cfg;
#endif
};

const struct combined_boot_struct_t BOOT_STRUCT_LOC combined_boot_struct = {
    .boot_struct = {
        .hdr = {
#ifdef PROGRAMMER
            .magic = BOOT_MAGIC_NUMBER,
#else
            .magic = ~0,
#endif
            .security = 1,
            .hash_type = BOOT_HASH_TYPE_SHA256,
            .key_type = BOOT_KEY_TYPE_RSA2048,
            .key_len = KEY_LEN,
            .sig_len = SIG_LEN,
#ifndef PROGRAMMER
#ifdef USER_SECURE_BOOT
            .build_info_start = (uint32_t)&system_info,
#else
            .build_info_start = (uint32_t)sys_build_info,
#endif
#endif
        },
    },
    .code_sig_struct = {
        .code_size = 0,
        .sig_len = SIG_LEN,
    },
#ifndef PROGRAMMER
    .norflash_cfg = {
        .neg_phase = 0,
        .pos_neg   = 0,
        .cmdquad   = 0,
        .div       = 0x2, /* div is 2 at least */
#ifdef SIMU
        .samdly    = 0x1,
#else
        .samdly    = 0x2,
#endif
        .dualmode  = 1,   /* dual mode */
        .holdpin   = 0,
        .wprpin    = 0,
        .quadmode  = 0,
#ifdef SIMU
        // Select 26M for 40M crystal case
        .mod_clk   = HAL_CMU_FREQ_26M,
#else
        .mod_clk   = HAL_CMU_FREQ_104M,
#endif
        .spiruen   = 0,
        .spirden   = 0,
        .dualiocmd = 0xBB, /* used */
#ifdef SIMU
        .rdcmd     = 0x03, // Below 50M: 0x03, Above 50M: 0x0B
#else
        .rdcmd     = 0x0B, // Below 50M: 0x03, Above 50M: 0x0B
#endif
        .frdcmd    = 0x0B,
        .qrdcmd    = 0xEB,
    },
#endif
};

#else

const struct boot_hdr_t BOOT_STRUCT_LOC boot_struct = {
    .magic = ~0,
    .security = 0,
    .hash_type = 0,
    .key_type = 0,
    .key_len = 0,
    .sig_len = 0,
#ifndef PROGRAMMER
#ifdef USER_SECURE_BOOT
    .build_info_start = (uint32_t)&system_info,
#else
    .build_info_start = (uint32_t)sys_build_info,
#endif
#endif
};

#endif

// -----------------------------------------------------------
// Reboot param
// -----------------------------------------------------------

#define REBOOT_PARAM_LOC __attribute((section(".reboot_param")))

struct REBOOT_PARAM_T REBOOT_PARAM_LOC reboot_param;

