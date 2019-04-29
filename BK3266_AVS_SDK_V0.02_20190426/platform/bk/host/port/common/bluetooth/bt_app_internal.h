/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef _BT_APP_H_
#define _BT_APP_H_

#include <jos.h>
#include <uw_lists.h>
#include <bt_api.h>
#include "driver_beken_includes.h"

/* TODO: fixme, forward declaration of j_snscanf, as we can't include
 * str_utils.h */
uint32_t j_snscanf(const char *buf, char **end_ptr, char *format, ...);

#define NOTIFY_BUFFER_SIZE              256

/* TODO: Migrate app context here? */

#define BTADDR_TMP(a) \
    &(a)[5], &(a)[4], &(a)[3], &(a)[2], &(a)[1], &(a)[0]

#define TMP_TO_BTADDR(t, a) do { \
    (a)->b[0] = (t)[0]; (a)->b[1] = (t)[1]; (a)->b[2] = (t)[2]; \
    (a)->b[3] = (t)[3]; (a)->b[4] = (t)[4]; (a)->b[5] = (t)[5]; } while(0)

#define SKIP_SPACES(params) \
    while (*(params) && (*(params) == ' ' || *(params) == '\t')) \
        params++;

#define NEXT_PARAM(params) \
do \
{ \
    while (*(params) && !j_is_space(*(params))) \
        params++; \
    SKIP_SPACES(params); \
} while (0)

/* TODO: typedef btaddr_tmp? gets used widely */

typedef result_t (*bt_app_cmd_handler_t)(char *params, unsigned int len);

/* Command description */
typedef struct bt_app_cmd_t {
    /* Command name */
    char *name;
    char *desc;
    char *long_desc; /* Long descriptive text, usage, etc. */
    bt_app_cmd_handler_t handler;
} bt_app_cmd_t;

/* Section description */
typedef struct bt_app_section_t {
    /* Section name */
    char *name;
    char *desc;
    /* Pointer to array of commands */
    bt_app_cmd_t *cmds;
    unsigned int cmd_count;
    LIST_ENTRY(bt_app_section_t) next;
} bt_app_section_t;

typedef uint32_t app_unit_id_t;

result_t backend_section_register(bt_app_section_t *section);
void backend_section_unregister(bt_app_section_t *section);

result_t backend_unit_lookup_id(app_unit_id_t id, bt_unit_h *unit_h);
result_t backend_unit_get_addr(app_unit_id_t id, btaddr_t *addr);

char *backend_unit_get_name(const btaddr_t *raddr);
result_t backend_unit_remove_the_name(const btaddr_t *raddr);
void backend_unit_remove_name(void);

#ifdef BEKEN_DEBUG
void bt_frontend_notification(const char *fmt, ...);
#else
#define bt_frontend_notification  while(0) os_printf
#endif

/* utility functions */
bt_svc_type_t str_to_svc(const char *str);
const char *svc_to_str(bt_svc_type_t svc);

result_t    frontend_init(void);
void        frontend_uninit(void);

/* Functions used by frontend and backend to communicate */
result_t    process_command(char *cmdline, unsigned int len);
#ifdef BEKEN_DEBUG
result_t    frontend_notify(const char *data, unsigned int len);
#endif

/* Unit backend */
result_t    unit_backend_init(void);
void        unit_backend_uninit(void);

#ifdef CONFIG_BLUETOOTH_SDAP
/* SDAP backend */
result_t    sdap_backend_init(void);
void        sdap_backend_uninit(void);
#endif

#ifdef CONFIG_BLUETOOTH_HSP
/* HSP backend */
result_t    hs_backend_init(void);
void        hs_backend_uninit(void);
#endif

#ifdef CONFIG_BLUETOOTH_HFP
/* HF backend */
result_t    hf_backend_init(void);
void        hf_backend_uninit(void);
#endif

#ifdef CONFIG_BLUETOOTH_AVRCP
result_t    avrcp_backend_init(void);
void        avrcp_backend_uninit(void);
#endif

#ifdef CONFIG_BLUETOOTH_PAN
result_t    pan_backend_init(void);
void        pan_backend_uninit(void);
#endif

#ifdef CONFIG_BLUETOOTH_TESTS
/* Tests backend */
result_t    tests_backend_init(void);
void        tests_backend_uninit(void);
#endif

#ifdef CONFIG_BLUETOOTH_PBAP
/* PBAP backend */
result_t    pbap_backend_init(void);
void        pbap_backend_uninit(void);
#endif

#ifdef CONFIG_BLUETOOTH_OPUSH
/* OPUSH backend */
result_t    opush_backend_init(void);
void        opush_backend_uninit(void);
#endif

#ifdef CONFIG_BLUETOOTH_MAP
/* MAP backend */
result_t    map_backend_init(void);
void        map_backend_uninit(void);
#endif

#ifdef CONFIG_BLUETOOTH_A2DP
/* A2DP backend */
result_t    a2dp_backend_init(void);
void        a2dp_backend_uninit(void);
#ifdef CONFIG_TWS
result_t a2dp_src_backend_init(void);
void a2dp_src_backend_uninit(void);
#endif
#endif
#ifdef CONFIG_BLUETOOTH_HID
result_t hid_backend_init(void);
uint16_t getDescriptorLength(void);

#endif

#ifdef CONFIG_BLUETOOTH_SPP
/* SPP backend */
result_t    spp_backend_init(void);
void        spp_backend_uninit(void);
#endif

#ifdef CONFIG_BLUETOOTH_DUN_DT
/* DUN backend */
result_t    dun_backend_init(void);
void        dun_backend_uninit(void);
#endif

#ifdef CONFIG_BLUETOOTH_WIAP
/* WIAP backend */
result_t    wiap_backend_init(void);
void        wiap_backend_uninit(void);
#endif

#endif
