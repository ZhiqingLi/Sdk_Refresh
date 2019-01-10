
#include "is31fl3236a.h"
#include "bes_hal_i2c.h"
#include "bes_hal_uart.h"
#include "string.h"
#include "cmsis.h"
#include "cmsis_os.h"
#include "bes_hal_board.h"

#define HAL_LIGHT_CTRL_DEBUG       BES_HAL_UART_DEBUG
#define HAL_LIGHT_CTRL_INFO        BES_HAL_UART_INFO
#define HAL_LIGHT_CTRL_WARNING     BES_HAL_UART_WARNING
#define HAL_LIGHT_CTRL_ERROR       BES_HAL_UART_ERROR
#define HAL_LIGHT_CTRL_FATAL       BES_HAL_UART_FATAL

#define IS31FL3236A_I2C_ADDR        (0x3C)
/* setup relevant micro */
//#define PRINTF_ALL_LIGHT_DATA
//#define POWER_ON_INDICATION_INKERNE
#define START_LIGHT_EFFECT_EN
#ifdef START_LIGHT_EFFECT_EN
#define POWER_ON_INDICATION_FADE_IN
#endif
#define DEFAULT_OUT_CTRL                    0x03
#define LIGHT_NUMS                          12
#define INIT_AFTER_PROBE
#define CLOSE_ALL_LIGHT_WHEN_MODULE_EXIT    1

#ifdef CLOSE_ALL_LIGHT_WHEN_MODULE_EXIT
#define EXIT_OUT_CTRL                       0x00
#endif
/* debug relevant micro */
#define OPEN_ALL_LIGHT_AFER_PROBE
//#define DEBUG_NOT_WRITE_IIC

/* reg relevant macro */
#define IS3236_ON               1
#define IS3236_OFF              0

#define IS3236_REG_WORK			0x00 //Shutdown Registe
#define	IS3236_REG_DATA_UPDATE	0x25 //PWM Update Register

#define IS3236_REG_LED_SYNC 	0x4A //Global Control Register
#define IS3236_REG_FREQ_SET     0x4B //4Bh Output Frequency Setting Register
#define IS3236_REG_RST 	        0x4F //Reset Register


#define IS3236_REG_PWM_OUT1 	0x01 //PWM Register(OUT1~OUT36)
#define IS3236_REG_PWM_OUT2 	0x02
#define IS3236_REG_PWM_OUT3 	0x03
#define IS3236_REG_PWM_OUT4 	0x04
#define IS3236_REG_PWM_OUT5 	0x05
#define IS3236_REG_PWM_OUT6 	0x06
#define IS3236_REG_PWM_OUT7 	0x07
#define IS3236_REG_PWM_OUT8 	0x08
#define IS3236_REG_PWM_OUT9 	0x09
#define IS3236_REG_PWM_OUT10 	0x0A
#define IS3236_REG_PWM_OUT11 	0x0B
#define IS3236_REG_PWM_OUT12 	0x0C
#define IS3236_REG_PWM_OUT13 	0x0D
#define IS3236_REG_PWM_OUT14 	0x0E
#define IS3236_REG_PWM_OUT15 	0x0F
#define IS3236_REG_PWM_OUT16 	0x10
#define IS3236_REG_PWM_OUT17 	0x11
#define IS3236_REG_PWM_OUT18 	0x12
#define IS3236_REG_PWM_OUT19 	0x13
#define IS3236_REG_PWM_OUT20 	0x14
#define IS3236_REG_PWM_OUT21 	0x15
#define IS3236_REG_PWM_OUT22 	0x16
#define IS3236_REG_PWM_OUT23 	0x17
#define IS3236_REG_PWM_OUT24 	0x18
#define IS3236_REG_PWM_OUT25 	0x19
#define IS3236_REG_PWM_OUT26 	0x1A
#define IS3236_REG_PWM_OUT27 	0x1B
#define IS3236_REG_PWM_OUT28 	0x1C
#define IS3236_REG_PWM_OUT29 	0x1D
#define IS3236_REG_PWM_OUT30 	0x1E
#define IS3236_REG_PWM_OUT31 	0x1F
#define IS3236_REG_PWM_OUT32 	0x20
#define IS3236_REG_PWM_OUT33 	0x21
#define IS3236_REG_PWM_OUT34 	0x22
#define IS3236_REG_PWM_OUT35 	0x23
#define IS3236_REG_PWM_OUT36 	0x24

#define IS3236_REG_CTRL_OUT1 	0x26 //26h~49h LED Control Register
#define IS3236_REG_CTRL_OUT2 	0x27
#define IS3236_REG_CTRL_OUT3 	0x28
#define IS3236_REG_CTRL_OUT4 	0x29
#define IS3236_REG_CTRL_OUT5 	0x2A
#define IS3236_REG_CTRL_OUT6 	0x2B
#define IS3236_REG_CTRL_OUT7 	0x2C
#define IS3236_REG_CTRL_OUT8 	0x2D
#define IS3236_REG_CTRL_OUT9 	0x2E
#define IS3236_REG_CTRL_OUT10 	0x2F
#define IS3236_REG_CTRL_OUT11 	0x30
#define IS3236_REG_CTRL_OUT12 	0x31
#define IS3236_REG_CTRL_OUT13 	0x32
#define IS3236_REG_CTRL_OUT14 	0x33
#define IS3236_REG_CTRL_OUT15 	0x34
#define IS3236_REG_CTRL_OUT16 	0x35
#define IS3236_REG_CTRL_OUT17 	0x36
#define IS3236_REG_CTRL_OUT18 	0x37
#define IS3236_REG_CTRL_OUT19 	0x38
#define IS3236_REG_CTRL_OUT20 	0x39
#define IS3236_REG_CTRL_OUT21 	0x3A
#define IS3236_REG_CTRL_OUT22 	0x3B
#define IS3236_REG_CTRL_OUT23 	0x3C
#define IS3236_REG_CTRL_OUT24 	0x3D
#define IS3236_REG_CTRL_OUT25 	0x3E
#define IS3236_REG_CTRL_OUT26 	0x3F
#define IS3236_REG_CTRL_OUT27 	0x40
#define IS3236_REG_CTRL_OUT28 	0x41
#define IS3236_REG_CTRL_OUT29 	0x42
#define IS3236_REG_CTRL_OUT30 	0x43
#define IS3236_REG_CTRL_OUT31 	0x44
#define IS3236_REG_CTRL_OUT32 	0x45
#define IS3236_REG_CTRL_OUT33 	0x46
#define IS3236_REG_CTRL_OUT34 	0x47
#define IS3236_REG_CTRL_OUT35 	0x48
#define IS3236_REG_CTRL_OUT36 	0x49


uint8_t start_light_en = 0;
static int32_t is31fl3236a_i2c_write_byte(uint8_t addr, uint8_t val)
{
    int32_t err = 0;

    err = bes_hal_i2c_write(IS31FL3236A_I2C_ADDR, addr,1,(uint8_t*)&val,1);
    return err;
}

#if 0
static int32_t is31fl3236a_i2c_read_byte(uint8_t addr, uint8_t *pdata)
{
    int32_t err = 0;
    err = bes_hal_i2c_read(IS31FL3236A_I2C_ADDR, addr,1,(uint8_t*)pdata,1);
    return err;
}

static int32_t is31fl3236a_set_reg_bit(uint8_t reg, uint8_t bitmask, uint8_t data_val) 
{
    uint8_t temp = 0;
    int32_t err;

    err = is31fl3236a_i2c_read_byte(reg, &temp);
    temp &= (u8)(~bitmask);
    temp |= data_val & bitmask;

    err = is31fl3236a_i2c_write_byte(reg, temp);

    return err;
}
#endif

static int32_t is31fl3236a_i2c_write_bytes(uint8_t addr, uint8_t* pdata, uint32_t len)
{
    int32_t err = 0;
    err = bes_hal_i2c_write(IS31FL3236A_I2C_ADDR, addr,1,pdata,len);
    return err;
}

static inline int32_t is31fl3236a_data_flush(void)
{
    int32_t err;
    err = is31fl3236a_i2c_write_byte(IS3236_REG_DATA_UPDATE, 0xff);
    return err;
}    

static inline int32_t is31fl3236a_out_ctrl(uint8_t ctrl)
{
    int32_t i;
        
    for (i=0; i<LIGHT_NUMS*3; i++)
 	    is31fl3236a_i2c_write_byte(IS3236_REG_CTRL_OUT1 + i, ctrl);
    is31fl3236a_data_flush() ;   

    return 0;
} 

static int32_t breath_light_set_a_light(uint8_t light_index, 
            uint8_t pwm_r, uint8_t pwm_g, uint8_t pwm_b)
{
    uint8_t addr_r, addr_g,addr_b;

    if((light_index > LIGHT_NUMS -1)  || (light_index < 0)) {
        HAL_LIGHT_CTRL_ERROR("%s: error light_index: %d\n", __func__,light_index);
        return -1;
    }
    
    addr_b = light_index * 3 + 0 + IS3236_REG_PWM_OUT1;
    addr_g = light_index * 3 + 1 + IS3236_REG_PWM_OUT1;
    addr_r = light_index * 3 + 2 + IS3236_REG_PWM_OUT1;

#ifndef DEBUG_NOT_WRITE_IIC
    is31fl3236a_i2c_write_byte(addr_r, pwm_r);
    is31fl3236a_i2c_write_byte(addr_g, pwm_g);
    is31fl3236a_i2c_write_byte(addr_b, pwm_b);
#endif
    is31fl3236a_data_flush();

    HAL_LIGHT_CTRL_DEBUG("%s: set light %02d  R: 0x%02x, G: 0x%02x, B: 0x%02x \n",__func__, light_index, pwm_r, pwm_g, pwm_b);
    return 0;
}

static int32_t is31fl3236a_set_all_light(struct all_light_data *all_data)
{
    int32_t i;
    int32_t err = 0;
    uint8_t tbuff[3*RGB_DATA_NUM] = {0};    

    if(!all_data) 
        return -1;

    for(i = 0; i < RGB_DATA_NUM; i++)
    {
        tbuff[3*i] = all_data->data[i].r_val;
        tbuff[3*i+1] = all_data->data[i].g_val;
        tbuff[3*i+2] = all_data->data[i].b_val;
    }

#ifdef PRINTF_ALL_LIGHT_DATA
    HAL_LIGHT_CTRL_DEBUG("%s: all data buffer is:",__func__);
    for (i=0; i<sizeof(tbuff); i++)
        HAL_LIGHT_CTRL_DEBUG("tbuff %d: 0x%02x", i,tbuff[i]);
#endif
//    err = is31fl3236a_i2c_write_bytes(IS3236_REG_PWM_OUT1,(uint8_t*)tbuff,sizeof(struct all_light_data));
    err = is31fl3236a_i2c_write_bytes(IS3236_REG_PWM_OUT1,(uint8_t*)tbuff,3*RGB_DATA_NUM);
    if(err < 0)
    {
        return err;
    }
    is31fl3236a_data_flush();
    return 0;
}

/*

all_data.data[m].r_val = 0x0;
all_data.data[m].g_val = 0x4f; cyan
all_data.data[m].b_val = 0xf;

all_data.data[m+4].r_val = 0x0;
all_data.data[m+4].g_val = 0x3f; green
all_data.data[m+4].b_val = 0x0;

all_data.data[m+8].r_val = 0x0;
all_data.data[m+8].g_val = 0x0; blue
all_data.data[m+8].b_val = 0x3f;


*/

#if 0
static void is31fl3236a_flush_thread(const void *arg)
{
    int32_t i;
    struct all_light_data all_data; 
    int32_t crt_index = 0;
    int32_t last_index = LIGHT_NUMS -1;
    
    memset(&all_data, 0, sizeof(struct all_light_data));
    for (i=0; i<LIGHT_NUMS; i++) {
        all_data.data[i].g_val = 0;
        all_data.data[i].b_val = 0xff;        
    }
    is31fl3236a_set_all_light(&all_data);

    while(start_light_en) {
        //HAL_LIGHT_CTRL_DEBUG("%s: last_index %d", __func__,last_index);
        all_data.data[last_index].g_val = 0;
        all_data.data[last_index].b_val = 0xff;  
        crt_index = last_index - 1;
        last_index--;
        if(last_index < 0) {
            last_index = LIGHT_NUMS -1;   
            crt_index = LIGHT_NUMS -1;
        }
        all_data.data[crt_index].g_val = 0xff;
        all_data.data[crt_index].b_val = 0xfc;
        //HAL_LIGHT_CTRL_DEBUG("%s: crt_index %d",__func__, crt_index);
        is31fl3236a_set_all_light(&all_data);
        osDelay(210);
    }
}



#define IS31FL3236A_FLUSH_THREAD_STACK_SIZE			(1*1024)
#define IS31FL3236A_FLUSH_THREAD_PRIORITY			(osPriorityAboveNormal)
osThreadDef(is31fl3236a_flush_thread, IS31FL3236A_FLUSH_THREAD_PRIORITY, IS31FL3236A_FLUSH_THREAD_STACK_SIZE);
osThreadId gis31fl3236aThreadID = NULL;
static void is31fl3236a_create_flush_thread(void)
{
    if(gis31fl3236aThreadID)
    {
        osThreadTerminate(gis31fl3236aThreadID);
        gis31fl3236aThreadID = NULL;
    }
    hal_light_ctrl_power_ctrl(1);
	gis31fl3236aThreadID = osThreadCreate(osThread(is31fl3236a_flush_thread), NULL);
}

static void is31fl3236a_destroy_flush_thread(void)
{
    if(gis31fl3236aThreadID)
    {
        hal_light_ctrl_power_ctrl(0);
        osThreadTerminate(gis31fl3236aThreadID);
        gis31fl3236aThreadID = NULL;
    }
}
#endif

static int32_t is31fl3236a_chip_initialization(void)
{
    int32_t err = 0;
#ifdef POWER_ON_INDICATION_INKERNE
    int32_t i;
    struct all_light_data all_data; 
#endif
    err = bes_hal_i2c_open();
    if(err < 0)
        return err;

    hal_light_ctrl_power_ctrl(1);
    is31fl3236a_i2c_write_byte(IS3236_REG_RST, 0xff); /*reset all register*/
    is31fl3236a_i2c_write_byte(IS3236_REG_WORK,IS3236_ON);
    is31fl3236a_i2c_write_byte(IS3236_REG_FREQ_SET,1);      // 0 3kHz ; 1 22kHz

#ifdef OPEN_ALL_LIGHT_AFER_PROBE
    is31fl3236a_out_ctrl(DEFAULT_OUT_CTRL);
#endif    
#ifdef POWER_ON_INDICATION_INKERNE
    for (i=0; i<LIGHT_NUMS; i++) {
        all_data.data[i].r_val = 0;
        all_data.data[i].g_val = 0;
        all_data.data[i].b_val = 0xff;
    }
    is31fl3236a_set_all_light(blight, &all_data);
#elif defined (POWER_ON_INDICATION_FADE_IN)
#if 0
    start_light_en = 1;
    // run is31fl3236a_flush_func
    is31fl3236a_create_flush_thread();
#endif
#endif

    HAL_LIGHT_CTRL_DEBUG("%s: chip initialization successfully",__func__);
    return 0;
}

static int32_t is31fl3236a_chip_deinitialization(void)
{
    //is31fl3236a_destroy_flush_thread();
    start_light_en = 0;
    hal_light_ctrl_power_ctrl(0);
    return 0;
}

int32_t is31fl3236a_dev_ctrl(uint8_t cmd, void* arg)  
{
    uint8_t ctrl = 0;
    struct light_data data;
    struct all_light_data all_data;

	switch (cmd) {
	case IS31FL3236A_CMD_CHIP_INIT:
	    TRACE("%s: light cmd: init",__func__);
        is31fl3236a_chip_initialization();
		break;
    case IS31FL3236A_CMD_OUT_CTRL:
        ctrl = (uint8_t)*((uint8_t*)arg);
        HAL_LIGHT_CTRL_DEBUG("%s: light cmd: out ctrl %d",__func__,ctrl);
        is31fl3236a_out_ctrl(ctrl);
		break;
    case IS31FL3236A_CMD_SET_RGB:
        HAL_LIGHT_CTRL_DEBUG("%s: light cmd: set rgb",__func__);
        memcpy((void*)&data,arg,sizeof(struct light_data));
        breath_light_set_a_light(data.index, data.data.r_val, data.data.g_val, data.data.b_val);
		break;
        
    case IS31FL3236A_CMD_SET_RGB_ALL:
       	//HAL_LIGHT_CTRL_DEBUG("%s: light cmd: set rgb all",__func__);
        memcpy((void*)&all_data,arg,sizeof(struct all_light_data));

        is31fl3236a_set_all_light(&all_data);
		break;
#ifdef POWER_ON_INDICATION_FADE_IN
    case IS31FL3236A_CMD_GET_START_EFFECT_EN:
        *((uint8_t*)arg) = (uint8_t)(start_light_en != 0 ? 1 : 0);
        break;
    case IS31FL3236A_CMD_SET_START_EFFECT_EN:
        ctrl = (uint8_t)*((uint8_t*)arg);
        HAL_LIGHT_CTRL_DEBUG("%s: light cmd: start effect %d",__func__,ctrl);
		#if 0
        start_light_en = ctrl != 0 ? 1 : 0;
        if(start_light_en)
            is31fl3236a_create_flush_thread();
        else
            is31fl3236a_destroy_flush_thread();
		#endif
        break;
#endif
    case IS31FL3236A_CMD_SET_WORK_MODE:
        ctrl = (uint8_t)*((uint8_t*)arg);
        HAL_LIGHT_CTRL_DEBUG("%s: light cmd: work mode %d",__func__,ctrl);
        if ((ctrl != 1) && (ctrl != 0))
            goto err_arg;
        
        is31fl3236a_i2c_write_byte(IS3236_REG_WORK,ctrl);
        break;
    case IS31FL3236A_CMD_CHIP_DEINIT:
        TRACE("%s: light cmd: deinit",__func__);
        is31fl3236a_chip_deinitialization();
        break;
    default:
        HAL_LIGHT_CTRL_ERROR("%s: light cmd: error cmd %d",__func__,cmd);
        goto err_arg;
    }
 
	return 0;
    
err_arg:
    HAL_LIGHT_CTRL_ERROR("%s: error arg, exit",__func__);

    return -1;
}