#include "driver_beken_includes.h"
#include "app_beken_includes.h"
#include "beken_external.h"

#if 0//def CONFIG_IRDA
button_action_func *IRKey_handler;
#endif
#ifdef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
extern u_int8 syspwr_cpu_halt;
#endif

#define KEY_VVLONG_COUNTER					900				// 9S 

#define BUTTON_CONFIG //  use BK3266 ToolKitV5.0 button config.

#ifdef BUTTON_CONFIG
// map to BK3266 ToolKitV5.0 button config,xugaojing 20180103.
CONST static uint32 buttonConfigEvent[]=
{
    MSG_NULL,
    MSG_KEY_PLAY,
    MSG_KEY_NEXT,
    MSG_KEY_PREV,
    MSG_KEY_REWIND,
    MSG_KEY_FORWARD,
    MSG_KEY_VOL_UP,
    MSG_KEY_VOL_DOWN,
    MSG_KEY_MUTE,
    MSG_KEY_MATCH,
    MSG_KEY_POWER_DOWN,
    MSG_KEY_HFP_ACK,
    MSG_KEY_HFP_REJECT,
    MSG_KEY_NUM_REDIAL,
    MSG_KEY_MIC_UP,
    MSG_KEY_MIC_DOWN,
    MSG_KEY_MIC_MUTE,
    MSG_KEY_VOICE_DIAL,
    MSG_KEY_TRANSFER_TOGGLE,
    MSG_KEY_CONN_DISCONN,
    MSG_KEY_STOP,
    MSG_CLEAR_BT_MEM,
    MSG_KEY_TEST_MODE,
    MSG_KEY_LANGE_CHANGE,
    MSG_KEY_TWC_HOLD_ACCEPT,
    MSG_KEY_TWC_HUNG_ACCEPT,
    MSG_KEY_TWC_REJECT_HOLD,
    MSG_KEY_TWC_SWITCH_WAY,
    MSG_KEY_2PHONE_SWITCH,
    MSG_KEY_MODE,
    MSG_KEY_RESERVED1,
    MSG_KEY_RESERVED2,
    MSG_KEY_RESERVED3,
};
#else
// user defined button event, don't use button config,xugaojing 20180103
CONST static uint32 buttonUserEvent[][4]=
{
    {MSG_KEY_PLAY,                               (1<<ADKEY2),                                                BUTTON_CLICK,                                                                                                                       BUTTON_TYPE_NON_HFP},    
    {MSG_KEY_HFP_ACK,                       (1<<ADKEY2),                                                 BUTTON_CLICK,                                                                                                                      BUTTON_TYPE_HFP},  
    {MSG_KEY_NEXT,                              (1<<GPIO10),                                                BUTTON_LONG_PRESS,                                                                                                          BUTTON_TYPE_NON_HFP},
    {MSG_KEY_PREV,                              (1<<GPIO8),                                                BUTTON_LONG_PRESS,                                                                                                           BUTTON_TYPE_NON_HFP},
    {MSG_KEY_VOL_UP,                        (1<<ADKEY3),                                                BUTTON_CLICK|BUTTON_REPEAT|BUTTON_DOUBLE_CLICK,                              BUTTON_TYPE_ALL},
    {MSG_KEY_VOL_DOWN,                  (1<<ADKEY4),                                                BUTTON_CLICK|BUTTON_REPEAT|BUTTON_DOUBLE_CLICK,                                                                                                                       BUTTON_TYPE_ALL},
    {MSG_KEY_MATCH,                         (1<<ADKEY0),                                                BUTTON_CLICK,                                                                                                                   BUTTON_TYPE_ALL},
    {MSG_KEY_FORWARD,                   (1<<ADKEY1),                                                BUTTON_CLICK,                                                                                                       BUTTON_TYPE_NON_HFP},
    {MSG_KEY_FORWARD,                   ((1<<GPIO10)|(1<<GPIO15)),                  BUTTON_CLICK,                                                                                                                   BUTTON_TYPE_NON_HFP},
};

#endif

volatile uint16 sniffmode_wakeup_dly = 0;

static uint32 keyValueUsed = 0;
static uint32 keyValueLast = 0;
static uint16 keycount = 0;
static uint16 keyRepeatCount =0;
static uint16 keyDoubleCount =0;

void buttonInit(void)
{
    uint32 i;
    app_handle_t app_h = app_get_sys_handler();
    app_env_handle_t  env_h = app_env_get_handle();
    app_h->button_mask =0;
    app_h->button_ad_channel=ADC_CHANNEL_NULL;

#ifdef BUTTON_CONFIG
    for(i = 0; i < sizeof(buttonConfigEvent)/4; i++)
    {
        // AD key
        if(env_h->env_cfg.button_code[i] &BUTTON_IS_AD_MASK)
        {
            uint8 j;
            app_h->button_ad_channel=(uint8) ((env_h->env_cfg.button_code[i] &BUTTON_AD_CHANNEL_MASK)>>19);
            for(j=5;j<15;j++)
            {
                if(env_h->env_cfg.button_code[i] &(1<<j))
                {
                    if(j==5)
                        app_h->button_code[i]|=(1<<17); //ADkey0:bit17,ADkey1-ADkey9:bit23-bit31.
                    else
                        app_h->button_code[i]|=(1<<(j+17));
                }
            }
        }
        else// IO key
        {
            app_h->button_code[i]=(env_h->env_cfg.button_code[i] & BUTTON_GPIO_MASK);// GPIO& AD 
            app_h->button_mask |= (env_h->env_cfg.button_code[i] & BUTTON_GPIO_MASK); // GPIO
        }
    }
   
#else
    if(ADC_KEY_CHANNEL!=ADC_CHANNEL_NULL)
        app_h->button_ad_channel = ADC_KEY_CHANNEL;
    for(i=0;i<sizeof(buttonUserEvent)/16;i++)
    {
        app_h->button_mask |= (buttonUserEvent[i][1]&BUTTON_GPIO_MASK);
    }
#endif
    for( i = 0; i <= GPIO_NUM; i++)
    {
        if( ((app_h->button_mask&BUTTON_GPIO_MASK) >> i) & 0x01 )
        {
            if((DEFAULT_WAKEUP_PIN == i)&&(DEFAULT_WAKEUP_PIN==env_h->env_cfg.system_para.wakup_pin))   // for shut down
                gpio_config( i+GPIO0, 0);  // input pull_down
            else
                gpio_config( i+GPIO0, 3);  // input pull_up    
        }
    }
}

uint32 buttonDetectIO(void)
{
    app_handle_t app_h = app_get_sys_handler();
    app_env_handle_t  env_h = app_env_get_handle();
    uint8 wakup_pin = env_h->env_cfg.system_para.wakup_pin + GPIO0;
    static uint32 power_on_5s_cnt = 0;
    uint32 keyIOmap = 0;
    int i;
    for( i = 0; i <= GPIO_NUM; i++) 
    {
        if( ((app_h->button_mask&BUTTON_GPIO_MASK) >> i) & 0x01 )
        {
            if((DEFAULT_WAKEUP_PIN == i)&&(DEFAULT_WAKEUP_PIN==env_h->env_cfg.system_para.wakup_pin))
            {
                if(gpio_input(i))
                {
                    keyIOmap |= (1 << i);

                    if(bt_flag2_is_set(APP_FLAG2_CHARGE_POWERDOWN)&&app_env_check_Charge_Mode_PwrDown())
                    {
                        bt_flag2_operate(APP_FLAG2_CHARGE_POWERDOWN,0);
                        BK3000_start_wdt(0xfff);
                        while(1);
                    }
            //#if(POWERKEY_5S_PARING == 1)
                    if(app_env_check_feature_flag(APP_ENV_FEATURE_FLAG_PAIRING_AFTER_PWR_ON))
                    {
                        if(power_on_5s_cnt < env_h->env_cfg.feature.enter_pairing_key_time)
                        {            
                            if(++power_on_5s_cnt ==env_h->env_cfg.feature.enter_pairing_key_time)
                                msg_put(MSG_KEY_MATCH);
                        }
                    }
                }
               else
            //#endif
                {
                    power_on_5s_cnt =env_h->env_cfg.feature.enter_pairing_key_time;
                }
            }
            else
            {
                if(!gpio_input(i))
                    keyIOmap |= (1 << i);
            }
    
        }
    }
   // os_printf("gpio_IO_map:%x\r\n",keyIOmap);
    return keyIOmap;
}
uint32  buttonDetectAD(void)
{
    uint32 keyAdMap = 0;
    app_handle_t app_h = app_get_sys_handler();
    uint16 adc_val = saradc_get_key_value();
    if( app_h->button_ad_channel !=ADC_CHANNEL_NULL)
    {
        if(adc_val < 40)	  //  s1 0R
        {
            keyAdMap = ADKEY0;
        }
        else if( adc_val< 110) // s2 1.8k
        {
            keyAdMap = ADKEY1;
        }
        else if( adc_val < 200) // s3 3.6k
        {							
            keyAdMap = ADKEY2;
        }
        else if( adc_val < 348) // s4 6.8k
        {
            keyAdMap = ADKEY3;
        }
        else if( adc_val < 500) // s5 15k			// 47k  AD LINE IN   ->500
        {
            keyAdMap = ADKEY4;
        }
        else if (adc_val < 700) // s6 33k
        {
            keyAdMap = ADKEY5;
        }
        else if (adc_val < 885) // s7 68k
        {
            keyAdMap = ADKEY6;
        }

    }
    if(keyAdMap)
        return (1<<keyAdMap);
    else
        return 0;
}
uint32 buttonDetect(void)
{
    static uint16 PwrKeyFirstCnt=50;
    uint32 keyValue=0;

    keyValue = buttonDetectIO()|buttonDetectAD();
    
#ifdef CONFIG_CRTL_POWER_IN_BT_SNIFF_MODE
    if(keyValue && syspwr_cpu_halt)
        app_exit_sniff_mode();
#endif
    //if(keyValue)
    //    os_printf("keyValue=%x,%d\r\n",keyValue,saradc_get_key_value()); 
    if(PwrKeyFirstCnt) //开机去抖动
    {
        if(keyValue!=0)
        {
            PwrKeyFirstCnt =50;
        }
        PwrKeyFirstCnt--;
        return 0;
    }
	
    return keyValue;
}


void buttonMsgCompare(uint32 keyAction,uint32 keyValue)
{
    uint8 i;
    app_env_handle_t  env_h = app_env_get_handle();
    app_handle_t app_h = app_get_sys_handler();
#ifdef BUTTON_CONFIG
    for(i=0;i<sizeof(buttonConfigEvent)/4;i++)
    {  
       // os_printf("app_h->button_code[%d]=:%x,keyAction=%x,keyValue=%x,app_h->button_type=%x\r\n",i,app_h->button_code[i],keyAction,keyValue,app_h->button_type);
        if((env_h->env_cfg.button_code[i]&keyAction)
            &&(app_h->button_code[i]==keyValue)
            &&(env_h->env_cfg.button_code[i]&app_h->button_type))
        {
            msg_put(buttonConfigEvent[i]);
        }
    }
#else
    for(i=0;i<sizeof(buttonUserEvent)/16;i++)
    {
        if((buttonUserEvent[i][2]&keyAction)
            &&(buttonUserEvent[i][1]==keyValue)
            &&(buttonUserEvent[i][3]&app_h->button_type))
        {
            msg_put(buttonUserEvent[i][0]);
        }
    }
#endif
}

void button_scanning(void)
{
    app_env_handle_t  env_h = app_env_get_handle();
    uint32 keyValNow;

    keyValNow = buttonDetect();
    if(keyValNow == 0)
    {
        if(keycount >= env_h->env_cfg.button_para.press)
        {
            if(keycount > KEY_VVLONG_COUNTER)
            {
           // buttonMsgCompare(BUTTON_VVLONG_PRESS_UP,keyValueUsed);
            }
            else if(keycount > env_h->env_cfg.button_para.vlongp)
            {
                //buttonMsgCompare(BUTTON_VLONG_PRESS_UP,keyValueUsed);
            }
            else if(keycount > env_h->env_cfg.button_para.longp)
            {
                 buttonMsgCompare(BUTTON_LONG_PRESS_UP,keyValueUsed);
            }
            else
            {
                if((keyValueLast==keyValueUsed)&&keyDoubleCount)
                {
                    buttonMsgCompare(BUTTON_DOUBLE_CLICK,keyValueUsed);
                    keyDoubleCount =0; 
                    keyValueLast = 0;
                }
                else
                {
                    if(env_h->env_cfg.button_para.doublep==0)
                    {
                        buttonMsgCompare(BUTTON_CLICK,keyValueUsed);
                        keyDoubleCount =0; 
                        keyValueLast = 0;
                    }
                    else
                    {
                        keyDoubleCount =env_h->env_cfg.button_para.doublep; 
                        keyValueLast = keyValueUsed;
                    }
                }
            }
        }

        if(keyValueLast!=0)
        {
            if(keyDoubleCount)
            {
                keyDoubleCount--;
                if(keyDoubleCount==0)
                { 
                    buttonMsgCompare(BUTTON_CLICK,keyValueLast);
                    keyValueLast = 0;
                }
            }
        }
         keycount = 0;
        keyRepeatCount =0;
        keyValueUsed = 0;	
    }
    else
    {
        if(keyValNow!=keyValueUsed)
        {
            keycount = 0;
            keyRepeatCount =0;
            keyValueUsed = keyValNow;
        }

        keycount++;
        if(keycount >= env_h->env_cfg.button_para.longp)
        {
            keyRepeatCount++;
            keyDoubleCount =0; 
            keyValueLast = 0;
            if(keyRepeatCount>=env_h->env_cfg.button_para.repeat)
            {
                keyRepeatCount = 0;
                buttonMsgCompare(BUTTON_REPEAT,keyValueUsed);
            }
            if(keycount == env_h->env_cfg.button_para.longp)
            {
                buttonMsgCompare(BUTTON_LONG_PRESS,keyValueUsed);
            }
            else if(keycount == env_h->env_cfg.button_para.vlongp)
            {
               buttonMsgCompare(BUTTON_VLONG_PRESS,keyValueUsed);
            }
            else if(keycount == KEY_VVLONG_COUNTER)
            {
                keycount = KEY_VVLONG_COUNTER+1;
              //  buttonMsgCompare(BUTTON_VVLONG_PRESS,keyValueUsed);
            }
        }
    }
}
