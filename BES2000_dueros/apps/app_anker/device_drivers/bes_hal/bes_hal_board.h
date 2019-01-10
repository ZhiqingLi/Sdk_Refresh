#ifndef __BES_HAL_BOARD_H__
#define __BES_HAL_BOARD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"
#include "hal_iomux.h"
#include "hal_gpio.h"
#include "hal_key.h"

#define HW_BOARD_BES_DEVELOP             0
#define HW_BOARD_ANKER_BT3163_DEMO       1
#define HW_BOARD_ANKER_BT3163            2
#define HW_BOARD_SEL  HW_BOARD_ANKER_BT3163   

#define LINEIN_SEQUENCE_APPLE       0 // plug in:low-> high; plug out: high-->low
#define LINEIN_SEQUENCE_SAMSUNG     1 // plug in:high-->low; plug out: low-> high

#if HW_BOARD_SEL == HW_BOARD_ANKER_BT3163_DEMO
#include "bes_anker_demo_board_io_def.h"
#elif HW_BOARD_SEL == HW_BOARD_ANKER_BT3163
#include "bes_anker_3163_board_io_def.h"
#endif

void bes_firmware_version_get(uint8_t* pVerStr);

void hal_iomux_set_i2c_bus(void);

#ifdef __ANKER_UI__
extern const struct HAL_KEY_GPIOKEY_CFG_T cfg_hw_gpio_key_cfg[CFG_HW_GPIOKEY_NUM];
// app_battery_ext_charger_detecter_cfg is for charing full detect function
extern const struct HAL_IOMUX_PIN_FUNCTION_MAP app_battery_ext_charger_detecter_cfg;
extern const struct HAL_IOMUX_PIN_FUNCTION_MAP app_battery_ext_charger_indicator_cfg;
#endif

typedef void (*gpio_intr_handle)(enum HAL_GPIO_PIN_T pin);

void gpio_intr_set_edge(enum HAL_GPIO_PIN_T pin, uint8_t down_edge, gpio_intr_handle intrHandle);
void hal_pa_output_ctrl(uint8_t flag);

void hal_charger_ctrl(uint8_t flag);
#ifdef HW_INT_CODEC_POWER_FROM_EXTRAL
void hal_codec_extral_power_ctrl(uint8_t flag);
#endif
void hal_light_ctrl_power_ctrl(uint8_t flag);
void hal_usb_5v_out_ctrl(uint8_t enable);
void switch_bt_blue_led_state(void);
void switch_led_bass_state(void);
int32_t hal_board_pre_init(void);
int32_t hal_board_init(void);
int32_t hal_board_late_init(void);
 bool linein_is_plug_in(void);
#ifdef BOARD_HW_MIC_CTRL_MODULE
void hal_mic_power_ctrl(uint8_t flag);
uint8_t hal_mic_power_state_get(void);
#endif

#ifdef EXTRAL_CODEC_AK7755_ENABLE
void hal_iomux_set_3163_spi_normal(void);
void ak7755_power_ctrl(uint8_t flag);
void ak7755_reset_ctrl(uint8_t flag);
#endif

void led_pin_init(void);
void power_red_led_set(bool state);
void power_red_led_switch_state(void);
void power_white_led_set(bool state);
void power_white_led_switch_state(void);
#ifdef __cplusplus
}
#endif
#endif