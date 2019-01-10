
#include "cmsis.h"
#include "cmsis_os.h"
#include "string.h"

#include "bes_hal_i2c.h"
#include "hal_i2s.h"

#ifdef VOICE_INPUT_MODULE_CX20921
#include "conexant_cx20921.h"
//#include "middle_transport.h"
#include "app_ai_voice.h"

static uint8_t conexantWorkFlag = 0;

static void (*cx20921_irq_event_handle)(void);

void cx20921_wake_up_event_register(void (* cb)(void))
{
	cx20921_irq_event_handle=cb;
}
static void cx20921_wakeup_int_handler(enum HAL_GPIO_PIN_T pin)
{
    // wakeup bes2000 bt mcu
    CONEXANT_CX20921_DEBUG("%s: CX20921 IRQ pin = %d~~~~~~~~~~~~~~~",__func__,pin);
    //app_ai_local_wakeup_callback();
    cx20921_irq_event_handle();
}

void cx20921_reset(void)
{
    //sets RESET PIN to low.
    hal_gpio_pin_clr(BOARD_GPIO_CX20921_RST);

    osDelay(100); // delay 100ms

    //sets RESET PIN to high.
    hal_gpio_pin_set(BOARD_GPIO_CX20921_RST);
}

void cx20921_power_onoff(uint8_t onoff)
{
#if 0
    uint8_t pwr_pin_state = 0;
    pwr_pin_state = hal_gpio_pin_get_val(BOARD_GPIO_CX20921_PWR);
    CONEXANT_CX20921_DEBUG("%s: enter!",__func__,pwr_pin_state);


    // cur power pin state same as input onoff param, just return;
    if((pwr_pin_state == onoff) || (pwr_pin_state > 0 && onoff > 0))
    {
        CONEXANT_CX20921_DEBUG("%s: exit!",__func__);
    //    return;
    }
#endif
    if(onoff > 0) // power on
    {
        hal_gpio_pin_set(BOARD_GPIO_CX20921_PWR);
        TRACE("%s: POWER ON!!!!!!!!!!!!!!",__func__);
    }
    else // power off
    {
        hal_gpio_pin_clr(BOARD_GPIO_CX20921_PWR);
        TRACE("%s: POWER OFF!!!!!!!!!!!!!!",__func__);
    }
    CONEXANT_CX20921_DEBUG("%s: exit!",__func__);
}


/*
 *  Transmit a command via I2C bus.
 *  
 * PARAMETERS
 *  
 *    header              [in] - A pointer to write buffer of header.
 *    num_header          [in] - Specifies the number of bytes to write 
 *    payload             [in] - A pointer to write buffer of command.                           
 *    num_payload         [in] - Specifies the number of bytes to write 
 *  
 * RETURN
 *  
 *    If the operation completes successfully, the return value is CXERR_NOERR.
 *    Otherwise, return ERRON_* error code. 
 */
int32_t cx20921_i2c_transmit(const uint8_t *header, uint32_t num_header, const uint8_t *payload, uint32_t num_payload)
{
    int32_t err_no = 0;

    if( num_payload && (err_no==CXERR_NOERR) )
    {
        if( payload == NULL )
        {
            err_no = CXERR_NULL_POINTER;
        }
        else
        {
            err_no = bes_hal_i2c_write(CX20921_I2C_SLAVE_ADDR,num_header,CX20921_I2C_REG_ADDR_LEN, (uint8_t*)payload,num_payload);
        }
    }
    err_no = err_no==CXERR_NOERR ? bes_hal_i2c_write(CX20921_I2C_SLAVE_ADDR,4,CX20921_I2C_REG_ADDR_LEN, (uint8_t *)header+4,num_header-4): err_no;
    err_no = err_no==CXERR_NOERR ? bes_hal_i2c_write(CX20921_I2C_SLAVE_ADDR,0,CX20921_I2C_REG_ADDR_LEN, (uint8_t *)header,4): err_no;
    return err_no;
}

int32_t cx20921_hal_init(void)
{
    int32_t ret = 0;
    // init i2c P26&P27
    CONEXANT_CX20921_DEBUG("%s: enter!",__func__);
    if(conexantWorkFlag == 0)
    {
        cx20921_power_onoff(0);
        osDelay(10);
        bes_hal_i2c_open();
        // init cx20921 wakeup bes2000Ik pin intr
        gpio_intr_set_edge(BOARD_GPIO_CX20921_WAKEUP,0,cx20921_wakeup_int_handler);

        // power on cx20921
        cx20921_power_onoff(1);
        
        osDelay(500);
        conexantWorkFlag = 1;
    }
    CONEXANT_CX20921_DEBUG("%s: exit!",__func__);
    return ret;
}

void cx20921_hal_deinit(void)
{
    cx20921_power_onoff(0);
    conexantWorkFlag = 0;
}

#endif