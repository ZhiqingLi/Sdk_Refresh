#include "common_func.h"
#include "key_alexa_led.h"

#if defined(SUPPORT_MIC_MUTE_FUNCTION)
void com_smart_mic_mute(uint8 flag, bool tone)
{
    act_writel(act_readl(GPIOAOUTEN) | (1 << 13), GPIOAOUTEN);
    if(flag == 1) {         //mic mute
#if !defined(BOARD_JL_36410_FAR)
        discrete_led_set(LED_ID_MIC_MUTE, LED_ALWAYS_ON, NULL);
#endif
#if defined(MIC_MUTE_GPIO_PIN)
        act_writel((act_readl(MIC_MUTE_GPIO_DATA) & (~MIC_MUTE_GPIO_PIN)), MIC_MUTE_GPIO_DATA);
#endif
#if defined(ENABLE_ALEXA_LED_NOTIFY)
        lp_update_alexa_state(ALEXA_STATE_TYPE_MIC_MUTE, 1);
#endif
        if(tone) {
            keytone_play(KEYTONE_MIC_MUTE, KEYTONE_BLOCK);
        }
    } else {
#if !defined(BOARD_JL_36410_FAR)
        discrete_led_set(LED_ID_MIC_MUTE, LED_ALWAYS_OFF, NULL);
#endif
#if defined(MIC_MUTE_GPIO_PIN)
        act_writel((act_readl(MIC_MUTE_GPIO_DATA) | MIC_MUTE_GPIO_PIN), MIC_MUTE_GPIO_DATA);
#endif
#if defined(ENABLE_ALEXA_LED_NOTIFY)
        lp_update_alexa_state(ALEXA_STATE_TYPE_MIC_MUTE, 0);
#endif
        if(tone) {
            keytone_play(KEYTONE_MIC_UNMUTE, KEYTONE_BLOCK);
        }
    }
}
#endif

