#include <stdio.h>
#include "string.h"
#include "plat_types.h"
#include "plat_addr_map.h"
#include "hal_i2c.h"
#include "hal_uart.h"
#include "hal_timer.h"
#include "hal_intersys.h"
#include "hal_trace.h"
#include "hal_psc.h"
#include "hal_cmu.h"
#include "hal_sysfreq.h"
#include "hal_chipid.h"
#include "hal_iomux.h"
#include "pmu.h"
#include "nvrecord_dev.h"
#include "bt_drv.h"
#include "bt_drv_internal.h"
#include "bt_drv_interface.h"

#if 0
typedef enum {
    BTDRV_INIT_IDLE,
    BTDRV_RF_INIT,
    BTDRV_INS_PATCH_INIT,
    BTDRV_DATA_PATCH_INIT,
    BTDRV_CONFIG_INIT,
    BTDRV_INIT_INIT_COMPLETE
} BTDRV_INIT_STATE;


typedef struct
{
    volatile BTDRV_INIT_STATE  step;
}BTDRV_STRUCT;


BTDRV_STRUCT  btdrv_cb={0};
#endif

#if 0
extern void btdrv_intersys_rx_thread(void const *argument);

uint32_t os_thread_def_stack_btdrv_rx [1024/ sizeof(uint32_t)];
osThreadDef_t os_thread_def_btdrv_intersys_rx_thread = { (btdrv_intersys_rx_thread), (osPriorityHigh), (1024), (os_thread_def_stack_btdrv_rx)};

//bt
static osThreadId btdrv_intersys_rx_thread_id;
static osThreadId btdrv_intersys_tx_thread_id;
#endif


static void btdrv_tx(const unsigned char *data, unsigned int len)
{
//  HciPacketSent(intersys_tx_pkt);
//  DRV_PRINT("tx\n");
 //   osSignalSet(btdrv_intersys_tx_thread_id, 0x1);
}

static unsigned int btdrv_rx(const unsigned char *data, unsigned int len)
{
    hal_intersys_stop_recv(HAL_INTERSYS_ID_0);

    DRV_PRINT("%s len:%d", __func__, len);
    DRV_DUMP("%02x ", data, len>7?7:len);
    hal_intersys_start_recv(HAL_INTERSYS_ID_0);

    return len;
}

extern "C" void btdrv_SendData(const uint8_t *buff,uint8_t len)
{

    hal_intersys_send(HAL_INTERSYS_ID_0, HAL_INTERSYS_MSG_HCI, buff, len);
    DRV_PRINT("%s", __func__);
    DRV_DUMP("%02x ", buff, len);
    btdrv_delay(1);
}



////open intersys interface for hci data transfer
void btdrv_hciopen(void)
{
    int ret = 0;
    ret = hal_intersys_open(HAL_INTERSYS_ID_0, HAL_INTERSYS_MSG_HCI, btdrv_rx, btdrv_tx, false);

    if (ret) {
        DRV_PRINT("Failed to open intersys");
        return;
    }
    hal_intersys_start_recv(HAL_INTERSYS_ID_0);
}

////open intersys interface for hci data transfer
void btdrv_hcioff(void)
{
    hal_intersys_close(HAL_INTERSYS_ID_0,HAL_INTERSYS_MSG_HCI);
}



/*  btdrv power on or off the bt controller*/
void btdrv_poweron(uint8_t en)
{
    //power on bt controller
    if(en)
    {
        hal_psc_bt_enable();
        hal_cmu_bt_clock_enable();
        hal_cmu_bt_reset_clear();
        //hal_cmu_bt_module_init();
        btdrv_delay(10);
    }
    else
    {
        btdrv_delay(10);
        hal_cmu_bt_reset_set();
        hal_cmu_bt_clock_disable();
        hal_psc_bt_disable();
    }
}

void btdrv_rf_init_ext(void)
{
    unsigned int xtal_fcap;

    if (!nvrec_dev_get_xtal_fcap(&xtal_fcap)){
        btdrv_rf_init_xtal_fcap(xtal_fcap);
        btdrv_delay(1);
        TRACE("%s 0xc2=0x%x", __func__, xtal_fcap);
    }else{
        TRACE("%s failed", __func__);
    }
}

void tx_ramp_new(void)
{
    BTDIGITAL_REG(0xd0220080) = 0x18421656;
    BTDIGITAL_REG(0xd0350360) = 0x00000120;
    BTDIGITAL_REG(0xd0350300) = 0x00000005;
    BTDIGITAL_REG(0xd0350340) = 0x00000005;
}

void bt_drv_extra_config_after_init(void)
{
	/* fix BLE control timing issue */
	uint32_t v = *(volatile uint32_t *)(0xD0227114);
	v &= 0xFFFF8BFF;		/* clear bit 10, 12, 13, 14 */
	v |= (1 << 8) | (1 << 9) | (1 << 11);
	(*(volatile uint32_t *)(0xD0227114)) = v;

    *((volatile uint32_t *)0XD02201B0) &= ~(1 << 15);
    *((volatile uint32_t *)0XD0227110) |= (1 << 15);    

    *((volatile uint32_t *)0XD02201c8) &= ~(1 << 13);

    if (hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_2) 
    {
        *(volatile uint16_t*)(0xc0003bec) = 0x0;         //rssi low
        *(volatile uint16_t *)(0xc0003bea) = 0xFFFF;  ///rssi high
    }      
}

///start active bt controller
void btdrv_start_bt(void)
{
    enum HAL_IOMUX_ISPI_ACCESS_T access;

    hal_sysfreq_req(HAL_SYSFREQ_USER_BT, HAL_CMU_FREQ_26M);

    access = hal_iomux_ispi_access_enable(HAL_IOMUX_ISPI_MCU_RF);

#ifndef NO_SLEEP
    pmu_sleep_en(0);
#endif

    TRACE("0x40087044=%x",*(uint32_t *)0x40087044);




    btdrv_poweron(BT_POWERON);

{
    //wakp interface
    unsigned short read_val;

    btdrv_read_rf_reg(0x50, &read_val);
}

    btdrv_hciopen();

    btdrv_rf_init();

    btdrv_rf_init_ext();

    BTDIGITAL_REG(0xd0350240)=0x0c01;  /////tx iqswap

    //change modem index to 0.34
    BTDIGITAL_REG(0xd0350320) = 0x00350021;//(BTDIGITAL_REG(0xd0350320)  & 0xffffff80)|17;
    BTDIGITAL_REG(0xd035031c) = 0x00050005;//(BTDIGITAL_REG(0xd035031c)  & 0xffffff80)|4;


//    BTDIGITAL_REG(0xd0350300)=0x01; //;//tx gfsk digital gain
//    BTDIGITAL_REG(0xd0350340)=0x01; //tx psk digital gain
    tx_ramp_new();
    
    BTDIGITAL_REG(0xd0350204)=0x06; //tx signed output

    BTDIGITAL_REG(0xd03502c8)=0x60;
    BTDIGITAL_REG(0xd03502cc)=0x15;
    BTDIGITAL_REG(0xd03502d8)=0x16;
    BTDIGITAL_REG(0xd03502d4)=0x2f;

    BTDIGITAL_REG(0xd03502a4)=0x009c001e;
    BTDIGITAL_REG(0xd035020c)=0x00010040; //

    

    btdrv_config_init();
#if 1//ndef _BEST1101_E_
    btdrv_ins_patch_init();
    btdrv_data_patch_init();
    btdrv_patch_en(1);
#endif

//    rx_dc_2200();

#ifdef BT_XTAL_SYNC
    btdrv_bt_spi_xtal_init();
#endif
    btdrv_sync_config();

#ifdef BT_REDUCE_EMI_CFG
 //   btdrv_bt_spi_rawbuf_init();
#endif


    btdrv_hcioff();

#ifndef NO_SLEEP
    pmu_sleep_en(1);
#endif

    if ((access & HAL_IOMUX_ISPI_MCU_RF) == 0) {
        hal_iomux_ispi_access_disable(HAL_IOMUX_ISPI_MCU_RF);
    }

    hal_sysfreq_req(HAL_SYSFREQ_USER_BT, HAL_CMU_FREQ_32K);
}


const uint8_t hci_cmd_enable_dut[] =
{
    0x01,0x03, 0x18, 0x00
};
const uint8_t hci_cmd_enable_allscan[] =
{
    0x01, 0x1a, 0x0c, 0x01, 0x03
};
const uint8_t hci_cmd_enable_pagescan[] =
{
    0x01, 0x1a, 0x0c, 0x01, 0x02
};
const uint8_t hci_cmd_autoaccept_connect[] =
{
    0x01,0x05, 0x0c, 0x03, 0x02, 0x00, 0x02
};
const uint8_t hci_cmd_hci_reset[] =
{
    0x01,0x03,0x0c,0x00
};


const uint8_t hci_cmd_nonsig_tx_dh1_pn9[] = {
0x01, 0x60, 0xfc, 0x14, 0x00, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x00, 0x04, 0x04, 0x1b, 0x00
};
const uint8_t hci_cmd_nonsig_tx_2dh1_pn9[] = {
0x01, 0x60, 0xfc, 0x14, 0x00, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x04, 0x04, 0x36, 0x00
};
const uint8_t hci_cmd_nonsig_tx_3dh1_pn9[] = {
0x01, 0x60, 0xfc, 0x14, 0x00, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x08, 0x04, 0x53, 0x00
};
const uint8_t hci_cmd_nonsig_tx_2dh3_pn9[] = {
0x01, 0x60, 0xfc, 0x14, 0x00, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x0a, 0x04, 0x6f, 0x01
};
const uint8_t hci_cmd_nonsig_tx_3dh3_pn9[] = {
0x01, 0x60, 0xfc, 0x14, 0x00, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x0b, 0x04, 0x28, 0x02
};

const uint8_t hci_cmd_nonsig_rx_dh1_pn9[] = {
0x01, 0x60, 0xfc, 0x14, 0x01, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x00, 0x04, 0x00, 0x1b, 0x00
};
const uint8_t hci_cmd_nonsig_rx_2dh1_pn9[] = {
0x01, 0x60, 0xfc, 0x14, 0x01, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x04, 0x00, 0x36, 0x00
};
const uint8_t hci_cmd_nonsig_rx_3dh1_pn9[] = {
0x01, 0x60, 0xfc, 0x14, 0x01, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x08, 0x00, 0x53, 0x00
};
const uint8_t hci_cmd_nonsig_rx_2dh3_pn9[] = {
0x01, 0x60, 0xfc, 0x14, 0x01, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x0a, 0x00, 0x6f, 0x01
};
const uint8_t hci_cmd_nonsig_rx_3dh3_pn9[] = {
0x01, 0x60, 0xfc, 0x14, 0x01, 0xe8, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x11, 0x11, 0x11, 0x11, 0x11, 0x11, 0x01, 0x01, 0x0b, 0x00, 0x28, 0x02
};

void btdrv_testmode_start(void)
{
    enum HAL_IOMUX_ISPI_ACCESS_T access;

    TRACE("%s\n", __func__);

    btdrv_start_bt();

    access = hal_iomux_ispi_access_enable(HAL_IOMUX_ISPI_MCU_RF);

//    BTDIGITAL_REG(0xd0350300)=0x1;    ///dig gain /2
//    BTDIGITAL_REG(0xd0350340)=0x1;  ///dig gain /2


//    BTDIGITAL_REG(0xd03502c8)=0x35;
//    BTDIGITAL_REG(0xd03502cc)=0x15;
//    BTDIGITAL_REG(0xd03502d8)=0x16;
//    BTDIGITAL_REG(0xd03502d4)=0x2f;

//    {0xd03502c8,0x00000035},
//  {0xd03502cc,0x00000015},
//  {0xd03502d8,0x00000016},
//  {0xd03502d4,0x0000002f},



    btdrv_hciopen();
//    btdrv_config_init();
//    btdrv_testmode_config_init();
//    btdrv_bt_spi_rawbuf_init();
//    btdrv_sleep_config(0);
//    btdrv_delay(10);
//    btdrv_feature_default();
//    btdrv_delay(10);

    if ((access & HAL_IOMUX_ISPI_MCU_RF) == 0) {
        hal_iomux_ispi_access_disable(HAL_IOMUX_ISPI_MCU_RF);
    }
}

void btdrv_write_localinfo(char *name, uint8_t len, uint8_t *addr)
{
    uint8_t hci_cmd_write_addr[5+6] =
    {
        0x01, 0x40, 0xfc, 0x07, 0x00
    };

    uint8_t hci_cmd_write_name[248+4] =
    {
        0x01, 0x13, 0x0c, 0xF8
    };
    memset(&hci_cmd_write_name[4], 0, sizeof(hci_cmd_write_name)-4);
    memcpy(&hci_cmd_write_name[4], name, len);
    btdrv_SendData(hci_cmd_write_name, sizeof(hci_cmd_write_name));
    btdrv_delay(50);
    memcpy(&hci_cmd_write_addr[5], addr, 6);
    btdrv_SendData(hci_cmd_write_addr, sizeof(hci_cmd_write_addr));
    btdrv_delay(20);
}

void btdrv_enable_dut(void)
{
    btdrv_SendData(hci_cmd_enable_dut, sizeof(hci_cmd_enable_dut));
    btdrv_delay(20);
    btdrv_SendData(hci_cmd_enable_allscan, sizeof(hci_cmd_enable_allscan));
    btdrv_delay(20);
    btdrv_SendData(hci_cmd_autoaccept_connect, sizeof(hci_cmd_autoaccept_connect));
    btdrv_delay(20);
}

void btdrv_enable_autoconnector(void)
{
    btdrv_SendData(hci_cmd_enable_pagescan, sizeof(hci_cmd_enable_pagescan));
    btdrv_delay(20);
    btdrv_SendData(hci_cmd_autoaccept_connect, sizeof(hci_cmd_autoaccept_connect));
    btdrv_delay(20);
}

void btdrv_hci_reset(void)
{
    btdrv_SendData(hci_cmd_hci_reset, sizeof(hci_cmd_hci_reset));
}

void btdrv_enable_nonsig_tx(uint8_t index)
{
    TRACE("%s\n", __func__);

    if (index == 0)
        btdrv_SendData(hci_cmd_nonsig_tx_2dh1_pn9, sizeof(hci_cmd_nonsig_tx_2dh1_pn9));
    else if (index == 1)
        btdrv_SendData(hci_cmd_nonsig_tx_3dh1_pn9, sizeof(hci_cmd_nonsig_tx_3dh1_pn9));
    else if (index == 2)
        btdrv_SendData(hci_cmd_nonsig_tx_2dh3_pn9, sizeof(hci_cmd_nonsig_tx_2dh1_pn9));
    else if (index == 3)
        btdrv_SendData(hci_cmd_nonsig_tx_3dh3_pn9, sizeof(hci_cmd_nonsig_tx_3dh1_pn9));
    else
        btdrv_SendData(hci_cmd_nonsig_tx_dh1_pn9, sizeof(hci_cmd_nonsig_tx_dh1_pn9));
    btdrv_delay(20);

}

void btdrv_enable_nonsig_rx(uint8_t index)
{
    TRACE("%s\n", __func__);

    if (index == 0)
        btdrv_SendData(hci_cmd_nonsig_rx_2dh1_pn9, sizeof(hci_cmd_nonsig_rx_2dh1_pn9));
    else if (index == 1)
        btdrv_SendData(hci_cmd_nonsig_rx_3dh1_pn9, sizeof(hci_cmd_nonsig_rx_3dh1_pn9));
    else if (index == 2)
        btdrv_SendData(hci_cmd_nonsig_rx_2dh3_pn9, sizeof(hci_cmd_nonsig_rx_2dh1_pn9));
    else if (index == 3)
        btdrv_SendData(hci_cmd_nonsig_rx_3dh3_pn9, sizeof(hci_cmd_nonsig_rx_3dh1_pn9));
    else
        btdrv_SendData(hci_cmd_nonsig_rx_dh1_pn9, sizeof(hci_cmd_nonsig_rx_dh1_pn9));
    btdrv_delay(20);
}

void btdrv_stop_bt(void)
{
    btdrv_poweron(BT_POWEROFF);

}

extern "C" void btdrv_write_memory(uint8_t wr_type,uint32_t address,const uint8_t *value,uint8_t length)
{
    uint8_t buff[256];
    if(length ==0 || length >128)
        return;
    buff[0] = 0x01;
    buff[1] = 0x02;
    buff[2] = 0xfc;
    buff[3] = length + 6;
    buff[4] = address & 0xff;
    buff[5] = (address &0xff00)>>8;
    buff[6] = (address &0xff0000)>>16;
    buff[7] = address>>24;
    buff[8] = wr_type;
    buff[9] = length;
    memcpy(&buff[10],value,length);
    btdrv_SendData(buff,length+10);
    btdrv_delay(2);


}

extern "C" void btdrv_send_cmd(uint16_t opcode,uint8_t cmdlen,const uint8_t *param)
{
    uint8_t buff[256];
    buff[0] = 0x01;
    buff[1] = opcode & 0xff;
    buff[2] = (opcode &0xff00)>>8;
    buff[3] = cmdlen;
    if(cmdlen>0)
        memcpy(&buff[4],param,cmdlen);
    btdrv_SendData(buff,cmdlen+4);
}

void btdrv_rxdpd_sample_init(void)
{
    //0x40000048,0x03ffffff; //release btcpu reset
    BTDIGITAL_REG(0xc00003ac)=0x00060020;//turn off bt sleep
    btdrv_delay(2000);
    //0xd0220050,0x0000b7b7;
    BTDIGITAL_REG(0xd0350350)=0x70810081;
    BTDIGITAL_REG(0xd0350208)=0x8;
    BTDIGITAL_REG(0xd0350228)=0x8;

    BTDIGITAL_REG(0xd035035c)=0x50000000; // enable dc cancel
    BTDIGITAL_REG(0xd02201bc)=0x800a00c4; //rx continue for dpdsample and freq 2470
    BTDIGITAL_REG(0xd0340020)=0x010E01C5;// open ana rxon for open adc clk
    BTDIGITAL_REG(0xd0330038)=0x950d; //dpd enable 4 open modem adcclk

    btdrv_write_rf_reg(0xe0,0x470d);//fix gain
    btdrv_write_rf_reg(0x0b,0x470d);//fix gain
    btdrv_write_rf_reg(0xc7,0xf0f8);//gen single tone inside
    btdrv_write_rf_reg(0x04,0x5255);//gen single tone inside

}

void btdrv_rxdpd_sample_deinit(void)
{
//  [switch to RX normal]
    BTDIGITAL_REG(0xd02201bc) = 0x0;
    BTDIGITAL_REG(0xd0340020) = 0x010E01C0;
    BTDIGITAL_REG(0xd0330038) = 0x850d;

//  [SET inside tone off]
    btdrv_write_rf_reg(0x04,0x5154);
    btdrv_write_rf_reg(0xc7,0xf008);
    btdrv_write_rf_reg(0xe0,0x470d);
}

#define BTTX_PATTEN (1)
#define BTTX_FREQ(freq) ((freq-2402)&0x7f)

void btdrv_rxdpd_sample_init_tx(void)
{
    //0x40000048,0x03ffffff; //release btcpu reset
    BTDIGITAL_REG(0xc00003ac)=0x00060020;//turn off bt sleep
    btdrv_delay(2000);
    //0xd0220050,0x0000b7b7;

    BTDIGITAL_REG(0xd0340020)=0x010E01C7;// open TRX path
#if BTTX_PATTEN
    BTDIGITAL_REG(0xd02201bc)=0x000aff00 | BTTX_FREQ(2479); //tx continue for dpdsample
#else
    BTDIGITAL_REG(0xd02201bc)=0x000a0000 | BTTX_FREQ(2479); //tx continue for dpdsample
#endif
#ifdef CHIP_BEST1000
    BTDIGITAL_REG(0x4001f034)=0x00001000; //open p1_4 for agpio
#endif
    BTDIGITAL_REG(0xd0330038)=0x950d; //dpd enable 4 open modem adcclk
#if 0
    //monitor adcclk from p2_6
    BTDIGITAL_REG(0x4001f004)=0x02000000;
    BTDIGITAL_REG(0x40000074)=0x00f000ff;
    BTDIGITAL_REG(0xd033002c)=0x00000060;
#endif

    btdrv_write_rf_reg(0x2d,0x7fa); //open ldo for ad/da
    btdrv_write_rf_reg(0x0e,0x700); //set dac gain
    btdrv_write_rf_reg(0x0f,0x2a18);//set dac gain dr
    btdrv_write_rf_reg(0x10,0x00be);//sel dac output

#if 0
    btdrv_write_rf_reg(0x12,0x1188);
    btdrv_write_rf_reg(0x04,0x52fe);
    btdrv_write_rf_reg(0x14,0x01fc);
    btdrv_write_rf_reg(0x0c,0x6d84);
    btdrv_write_rf_reg(0x07,0x02bb);
    btdrv_write_rf_reg(0x0a,0x0223);

    btdrv_write_rf_reg(0x02,0x2694);
    btdrv_write_rf_reg(0x0b,0x402d);
    btdrv_write_rf_reg(0x10,0x00b5);
#else
    //set ad/da loop
    BTDIGITAL_REG(0xd0350308)=0x05;// digital gain * 5
    BTDIGITAL_REG(0xd0350300)=0x04;// digital gain / 16
    btdrv_write_rf_reg(0x07,0x2bd); //
    btdrv_write_rf_reg(0x02,0x2294); //
    btdrv_write_rf_reg(0x0b,0x4028);//
    btdrv_write_rf_reg(0x0c,0x6584);//
#endif
}

void btdrv_rxdpd_sample_enable(uint8_t rxon, uint8_t txon)
{
#ifdef CHIP_BEST1000
    if (rxon){
        BTDIGITAL_REG(DPD_RX_BASE + 0x30) = 1;
        BTDIGITAL_REG(DPD_RX_BASE) = 3;
    }else{
        BTDIGITAL_REG(DPD_RX_BASE) = 0;
    }
    if (txon){
        BTDIGITAL_REG(DPD_TX_BASE + 0x30) = 1;
        BTDIGITAL_REG(DPD_TX_BASE) = 3;
    }else{
        BTDIGITAL_REG(DPD_TX_BASE) = 0;
    }
#endif
}
extern "C" void btdrv_btcore_extwakeup_irq_enable(bool on)
{
    if (on){
        *(volatile uint32_t *)(0xd033003c) |= (1<<14);
    }else{
        *(volatile uint32_t *)(0xd033003c) &= ~(1<<14);
    }
}

//[26:0] 0x07ffffff
//[27:0] 0x0fffffff

uint32_t btdrv_syn_get_curr_ticks(void)
{
    uint32_t value;

    value = BTDIGITAL_REG(0xd0220020) & 0x07ffffff;
    return value * 2;
}

// offset btclk = Master - Slave
static int32_t btdrv_syn_get_offset_ticks(uint16_t conidx)
{
    int32_t offset;
    offset = BTDIGITAL_REG(0xd02101c0+conidx*96) & 0x07ffffff;
    offset = (offset << 5)>>5;
    if(offset)
    {
        offset -= 1;    // Slave
        return offset *2;
    }
    else
    {
        return 0;       //Master
    }
}

// Clear trigger signal with software
extern "C" void  btdrv_syn_clr_trigger(void)
{
    BTDIGITAL_REG(0xd022007c) = BTDIGITAL_REG(0xd022007c) | (1<<31);
}


extern "C" char app_tws_mode_is_master(void);
extern  char app_tws_mode_is_only_mobile(void);

static void btdrv_syn_set_tg_ticks(uint32_t num)
{
#if 0
//    REG(0xd022007c) = REG(0xd022007c) | (1<<31);
    BTDIGITAL_REG(0xd022007c) = (BTDIGITAL_REG(0xd022007c) & 0x70000000) | (num & 0x0fffffff) | 0x60000000;
#else

#ifdef __TWS__
    if (app_tws_mode_is_master() || app_tws_mode_is_only_mobile())
    {
        BTDIGITAL_REG(0xd022007c) = (BTDIGITAL_REG(0xd022007c) & 0x70000000) | (num & 0x0fffffff) | 0x60000000;
    }
    else
#endif
    {
        BTDIGITAL_REG(0xd022007c) = (BTDIGITAL_REG(0xd022007c) & 0x70000000) | (num & 0x0fffffff);
    }
#endif
}

extern "C" void btdrv_syn_trigger_codec_en(uint32_t v)
{
    uint32_t reg;

    reg = BTDIGITAL_REG(0xd02201b4);
    if(v)
    {
        BTDIGITAL_REG(0xd02201b4) = reg | (1<<10);
    }
    else
    {
        BTDIGITAL_REG(0xd02201b4) = reg & (~(1<<10));
    }
}


extern "C" uint32_t btdrv_get_syn_trigger_codec_en(void)
{

    return BTDIGITAL_REG(0xd02201b4);

}


extern "C" uint32_t btdrv_get_trigger_ticks(void)
{

    return BTDIGITAL_REG(0xd022007c);

}


// Can be used by master or slave
// Ref: Master bt clk
extern "C" uint32_t bt_syn_get_curr_ticks(uint16_t conhdl)
{
    int32_t curr,offset;

    curr = btdrv_syn_get_curr_ticks();
    if(conhdl>=0x80)
    offset = btdrv_syn_get_offset_ticks(conhdl-0x80);
    else
        offset = 0;
//    TRACE("[%s] curr(%d) + offset(%d) = %d", __func__, curr , offset,curr + offset);
    return (curr + offset) & 0x0fffffff;
}

// Can be used by master or slave
// Ref: Master bt clk
extern "C" void bt_syn_set_tg_ticks(uint32_t val,uint16_t conhdl)
{
    int32_t offset;
    if(conhdl>=0x80)
        offset = btdrv_syn_get_offset_ticks(conhdl-0x80);
    else
        offset = 0;
#ifdef __TWS__
    if (app_tws_mode_is_master() && (offset !=0))
        TRACE("ERROR OFFSET !!");
#endif
    btdrv_syn_set_tg_ticks(val - offset);
}




extern "C"  void btdrv_enable_playback_triggler(uint8_t triggle_mode)
{
    if(triggle_mode == ACL_TRIGGLE_MODE)
    {
        BTDIGITAL_REG(0xd02201cc) &= (~0x4000000);
    }
    else if(triggle_mode == SCO_TRIGGLE_MODE)
    {
        BTDIGITAL_REG(0xd02201cc) |= 0x4000000;
    }
}

/*
bit23  1
bit17  1:master  0:slave
*/

extern "C"  void btdrv_set_tws_role_triggler(uint8_t tws_mode)
{
    TRACE("SET TWS TRIGGLE ROLE =%x",tws_mode);
    BTDIGITAL_REG(0xd02201cc) |= 0x800000;
    if(tws_mode == 1 || tws_mode == 3)
    {
        BTDIGITAL_REG(0xd02201cc) |= 0x20000;
    }
    else if(tws_mode == 2)
    {
        BTDIGITAL_REG(0xd02201cc) &= (~0x20000);
    }

}





extern "C"  void btdrv_set_bt_pcm_triggler_en(uint8_t  en)
{
    BTDIGITAL_REG(0xd0224024) |= 0x8;
    if(en)
    {
        BTDIGITAL_REG(0xd0224024) &= (~0x10);
    }
    else
    {
        BTDIGITAL_REG(0xd0224024) |= 0x10;
    }
}

extern "C"  void btdrv_set_bt_pcm_triggler_delay(uint8_t  delay)
{
    if(delay > 0x3f)
    {
        TRACE("delay is error value");
        return;
    }
    TRACE("0XD0224024=%x",BTDIGITAL_REG(0xd0224024));
    BTDIGITAL_REG(0xd0224024) &= ~0x3f00;
    BTDIGITAL_REG(0xd0224024) |= (delay<<8);
    TRACE("exit :0XD0224024=%x",BTDIGITAL_REG(0xd0224024));


}

void btdrv_set_powerctrl_rssi_low(uint16_t rssi)
{
	if(hal_get_chip_metal_id()>=2)    
	{
		*(volatile uint16_t*)(0xc0003bec) = rssi;         //rssi low
	}
}
