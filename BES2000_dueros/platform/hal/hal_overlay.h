#ifndef __HAL_OVERLAY_H__
#define __HAL_OVERLAY_H__

#ifdef __cplusplus
extern "C" {
#endif

#define HAL_OVERLAY_TEXT0_LOC       __attribute__((section(".overlay_text0")))
#define HAL_OVERLAY_TEXT1_LOC       __attribute__((section(".overlay_text1")))
#define HAL_OVERLAY_TEXT2_LOC       __attribute__((section(".overlay_text2")))
#define HAL_OVERLAY_TEXT3_LOC       __attribute__((section(".overlay_text3")))
#define HAL_OVERLAY_TEXT4_LOC       __attribute__((section(".overlay_text4")))
#define HAL_OVERLAY_TEXT5_LOC       __attribute__((section(".overlay_text5")))
#define HAL_OVERLAY_TEXT6_LOC       __attribute__((section(".overlay_text6")))
#define HAL_OVERLAY_TEXT7_LOC       __attribute__((section(".overlay_text7")))

#define HAL_OVERLAY_RODATA0_LOC     __attribute__((section(".overlay_rodata0")))
#define HAL_OVERLAY_RODATA1_LOC     __attribute__((section(".overlay_rodata1")))
#define HAL_OVERLAY_RODATA2_LOC     __attribute__((section(".overlay_rodata2")))
#define HAL_OVERLAY_RODATA3_LOC     __attribute__((section(".overlay_rodata3")))
#define HAL_OVERLAY_RODATA4_LOC     __attribute__((section(".overlay_rodata4")))
#define HAL_OVERLAY_RODATA5_LOC     __attribute__((section(".overlay_rodata5")))
#define HAL_OVERLAY_RODATA6_LOC     __attribute__((section(".overlay_rodata6")))
#define HAL_OVERLAY_RODATA7_LOC     __attribute__((section(".overlay_rodata7")))

#define HAL_OVERLAY_DATA0_LOC       __attribute__((section(".overlay_data0")))
#define HAL_OVERLAY_DATA1_LOC       __attribute__((section(".overlay_data1")))
#define HAL_OVERLAY_DATA2_LOC       __attribute__((section(".overlay_data2")))
#define HAL_OVERLAY_DATA3_LOC       __attribute__((section(".overlay_data3")))
#define HAL_OVERLAY_DATA4_LOC       __attribute__((section(".overlay_data4")))
#define HAL_OVERLAY_DATA5_LOC       __attribute__((section(".overlay_data5")))
#define HAL_OVERLAY_DATA6_LOC       __attribute__((section(".overlay_data6")))
#define HAL_OVERLAY_DATA7_LOC       __attribute__((section(".overlay_data7")))

enum HAL_OVERLAY_ID_T {
    HAL_OVERLAY_ID_0,
    HAL_OVERLAY_ID_1,
    HAL_OVERLAY_ID_2,
    HAL_OVERLAY_ID_3,
    HAL_OVERLAY_ID_4,
    HAL_OVERLAY_ID_5,
    HAL_OVERLAY_ID_6,
    HAL_OVERLAY_ID_7,

    HAL_OVERLAY_ID_QTY,
    HAL_OVERLAY_ID_IN_CFG,
};

enum HAL_OVERLAY_RET_T {
    HAL_OVERLAY_RET_OK,
    HAL_OVERLAY_RET_BAD_ID,
    HAL_OVERLAY_RET_IN_CFG,
    HAL_OVERLAY_RET_IN_USE,
};

enum HAL_OVERLAY_RET_T hal_overlay_load(enum HAL_OVERLAY_ID_T id);

enum HAL_OVERLAY_RET_T hal_overlay_unload(enum HAL_OVERLAY_ID_T id);

#ifdef __cplusplus
}
#endif

#endif

