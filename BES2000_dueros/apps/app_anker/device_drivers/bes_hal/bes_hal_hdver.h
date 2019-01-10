
#ifndef __BES_HAL_HD_VER_H__
#define __BES_HAL_HD_VER_H__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    BES_HW_VER_UNKONW = 0,
    BES_HW_VER_3163_DEMO,      //0
    BES_HW_VER_3163_TR1,
    BES_HW_VER_3163_VER1,
    BES_HW_VER_3163_VER2,
    BES_HW_VER_3163_VER3,
    BES_HW_VER_3163_VER4,
    BES_HW_VER_3163_MAX
}BES_HW_VER_TYPE_T;

uint8_t hardware_version_open(void);
BES_HW_VER_TYPE_T hardware_version_get_ver(void);
#ifdef __cplusplus
}
#endif
#endif