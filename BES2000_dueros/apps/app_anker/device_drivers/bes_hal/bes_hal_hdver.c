#include "cmsis_os.h"
#include "tgt_hardware.h"
#include "pmu.h"
#include "hal_timer.h"
#include "hal_gpadc.h"
#include "hal_trace.h"
#include "string.h"
#include "hal_gpio.h"
#include "hal_iomux.h"
#include "hal_chipid.h"
#include "bes_hal_board.h"
#include "hwtimer_list.h"
#include "bes_hal_hdver.h"

typedef struct 
{
    uint8_t hdVerVolue;
    uint16_t hdVerVolt;
    uint16_t upDaltVolt;
    int16_t downDaltVolt;
    const char* string;
}HD_VER_INFO_T;

#define HD_VER_INFO_MAX_NUM         (6)
HD_VER_INFO_T hdVerVoltArray[HD_VER_INFO_MAX_NUM] = {
    {BES_HW_VER_3163_DEMO,3260,100,-100,"3163 Demo Ver"},
    {BES_HW_VER_3163_TR1,3460,100,-100,"3163 Trial-Produce Ver"},
    {BES_HW_VER_3163_VER1,3660,100,-100,"NULL"},
    {BES_HW_VER_3163_VER2,3860,100,-100,"NULL"},
    {BES_HW_VER_3163_VER3,4060,100,-100,"NULL"},
    {BES_HW_VER_3163_VER4,4260,100,-100,"NULL"}
};

#define HDVER_ADC_VOL_2_VER_VOL(vol)           (uint16_t)(4*vol)
#define HDVER_CAPTURE_STABLE_COUNT             7

struct HDVER_CAPTURE_MEASURE_T
{
    uint16_t avgVolt;
    uint16_t index;
    uint16_t voltage[HDVER_CAPTURE_STABLE_COUNT];
};
static struct HDVER_CAPTURE_MEASURE_T gHDVerMeasure;
static BES_HW_VER_TYPE_T gHardwareVer = BES_HW_VER_UNKONW;
static void hardware_version_capture_irqhandler(uint16_t irq_val, HAL_GPADC_MV_T volt)
{
    uint32_t meanVolt = 0;
    struct HDVER_CAPTURE_MEASURE_T* hdver_measure =  (struct HDVER_CAPTURE_MEASURE_T*)&gHDVerMeasure;
    TRACE("%s adc vol = %d, hdver vol = %d, irq:0x%04x",__func__, volt,HDVER_ADC_VOL_2_VER_VOL(volt), irq_val);

    if (volt == HAL_GPADC_BAD_VALUE)
    {
        return;
    }

    hdver_measure->voltage[hdver_measure->index++%HDVER_CAPTURE_STABLE_COUNT] = HDVER_ADC_VOL_2_VER_VOL(volt);
    TRACE("%s cur adc index = %d val = :%d",__func__,index, volt);
}

uint8_t hardware_version_open(void)
{
    uint16_t val, i = 0, maxVal = 0, minVal = 0;
    uint32_t sum = 0;
    struct HDVER_CAPTURE_MEASURE_T* hdver_measure =  (struct HDVER_CAPTURE_MEASURE_T*)&gHDVerMeasure;
    hdver_measure->avgVolt = 0;
    hdver_measure->index = 0;

    hal_analogif_reg_read(0x1e, &val);
    TRACE("PMU 1E=0x%04x", val);
    val = (val & ~(3<<3)) | (2<<3);
    TRACE("PMU 1E=0x%04x", val);
    hal_analogif_reg_write(0x1e, val);    
    for(i = 0; i < HDVER_CAPTURE_STABLE_COUNT; i++)
    {
        hal_gpadc_open(BOARD_HW_VERSION_DET_CH, HAL_GPADC_ATP_ONESHOT, hardware_version_capture_irqhandler);
        osDelay(2);
    }

    for(i = 0; i < HDVER_CAPTURE_STABLE_COUNT; i++)
    {
        sum += hdver_measure->voltage[i];
        if(i == 0)
        {
            maxVal = hdver_measure->voltage[i];
            minVal = hdver_measure->voltage[i];
        }
        else
        {
            if(hdver_measure->voltage[i] > maxVal)
                maxVal = hdver_measure->voltage[i];

            if(hdver_measure->voltage[i] < minVal)
                minVal = hdver_measure->voltage[i];
        }
    }
    hdver_measure->avgVolt = (sum - minVal - maxVal)/(HDVER_CAPTURE_STABLE_COUNT - 2);

    for(i = 0; i < HD_VER_INFO_MAX_NUM; i++)
    {
        if((hdver_measure->avgVolt <  hdVerVoltArray[i].hdVerVolt + hdVerVoltArray[i].upDaltVolt)&&
            (hdver_measure->avgVolt >= hdVerVoltArray[i].hdVerVolt + hdVerVoltArray[i].downDaltVolt))
        {
            gHardwareVer = hdVerVoltArray[i].hdVerVolue;
            break;
        }
    }

    TRACE("HARDWARE Ver: volt = %d, ver = %d~~~~~~~~~~~~~~~~++++++++++++++", hdver_measure->avgVolt,gHardwareVer);
    return (gHardwareVer != BES_HW_VER_UNKONW);
}

BES_HW_VER_TYPE_T hardware_version_get_ver(void)
{
    TRACE("HW VER = %d",gHardwareVer);
    return gHardwareVer;
}
