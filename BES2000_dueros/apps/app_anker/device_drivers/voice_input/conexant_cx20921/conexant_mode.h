#ifndef _CONEXANT_MODES_H_
#define _CONEXANT_MODES_H_

#ifdef __cplusplus
extern "C" {
#endif
#include "plat_types.h"
#ifdef VOICE_INPUT_MODULE_CX20921
typedef enum{
    CONEXANT_MODE_CAPTURE_16K_16B_STERO = 0,
    CONEXANT_MODE_CAPTURE_16K_16B_MONO,
    CONEXANT_MODE_STANDBY,
    CONEXANT_MODE_SLEEP,
    CONEXANT_MODE_MAX
}CONEXANT_WORK_MODE_T;

#define CONEXANT_CAPTURE_MODE_TYPE      (CONEXANT_MODE_CAPTURE_16K_16B_STERO)
#define CONEXANT_STOP_MODE_TYPE         (CONEXANT_MODE_STANDBY)

/*pVersion assigned a mem area size of uin32_t*4 */
int32_t get_fw_version (uint32_t *pVersion);
int32_t add_mode(uint32_t id);
int32_t get_mode_id(int32_t index);
uint8_t* get_mode_name(int32_t index);
int32_t get_num_of_avail_modes (void);
void init_num_of_avail_modes(void);
int32_t get_tuned_modes(void);
int32_t get_current_mode (uint32_t* pCurMode);
int32_t get_angle_of_arrival(float* angle_of_arrival);
int32_t switch_to_mode (uint32_t new_mode);
int32_t conexant_firmware_version_get(uint8_t* pVerStr);
int32_t conexant_mode_init(void);
int32_t conexant_mode_deinit(void);
int32_t conexant_mode_change(CONEXANT_WORK_MODE_T mode);
#endif
#ifdef __cplusplus
}
#endif
#endif