#ifndef __HWFIR_HEADER__
#define __HWFIR_HEADER__

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HWFIR_CHANNEL_ID_0 = 0,
    HWFIR_CHANNEL_ID_1,
    HWFIR_CHANNEL_ID_2,
    HWFIR_CHANNEL_ID_3,
    HWFIR_CHANNEL_ID_4,
    HWFIR_CHANNEL_ID_5,
    HWFIR_CHANNEL_ID_6,
    HWFIR_CHANNEL_ID_7,
    HWFIR_CHANNEL_ID_QTY,
} HAL_HWFIR_CHANNEL_ID;

HAL_HWFIR_CHANNEL_ID hal_hwfir_get_id(void);
int hal_hwfir_acquire_id(HAL_HWFIR_CHANNEL_ID id);
unsigned int hal_hwfir_get_data_memory_addr(HAL_HWFIR_CHANNEL_ID id);
unsigned int hal_hwfir_get_coef_memory_addr(HAL_HWFIR_CHANNEL_ID id);
int hal_hwfir_free_id(HAL_HWFIR_CHANNEL_ID id);

#ifdef __cplusplus
}
#endif

#endif /* __HWFIR_HEADER__ */
