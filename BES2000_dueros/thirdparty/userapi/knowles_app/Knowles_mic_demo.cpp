#ifdef __KNOWLES
#include "cmsis_os.h"
#include "stdint.h"
#include "stdbool.h"
#include "string.h"
#include "cmsis_nvic.h"
#include "hal_timer_raw.h"
#include "hal_iomux.h"
#include "hal_gpio.h"
#include "hal_trace.h"
#include "tgt_hardware.h"
#include "app_thread.h"
#include "Knowles_mic_demo.h"
#include "Knowles_mic_communication.h"

#include "knowles_uart.h" //this file supports both i2s and pdm, need to give generic name	

#include "app_key.h"


#define SMART_MIC_DEBUG_OPEN
#ifdef SMART_MIC_DEBUG_OPEN
#define KNOWLES_SMART_MIC_DEBUG             TRACE
#else
#define KNOWLES_SMART_MIC_DEBUG

#endif


void Smart_mic_Message_Send(enum iacore_message message_id );

extern bool start_receive_interupt;
static void hal_smart_mic_irq_pin_exit_Detection_mode();

bool after_wake_up=false;
void Smart_mic_interupt_event_handle(enum HAL_GPIO_PIN_T pin)
{
	KNOWLES_SMART_MIC_DEBUG("HS: Knowles Voice Wake Event!!!!!\n");
	
	if (start_receive_interupt){
		after_wake_up=true;
		start_receive_interupt=false;
		Smart_mic_Message_Send(BES_KEYWORD);
	}
}


void hal_smart_mic_wake_up_gpio_setirq(enum HAL_GPIO_PIN_T pin, enum HAL_GPIO_IRQ_POLARITY_T porlariry, void (* HAL_GPIO_PIN_IRQ_HANDLER)(enum HAL_GPIO_PIN_T))
{
    struct HAL_GPIO_IRQ_CFG_T gpiocfg;

    if (HAL_GPIO_PIN_IRQ_HANDLER){
        gpiocfg.irq_enable = true;
        gpiocfg.irq_debounce = true;
        gpiocfg.irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;
        gpiocfg.irq_polarity = porlariry;
        gpiocfg.irq_handler = HAL_GPIO_PIN_IRQ_HANDLER;
        hal_gpio_pin_set_dir(pin, HAL_GPIO_DIR_IN, 0);
        hal_gpio_setup_irq(pin, &gpiocfg);
    }else{
        gpiocfg.irq_enable = false;
        gpiocfg.irq_debounce = false;
        gpiocfg.irq_type = HAL_GPIO_IRQ_TYPE_EDGE_SENSITIVE;
        gpiocfg.irq_polarity = HAL_GPIO_IRQ_POLARITY_LOW_FALLING;
        gpiocfg.irq_handler = NULL;
        hal_gpio_pin_set_dir(pin, HAL_GPIO_DIR_IN, 0);
        hal_gpio_setup_irq(pin, &gpiocfg);
    }
}

void hal_smart_mic_irq_pin_exit_Detection_mode()
{
	struct HAL_GPIO_IRQ_CFG_T gpiocfg;
	
	gpiocfg.irq_enable = false,
	gpiocfg.irq_debounce = false,
	gpiocfg.irq_polarity = HAL_GPIO_IRQ_POLARITY_LOW_FALLING,
	gpiocfg.irq_handler = NULL,
	gpiocfg.irq_type = HAL_GPIO_IRQ_TYPE_LEVEL_SENSITIVE,

    hal_gpio_setup_irq((enum HAL_GPIO_PIN_T)(app_smart_mic_irq_pin_cfg.pin), &gpiocfg);
}

void hal_smart_mic_pin_irq_enter_Detection_mode()
{
	if (app_smart_mic_irq_pin_cfg.pin != HAL_IOMUX_PIN_NUM){
		hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&app_smart_mic_irq_pin_cfg, 1);
		hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)app_smart_mic_irq_pin_cfg.pin, HAL_GPIO_DIR_IN, 1);
		hal_smart_mic_wake_up_gpio_setirq((enum HAL_GPIO_PIN_T)(app_smart_mic_irq_pin_cfg.pin), HAL_GPIO_IRQ_POLARITY_LOW_FALLING, Smart_mic_interupt_event_handle);
	}
}




extern void bt_key_handle_func_key(enum APP_KEY_EVENT_T event);

extern void bt_key_handle_up_key(enum APP_KEY_EVENT_T event);
extern void bt_key_handle_down_key(enum APP_KEY_EVENT_T event);


#if defined (__SMARTVOICE__)||defined (__AMA_VOICE__)||defined(__DMA_VOICE__)
#include "ama_stream.h"
extern int ai_voice_irq_wake_up();

#endif


int Smart_mic_proc(APP_MESSAGE_BODY *msg_body)
{
    int ret;
    TRACE("smart_mic_proc enter");
    switch (msg_body->message_Param0) {
        case IA61X_VW:
        {			
            /* check existing keywords changed? */
            if(iacore_is_keywords_changed())
            {
                iacore_init_model(iacore);
                firmware_state_machine(iacore,FW_LOADED);
                iacore->voice_sense->kw_model_loaded = FALSE;
            }
			if(iacore->fw_state != VS_SLEEP)
			{
				if (iacore->voice_sense->params.vad == IA_NO_VAD)
				{
					firmware_state_machine(iacore,FW_LOADED);
				}
				/* set firmware state machine to VS sleep */
				firmware_state_machine(iacore,VS_SLEEP);
			}
        }
		hal_smart_mic_pin_irq_enter_Detection_mode();

        break;

        case IA61X_PDM_BYPASS:
        {
#ifdef KNOWLES_I2S_INTERFACE			
            /* set firmware state machine to PDM bypass */
           firmware_state_machine(iacore, FW_HW_BYPASS);
#else
		if((iacore->multimic == SHARED_TWO_MIC) || (iacore->multimic == COMBINED_TWO_MIC))
		{
			/* set firmware state machine to SW bypass */
			firmware_state_machine(iacore, SW_BYPASS);
		}
		else
		{
			/* set firmware state machine to PDM bypass */
			firmware_state_machine(iacore, FW_HW_BYPASS);
		}
#endif
        }
		
        break;

        case IA61X_BURSTING:
        {		
           /* set firmware state machine to Bursting */
           firmware_state_machine(iacore, VS_BURSTING);
        }
        break;

		case  BES_KEYWORD:
			iacore->fw_state = VS_MODE;
			
		/* Check which kw is detected */
		iacore->event_id = iacore_event_status();
		KNOWLES_SMART_MIC_DEBUG("Knowles Event Id %d\n",iacore->event_id);
	
		switch(iacore->event_id)
		{
			case ALEXA:
				TRACE("ALEXA!!!");
#if defined (__SMARTVOICE__)||defined (__AMA_VOICE__)||defined(__DMA_VOICE__)
				hal_smart_mic_irq_pin_exit_Detection_mode();

				if(ai_voice_irq_wake_up()){
					TRACE("wakeup returned 1\n");
					Smart_mic_Message_Send(IA61X_VW);
				}
				else
				{
#ifdef __AMA_VOICE__
				#if AMA_WAKEUP_TYPE==AMA_KEYWORD_WAKEUP
					Smart_mic_Message_Send(IA61X_BURSTING);
				#endif
#endif
#ifdef __DMA_VOICE__
				#ifndef DMA_PUSH_TO_TALK
					TRACE("DMA BURST");
					Smart_mic_Message_Send(IA61X_BURSTING);
				#endif
#endif
				}
#endif
				return 0;
			break;
			default:
			break;
		};
		/* put the chip to detection mode again */
		Smart_mic_Message_Send(IA61X_VW);
		break;

        default:            
            break;
    }
    
exit:

    return 0;
}

void Smart_mic_Message_Send(enum iacore_message message_id )
{
    APP_MESSAGE_BLOCK msg;
    msg.mod_id = APP_MODUAL_SMART_MIC;
    msg.msg_body.message_Param0 = message_id;
    msg.msg_body.message_Param1 = 0;
    app_mailbox_put(&msg);
}


int iacore_handle_state(enum sink_state pNewState,void *param)
{
    if(pNewState == deviceLimbo)
    {
        if(iacore != NULL)
        {
            /* Power off IA61x in limbo state */
            iacore_exit();
        }
    }
    else if((pNewState == deviceActiveCallSCO) || (pNewState == deviceActiveCallNoSCO))
    {
    	hal_smart_mic_irq_pin_exit_Detection_mode();
        /* In active call switch IA610 to PDMBypass for standard mic usecase */
        Smart_mic_Message_Send(IA61X_PDM_BYPASS);
        KNOWLES_SMART_MIC_DEBUG("call sco\n");
    }
	else if((pNewState == deviceConnectable) || (pNewState == deviceConnDiscoverable) || (pNewState == deviceConnected)|| (pNewState ==deviceCallEnd) || (pNewState = deviceIncomingCallEstablish) || (pNewState == deviceA2DPStreaming) )
    {
        if(iacore == NULL)
        {
            /* Power on IA61x */
            iacore_init();
        }
        iacore->voice_sense->kwds[0] = ALEXA;
        iacore->voice_sense->kw_count = 1;
        Smart_mic_Message_Send(IA61X_VW);
        KNOWLES_SMART_MIC_DEBUG("knowles alexa\n");

    }
    else
    {
        KNOWLES_SMART_MIC_DEBUG("There is no action required for other states\n");
    }
}




extern "C" void analog_aud_mickey_enable(bool en);

int Smart_mic_module_init(bool on,void *param)
{
	if(on){
		analog_aud_mickey_enable(true);//open VMIC FOR IA610

		smart_mic_init_transport();//open uart
		if (app_smart_mic_ldo_en_pin_cfg.pin != HAL_IOMUX_PIN_NUM)
	    {
	        hal_iomux_init((struct HAL_IOMUX_PIN_FUNCTION_MAP *)&app_smart_mic_ldo_en_pin_cfg, 1);
	        hal_gpio_pin_set_dir((enum HAL_GPIO_PIN_T)app_smart_mic_ldo_en_pin_cfg.pin, HAL_GPIO_DIR_OUT, 1);
	    }			
		app_set_threadhandle(APP_MODUAL_SMART_MIC, Smart_mic_proc);
		
		smart_mic_uart_config_register(smart_mic_reinit_transport);
		smart_mic_uart_write_register(smart_mic_send_data);
		smart_mic_uart_read_register(smart_mic_recv_data);
		smart_mic_uart_read_timeout_register(smart_mic_set_recv_timeout);
		smart_mic_irq_pin_active_register(hal_smart_mic_pin_irq_enter_Detection_mode);
	}
	return 0;
}



#include "app_thirdparty.h"

THIRDPARTY_EVENT_HANDLER_TAB_ADD(knowles,LIB_ID2_EVENT_NUM)
{
    {{THIRDPARTY_ID_NO2,THIRDPARTY_INIT},(APP_THIRDPARTY_HANDLE_CB_T)Smart_mic_module_init,true,NULL},
    {{THIRDPARTY_ID_NO2,THIRDPARTY_DEINIT},(APP_THIRDPARTY_HANDLE_CB_T)iacore_handle_state,deviceLimbo,NULL},
    {{THIRDPARTY_ID_NO2,THIRDPARTY_BT_CONNECTABLE},(APP_THIRDPARTY_HANDLE_CB_T)iacore_handle_state,deviceConnectable,NULL},
    {{THIRDPARTY_ID_NO2,THIRDPARTY_BT_DISCOVERABLE},(APP_THIRDPARTY_HANDLE_CB_T)iacore_handle_state,deviceConnDiscoverable,NULL},
    {{THIRDPARTY_ID_NO2,THIRDPARTY_BT_CONNECTED},(APP_THIRDPARTY_HANDLE_CB_T)iacore_handle_state,deviceConnected,NULL},
    {{THIRDPARTY_ID_NO2,THIRDPARTY_A2DP_STREAMING},(APP_THIRDPARTY_HANDLE_CB_T)iacore_handle_state,deviceA2DPStreaming,NULL},
    {{THIRDPARTY_ID_NO2,THIRDPARTY_HFP_SETUP},(APP_THIRDPARTY_HANDLE_CB_T)iacore_handle_state,deviceIncomingCallEstablish,NULL},
    {{THIRDPARTY_ID_NO2,THIRDPARTY_MIC_OPEN},(APP_THIRDPARTY_HANDLE_CB_T)iacore_handle_state,deviceActiveCallSCO,NULL},
    {{THIRDPARTY_ID_NO2,THIRDPARTY_MIC_CLOSE},(APP_THIRDPARTY_HANDLE_CB_T)iacore_handle_state,deviceCallEnd,NULL},
    
};
#endif




