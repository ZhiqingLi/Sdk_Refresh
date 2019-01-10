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
#include "pmu.h"

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

#define DEFAULT_XTAL_FCAP                       0x8000 //0x3600

#define XTAL_FCAP_NORMAL_SHIFT                  8
#define XTAL_FCAP_NORMAL_MASK                   (0xFF << XTAL_FCAP_NORMAL_SHIFT)
#define XTAL_FCAP_NORMAL(n)                     BITFIELD_VAL(XTAL_FCAP_NORMAL, n)

#define RF_REG_XTAL_FCAP                        0xC2

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
#if 0
    {{
	    {0x1d8,0x0006,0},	//tx filter gain
	    {0x1d9,0x0006,0},	
	    {0x1da,0x1006,0},	//0db
	    {0x1db,0x1006,0},
	    {0x1dc,0x1006,0},
	    {0x1dd,0x1006,0}, 
	    {0x1de,0x1006,0}, 
	    {0x1df,0x1006,0}, 
	    
	    {0x1c8,0x00f0,0},	//dac gain
	    {0x1c9,0x0070,0},	
	    {0x1ca,0x00f0,0},	//0db
	    {0x1cb,0x0070,0},
	    {0x1cc,0x0070,0},
	    {0x1cd,0x0070,0}, 
	    {0x1ce,0x0070,0}, 
	    {0x1cf,0x0070,0}, 
    }},
#else
    {{

	    {0x1d8,0x0002,0},	//tx filter gain
	    {0x1d9,0x0006,0},	//tx filter gain
	    {0x1da,0x0006,0},	
//	    {0x1da,0x1006,0},	//0db
	    {0x1db,0x1006,0},
	    {0x1dc,0x1006,0},
	    {0x1dd,0x1006,0}, 
	    {0x1de,0x1006,0}, 
	    {0x1df,0x1006,0}, 
	    
	    {0x1c8,0x0070,0},	//dac gain
	    {0x1c9,0x00f0,0},	//dac gain
	    {0x1ca,0x0070,0},	
	//    {0x1ca,0x00f0,0},	//0db
	    {0x1cb,0x0070,0},
	    {0x1cc,0x0070,0},
	    {0x1cd,0x0070,0}, 
	    {0x1ce,0x0070,0}, 
	    {0x1cf,0x0070,0}, 
    }},

#endif
    {{
	    {0x1d8,0x0006,0},	//tx filter gain
	    {0x1d9,0x1006,0},	
	    {0x1da,0x0006,0},	//3db
	    {0x1db,0x1006,0},
	    {0x1dc,0x1006,0},
	    {0x1dd,0x1006,0}, 
	    {0x1de,0x0006,0}, 
	    {0x1df,0x0004,0}, 
	    
	    {0x1c8,0x0070,0},	//dac gain
	    {0x1c9,0x00f0,0},	
	    {0x1ca,0x0000,0},	//3db
	    {0x1cb,0x0070,0},
	    {0x1cc,0x0070,0},
	    {0x1cd,0x00f0,0}, 
	    {0x1ce,0x00f0,0}, 
	    {0x1cf,0x00f0,0}, 
    }},
    {{
	    {0x1d8,0x1006,0},	//tx filter gain
	    {0x1d9,0x1006,0},	
	    {0x1da,0x1006,0},	//6db
	    {0x1db,0x1006,0},
	    {0x1dc,0x1006,0},
	    {0x1dd,0x1006,0}, 
	    {0x1de,0x1006,0}, 
	    {0x1df,0x1006,0}, 
	    
	    {0x1c8,0x00f0,0},	//dac gain
	    {0x1c9,0x0070,0},	
	    {0x1ca,0x0030,0},	//6db
	    {0x1cb,0x0070,0},
	    {0x1cc,0x0070,0},
	    {0x1cd,0x0070,0}, 
	    {0x1ce,0x0070,0}, 
	    {0x1cf,0x0070,0}, 
    }},
    {{
	    {0x1d8,0x1006,0},	//tx filter gain
	    {0x1d9,0x1006,0},	
	    {0x1da,0x1006,0},	//9db
	    {0x1db,0x1006,0},
	    {0x1dc,0x1006,0},
	    {0x1dd,0x1006,0}, 
	    {0x1de,0x1006,0}, 
	    {0x1df,0x1006,0}, 
	    
	    {0x1c8,0x0070,0},	//dac gain
	    {0x1c9,0x0030,0},	
	    {0x1ca,0x0010,0},	//9db
	    {0x1cb,0x0070,0},
	    {0x1cc,0x0070,0},
	    {0x1cd,0x0070,0}, 
	    {0x1ce,0x0070,0}, 
	    {0x1cf,0x0070,0}, 
    }},
    {{
	    {0x1d8,0x1006,0},	//tx filter gain
	    {0x1d9,0x1006,0},	
	    {0x1da,0x1006,0},	//9db
	    {0x1db,0x1006,0},
	    {0x1dc,0x1006,0},
	    {0x1dd,0x1006,0}, 
	    {0x1de,0x1006,0}, 
	    {0x1df,0x1006,0}, 
	    
	    {0x1c8,0x0070,0},	//dac gain
	    {0x1c9,0x0030,0},	
	    {0x1ca,0x0010,0},	//9db
	    {0x1cb,0x0070,0},
	    {0x1cc,0x0070,0},
	    {0x1cd,0x0070,0}, 
	    {0x1ce,0x0070,0}, 
	    {0x1cf,0x0070,0}, 
    }}
};


const struct bt_drv_tx_table_t rf_tx_pwr_tbl_2[] =
{
    {{

	    {0x1d8,0x0002,0},	//tx filter gain
	    {0x1d9,0x0002,0},	//tx filter gain
	    {0x1da,0x1006,0},	
//	    {0x1da,0x1006,0},	//0db
	    {0x1db,0x1006,0},
	    {0x1dc,0x1006,0},
	    {0x1dd,0x1006,0}, 
	    {0x1de,0x1006,0}, 
	    {0x1df,0x1006,0}, 
	    
	    {0x1c8,0x00f0,0},	//dac gain
	    {0x1c9,0x0070,0},	//dac gain
	    {0x1ca,0x0020,0},	
	//    {0x1ca,0x00f0,0},	//0db
	    {0x1cb,0x0070,0},
	    {0x1cc,0x0070,0},
	    {0x1cd,0x0070,0}, 
	    {0x1ce,0x0070,0}, 
	    {0x1cf,0x0070,0}, 
    }},

    {{
	    {0x1d8,0x0006,0},	//tx filter gain
	    {0x1d9,0x0006,0},	
	    {0x1da,0x0006,0},	//3db
	    {0x1db,0x1006,0},
	    {0x1dc,0x1006,0},
	    {0x1dd,0x1006,0}, 
	    {0x1de,0x0006,0}, 
	    {0x1df,0x0004,0}, 
	    
	    {0x1c8,0x00f0,0},	//dac gain
	    {0x1c9,0x0090,0},	
	    {0x1ca,0x0020,0},	//3db
	    {0x1cb,0x0070,0},
	    {0x1cc,0x0070,0},
	    {0x1cd,0x00f0,0}, 
	    {0x1ce,0x00f0,0}, 
	    {0x1cf,0x00f0,0}, 
    }},
};



const uint16_t rf_init_tbl_0[][3] =
{
{0x99,0x6d42,1},
{0x9b,0x089c,1},
{0x9c,0x005c,1},
//{0xc0,0x8000,1},//page 1

{0x19b,0xdd2a,1},
{0x19c,0xdd2a,1},

{0x188,0x7890,1},
//{0xc0,0x0000,1},//page back 0
//{0xc0,0x8000,1},//page 1
{0x181,0x0820,1},//tmx cap bank //0x0810
{0x182,0x810d,1},//pad cap bank //0x8108
{0x183,0x8830,1},//pad cscd bit
{0x184,0x6c2c,1},//pad & vco
{0x185,0x00c3,1},
{0x186,0x0020,1},
{0x187,0x0020,1},
{0x189,0x48a2,1},
{0x18a,0x040a,1},
{0x18f,0xdc00,1},//enhance dac driver
{0x191,0x15de,1},
//{0x194,0x0000,1},//rx current
{0x19a,0x6ecc,1},
{0x1e3,0x032f,1},//rf timing
{0x1ec,0x0190,1},//pu log & adc clk
{0x1f9,0x3c00,1},//pu tx en
//{0xc0,0x0000,1},//page back 0
//{0xc0,0x4000,1},//page 2 tx dc cal mannual
{0x290,0x8080,1},
{0x298,0x8e71,1},
//{0xc0,0x0000,1},//page back 0
{0x81,0x7252,1},
{0x82,0xe002,1},//kvco
{0x83,0x0862,1},//set dr of gain and cap bit
{0x84,0x0004,1},//pu ldo filter and enable 2g4 path
{0x85,0x7e90,1},//sel clk_edge and opamp mode
{0x86,0x310c,1},//tx flt
{0x87,0x0519,1},//set dr of dc cal bit
{0x8a,0x4a12,1},
{0x8b,0x4300,1},
{0x8c,0x47bc,1},//set cap and gain
{0x8d,0x2025,1},
{0x8e,0xd10c,1},
{0x8f,0x0420,1},//pu tx chain
{0x90,0x8c00,1},
{0x91,0xc240,1},
{0x94,0x0f00,1},
{0x95,0x8700,1},
{0x98,0xe058,1},//vco bt div2 freq
{0x9e,0x000a,1},
{0xa8,0x0600,1},//IF=1.5M
{0xaa,0x07c0,1},//multx1
{0xac,0x905c,1},
{0xb7,0x0400,1},
{0xb8,0xa200,1},//log div2
{0xb9,0x809f,1},//vco buffer rc
{0xba,0x4000,1},
{0xbc,0x4920,1},//log buffer
{0xbd,0x0623,1},//set div ratio for adc
{0xbe,0x4164,1},
{0xc1,0x5960,1},//set avdd/dvdd rc
{RF_REG_XTAL_FCAP,DEFAULT_XTAL_FCAP,1},//for sk cap
{0xc4,0x45a9,1},
{0xc7,0x8000,1},
{0xc8,0x1414,1},
{0xc9,0xe312,1},
{0xca,0x8008,1},
{0xcb,0x8000,1},//refclk for sdm
{0xd5,0x4003,1},
{0xdc,0x0316,1},
{0xdd,0x4210,1},
{0xde,0x848e,1},
{0xdf,0x430e,1},
{0xe0,0x15a8,1},//set inter frequency and cplx
{0xe6,0x0400,1},
{0xe9,0x1000,1},//enable flt output to test buffer
{0xf0,0x8000,1},//5g top current bias
{0xf3,0x08ff,1},
{0xf4,0x40c0,1},
{0xfc,0x3b80,1},//small lna gain
{0xda,0x8038,1},//mdll_reset_dr resetb=0
{0xda,0x803c,1},//resetb=1
{0xab,0x3620,1},//2bit frac
{0xf4,0x4080,1},
{0xa3,0x2203,1},
{0x81,0x7242,1},

//rxgain
{0x1f0,0x0038,0}, //GRx:69 GOffset:5
{0x1b8,0x203d,0},
//{0x1b8,0x3c3d,0},
{0x1f1,0x0038,0}, 
//{0x1b9,0x2024,0},//GRx:47 GOffset:5
{0x1b9,0x202c,0},  //GRx:57 GOffset:5
{0x1f2,0x0038,0}, 
//{0x1ba,0x2023,0},//GRx:41 GOffset:5
{0x1ba,0x201b,0},//GRx:45 GOffset:5
{0x1f3,0x0038,0}, //GRx:34 GOffset:5
{0x1bb,0x200a,0},
{0x1f4,0x0018,0}, //GRx:22 GOffset:5
{0x1bc,0x2009,0},
{0x1f5,0x0019,0}, //GRx:15 GOffset:5
{0x1bd,0x2009,0},
{0x1f6,0x0009,0}, //GRx:3 GOffset:5
{0x1be,0x2008,0},
{0x1f7,0x0009,0}, //GRx:3 GOffset:5
{0x1bf,0x2008,0},
#ifdef __PASS_CI_TEST_SETTING__
{0x1f1,0x0039,0}, 
{0x1b9,0x2424,0},
{0x1f2,0x0039,0}, 
{0x1ba,0x202b,0},
#endif
};

const uint16_t rf_init_tbl_1[][3] =
{
{0x99,0x6d42,1},
{0x9b,0x089c,1},
{0x9c,0x005c,1},
//{0xc0,0x8000,1},//page 1

{0x19b,0xdd2a,1},
{0x19c,0xdd2a,1},

{0x188,0x7890,1},
//{0xc0,0x0000,1},//page back 0
//{0xc0,0x8000,1},//page 1
//{0x181,0x0810,1},//tmx cap bank
{0x181,0x0814,1},//tmx cap bank  xierenzheong for 2402->2480 <1db
//{0x182,0x8108,1},//pad cap bank
{0x182,0x8108,1},//pad cap bank  xierenzheong for 2402->2480 <1db
{0x183,0x8830,1},//pad cscd bit
{0x184,0x8c2c,1},//pad & vco
{0x185,0x00c3,1},
{0x186,0x0020,1},
{0x187,0x0020,1},
{0x189,0x48a2,1},
{0x18a,0x040a,1},
{0x18f,0xdc00,1},//enhance dac driver
{0x191,0x15de,1},
//{0x194,0x8000,1},//rx current
{0x19a,0x6ecc,1},
{0x1e3,0x032f,1},//rf timing
{0x1ec,0x0190,1},//pu log & adc clk
{0x1f9,0x3c00,1},//pu tx en
//{0xc0,0x0000,1},//page back 0
//{0xc0,0x4000,1},//page 2 tx dc cal mannual
{0x290,0x8080,1},
{0x298,0x8e71,1},
//{0xc0,0x0000,1},//page back 0
{0x81,0x7252,1},
{0x82,0xe002,1},//kvco
{0x83,0x0862,1},//set dr of gain and cap bit
{0x84,0x0004,1},//pu ldo filter and enable 2g4 path
{0x85,0x7e90,1},//sel clk_edge and opamp mode
{0x86,0x310c,1},//tx flt
{0x87,0x0519,1},//set dr of dc cal bit
{0x8a,0x4a12,1},
{0x8b,0x4300,1},
{0x8c,0x47bc,1},//set cap and gain
{0x8d,0x2025,1},
{0x8e,0xd106,1},
{0x8f,0x0420,1},//pu tx chain
{0x90,0x8c00,1},
{0x91,0xc240,1},
{0x94,0x0f00,1},
{0x95,0x8700,1},
{0x98,0xe058,1},//vco bt div2 freq
{0x9e,0x000a,1},
{0xa8,0x0600,1},//IF=1.5M
{0xaa,0x07c0,1},//multx1
{0xac,0x905c,1},
{0xb7,0x0400,1},
{0xb8,0xa200,1},//log div2
{0xb9,0x809f,1},//vco buffer rc
{0xba,0x4000,1},
{0xbc,0x4920,1},//log buffer
{0xbd,0x0621,1},//set div ratio for adc
{0xbe,0x417c,1}, 
{0xc1,0x5960,1},//set avdd/dvdd rc
{RF_REG_XTAL_FCAP,DEFAULT_XTAL_FCAP,1},//for sk cap
{0xc4,0x45a9,1},
{0xc7,0x8000,1},
{0xc8,0x1414,1},
{0xc9,0xe312,1},
{0xca,0x8008,1},
{0xcb,0x8000,1},//refclk for sdm
{0xd5,0x4003,1},
{0xdc,0x0316,1},
{0xdd,0x4210,1},
{0xde,0x848e,1},
{0xdf,0x430e,1},
{0xe0,0x15a8,1},//set inter frequency and cplx
{0xe6,0x0400,1},
{0xe9,0x1000,1},//enable flt output to test buffer
{0xec,0xeaaa,1},
{0xed,0xdc21,1},

{0xf0,0x8000,1},//5g top current bias
{0xf3,0x08ff,1},
{0xf4,0x40c0,1},
{0xfc,0x3b80,1},//small lna gain
{0xda,0x8038,1},//mdll_reset_dr resetb=0
{0xda,0x803c,1},//resetb=1
{0xab,0x3620,1},//2bit frac
{0xf4,0x4080,1},
{0xa3,0x2203,1},
{0x81,0x7242,1},

//rxgain
{0x1f0,0x0038,0}, //GRx:69 GOffset:5
{0x1b8,0x203d,0},
//{0x1b8,0x3c3d,0},
{0x1f1,0x0038,0},  
//{0x1b9,0x2024,0},//GRx:47 GOffset:5
{0x1b9,0x202c,0},  //GRx:57 GOffset:5
{0x1f2,0x0038,0}, 
//{0x1ba,0x2023,0},//GRx:41 GOffset:5
{0x1ba,0x201b,0},//GRx:45 GOffset:5
{0x1f3,0x0038,0}, //GRx:34 GOffset:5
{0x1bb,0x200a,0},
{0x1f4,0x0018,0}, //GRx:22 GOffset:5
{0x1bc,0x2009,0},
{0x1f5,0x0019,0}, //GRx:15 GOffset:5
{0x1bd,0x2009,0},
{0x1f6,0x0009,0}, //GRx:3 GOffset:5
{0x1be,0x2008,0},
{0x1f7,0x0009,0}, //GRx:3 GOffset:5
{0x1bf,0x2008,0},
#ifdef __PASS_CI_TEST_SETTING__
{0x1f1,0x0039,0}, 
{0x1b9,0x2424,0},
{0x1f2,0x0039,0}, 
{0x1ba,0x202b,0},
#endif
};

#if 0

const short rf_div_2[][3] = 
{
{0x1f9,0x0000,1},//pu tx en
{0x8e,0xf106,1},
{0x98,0x00b8,1},//vco bt div2 freq
{0xb7,0x0200,1},
{0xb8,0xe200,1},//log div2
{0xbc,0x4fb0,1},//log buffer
{0xbd,0x0627,1},//set div ratio for adc
{0xdd,0x0010,1},
{0x184,0x8c0c,1},//pad & vco
{0x186,0x0060,1},
{0x187,0x0060,1},
};
#else

const short rf_div_2[][3] = 
{
{0x1f9,0x0000,1},//pu tx en
{0x8e,0xf106,1},
{0x98,0x00b8,1},//vco bt div2 freq
{0xb7,0x0200,1},
{0xb8,0xe200,1},//log div2
{0xbc,0x4fb0,1},//log buffer
{0xbd,0x0627,1},//set div ratio for adc
{0xdd,0x0010,1},
{0x183,0x8830,1},//
{0x184,0x8c20,1},//pad & vco
{0x185,0x00c3,1},//
{0x186,0x0040,1},
{0x187,0x0000,1},
};

#endif


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

    bitoffset_cmd_reg = 0xc0000034;
    bitoffset_value_reg = 0xc0000038;
    bitoffset_linkid_reg = 0xc0000030;
}


#if 0
uint16_t dcval[8][2];
void rx_dc_2200(void)
{
    unsigned short read_value;
    unsigned short read_value_2nd_stage;
    TRACE("rx_dc_2200\n");
    
    *(volatile uint32_t*)(0xd02201bc) = 0xa0080;// set rx continue
    btdrv_write_rf_reg(0xc0, 0x8000); //page 1
    btdrv_write_rf_reg(0xea, 0xfc02); // set cal clk freq  
    btdrv_write_rf_reg(0xc0, 0x0000); // page 0
    btdrv_write_rf_reg(0xfc, 0x4780); // set lna att   
    btdrv_write_rf_reg(0x88, 0x3f3f); // cal wait time   
    btdrv_write_rf_reg(0x84, 0x0c04); // pu comparator 

    
    btdrv_write_rf_reg(0x89, 0x0010); //////////////////////////////////////rx gain index 0
    btdrv_write_rf_reg(0x87, 0x0018); //  reset dc cal  
    btdrv_write_rf_reg(0x87, 0x0019); 
    btdrv_write_rf_reg(0x87, 0x4019); //  enable 1st stage cal 
    do{
        btdrv_read_rf_reg(0xc0, &read_value);
    }while(!((read_value & 0x0002)==0x0002));
    btdrv_write_rf_reg(0xc0, 0x8000); // page 1
    btdrv_read_rf_reg(0xc4, &read_value);
    btdrv_write_rf_reg(0xc0, 0x4000); // page 2
    btdrv_write_rf_reg(0x90,read_value);
//    TRACE("index 0 290:%x\n",read_value);
    dcval[0][0] = read_value;
    btdrv_write_rf_reg(0xc0, 0x0000); // page 0
    btdrv_write_rf_reg(0x87, 0x6019); //  enable 2nd stage cal  
    do{
        btdrv_read_rf_reg(0xc0, &read_value);
    }while(!((read_value & 0x0004)==0x0004));
    btdrv_write_rf_reg(0xc0, 0x8000); //page 1
    btdrv_read_rf_reg(0xc5, &read_value); 
//    TRACE("index 0 1c5:%x\n",read_value);
        dcval[0][1] = read_value;
    read_value_2nd_stage = ((read_value&0x3f00)>>2) | (read_value&0x003f);
    btdrv_write_rf_reg(0xc0, 0x4000); //page 2
    btdrv_write_rf_reg(0xb0,read_value_2nd_stage);
//    TRACE("index 0 2b0:%x\n",read_value_2nd_stage);


    btdrv_write_rf_reg(0xc0, 0x0000); // page 0
    btdrv_write_rf_reg(0x89, 0x0011); //////////////////////////////////////rx gain index 1
    btdrv_write_rf_reg(0x87, 0x0018); //  reset dc cal  
    btdrv_write_rf_reg(0x87, 0x0019); 
    btdrv_write_rf_reg(0x87, 0x4019); //  enable 1st stage cal 
    do{
        btdrv_read_rf_reg(0xc0, &read_value);
    }while(!((read_value & 0x0002)==0x0002));
    btdrv_write_rf_reg(0xc0, 0x8000); // page 1
    btdrv_read_rf_reg(0xc4, &read_value);
    btdrv_write_rf_reg(0xc0, 0x4000); // page 2
    btdrv_write_rf_reg(0x91,read_value);
 //   TRACE("index 1 291:%x\n",read_value);
    dcval[1][0] = read_value;
    
    btdrv_write_rf_reg(0xc0, 0x0000);
    btdrv_write_rf_reg(0x87, 0x6019); //  enable 2nd stage cal  
    do{
        btdrv_read_rf_reg(0xc0, &read_value);
    }while(!((read_value & 0x0004)==0x0004));
    btdrv_write_rf_reg(0xc0, 0x8000); //page 1
    btdrv_read_rf_reg(0xc5, &read_value); 
 //   TRACE("index 1 1c5:%x\n",read_value);
    dcval[1][1] = read_value;
    
    read_value_2nd_stage = ((read_value&0x3f00)>>2) | (read_value&0x003f);
    btdrv_write_rf_reg(0xc0, 0x4000); //page 2
    btdrv_write_rf_reg(0xb1,read_value_2nd_stage);
//    TRACE("index 1 2b1:%x\n",read_value_2nd_stage);


    btdrv_write_rf_reg(0xc0, 0x0000); // page 0
    btdrv_write_rf_reg(0x89, 0x0012); //////////////////////////////////////rx gain index 2
    btdrv_write_rf_reg(0x87, 0x0018); //  reset dc cal  
    btdrv_write_rf_reg(0x87, 0x0019); 
    btdrv_write_rf_reg(0x87, 0x4019); //  enable 1st stage cal 
    do{
        btdrv_read_rf_reg(0xc0, &read_value);
    }while(!((read_value & 0x0002)==0x0002));
    btdrv_write_rf_reg(0xc0, 0x8000); // page 1
    btdrv_read_rf_reg(0xc4, &read_value);
    btdrv_write_rf_reg(0xc0, 0x4000); // page 2
    btdrv_write_rf_reg(0x92,read_value);
//    TRACE("index 2 292:%x\n",read_value);
    dcval[2][0] = read_value;
    
    btdrv_write_rf_reg(0xc0, 0x0000);
    btdrv_write_rf_reg(0x87, 0x6019); //  enable 2nd stage cal  
    do{
        btdrv_read_rf_reg(0xc0, &read_value);
    }while(!((read_value & 0x0004)==0x0004));
    btdrv_write_rf_reg(0xc0, 0x8000); //page 1
    btdrv_read_rf_reg(0xc5, &read_value); 
    dcval[2][1] = read_value;
    
//    TRACE("index 2 1c5:%x\n",read_value);
    read_value_2nd_stage = ((read_value&0x3f00)>>2) | (read_value&0x003f);
    btdrv_write_rf_reg(0xc0, 0x4000); //page 2
    btdrv_write_rf_reg(0xb2,read_value_2nd_stage);
//    TRACE("index 2 2b2:%x\n",read_value_2nd_stage);


    btdrv_write_rf_reg(0xc0, 0x0000); // page 0
    btdrv_write_rf_reg(0x89, 0x0013); //////////////////////////////////////rx gain index 3
    btdrv_write_rf_reg(0x87, 0x0018); //  reset dc cal  
    btdrv_write_rf_reg(0x87, 0x0019); 
    btdrv_write_rf_reg(0x87, 0x4019); //  enable 1st stage cal 
    do{
        btdrv_read_rf_reg(0xc0, &read_value);
    }while(!((read_value & 0x0002)==0x0002));
    btdrv_write_rf_reg(0xc0, 0x8000); // page 1
    btdrv_read_rf_reg(0xc4, &read_value);
    btdrv_write_rf_reg(0xc0, 0x4000); // page 2
    btdrv_write_rf_reg(0x93,read_value);
 //   TRACE("index 3 293:%x\n",read_value);
    dcval[3][0] = read_value;
    
    btdrv_write_rf_reg(0xc0, 0x0000);
    btdrv_write_rf_reg(0x87, 0x6019); //  enable 2nd stage cal  
    do{
        btdrv_read_rf_reg(0xc0, &read_value);
    }while(!((read_value & 0x0004)==0x0004));
    btdrv_write_rf_reg(0xc0, 0x8000); //page 1
    btdrv_read_rf_reg(0xc5, &read_value); 
//    TRACE("index 3 1c5:%x\n",read_value);
    dcval[3][1] = read_value;
    
    read_value_2nd_stage = ((read_value&0x3f00)>>2) | (read_value&0x003f);
    btdrv_write_rf_reg(0xc0, 0x4000); //page 2
    btdrv_write_rf_reg(0xb3,read_value_2nd_stage);
  //  TRACE("index 3 2b3:%x\n",read_value_2nd_stage);
    

    btdrv_write_rf_reg(0xc0, 0x0000); // page 0
    btdrv_write_rf_reg(0x89, 0x0014); //////////////////////////////////////rx gain index 4
    btdrv_write_rf_reg(0x87, 0x0018); //  reset dc cal  
    btdrv_write_rf_reg(0x87, 0x0019); 
    btdrv_write_rf_reg(0x87, 0x4019); //  enable 1st stage cal 
    do{
        btdrv_read_rf_reg(0xc0, &read_value);
    }while(!((read_value & 0x0002)==0x0002));
    btdrv_write_rf_reg(0xc0, 0x8000); // page 1
    btdrv_read_rf_reg(0xc4, &read_value);
    btdrv_write_rf_reg(0xc0, 0x4000); // page 2
    btdrv_write_rf_reg(0x94,read_value);
 //   TRACE("index 4 294:%x\n",read_value);
    dcval[4][0] = read_value;
    
    btdrv_write_rf_reg(0xc0, 0x0000);
    btdrv_write_rf_reg(0x87, 0x6019); //  enable 2nd stage cal  
    do{
        btdrv_read_rf_reg(0xc0, &read_value);
    }while(!((read_value & 0x0004)==0x0004));
    btdrv_write_rf_reg(0xc0, 0x8000); //page 1
    btdrv_read_rf_reg(0xc5, &read_value); 
    dcval[4][1] = read_value;
    
//    TRACE("index 4 1c5:%x\n",read_value);
    read_value_2nd_stage = ((read_value&0x3f00)>>2) | (read_value&0x003f);
    btdrv_write_rf_reg(0xc0, 0x4000); //page 2
    btdrv_write_rf_reg(0xb4,read_value_2nd_stage);
//    TRACE("index 4 2b4:%x\n",read_value_2nd_stage);


    btdrv_write_rf_reg(0xc0, 0x0000); // page 0
    btdrv_write_rf_reg(0x89, 0x0015); //////////////////////////////////////rx gain index 5
    btdrv_write_rf_reg(0x87, 0x0018); //  reset dc cal  
    btdrv_write_rf_reg(0x87, 0x0019); 
    btdrv_write_rf_reg(0x87, 0x4019); //  enable 1st stage cal 
    do{
        btdrv_read_rf_reg(0xc0, &read_value);
    }while(!((read_value & 0x0002)==0x0002));
    btdrv_write_rf_reg(0xc0, 0x8000); // page 1
    btdrv_read_rf_reg(0xc4, &read_value);
    btdrv_write_rf_reg(0xc0, 0x4000); // page 2
    btdrv_write_rf_reg(0x95,read_value);
//    TRACE("index 5 295:%x\n",read_value);
    dcval[5][0] = read_value;
    
    btdrv_write_rf_reg(0xc0, 0x0000);
    btdrv_write_rf_reg(0x87, 0x6019); //  enable 2nd stage cal  
    do{
        btdrv_read_rf_reg(0xc0, &read_value);
    }while(!((read_value & 0x0004)==0x0004));
    btdrv_write_rf_reg(0xc0, 0x8000); //page 1
    btdrv_read_rf_reg(0xc5, &read_value); 
//    TRACE("index 5 1c5:%x\n",read_value);
    dcval[5][1] = read_value;
    
    read_value_2nd_stage = ((read_value&0x3f00)>>2) | (read_value&0x003f);
    btdrv_write_rf_reg(0xc0, 0x4000); //page 2
    btdrv_write_rf_reg(0xb5,read_value_2nd_stage);
//    TRACE("index 5 2b5:%x\n",read_value_2nd_stage);


    btdrv_write_rf_reg(0xc0, 0x0000); // page 0
    btdrv_write_rf_reg(0x89, 0x0016); //////////////////////////////////////rx gain index 6
    btdrv_write_rf_reg(0x87, 0x0018); //  reset dc cal  
    btdrv_write_rf_reg(0x87, 0x0019); 
    btdrv_write_rf_reg(0x87, 0x4019); //  enable 1st stage cal 
    do{
        btdrv_read_rf_reg(0xc0, &read_value);
    }while(!((read_value & 0x0002)==0x0002));
    btdrv_write_rf_reg(0xc0, 0x8000); // page 1
    btdrv_read_rf_reg(0xc4, &read_value);
    btdrv_write_rf_reg(0xc0, 0x4000); // page 2
    btdrv_write_rf_reg(0x96,read_value);
 //   TRACE("index 6 296:%x\n",read_value);
    dcval[6][0] = read_value;
    
    btdrv_write_rf_reg(0xc0, 0x0000);
    btdrv_write_rf_reg(0x87, 0x6019); //  enable 2nd stage cal  
    do{
        btdrv_read_rf_reg(0xc0, &read_value);
    }while(!((read_value & 0x0004)==0x0004));
    btdrv_write_rf_reg(0xc0, 0x8000); //page 1
    btdrv_read_rf_reg(0xc5, &read_value); 
    dcval[6][1] = read_value;
//    TRACE("index 6 1c5:%x\n",read_value);
    
    read_value_2nd_stage = ((read_value&0x3f00)>>2) | (read_value&0x003f);
    btdrv_write_rf_reg(0xc0, 0x4000); //page 2
    btdrv_write_rf_reg(0xb6,read_value_2nd_stage);
//    TRACE("index 6 2b6:%x\n",read_value_2nd_stage);


    btdrv_write_rf_reg(0xc0, 0x0000); // page 0
    btdrv_write_rf_reg(0x89, 0x0017); //////////////////////////////////////rx gain index 7
    btdrv_write_rf_reg(0x87, 0x0018); //  reset dc cal  
    btdrv_write_rf_reg(0x87, 0x0019); 
    btdrv_write_rf_reg(0x87, 0x4019); //  enable 1st stage cal 
    do{
        btdrv_read_rf_reg(0xc0, &read_value);
    }while(!((read_value & 0x0002)==0x0002));
    btdrv_write_rf_reg(0xc0, 0x8000); // page 1
    btdrv_read_rf_reg(0xc4, &read_value);
    btdrv_write_rf_reg(0xc0, 0x4000); // page 2
    btdrv_write_rf_reg(0x97,read_value);
//    TRACE("index 7 297:%x\n",read_value);
    dcval[7][0] = read_value;
    
    btdrv_write_rf_reg(0xc0, 0x0000);
    btdrv_write_rf_reg(0x87, 0x6019); //  enable 2nd stage cal  
    do{
        btdrv_read_rf_reg(0xc0, &read_value);
    }while(!((read_value & 0x0004)==0x0004));
    btdrv_write_rf_reg(0xc0, 0x8000); //page 1
    btdrv_read_rf_reg(0xc5, &read_value); 
//    TRACE("index 7 1c5:%x\n",read_value);
    dcval[7][1] = read_value;
    
    read_value_2nd_stage = ((read_value&0x3f00)>>2) | (read_value&0x003f);
    btdrv_write_rf_reg(0xc0, 0x4000); //page 2
    btdrv_write_rf_reg(0xb7,read_value_2nd_stage);
//    TRACE("index 7 2b7:%x\n",read_value_2nd_stage);


    *(volatile uint32_t*)(0xd02201bc) = 0x0;
    btdrv_write_rf_reg(0xc0, 0x0000); // page 0
    btdrv_write_rf_reg(0x87, 0x0019);
    btdrv_write_rf_reg(0x89, 0x0000);
    btdrv_write_rf_reg(0xfc, 0x3b80); 
    TRACE("RX DAC VAL:");
    DUMP16("%04x ",dcval,16);
} 
#else
//uint16_t dcval[8][2];
void rx_dc_2200_dcval(uint16_t *dcval1,uint16_t *dcval2,uint8_t count,uint8_t total_count)
{
    unsigned short read_value;
    unsigned short read_value_2nd_stage;
//    TRACE("rx_dc_2200\n");

    uint8_t i=0;
    //uint16_t read_val = 0;
    //uint8_t cnt = 0;
    
    
    *(volatile uint32_t*)(0xd02201bc) = 0xa0080;// set rx continue
//    btdrv_write_rf_reg(0xc0, 0x8000); //page 1
    btdrv_write_rf_reg(0x1ea, 0xfc02); // set cal clk freq  
//    btdrv_write_rf_reg(0xc0, 0x0000); // page 0
    btdrv_write_rf_reg(0xfc, 0x4780); // set lna att   
    btdrv_write_rf_reg(0x88, 0x3f3f); // cal wait time   
    btdrv_write_rf_reg(0x84, 0x0c04); // pu comparator 

    for(i=0;i<8;i++)
    {
        btdrv_write_rf_reg(0x89, 0x0010+i); //////////////////////////////////////rx gain index 0
        btdrv_write_rf_reg(0x87, 0x0018); //  reset dc cal  
        btdrv_delay(2);
        btdrv_write_rf_reg(0x87, 0x0019); 
        btdrv_delay(2);
        btdrv_write_rf_reg(0x87, 0x4019); //  enable 1st stage cal 
        btdrv_delay(2);
        
        do{
            btdrv_read_rf_reg(0xc0, &read_value);
        }while(!((read_value & 0x0002)==0x0002));
        btdrv_read_rf_reg(0x1c4, &read_value);
   //     TRACE("0x1c4:%x\n",read_value);

        
        *(dcval1+i*total_count+count) = read_value;
        btdrv_write_rf_reg(0x290+i,read_value);
        btdrv_write_rf_reg(0x87, 0x6019); //  enable 2nd stage cal  
        do{
            btdrv_read_rf_reg(0xc0, &read_value);
        }while(!((read_value & 0x0004)==0x0004));        
        btdrv_read_rf_reg(0x1c5, &read_value); 
//        TRACE("0x1c5:%x\n",read_value);
        *(dcval2+i*total_count + count) = read_value;
        
        read_value_2nd_stage = ((read_value&0x3f00)>>2) | (read_value&0x003f);
        btdrv_write_rf_reg(0x2b0+i,read_value_2nd_stage);       
    }


    *(volatile uint32_t*)(0xd02201bc) = 0x0;
//    btdrv_write_rf_reg(0xc0, 0x0000); // page 0
    btdrv_write_rf_reg(0x87, 0x0019);
    btdrv_write_rf_reg(0x89, 0x0000);
    btdrv_write_rf_reg(0xfc, 0x3b80); 
}



uint16_t btdrv_2200_get_average_dcval(uint16_t *val,uint8_t count,uint8_t type)
{
    uint8_t i;
    uint16_t max=0,min=0xffff;
    uint8_t max_i=0,min_i=0;
    uint16_t temp;
    uint32_t total_value1,total_value2;
    for(i=0;i<count;i++)
    {
        if(*(val+i)>max)
        {
            max = *(val+i);
            max_i = i;
        }
        if(*(val+i)<min)
        {
            min = *(val+i);
            min_i = i;
        }
    }
    temp = val[0];
    val[0] = max;
    *(val+max_i) = temp;
    temp = val[count-1];
    val[count-1] = min;
    *(val+min_i) = temp;

    max=0;
    min=0xffff;
    for(i=1;i<(count-1);i++)
    {
        if(*(val+i)>max)
        {
            max = *(val+i);
            max_i = i;
        }
        if(*(val+i)<min)
        {
            min = *(val+i);
            min_i = i;
        }
    }
    temp = val[1];
    val[1] = max;
    *(val+max_i) = temp;
    temp = val[count-2];
    val[count-2] = min;
    *(val+min_i) = temp;

    max=0;
    min=0xffff;
    for(i=2;i<(count-2);i++)
    {
        if(*(val+i)>max)
        {
            max = *(val+i);
            max_i = i;
        }
        if(*(val+i)<min)
        {
            min = *(val+i);
            min_i = i;
        }
    }
    temp = val[2];
    val[2] = max;
    *(val+max_i) = temp;
    temp = val[count-3];
    val[count-3] = min;
    *(val+min_i) = temp;
    total_value1  = 0;
    total_value2 = 0;
    DUMP16("%04x ",&val[3],4);
    if(type == 1) ////data is 8bit length
    {
        for(i=0;i<(count-6);i++)
        {
            total_value1 += (val[i+3]&0xff);
            total_value2 += (val[i+3]&0xff00);
        }
//        TRACE("type1:total_value1=%x,total_value2=%x",total_value1,total_value2);

        total_value1 /=(count-6);
//        TRACE("type1:total_value1=%x,total_value2=%x",total_value1,total_value2);

        total_value2 /=(count-6);
        total_value2 &=0xff00;
 //       TRACE("type1:total_value1=%x,total_value2=%x",total_value1,total_value2);
        return (uint16_t)(total_value1|total_value2);
    }
    else //if(type ==2)
    {
        for(i=0;i<(count-6);i++)
        {
            total_value1 += (val[i+3]&0x3f);
            total_value2 += (val[i+3]&0x3f00);
        }
        total_value1 /=(count-6);
        total_value2 /=(count-6);
        total_value2 &=0x3f00;

 //       TRACE("type2:total_value1=%x,total_value2=%x",total_value1,total_value2);
        return (total_value1|total_value2);
    }
    return (0);
}

#endif

void tx_dc_2200(void)
{
    unsigned short read_value;

    *(volatile uint32_t*)(0xd02201bc) = 0xa0000;// set tx continue

    btdrv_write_rf_reg(0xe4, 0x8800); //

    btdrv_write_rf_reg(0xe5, 0x0800); //set dac
    btdrv_write_rf_reg(0x90, 0x8000); //
    btdrv_write_rf_reg(0x86, 0x390c); ///set analog comparator reg
    btdrv_write_rf_reg(0x91, 0xea40); ///reset
    btdrv_delay(2);
    btdrv_write_rf_reg(0x91, 0x6b40); ///cal en on
    btdrv_delay(2);
    btdrv_write_rf_reg(0x91, 0xeb40); ///cal en off
    btdrv_delay(2);
    btdrv_write_rf_reg(0x91, 0xea40); ///turn off cal clk
    btdrv_write_rf_reg(0xe4, 0x0800); //
    do{
        btdrv_read_rf_reg(0xc0, &read_value);
    }while(!((read_value & 0x0001)==0x0001));
    
    btdrv_read_rf_reg(0x1c1, &read_value);
    TRACE("TX DAC VAL:%x\n",read_value);
    btdrv_write_rf_reg(0x298, read_value); 

    btdrv_write_rf_reg(0x90, 0x8C00); //
    btdrv_write_rf_reg(0x86, 0x310c); ///disable cal clock

    *(volatile uint32_t*)(0xd02201bc) = 0x0000;// set tx continue

    
    
    
}

static int txpower_calib_checksum_valid(uint16_t efuse)
{
    int i;
    uint32_t cnt = 0;

    for (i = 0; i < 12; i++) {
        if (efuse & (1 << i)) {
            cnt++;
        }
    }

    return (((~cnt) & 0xF) == ((efuse >> 12) & 0xF));
}

void btdrv_txpower_calib(void)
{
    uint16_t efuse;
    int calib_ver = 0;    
    uint16_t reg_adr = 0;
    uint16_t reg_val = 0;
    
    pmu_get_efuse(PMU_EFUSE_PAGE_RESERVED_5, &efuse);

    if (efuse & 0x01){
        calib_ver = 1;
    }

    pmu_get_efuse(PMU_EFUSE_PAGE_RESERVED_8, &efuse);
    
    if (!txpower_calib_checksum_valid(efuse)){
        return;
    }

    TRACE("%s ver:%d",__func__, calib_ver);

    switch (calib_ver) {
        case 0:
            {                
                uint16_t reg_2g4_pad_core_ibit_bt_184_5_0;
                struct bt_drv_tx_pwr_calib_tlb_t{
                    uint16_t tbl[2][3];
                };
                const struct bt_drv_tx_pwr_calib_tlb_t rf_tx_pwr_calib_tbl[] =
                {
                    {{
                        {0x181,0x0818,0},
                        {0x182,0x810A,0}, 
                    }},
                    {{
                        {0x181,0x0820,0},
                        {0x182,0x810D,0}, 
                    }},
                    {{
                        {0x181,0x0814,0},
                        {0x182,0x8108,0}, 
                    }},
                };
                
                const struct bt_drv_tx_pwr_calib_tlb_t *rf_tx_pwr_calib_tbl_p = &rf_tx_pwr_calib_tbl[efuse&0x0f];
                for(uint8_t i=0; i< sizeof(((struct bt_drv_tx_pwr_calib_tlb_t *)(0))->tbl)/sizeof(((struct bt_drv_tx_pwr_calib_tlb_t *)(0))->tbl[0]); i++){
                    btdrv_write_rf_reg(rf_tx_pwr_calib_tbl_p->tbl[i][0],rf_tx_pwr_calib_tbl_p->tbl[i][1]);
                    if(rf_tx_pwr_calib_tbl_p->tbl[i][2] !=0)
                        btdrv_delay(rf_tx_pwr_calib_tbl_p->tbl[i][2]);//delay
                }
                reg_2g4_pad_core_ibit_bt_184_5_0 = (efuse & 0x03f0)>>4;
                TRACE("idx=0x%x 184_5_0=0x%x",efuse&0x0f,
                                              reg_2g4_pad_core_ibit_bt_184_5_0);
                
                reg_adr = 0x184;
                btdrv_read_rf_reg(reg_adr, &reg_val);
                reg_val = (reg_val & (~0x3f)) | reg_2g4_pad_core_ibit_bt_184_5_0;
                btdrv_write_rf_reg(reg_adr,reg_val);
                TRACE("0x%04x = 0x%04x ",reg_adr, reg_val);
            }
            break;
        case 1:
            {
                uint16_t reg_vco_capbank_1c6_8_4;
                uint16_t reg_2g4_tmx_cap_bank_bt_181_5_0;
                uint16_t reg_2g4_pad_cap_bank_182_4_0;

                reg_vco_capbank_1c6_8_4 = efuse & 0x1f;
                reg_2g4_tmx_cap_bank_bt_181_5_0 = 0x2d - reg_vco_capbank_1c6_8_4;
                reg_2g4_pad_cap_bank_182_4_0 = 0x1d - reg_vco_capbank_1c6_8_4;
                TRACE("1c6_8_4=0x%x 181_5_0=0x%x 182_8_4=0x%x",reg_vco_capbank_1c6_8_4,
                                                                  reg_2g4_tmx_cap_bank_bt_181_5_0,
                                                                  reg_2g4_pad_cap_bank_182_4_0);

                reg_adr = 0x181;
                btdrv_read_rf_reg(reg_adr, &reg_val);
                reg_val = (reg_val & (~0x3f)) | reg_2g4_tmx_cap_bank_bt_181_5_0;
                btdrv_write_rf_reg(reg_adr,reg_val);
                TRACE("0x%04x = 0x%04x ",reg_adr, reg_val);

                reg_adr = 0x182;
                btdrv_read_rf_reg(reg_adr, &reg_val);
                reg_val = (reg_val & (~0x1f)) | (reg_2g4_pad_cap_bank_182_4_0);
                btdrv_write_rf_reg(reg_adr,reg_val);
                TRACE("0x%04x = 0x%04x ",reg_adr, reg_val);
            }
        default:
            break;
    }

}

void btdrv_rf_tx_init(int idx)
{
    const struct bt_drv_tx_table_t *tx_tbl_p;
    if(hal_get_chip_metal_id() <= HAL_CHIP_METAL_ID_1){
        tx_tbl_p = &rf_tx_pwr_tbl[idx];
    }
    else
    {
        tx_tbl_p = &rf_tx_pwr_tbl_2[idx];    
    }

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
    uint16_t read_val;
//	uint8_t ret;
    uint16_t dcval1[8][10],dcval2[8][10];
    uint8_t i;


    if(hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_0){
        rf_init_tbl_p = &rf_init_tbl_0[0];
        tbl_size = sizeof(rf_init_tbl_0)/sizeof(rf_init_tbl_0[0]);
    }
    else {
        rf_init_tbl_p = &rf_init_tbl_1[0];
        tbl_size = sizeof(rf_init_tbl_1)/sizeof(rf_init_tbl_1[0]);
    }

    for(uint8_t i=0;i< tbl_size;i++)
    {
        btdrv_write_rf_reg(rf_init_tbl_p[i][0],rf_init_tbl_p[i][1]);
        if(rf_init_tbl_p[i][2] !=0)
            btdrv_delay(rf_init_tbl_p[i][2]);//delay
        btdrv_read_rf_reg(rf_init_tbl_p[i][0],&value);
 //       TRACE("reg=%x,v=%x",rf_init_tbl_p[i][0],value);
    }

#ifdef __BES2200__DIV_2__
    for(uint8_t i=0;i<  sizeof(rf_div_2)/sizeof(rf_div_2[0]);i++)
    {
        btdrv_write_rf_reg(rf_div_2[i][0],rf_div_2[i][1]);
        if(rf_div_2[i][2] !=0)
            btdrv_delay(rf_div_2[i][2]);//delay
        btdrv_read_rf_reg(rf_div_2[i][0],&value);
 //       TRACE("reg=%x,v=%x",rf_div_2[i][0],value);
    }    
    btdrv_rf_tx_init(0);

#else
    btdrv_rf_tx_init(1);
#endif   

#if 1
   btdrv_read_rf_reg(0xa7,&read_val);
   TRACE("0xa7=%x",read_val);
   read_val &= 0xff80;
   read_val |= 0x58;
   btdrv_write_rf_reg(0xa7,read_val);
   btdrv_read_rf_reg(0xa7,&read_val);
   
   TRACE("0xa7=%x",read_val);

    for(i=0;i<10;i++)
    {
        rx_dc_2200_dcval(dcval1[0],dcval2[0],i,10);
    }


#if 0
    for(i=0;i<8;i++)
    {
        DUMP16("%04x ", dcval1[i], 10);
        TRACE("\n");
    }
    for(i=0;i<8;i++)
    {
        DUMP16("%04x ", dcval2[i], 10);
        TRACE("\n");
    }


#endif


    for(i=0;i<8;i++)
    {
        read_val = btdrv_2200_get_average_dcval(dcval1[i],10,1);
        btdrv_write_rf_reg((0x290+i), read_val);  /* write cal1 value */

        read_val = btdrv_2200_get_average_dcval(dcval2[i],10,2);
        btdrv_write_rf_reg((0x2b0+i), read_val);

    }
    
#endif
//    rx_dc_2200();

    tx_dc_2200();

    btdrv_txpower_calib();

    btdrv_xtal_rf_init();
    return 1;
}

