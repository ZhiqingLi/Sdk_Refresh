#include "plat_types.h"
#include "hal_i2c.h"
#include "hal_uart.h"
#include "bt_drv.h"
#include "cmsis_os.h"
#include "hal_cmu.h"
#include "hal_trace.h"
#include "hal_timer.h"
#include "tgt_hardware.h"
#include "bt_drv_internal.h"
#include "hal_chipid.h"

#define AUTO_CAL                                0
//#define BT_LOW_POWER_MODE                       1
//#define BT_HIGH_PERFORMANCE_MODE                2
//#define BT_RFMODE                               (BT_LOW_POWER_MODE)
//#define __BES2200__DIV_2__

//#define BTDRV_XTAL_FCAP_DIRECT_UPDATE
//#define DEBUG_FCAP_SPI_UPDATE

#ifndef BT_RF_MAX_XTAL_TUNE_PPB
// Default 10 ppm/bit or 10000 ppb/bit
#define BT_RF_MAX_XTAL_TUNE_PPB                 10000
#endif

#ifndef BT_RF_XTAL_TUNE_FACTOR
// Default 0.2 ppm/bit or 200 ppb/bit
#define BT_RF_XTAL_TUNE_FACTOR                  200
#endif

#define DEFAULT_XTAL_FCAP                       0x8080

#define XTAL_FCAP_NORMAL_SHIFT                  0
#define XTAL_FCAP_NORMAL_MASK                   (0xFF << XTAL_FCAP_NORMAL_SHIFT)
#define XTAL_FCAP_NORMAL(n)                     BITFIELD_VAL(XTAL_FCAP_NORMAL, n)

#define BT_XTAL_CMOM_DR                         (1 << 13)

#define RF_REG_XTAL_FCAP                        0xE9
#define RF_REG_XTAL_CMOM_DR                     0xE8

/* ljh add for sync modify rf RF_REG_XTAL_FCAP value*/
#define SPI_TRIG_NEG_BUF_ADDR                   0xD0217204
#define SPI_TRIG_CTRL_ADDR                      0xD02201C0

#define SPI_TRIG_TX_NEG_ON                      (0x7F << 0)
#define SPI_TRIG_TX_NEG_COUNT(n)                (((n) & 0x7F) << 0)
#define SPI_TRIG_TX_NEG_TIMEOUT                 MS_TO_TICKS(50)

#define SPI_TRIG_RX_NEG_ON                      (0x7F << 8)
#define SPI_TRIG_RX_NEG_COUNT(n)                (((n) & 0x7F) << 8)
#define SPI_TRIG_RX_NEG_TIMEOUT                 MS_TO_TICKS(3)

#define SPI_TRIG_NEG_ON                         SPI_TRIG_RX_NEG_ON
#define SPI_TRIG_NEG_COUNT                      SPI_TRIG_RX_NEG_COUNT(3)
#define SPI_TRIG_NEG_TIMEOUT                    SPI_TRIG_RX_NEG_TIMEOUT

#ifdef BTDRV_XTAL_FCAP_DIRECT_UPDATE
#define BIT_OFFSET_VALUE_REG                    0xD02101C8
#else
//#define BIT_OFFSET_CMD_REG                      0xD0217210
//#define BIT_OFFSET_VALUE_REG                    0xD0217214

#define BIT_OFFSET_CMD_REG                      (bitoffset_cmd_reg)
#define BIT_OFFSET_VALUE_REG                    (bitoffset_value_reg)

#endif
#define BIT_OFFSET_MODE_REG                     0xD02200F0

#define BIT_OFFSET_MODE_AUTO_TRACK              (1 << 0)

enum BIT_OFFSET_CMD_TYPE_T {
    BIT_OFFSET_CMD_STOP = 0,
    BIT_OFFSET_CMD_START,
    BIT_OFFSET_CMD_ACK,
};

static uint16_t xtal_fcap = DEFAULT_XTAL_FCAP;
static uint16_t init_xtal_fcap = DEFAULT_XTAL_FCAP;

#ifndef BTDRV_XTAL_FCAP_DIRECT_UPDATE
static uint32_t spi_trig_time;
#endif


//rampup start     1--->2->3 rampup ok
//rampdown start 3-->2-->1-->0  rampdown ok


//3a06=3b3f
//for pa@1.8v
struct bt_drv_tx_table_t{
    uint16_t tbl[16][3];
};

const struct bt_drv_tx_table_t rf_tx_pwr_tbl[] =
{
    {{
    }},
    {{
    }},
    {{
    }},
    {{
    }},
    {{
    }}
};

struct RF_SYS_INIT_ITEM {
    uint16_t reg;
    uint16_t set;
    uint16_t mask;
    uint16_t delay;
};

static const struct RF_SYS_INIT_ITEM rf_sys_init_tbl[] =
{
    {0xb5,0x8000,0x0000,0},
    {0xc2,0x0188,0x000f,0},
    {0xc4,0x0003,0x0000,0},
};

const uint16_t rf_init_tbl_0[][3] =
{
    {0x88,0x8640,0},
    {0x8b,0x8a4a,0},//set rx flt cap,filter start up
    {0x8c,0x9000,0},
    {0x8e,0x0128,0},//adc noise reduction
    {0x90,0x8814,0},//enlarge txflt BW
    //{0x91,0x2100,0},
    {0x91,0x2040,0},//by walker 20180427
    {0x92,0xc484,0},
    {0x97,0x2643,0},
    {0x98,0x0324,0},
    {0x9a,0x4470,0},//div2 rc
    {0x9b,0xfe42,0},
    {0x9d,0x286c,0},//enhance xtal drv
    {0xa3,0x0789,0},
    {0xa8,0x2c0e,0},////delay paon
    {0xab,0x100c,0}, //flt pwrup delay
    {0xb0,0x0000,0},
    {0xb1,0x0000,0},
    {0xb3,0x30f3,0},//
    {0xb4,0x883c,0},
    {0xb6,0x3150,0},
    {0xb7,0x5828,0},// 2g4_vco_div2=1
    {0xb9,0x0000,0},//cap3g6 off
    {0xba,0x104e,0},
    {0xc3,0x0090,0},
    {0xc5,0x4b50,0},//vco ictrl dr
    {0xc9,0x3e08,0},//vco ictrl
    //{0xcf,0x8000,0},//rx lna att
    //{0xd0,0xf980,0},//rx gain ctrl, increase filter gain
    {0xd1,0x8422,0},//set gain 0dB   ////disable dr tx gain
    {0xd3,0x1d1d,0},//vcdl_bit=001110 delay ctrl
    {0xd4,0x000f,0},
    {0xd5,0x4000,0},
    {0xd6,0x7980,0},
    //{0xdf,0x0007,0},
    {0xe8,0xe000,0},
    {0xf3,0x0c41,0},
    //{0x80,0xa010,0},//page 1
    {0x1a6,0x0600,0},
    {0x1d4,0x0000,0},
    //{0x1d6,0x7858,0},//dccal ready=1
    {0x1de,0x2000,0},
    {0x1df,0x2087,0},
    {0x1f3,0x0300,0},//dig ctrl lo offset -0.75MHz
    //{0x1f4,0x2242,0},//sdm_dig dec_sel=010/001
    {0x1f4,0x2241,0},//by walker 20180427
    {0x1fa,0x03df,0},//rst needed
    {0x1fa,0x83df,0},
	{0xeb,0x000c,0},//gain_idx:0
    {0x181,0x004f,0},
    {0xec,0x002d,0},//gain_idx:1
    {0x182,0x004f,0},
    {0xed,0x025,0},//gain_idx:2
    {0x183,0x004f,0},
    {0xee,0x0127,0},//gain_idx:3
    {0x184,0x004f,0},
    {0xef,0x0377,0},//gain_idx:4
    {0x185,0x004f,0},
    {0xf0,0x0387,0},//gain_idx:5
    {0x186,0x004f,0},
    {0xf1,0x03f7,0},//gain_idx:6
    {0x187,0x004d,0},
    {0xf2,0x03f7,0},//gain_idx:7
    {0x188,0x0049,0},


#ifdef __HW_SW_AGC__
    //[HW-SW AGC]
    {0x1cc,0x334f,0},
    {0x1cd,0x293f,0},
    {0x1ce,0x202f,0},
    {0x1cf,0x1524,0},
    {0x1d0,0x0f1e,0},
    {0x1d1,0x0b19,0},
    {0x1d2,0x0512,0},
    {0x1d3,0x0000,0},
#endif

    
    //[below only nfmi ]
    //for 27.25m @ air, need tx if = -0.75m                                                               
    {0x1dc,0x0040,0},//set nfmi lccap                                 
    {0x1d6,0x7e58,0},//set nfmi rxflt dccal_ready=1       
    {0x1d8,0xbd89,0},//nfmi tx freqword
    {0x1d9,0xbd89,0},//nfmi tx freqword
    {0x1da,0xbd89,0},//nfmi tx freqword
    {0x1db,0xbd89,0},//nfmi tx freqword
    {0x1e9,0x0b0b,0},//set rfpll divn in nfmi tx mode                 
    {0x1ea,0x0b0b,0},                                                 
    {0x1eb,0x0c0c,0},//set rfpll divn in nfmi rx mode                 
    {0x1ec,0x0c0c,0},      
    {0x1ef,0xbe27,0},//nfmi rx freqword
    {0x1f0,0xbe27,0},//nfmi rx freqword
    {0x1f1,0xbe27,0},//nfmi rx freqword
    {0x1f2,0xbe27,0},//nfmi rx freqword
    {0x1c8,0xc3c0,0},//en rfpll bias in nfmi mode   
    {0x81,0x920f,0},
    {0x82,0x0caa,0}, // select rfpll and cplx_dir=0 and en rxflt test
    //{0xb4,0x883e,0}, // set debug freqword mode
    {0xb4,0x883c,0}, // 
    {0xce,0xfc08,0}, //set nfmi rx gain     
    //{0xcf,0x8032,0}, // set rx_flt_gain=0x01 & gm_cell_gain=0x02     
    {0xf4,0x2181,0}, // set flt_cap to match flt_gain                
   // {0xc2,0x9988,0}, // set divn_adclk=12                            
    //{0x8a,0x4eb4,0},// fix nfmi mode and use rfpll       
#ifdef __HW_SW_AGC__
    {0x8a,0x4eac,0},//  use rfpll for nfmi adclk   
#else
    {0x8a,0x4ea4,0},//  use rfpll for nfmi adclk   
#endif
    //{0x90,0x8918,0},//txflt cap                                      
    {0x86,0xdc99,0},//set nfmi tmix                                  
    {0xd0,0xe91f,0},//set nfmi pa ibit                               
    {0xcf,0x7f32,0},//set nfmi pa eff                                
    {0x87,0x8711,0},//nfmi tx_mix cim3 res and tx_pa res             
    {0xdf,0x2006,0},//set nfmi tmix vcm   


    {0x189,0x000b,0},//min tx gain
    {0x18a,0x0007,0},
    {0x18b,0x0001,0},  //max tx gain 
#ifdef __HW_AGC__
    {0x1c7,0x007d,0},//open nfmi adc and vcm bias current
    {0xc4,0x00c3,0},// enalbe nfmi adc clk
    {0x8c,0x5000,0},//select i2v output of bt rxflt
    //{0xad,0x008c},//hwagc_on
    
    {0xeb,0x002f,0},// -20.7dB 0 lna gain index
    {0xec,0x0005,0},//-16dB 1
    {0xed,0x000d,0},//-10dB 2
    {0xee,0x000d,0},//-10dB 3
    {0xef,0x0004,0},//-6dB 4
    {0xf0,0x0004,0},//-6dB 5
    {0xf1,0x000c,0},//0dB 6
    {0xf2,0x000c,0},//0dB 7 lna gain index

    {0x1c0,0x01ce,0},//pu rxflt ibias at idle mode
    {0x1c4,0xffcf,0},//pu tst buf ibias at bt mode
    {0x1c5,0xf8ff,0},//pu tst buf ibias at bt mode

    {0xad,0x00cc,1},//hwagc en=1
    {0xd0,0xf99f,0},//i2v flt gain dr=1
    {0xb0,0x0003,1},//pu vcm gen maunal

    {0xaf,0x4000,0}, // pwup=0
    {0x90,0x8858,0}, // rstb=0
    {0xaf,0xc000,0}, // pwup=1
    {0x90,0x88d4,1}, // rstb=1

    {0xcf,0x7f32,0},//lna gain dr=0
    {0xdf,0x2006,0},//lna gain dr=0
    {0xd0,0xe91f,0},//i2v flt gain dr=0
    {0xa2,0x67c2,0},
#endif    
};

static uint16_t current_conidx = 0;

uint32_t bitoffset_cmd_reg;
uint32_t bitoffset_value_reg;
uint32_t bitoffset_linkid_reg;


void btdrv_rf_set_conidx(uint32_t conidx)
{
    TRACE("%s conidx %d\n", __func__, conidx);
    current_conidx = conidx;
    BTDIGITAL_REG(bitoffset_linkid_reg) = conidx;
}


uint8_t btdrv_rf_get_conidx(void)
{
    return current_conidx;
}



///tws triggle
void btdrv_rf_trig_patch_enable(bool enable)
{

        if (enable) {
            BTDIGITAL_REG(0xd02201b8) |= 0x80000000;
        }
        else {
            BTDIGITAL_REG(0xd02201b8) &= (~0x80000000);
        }
}

void btdrv_tws_trig_role(uint8_t role)
{
      BTDIGITAL_REG(0xd02201b8) |= ((role&1)<<30);
}


uint32_t btdrv_rf_get_max_xtal_tune_ppb(void)
{
    return BT_RF_MAX_XTAL_TUNE_PPB;
}


uint32_t btdrv_rf_get_xtal_tune_factor(void)
{
    return BT_RF_XTAL_TUNE_FACTOR;
}

  

void btdrv_rf_init_xtal_fcap(uint32_t fcap)
{
    return;

    xtal_fcap = SET_BITFIELD(xtal_fcap, XTAL_FCAP_NORMAL, fcap);
    btdrv_write_rf_reg(RF_REG_XTAL_FCAP, xtal_fcap);
    init_xtal_fcap = xtal_fcap;
}

uint32_t btdrv_rf_get_init_xtal_fcap(void)
{
    return GET_BITFIELD(init_xtal_fcap, XTAL_FCAP_NORMAL);
}

uint32_t btdrv_rf_get_xtal_fcap(void)
{
    return GET_BITFIELD(xtal_fcap, XTAL_FCAP_NORMAL);
}

void btdrv_rf_set_xtal_fcap(uint32_t fcap)
{
    return;

    xtal_fcap = SET_BITFIELD(xtal_fcap, XTAL_FCAP_NORMAL, fcap);
#ifdef BTDRV_XTAL_FCAP_DIRECT_UPDATE
    btdrv_write_rf_reg(RF_REG_XTAL_FCAP, xtal_fcap);
#else
    // Set the RF_REG_XTAL_FCAP value and the flag in BT controller
    uint16_t value = ((xtal_fcap & 0xFF00) >> 8) | ((xtal_fcap & 0xFF) << 8);
    BTDIGITAL_REG(SPI_TRIG_NEG_BUF_ADDR) = (BTDIGITAL_REG(SPI_TRIG_NEG_BUF_ADDR) & 0x0000ffFF) |
        (value << 16);
    BTDIGITAL_REG(SPI_TRIG_CTRL_ADDR) = SPI_TRIG_NEG_COUNT;
    spi_trig_time = hal_sys_timer_get();
#endif
}

int btdrv_rf_xtal_fcap_busy(void)
{
    return 0;

#ifdef BTDRV_XTAL_FCAP_DIRECT_UPDATE
    return 0;
#else
    if (BTDIGITAL_REG(SPI_TRIG_CTRL_ADDR) & SPI_TRIG_NEG_ON) {
        if (hal_sys_timer_get() - spi_trig_time > SPI_TRIG_NEG_TIMEOUT) {
            BTDIGITAL_REG(SPI_TRIG_CTRL_ADDR) &= ~SPI_TRIG_NEG_ON;
        }
#ifdef DEBUG_FCAP_SPI_UPDATE
        uint16_t val;
        btdrv_read_rf_reg(RF_REG_XTAL_FCAP, &val);
        TRACE("*** NEW %02X reg: 0x%04x fcap=%u time=%u\n\n", RF_REG_XTAL_FCAP, val, val >> 8, TICKS_TO_MS(hal_sys_timer_get() - spi_trig_time));
#endif
        return 1;
    }
    if (BTDIGITAL_REG(BIT_OFFSET_CMD_REG) != BIT_OFFSET_CMD_ACK) {
        return 1;
    }
    return 0;
#endif
}

void btdrv_rf_bit_offset_track_enable(bool enable)
{
    return;

    // BT controller seems to set this register too
    if (enable) {
        BTDIGITAL_REG(BIT_OFFSET_MODE_REG) &= ~BIT_OFFSET_MODE_AUTO_TRACK;
#ifndef BTDRV_XTAL_FCAP_DIRECT_UPDATE
        BTDIGITAL_REG(BIT_OFFSET_CMD_REG) = BIT_OFFSET_CMD_START;
#endif
    } else {
        BTDIGITAL_REG(BIT_OFFSET_MODE_REG) |= BIT_OFFSET_MODE_AUTO_TRACK;
#ifndef BTDRV_XTAL_FCAP_DIRECT_UPDATE
        BTDIGITAL_REG(BIT_OFFSET_CMD_REG) = BIT_OFFSET_CMD_STOP;
#endif
    }
}

uint32_t btdrv_rf_bit_offset_get(void)
{
    return BTDIGITAL_REG(BIT_OFFSET_VALUE_REG);
}

void btdrv_xtal_rf_init(void)
{
    uint16_t val;

    btdrv_read_rf_reg(RF_REG_XTAL_CMOM_DR, &val);
    val |= BT_XTAL_CMOM_DR;
    btdrv_write_rf_reg(RF_REG_XTAL_CMOM_DR, val);

    bitoffset_cmd_reg = 0xc0000034;
    bitoffset_value_reg = 0xc0000038;
    bitoffset_linkid_reg = 0xc0000030;
}

void btdrv_rf_tx_init(int idx)
{
    const struct bt_drv_tx_table_t *tx_tbl_p;

    tx_tbl_p = &rf_tx_pwr_tbl[idx];

    for(uint8_t i=0; i< sizeof(((struct bt_drv_tx_table_t *)(0))->tbl)/sizeof(((struct bt_drv_tx_table_t *)(0))->tbl[0]); i++){
        btdrv_write_rf_reg(tx_tbl_p->tbl[i][0],tx_tbl_p->tbl[i][1]);
        if(tx_tbl_p->tbl[i][2] !=0)
            btdrv_delay(tx_tbl_p->tbl[i][2]);//delay
    }
}

uint8_t btdrv_rf_init(void)
{
    uint16_t value;
    const uint16_t (*rf_init_tbl_p)[3];
    uint32_t tbl_size;
    //uint8_t ret;
    uint8_t i;

    for (i = 0; i < ARRAY_SIZE(rf_sys_init_tbl); i++) {
        btdrv_read_rf_reg(rf_sys_init_tbl[i].reg, &value);
        value = (value & ~rf_sys_init_tbl[i].mask) | rf_sys_init_tbl[i].set;
        if (rf_sys_init_tbl[i].delay) {
            btdrv_delay(rf_sys_init_tbl[i].delay);
        }
        btdrv_write_rf_reg(rf_sys_init_tbl[i].reg, value);
    }

    rf_init_tbl_p = &rf_init_tbl_0[0];
    tbl_size = sizeof(rf_init_tbl_0)/sizeof(rf_init_tbl_0[0]);

    for(i=0;i< tbl_size;i++)
    {
        btdrv_write_rf_reg(rf_init_tbl_p[i][0],rf_init_tbl_p[i][1]);
        if(rf_init_tbl_p[i][2] !=0)
            btdrv_delay(rf_init_tbl_p[i][2]);//delay
        btdrv_read_rf_reg(rf_init_tbl_p[i][0],&value);
        //TRACE("reg=%x,v=%x",rf_init_tbl_p[i][0],value);
    }

    btdrv_rf_tx_init(1);

    btdrv_xtal_rf_init();

    return 1;
}

