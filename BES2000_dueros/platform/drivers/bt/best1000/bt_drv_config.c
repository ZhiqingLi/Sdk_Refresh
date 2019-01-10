#include "plat_types.h"
#include "tgt_hardware.h"
#include "hal_i2c.h"
#include "hal_uart.h"
#include "bt_drv.h"
#include "hal_timer.h"
#include "hal_chipid.h"

//typedef void (*btdrv_config_func_t)(uint8_t parlen, uint8_t *param);

extern void btdrv_send_cmd(uint16_t opcode,uint8_t cmdlen,const uint8_t *param);
extern void btdrv_write_memory(uint8_t wr_type,uint32_t address,const uint8_t *value,uint8_t length);


typedef struct
{
//    btdrv_config_func_t    func;
    uint8_t is_act;
    uint16_t opcode;
    uint8_t parlen;
    uint8_t *param;

}BTDRV_CFG_TBL_STRUCT;


#define BTDRV_CONFIG_ACTIVE   1
#define BTDRV_CONFIG_INACTIVE 0

/*
[0][0] = 63, [0][1] = 0,[0][2] = (-80), 		  472d
[1][0] = 51, [2][1] = 0,[2][2] = (-80), 		 472b
[2][0] = 42, [4][1] = 0,[4][2] = (-75), 		  4722
[3][0] = 36, [6][1] = 0,[6][2] = (-55), 		  c712
[4][0] = 30, [8][1] = 0,[8][2] = (-40), 		  c802
[5][0] = 21,[10][1] = 0,[10][2] = 0x7f, 		c102
[6][0] = 12,[11][1] = 0,[11][2] = 0x7f, 	  c142
[7][0] = 3,[13][1] = 0,[13][2] = 0x7f,		  c1c2
[8][0] = -3,[14][1] = 0,[14][2] = 0x7f};	  c0c2
*/

#if (BT_RFMODE == BT_LOW_POWER_MODE)
const int8_t btdrv_rf_env[]=
{
  0x01,0x00,  //rf api
  0x01,   //rf env
  136,     //rf length
  0x2,     //txpwr_max
  -20,    ///rssi high thr
  -60,   //rssi low thr
  -70,  //rssi interf thr
  0xf,  //rssi interf gain thr
  2,  //wakeup delay
  0xe, 0, //skew
  0xe8,0x3,    //ble agc inv thr
  0xff,     //sw gain set
  1,      //sw gain reset factor
  1,    //bt sw gain cntl enable
  1,   //ble sw gain cntl en
  1,  //bt interfere  detector en
  0,  //ble interfere detector en
#if 0
  51,0,-80,
  42,0,-80,
  29,0,-80,
  15,0,-80,
  0x7f,0x7f,0x7f,
  0x7f,0x7f,0x7f,
  0x7f,0x7f,0x7f,
  0x7f,0x7f,0x7f,
  0x7f,0x7f,0x7f,
  0x7f,0x7f,0x7f,
  0x7f,0x7f,0x7f,
  0x7f,0x7f,0x7f,
  0x7f,0x7f,0x7f,
  0x7f,0x7f,0x7f,
  0x7f,0x7f,0x7f,  //rx gain tbl ptr
  -67,-67,
  -50,-50,
  -35,-35,
  //-55,-55,
  //-90,-90,
  //-90,-90,
  127,127,
  0x7f,0x7f,
  0x7f,0x7f,
  0x7f,0x7f,
  0x7f,0x7f,
  0x7f,0x7f,
  0x7f,0x7f,
  0x7f,0x7f,
  0x7f,0x7f,
  0x7f,0x7f,
  0x7f,0x7f,
  0x7f,0x7f,    //rx gain ths tbl ptr
#else
  66,13,-80,
  57,13,-80,
  47,13,-80,
  36,13,-80,
  26,15,-80,
  16,15,-80,
  10,15,-80,

  0x7f,0x7f,0x7f,
  0x7f,0x7f,0x7f,
  0x7f,0x7f,0x7f,
  0x7f,0x7f,0x7f,
  0x7f,0x7f,0x7f,
  0x7f,0x7f,0x7f,
  0x7f,0x7f,0x7f,
  0x7f,0x7f,0x7f,  //rx gain tbl ptr
#if 0
  -75,-75,
  -62,-62,
  -53,-53,
  -42,-42,
  -31,-31,
  -20,-20,
  #else
  -78,-78,
  -70,-70,
  -61,-61,
  -50,-50,
  -39,-39,
  -28,-28,
  #endif
  0x7f,0x7f,
  0x7f,0x7f,
  0x7f,0x7f,
  0x7f,0x7f,
  0x7f,0x7f,
  0x7f,0x7f,
  0x7f,0x7f,
  0x7f,0x7f,
  0x7f,0x7f,    //rx gain ths tbl ptr

#endif
  0,0,
  0,0,
  0,0,
  0,0,
  0,0,
  0,1,
  0,2,
  0,2,
  0,2,
  0x7f,0x7f,
  0x7f,0x7f,
  0x7f,0x7f,
  0x7f,0x7f,
  0x7f,0x7f,
  0x7f,0x7f,    //flpha filter factor ptr
  -23,-20,-17,-14,-11,-8,-5,-2,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,   //tx pw onv tbl ptr
};
#elif (BT_RFMODE == BT_HIGH_PERFORMANCE_MODE)



const int8_t btdrv_rf_env[]=
{
  0x01,0x00,  //rf api
  0x01,   //rf env
  136,     //rf length
  0x7,     //txpwr_max
  -20,    ///rssi high thr
  -60,   //rssi low thr
  -70,  //rssi interf thr
  0xf,  //rssi interf gain thr
  2,  //wakeup delay
  0xe, 0, //skew
  0xe8,0x3,    //ble agc inv thr
  0xff,     //sw gain set
  1,      //sw gain reset factor
  1,    //bt sw gain cntl enable
  1,   //ble sw gain cntl en
  0,  //bt interfere  detector en
  0,  //ble interfere detector en


  66,0,-80,
  57,0,-80,
  47,0,-80,
  36,0,-80,
  26,0,-80,
  16,0,-80,
  10,0,-80,

  0x7f,0x7f,0x7f,
  0x7f,0x7f,0x7f,
  0x7f,0x7f,0x7f,
  0x7f,0x7f,0x7f,
  0x7f,0x7f,0x7f,
  0x7f,0x7f,0x7f,
  0x7f,0x7f,0x7f,
  0x7f,0x7f,0x7f,  //rx gain tbl ptr

  -70,-70,
  -62,-62,
  -53,-53,
  -42,-42,
  -31,-31,
  -20,-20,
  0x7f,0x7f,
  0x7f,0x7f,
  0x7f,0x7f,
  0x7f,0x7f,
  0x7f,0x7f,
  0x7f,0x7f,
  0x7f,0x7f,
  0x7f,0x7f,
  0x7f,0x7f,    //rx gain ths tbl ptr


  0,0,
  0,0,
  0,0,
  0,0,
  0,0,
  0,1,
  0,2,
  0,2,
  0,2,
  0x7f,0x7f,
  0x7f,0x7f,
  0x7f,0x7f,
  0x7f,0x7f,
  0x7f,0x7f,
  0x7f,0x7f    //flpha filter factor ptr
  -23,-20,-17,-14,-11,-8,-5,-2,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,0x7f,   //tx pw onv tbl ptr
};
#endif

const int8_t btdrv_bt_spi_cntl[]=
{
	0x01,0x00,	/*set rf env*/
	0x02,		/*set bt_spicntl*/
	0x10,		/*cmd length*/
	0xff,0xff,0xff,0xff,/*spiposbuf_addr*/
	0xff,0xff,0xff,0xff,/*spiposbuf_addr*/
	0x00,0x00,	/*spibufoffset*/
	0x00,		/*bit[30:24]: spi trig after rxon pos*/
	0x00,		/*bit[22:16]: spi trig before rxon pos*/
	0x33,		/*bit[14:8] : spi trig after txon pos*/
	0x00,			/*bit[6:0] : spi trig before txon pos*/
	0x00,		/*bit[14:8]: spi trig after rxon neg*/
	3			/*bit[6:0]: spi trig after txon neg*/
};

const int8_t btdrv_ble_spi_cntl[]=
{
	0x01,0x00,	/*set rf env*/
	0x04,		/*set ble_spicntl*/
	0x0f,		/*cmd length*/
	0xff,0xff,0xff,0xff,/*spiposbuf_addr*/
	0xff,0xff,0xff,0xff,/*spiposbuf_addr*/
	0x00,0x00,	/*spibufoffset*/
	0x00,		/*bit[23:16]: spi trig before/after the first txon/rxon pos*/
	0x00,		/*bit[14:8]: spi trig after rxon neg*/
	15,		/*bit[6:0]: spi trig after txon neg*/
	0x00,		/*bit[15:8]: spi trig before/after the no-first rxon pos*/
	15		/*bit[7:0]: spi trig before/after the no-first	txon pos*/
};


const int8_t btdrv_afh_env[] =
{
    0x02,0x00,   //afh env
    0x00,      //ignore
    33,          //length
    5,   //nb_reass_chnl
    10,  //win_len
    -70,  //rf_rssi_interf_thr
    10,  //per_thres_bad
    20,  //reass_int
    20,   //n_min
    20,   //afh_rep_intv_max
    96,    //ths_min
    2,   //chnl_assess_report_style
    15,  //chnl_assess_diff_thres
    60, // chnl_assess_interfere_per_thres_bad
    9,  //chnl_assess_stat_cnt_max
    -9,  //chnl_assess_stat_cnt_min
    1,2,3,2,1,   //chnl_assess_stat_cnt_inc_mask[5]
    1,2,3,2,1,    //chnl_assess_stat_cnt_dec_mask
    0xd0,0x7,      //chnl_assess_timer
    -48,        //chnl_assess_min_rssi
    0x64,0,   //chnl_assess_nb_pkt
    0x32,0,     //chnl_assess_nb_bad_pkt
    6,    //chnl_reassess_cnt_val
    0x3c,0,     //chnl_assess_interfere_per_thres_bad
};

const uint8_t lpclk_drift_jitter[] =
{
    0xfa,0x00,  //  drift  250ppm
    0x0a,0x00    //jitter  +-10us

};

const uint8_t  wakeup_timing[] =
{
    0xe8,0x3,   //exernal_wakeup_time 600us
    0xe8,0x3,    //oscillater_wakeup_time  600us
    0xe8,0x3,    //radio_wakeup_time  600us
    0xa0,0xf,    //wakeup_delay_time
};


uint8_t  sleep_param[] =
{
#ifdef __TWS__
   0,
#else
   1,    // sleep_en;
#endif
   1,    // exwakeup_en;
   0xc8,0,    //  lpo_calib_interval;   lpo calibration interval
   0xa,0,0,0,    // lpo_calib_time;  lpo count lpc times
};

uint8_t  unsleep_param[] =
{

   0,    // sleep_en;
   0,    // exwakeup_en;
   0xc8,0,    //  lpo_calib_interval;   lpo calibration interval
   0xa,0,0,0,    // lpo_calib_time;  lpo count lpc times
};

uint32_t me_init_param[][2] =
{
	//{0xffffffff, 0xffffffff},
    {0xc00044ec, 0xd0213bdc},

	//  {0xd02200f0, 0x021F0103},
};


uint32_t me_init_param_g[][2] =
{
	{0xc0003a78, 0xd02134fc},
	//  {0xd02200f0, 0x021F0103},
};

const uint16_t me_bt_default_page_timeout = 0x2000;

const uint8_t  sync_config[] =
{
    1,1,   //sco path config   0:hci  1:pcm
    0,      //sync use max buff length   0:sync data length= packet length 1:sync data length = host sync buff len
    0,        //cvsd bypass     0:cvsd2pcm   1:cvsd transparent

};


//pcm general ctrl
#define PCM_PCMEN_POS            15
#define PCM_LOOPBCK_POS          14
#define PCM_MIXERDSBPOL_POS      11
#define PCM_MIXERMODE_POS        10
#define PCM_STUTTERDSBPOL_POS    9
#define PCM_STUTTERMODE_POS      8
#define PCM_CHSEL_POS            6
#define PCM_MSTSLV_POS           5
#define PCM_PCMIRQEN_POS         4
#define PCM_DATASRC_POS          0


//pcm phy ctrl
#define PCM_LRCHPOL_POS     15
#define PCM_CLKINV_POS      14
#define PCM_IOM_PCM_POS     13
#define PCM_BUSSPEED_LSB    10
#define PCM_SLOTEN_MASK     ((uint32_t)0x00000380)
#define PCM_SLOTEN_LSB      7
#define PCM_WORDSIZE_MASK   ((uint32_t)0x00000060)
#define PCM_WORDSIZE_LSB    5
#define PCM_DOUTCFG_MASK    ((uint32_t)0x00000018)
#define PCM_DOUTCFG_LSB     3
#define PCM_FSYNCSHP_MASK   ((uint32_t)0x00000007)
#define PCM_FSYNCSHP_LSB    0




/// Enumeration of PCM status
enum PCM_STAT
{
    PCM_DISABLE = 0,
    PCM_ENABLE
};

/// Enumeration of PCM channel selection
enum PCM_CHANNEL
{
    PCM_CH_0 = 0,
    PCM_CH_1
};

/// Enumeration of PCM role
enum PCM_MSTSLV
{
    PCM_SLAVE = 0,
    PCM_MASTER
};

/// Enumeration of PCM data source
enum PCM_SRC
{
    PCM_SRC_DPV = 0,
    PCM_SRC_REG
};



/// Enumeration of PCM left/right channel selection versus frame sync polarity
enum PCM_LR_CH_POL
{
    PCM_LR_CH_POL_RIGHT_LEFT = 0,
    PCM_LR_CH_POL_LEFT_RIGHT
};

/// Enumeration of PCM clock inversion
enum PCM_CLK_INV
{
    PCM_CLK_RISING_EDGE = 0,
    PCM_CLK_FALLING_EDGE
};

/// Enumeration of PCM mode selection
enum PCM_MODE
{
    PCM_MODE_PCM = 0,
    PCM_MODE_IOM
};

/// Enumeration of PCM bus speed
enum PCM_BUS_SPEED
{
    PCM_BUS_SPEED_128k = 0,
    PCM_BUS_SPEED_256k,
    PCM_BUS_SPEED_512k,
    PCM_BUS_SPEED_1024k,
    PCM_BUS_SPEED_2048k
};

/// Enumeration of PCM slot enable
enum PCM_SLOT
{
    PCM_SLOT_NONE = 0,
    PCM_SLOT_0,
    PCM_SLOT_0_1,
    PCM_SLOT_0_2,
    PCM_SLOT_0_3
};

/// Enumeration of PCM word size
enum PCM_WORD_SIZE
{
    PCM_8_BITS = 0,
    PCM_13_BITS,
    PCM_14_BITS,
    PCM_16_BITS
};

/// Enumeration of PCM DOUT pad configuration
enum PCM_DOUT_CFG
{
    PCM_OPEN_DRAIN = 0,
    PCM_PUSH_PULL_HZ,
    PCM_PUSH_PULL_0
};

/// Enumeration of PCM FSYNC physical shape
enum PCM_FSYNC
{
    PCM_FSYNC_LF = 0,
    PCM_FSYNC_FR,
    PCM_FSYNC_FF,
    PCM_FSYNC_LONG,
    PCM_FSYNC_LONG_16
};




const uint32_t pcm_setting[] =
{
//pcm_general_ctrl
    (PCM_DISABLE<<PCM_PCMEN_POS) |                      //enable auto
    (PCM_DISABLE << PCM_LOOPBCK_POS)  |                 //LOOPBACK test
    (PCM_DISABLE << PCM_MIXERDSBPOL_POS)  |
    (PCM_DISABLE << PCM_MIXERMODE_POS)  |
    (PCM_DISABLE <<PCM_STUTTERDSBPOL_POS) |
    (PCM_DISABLE <<PCM_STUTTERMODE_POS) |
    (PCM_CH_0<< PCM_CHSEL_POS) |
    (PCM_MASTER<<PCM_MSTSLV_POS) |                      //BT clock
    (PCM_DISABLE << PCM_PCMIRQEN_POS) |
    (PCM_SRC_DPV<<PCM_DATASRC_POS),

//pcm_phy_ctrl
    (PCM_LR_CH_POL_RIGHT_LEFT << PCM_LRCHPOL_POS) |
    (PCM_CLK_FALLING_EDGE << PCM_CLKINV_POS) |
    (PCM_MODE_PCM << PCM_IOM_PCM_POS) |
    (PCM_BUS_SPEED_2048k << PCM_BUSSPEED_LSB) |         //8k sample rate; 2048k = slot_num * sample_rate * bit= 16 * 8k * 16
    (PCM_SLOT_0_1 << PCM_SLOTEN_LSB) |
    (PCM_16_BITS << PCM_WORDSIZE_LSB) |
    (PCM_PUSH_PULL_0 << PCM_DOUTCFG_LSB) |
    (PCM_FSYNC_LF << PCM_FSYNCSHP_LSB),
};

#if 1
uint8_t local_feature[] =
{
    0xBF,0xFE,0xCD,0xFa,0xD8,0xbd,0x7b,0x87
};

#else
// disable simple pairing
uint8_t local_feature[] =
{
    0xBF,0xFE,0xCD,0xFE,0xD8,0xbd,0x73,0x87
};
#endif
const uint8_t local_ex_feature_page1[] =
{
   1,   //page
   0,0,0,0,0,0,0,0,   //page 1 feature
};

const uint8_t local_ex_feature_page2[] =
{
   2,   //page
   0,0,0,0,0,0,0,0,   //page 2 feature
};

const uint8_t bt_rf_timing[] =
{
    0xE,   //tx delay
    0x13,    //rx delay
    0x42,  //rx pwrup
    0x0f, ///tx down
    0x56,  //tx pwerup

};

const uint8_t ble_rf_timing[] =
{
    0xC,    //tx delay   tx after rx delay
    0x2C,    //win count   rx window count
    0xc,    ///ble rtrip delay
    0x42,  ///ble rx pwrup
    0x7,    ///ble tx pwerdown
    0x40,   ///ble tx pwerup
};



const uint8_t ble_rl_size[] =
{
    10,    //rl size
};


uint8_t bt_setting_3[] =
{
    1,    //blescan event type   1:don't add scan to the interval node   0:add to interval node
    0,    //rf trx irq enable       0:disable trx irq   1:enable
    0xc8,0,    //hci sleep timeout
    1,       //hci sleep enable
    1,     //esco_ignore_retran_effort
    0,   //bt_xtal_enable
    1,   //inc_win_sz_by_1
    1,  //max_poll_rep

};

uint8_t bt_setting_2[] =
{
    1,    //blescan event type   1:don't add scan to the interval node   0:add to interval node
    0,    //rf trx irq enable       0:disable trx irq   1:enable
    0xC8,0,    //hci sleep timeout
    1       //hci sleep enable

};

uint8_t bt_setting_1[] =
{
    1,    //blescan event type   1:don't add scan to the interval node   0:add to interval node
    0,    //rf trx irq enable       0:disable trx irq   1:enable
};


const uint8_t bt_edr_thr[] =
{
30,0,60,0,5,0,60,0,0,1,
30,0,60,0,5,0,60,0,1,2,
30,0,60,0,5,0,60,0,1,2,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,
};


const uint8_t bt_edr_algo[]=
{
0,0,1,
8,0,3,
16,0,0,
0xff,0xff,0xff,
0xff,0xff,0xff,
0xff,0xff,0xff,
0xff,0xff,0xff,
};



const static BTDRV_CFG_TBL_STRUCT  btdrv_cfg_tbl[] =
{
        {BTDRV_CONFIG_INACTIVE,HCI_DBG_SET_BT_SETTING_CMD_OPCODE,sizeof(bt_setting_3),bt_setting_3},
        {BTDRV_CONFIG_INACTIVE,HCI_DBG_SET_BT_SETTING_CMD_OPCODE,sizeof(bt_setting_2),bt_setting_2},
        {BTDRV_CONFIG_INACTIVE,HCI_DBG_SET_BT_SETTING_CMD_OPCODE,sizeof(bt_setting_1),bt_setting_1},
        {BTDRV_CONFIG_ACTIVE,  HCI_DBG_SET_SLEEP_EXWAKEUP_EN_CMD_OPCODE,sizeof(unsleep_param),unsleep_param},
//        {BTDRV_CONFIG_INACTIVE,HCI_DBG_SET_BD_ADDR_CMD_OPCODE,sizeof(bt_addr),bt_addr},
        {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_CUSTOM_PARAM_CMD_OPCODE,140,(uint8_t *)&btdrv_rf_env},
        {BTDRV_CONFIG_INACTIVE,HCI_DBG_SET_CUSTOM_PARAM_CMD_OPCODE,sizeof(btdrv_bt_spi_cntl),(uint8_t *)&btdrv_bt_spi_cntl},
        {BTDRV_CONFIG_INACTIVE,HCI_DBG_SET_CUSTOM_PARAM_CMD_OPCODE,sizeof(btdrv_ble_spi_cntl),(uint8_t *)&btdrv_ble_spi_cntl},
        {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_CUSTOM_PARAM_CMD_OPCODE,sizeof(btdrv_afh_env),(uint8_t *)&btdrv_afh_env},
        {BTDRV_CONFIG_INACTIVE,HCI_DBG_SET_LPCLK_DRIFT_JITTER_CMD_OPCODE,sizeof(lpclk_drift_jitter),(uint8_t *)&lpclk_drift_jitter},
        {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_WAKEUP_TIME_CMD_OPCODE,sizeof(wakeup_timing),(uint8_t *)&wakeup_timing},
   //     {BTDRV_CONFIG_INACTIVE,HCI_DBG_SET_SLEEP_EXWAKEUP_EN_CMD_OPCODE,sizeof(sleep_param),sleep_param},
#ifdef _SCO_BTPCM_CHANNEL_
        {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_SYNC_CONFIG_CMD_OPCODE,sizeof(sync_config),(uint8_t *)&sync_config},
        {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_PCM_SETTING_CMD_OPCODE,sizeof(pcm_setting),(uint8_t *)&pcm_setting},
#else
        {BTDRV_CONFIG_INACTIVE,HCI_DBG_SET_SYNC_CONFIG_CMD_OPCODE,sizeof(sync_config),(uint8_t *)&sync_config},
        {BTDRV_CONFIG_INACTIVE,HCI_DBG_SET_PCM_SETTING_CMD_OPCODE,sizeof(pcm_setting),(uint8_t *)&pcm_setting},
#endif
        {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_LOCAL_FEATURE_CMD_OPCODE,sizeof(local_feature),local_feature},
        {BTDRV_CONFIG_INACTIVE,HCI_DBG_SET_LOCAL_EX_FEATURE_CMD_OPCODE,sizeof(local_ex_feature_page1),(uint8_t *)&local_ex_feature_page1},
        {BTDRV_CONFIG_INACTIVE,HCI_DBG_SET_LOCAL_EX_FEATURE_CMD_OPCODE,sizeof(local_ex_feature_page2),(uint8_t *)&local_ex_feature_page2},
        {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_BT_RF_TIMING_CMD_OPCODE,sizeof(bt_rf_timing),(uint8_t *)&bt_rf_timing},
        {BTDRV_CONFIG_INACTIVE,HCI_DBG_SET_BLE_RF_TIMING_CMD_OPCODE,sizeof(ble_rf_timing),(uint8_t *)&ble_rf_timing},
        {BTDRV_CONFIG_INACTIVE,HCI_DBG_SET_RL_SIZE_CMD_OPCODE,sizeof(ble_rl_size),(uint8_t *)&ble_rl_size},

        {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_EDR_THRESHOLD_CMD_OPCODE,sizeof(bt_edr_thr),(uint8_t *)&bt_edr_thr},
        {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_EDR_ALGORITHM_CMD_OPCODE,sizeof(bt_edr_algo),(uint8_t *)&bt_edr_algo},
//        {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_SLEEP_EXWAKEUP_EN_CMD_OPCODE,8,sleep_param},
};

const uint32_t mem_config[][2] =
{
//  {0xd0220080,0x18420F56}, ///txon off delay

    {0xc0000024,0x00000001},        ///inyersys msg patch
//  {0xd0350240,0x00000c03},   ////modem config
//  {0xd03502c8,0x00000035},
//  {0xd03502cc,0x00000015},
//  {0xd03502d8,0x00000016},
//  {0xd03502d4,0x0000002f},

  //  {0xd0330024,0x420301dC},
};


const uint32_t mem_config_g[][2] =
{
#ifdef BT_XTAL_SYNC
    {0xc00005bc,0x01010100},        ///byte2 is bt_xtal_enable
#else
    {0xc00005bc,0x01010000},        ///byte2 is bt_xtal_enable
#endif
#if defined(_INTERSYS_NO_THREAD_)
    {0xc0000020,0x00000001},
#endif
};

//set ble heap to 0x900
const uint32_t mem_config_f[][2] =
{
    {0xC00042B0,0x1F000900},
    {0xC00014A8,0xA55A0900},
#if defined(_INTERSYS_NO_THREAD_)
    {0xc0000394,0x00000001},
#endif
};

void btdrv_config_init(void)
{
    if(hal_get_chip_metal_id() <= HAL_CHIP_METAL_ID_1)
    {
        for(uint8_t i=0;i<sizeof(mem_config)/sizeof(mem_config[0]);i++)
        {
                btdrv_write_memory(_32_Bit,mem_config[i][0],(uint8_t *)&mem_config[i][1],4);
                btdrv_delay(1);
        }
    }

    for(uint8_t i=0;i<sizeof(btdrv_cfg_tbl)/sizeof(btdrv_cfg_tbl[0]);i++)
    {
        if(btdrv_cfg_tbl[i].opcode== HCI_DBG_SET_BT_SETTING_CMD_OPCODE && btdrv_cfg_tbl[i].parlen==5)
        {
            if(hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_2 || hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_3)
            {
                unsleep_param[0] = 1;
                unsleep_param[1] = 1;
                btdrv_send_cmd(btdrv_cfg_tbl[i].opcode,btdrv_cfg_tbl[i].parlen,btdrv_cfg_tbl[i].param);
                btdrv_delay(1);
            }

        }
        else if(btdrv_cfg_tbl[i].opcode== HCI_DBG_SET_BT_SETTING_CMD_OPCODE && btdrv_cfg_tbl[i].parlen==9)
        {
            if(hal_get_chip_metal_id() >= HAL_CHIP_METAL_ID_4)
            {
                unsleep_param[0] = 1;
                unsleep_param[1] = 1;
                btdrv_send_cmd(btdrv_cfg_tbl[i].opcode,btdrv_cfg_tbl[i].parlen,btdrv_cfg_tbl[i].param);
                btdrv_delay(1);
            }
        }
        else if(btdrv_cfg_tbl[i].is_act == BTDRV_CONFIG_ACTIVE)
        {
            btdrv_send_cmd(btdrv_cfg_tbl[i].opcode,btdrv_cfg_tbl[i].parlen,btdrv_cfg_tbl[i].param);
            btdrv_delay(1);
        }
    }
    if(hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_4)
    {
        for(uint8_t i=0;i<sizeof(mem_config_g)/sizeof(mem_config_g[0]);i++)
        {
                btdrv_write_memory(_32_Bit,mem_config_g[i][0],(uint8_t *)&mem_config_g[i][1],4);
                btdrv_delay(1);
        }
    }
    else if(hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_2 || hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_3)
    {
        for(uint8_t i=0;i<sizeof(mem_config_f)/sizeof(mem_config_f[0]);i++)
        {
            btdrv_write_memory(_32_Bit,mem_config_f[i][0],(uint8_t *)&mem_config_f[i][1],4);
            btdrv_delay(1);
        }

    }
}

uint8_t testmode_local_feature[] =
{
    0xBF,0xFE,0xCD,0xFE,0xDB,0xFd,0x7b,0x87
};

const static BTDRV_CFG_TBL_STRUCT  btdrv_testmode_cfg_tbl[] =
{
    {BTDRV_CONFIG_ACTIVE,HCI_DBG_SET_LOCAL_FEATURE_CMD_OPCODE,sizeof(testmode_local_feature),testmode_local_feature},
};

void btdrv_testmode_config_init(void)
{
    for(uint8_t i=0;i<sizeof(btdrv_testmode_cfg_tbl)/sizeof(btdrv_testmode_cfg_tbl[0]);i++)
    {
        if(btdrv_testmode_cfg_tbl[i].is_act == BTDRV_CONFIG_ACTIVE)
        {
            btdrv_send_cmd(btdrv_testmode_cfg_tbl[i].opcode,btdrv_testmode_cfg_tbl[i].parlen,btdrv_testmode_cfg_tbl[i].param);
            btdrv_delay(1);
        }
    }
}

void btdrv_sleep_config(uint8_t sleep_en)
{
    sleep_param[0] = sleep_en;
    btdrv_send_cmd(HCI_DBG_SET_SLEEP_EXWAKEUP_EN_CMD_OPCODE,8,sleep_param);
    btdrv_delay(1);
}


void btdrv_feature_default(void)
{
    uint8_t feature[] = {0xBF,0xFE,0xCD,0xFE,0xDB,0xFd,0x7b,0x87};
    btdrv_send_cmd(HCI_DBG_SET_LOCAL_FEATURE_CMD_OPCODE,8,feature);
    btdrv_delay(1);
}
uint8_t test_mode_addr[6] = {0x77,0x77,0x77,0x77,0x77,0x77};
void btdrv_test_mode_addr_set(void)
{
    btdrv_send_cmd(HCI_DBG_SET_BD_ADDR_CMD_OPCODE,sizeof(test_mode_addr),test_mode_addr);
    btdrv_delay(1);
}

#define REG_BT_EM_CS_BASE_ADDR 0xD0210000
#define BT_BTSPIPOSBUF_COUNT  632
#define BT_BTSPINEGBUF_COUNT  632
#define BT_BLESPIPOSBUF_COUNT  320


#define EM_BT_SPIPOSBUF_OFFSET (0x00006D14)
#define EM_BT_SPINEGBUF_OFFSET (EM_BT_SPIPOSBUF_OFFSET+(BT_BTSPIPOSBUF_COUNT<<1))
#define EM_BLE_SPIPOSBUF_OFFSET (EM_BT_SPINEGBUF_OFFSET+(BT_BTSPINEGBUF_COUNT<<1))
#define EM_BLE_SPINEGBUF_OFFSET (EM_BLE_SPIPOSBUF_OFFSET+(BT_BLESPIPOSBUF_COUNT<<1))

#define EM_BT_SPIPOSBUF_ADRESS_BASE (REG_BT_EM_CS_BASE_ADDR + EM_BT_SPIPOSBUF_OFFSET)
#define EM_BT_SPINEGBUF_ADRESS_BASE (REG_BT_EM_CS_BASE_ADDR + EM_BT_SPINEGBUF_OFFSET)
#define EM_BLE_SPIPOSBUF_ADRESS_BASE (REG_BT_EM_CS_BASE_ADDR + EM_BLE_SPIPOSBUF_OFFSET)
#define EM_BLE_SPINEGBUF_ADRESS_BASE (REG_BT_EM_CS_BASE_ADDR + EM_BLE_SPINEGBUF_OFFSET)



#define BT_RFSPI_WR (0<<7)
#define BT_RFSPI_RD (1<<7)
#define BT_RFSPI_SETFREQ (1<<6)

#define BT_RFSPI_FRAME(n) (((n-1)&0xf)<<2)
#define BT_RFSPI_BYTEPREFRAME(n) (((n-2)&0x3)<<0)

#if 0
 /*[vco init] */
{0x18,0x0070,0}, /*0x077f */
{0x19,0x3C40,0}, /*0x3f80 */
{0x1a,0x0000,0}, /*0xc010 */

/*[vco init] */
{0x18,0x007f,0}, /*0x077f */
{0x19,0x3ff0,0}, /*0x3f80 */
{0x1a,0x0000,0}, /*0xc010 */
#endif

#define pos_frame  3
#define neg_frame  (3 + pos_frame)

uint8_t btdrv_bt_spi_pos_raw[]=
{
	(BT_RFSPI_WR|BT_RFSPI_FRAME(pos_frame)|BT_RFSPI_BYTEPREFRAME(3)),
	0x09,0x16,0x00,//sel ee
	0x09,0x15,0x00,//sel ed
	0x09,0x00,0x00,//back to agc control
};

const uint8_t btdrv_bt_spi_neg_raw[]=
{
	(BT_RFSPI_WR|BT_RFSPI_FRAME(neg_frame)|BT_RFSPI_BYTEPREFRAME(3)),
	0x09,0x15,0x00,//sel ed
	0x09,0x16,0x00,//sel ee
	0x09,0x17,0x00,//sel ef
//#endif
};

const uint8_t btdrv_bt_spi_bitoffset_and_btclock_raw[]=
{
	(BT_RFSPI_WR|BT_RFSPI_FRAME(3)|BT_RFSPI_BYTEPREFRAME(3)),
	0x40,0x00,0x80, //page on
	0x42,0x79,0x00,
	0x40,0x00,0x00, //page off
};

const uint8_t btdrv_ble_spi_pos_raw[16]=
{
	(BT_RFSPI_WR|BT_RFSPI_FRAME(3)|BT_RFSPI_BYTEPREFRAME(3)),
	0x18,0x00,0x7f,
	0x19,0x3C,0x40,
	0x1a,0x00,0x00,
};

const uint8_t btdrv_ble_spi_neg_raw[16]=
{
	(BT_RFSPI_WR|BT_RFSPI_FRAME(3)|BT_RFSPI_BYTEPREFRAME(3)),
	0x18,0x00,0x6c,
	0x19,0x24,0x40,
	0x1a,0x00,0x10,
};

void btdrv_bt_spi_setneg_rawbuf(uint8_t wr_type, const uint8_t *spirawbuf, uint8_t pos_len,uint8_t neg_len)
{
	if (1 == wr_type)
		btdrv_write_memory(_8_Bit, EM_BT_SPIPOSBUF_ADRESS_BASE, spirawbuf, neg_len);

	if (2 == wr_type)
		btdrv_write_memory(_8_Bit, (EM_BT_SPINEGBUF_ADRESS_BASE + pos_len), spirawbuf, neg_len);

	if (3 == wr_type)
		btdrv_write_memory(_8_Bit, EM_BLE_SPIPOSBUF_ADRESS_BASE, spirawbuf, neg_len);

	if (4 == wr_type)
		btdrv_write_memory(_8_Bit, EM_BLE_SPINEGBUF_ADRESS_BASE, spirawbuf, neg_len);
}

void btdrv_bt_spi_setpos_rawbuf(uint8_t wr_type, const uint8_t *spirawbuf, uint8_t pos_len)
{
	if (1 == wr_type)
		btdrv_write_memory(_8_Bit, EM_BT_SPIPOSBUF_ADRESS_BASE, spirawbuf, pos_len);

	if (2 == wr_type)
		btdrv_write_memory(_8_Bit, EM_BT_SPINEGBUF_ADRESS_BASE , spirawbuf, pos_len);

	if (3 == wr_type)
		btdrv_write_memory(_8_Bit, EM_BLE_SPIPOSBUF_ADRESS_BASE, spirawbuf, pos_len);

	if (4 == wr_type)
		btdrv_write_memory(_8_Bit, EM_BLE_SPINEGBUF_ADRESS_BASE, spirawbuf, pos_len);
}

void btdrv_bt_spi_xtal_init(void)
{
    //	TRACE("%s\n", __func__);
    //	btdrv_bt_spi_set_rawbuf(1, (uint8_t *)btdrv_bt_spi_pos_raw, sizeof(btdrv_bt_spi_pos_raw));
    //	btdrv_bt_spi_set_rawbuf(2, (uint8_t *)btdrv_bt_spi_neg_raw, sizeof(btdrv_bt_spi_neg_raw));
    //	btdrv_bt_spi_set_rawbuf(3, (uint8_t *)btdrv_ble_spi_pos_raw, sizeof(btdrv_ble_spi_pos_raw));
    //	btdrv_bt_spi_set_rawbuf(4, (uint8_t *)btdrv_ble_spi_neg_raw, sizeof(btdrv_ble_spi_neg_raw));

    btdrv_bt_spi_setneg_rawbuf(2, btdrv_bt_spi_bitoffset_and_btclock_raw, 0,sizeof(btdrv_bt_spi_bitoffset_and_btclock_raw));

    //    btdrv_send_cmd(HCI_DBG_SET_CUSTOM_PARAM_CMD_OPCODE,sizeof(btdrv_bt_spi_cntl), (uint8_t *)&btdrv_bt_spi_cntl);
}

void btdrv_bt_spi_rawbuf_init(void)
{
    BTDIGITAL_REG(0xd02201c8)=0x348C643C; // enable spi high speed


//	TRACE("%s\n", __func__);
	btdrv_bt_spi_setpos_rawbuf(1, btdrv_bt_spi_pos_raw, sizeof(btdrv_bt_spi_pos_raw));
	btdrv_bt_spi_setneg_rawbuf(2, btdrv_bt_spi_neg_raw, (sizeof(btdrv_bt_spi_pos_raw)),sizeof(btdrv_bt_spi_neg_raw));
	//btdrv_bt_spi_setneg_rawbuf(2, btdrv_bt_spi_neg_raw, 0,sizeof(btdrv_bt_spi_neg_raw));

//    TRACE("!!!5 *(volatile uint32_t *)(0xD0220074): %x\n",*(volatile uint32_t *)(0xD0220074));
//    TRACE("!!!5 *(volatile uint32_t *)(0xD0220078): %x\n",*(volatile uint32_t *)(0xD0220078));

//	btdrv_bt_spi_set_rawbuf(3, (uint8_t *)btdrv_ble_spi_pos_raw, sizeof(btdrv_ble_spi_pos_raw));
//	btdrv_bt_spi_set_rawbuf(4, (uint8_t *)btdrv_ble_spi_neg_raw, sizeof(btdrv_ble_spi_neg_raw));

    //BTDIGITAL_REG(0xd021720c)=0x0f150000 ; // update neg raw buff
    //BTDIGITAL_REG(0xd0217210)=0x150e187a ; // update neg raw buff
    //BTDIGITAL_REG(0xd0217214)=0x00070e00 ; // update neg raw buff
//    BTDIGITAL_REG(0xd0220050)=0xbcb3        ; // smp to spi
//    BTDIGITAL_REG(0x4001f010)=0x00000100 ; // work with i2c
//    BTDIGITAL_REG(0x4001f004)=0x80000000 ; // enable smp
//    BTDIGITAL_REG(0x4001f020)=0x00000000 ; // sel vio


    //gfsk digital gain/2
    //BTDIGITAL_REG(0xd0350300)=0x01 ;



    //smooth the tx pwr
    //btdrv_write_rf_reg(0x15,0x1057);

//    btdrv_bt_spi_set_rawbuf(2, btdrv_bt_spi_bitoffset_and_btclock_raw, sizeof(btdrv_bt_spi_bitoffset_and_btclock_raw));
    btdrv_send_cmd(HCI_DBG_SET_CUSTOM_PARAM_CMD_OPCODE,sizeof(btdrv_bt_spi_cntl), (uint8_t *)&btdrv_bt_spi_cntl);
    *(volatile uint32_t *)(0xD0220074) = 0x26d14;    //spi ptr
    *(volatile uint32_t *)(0xD0220078) = 0x72040420; //spi neg ptr

    btdrv_delay(10);
}

uint8_t meInit_param_get_entry_idx = 0;

int btdrv_meinit_param_init(void)
{
    int size = 0;
    if(hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_2 || hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_3)    
    {
        if ((me_init_param[0][0] == 0xffffffff) &&
            (me_init_param[0][1] == 0xffffffff)){
            size = -1;
        }
    }
    else if(hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_4)
    {
        if ((me_init_param_g[0][0] == 0xffffffff) &&
            (me_init_param_g[0][1] == 0xffffffff)){
            size = -1;
        }    
    }
    meInit_param_get_entry_idx = 0;
    return size;
}

int btdrv_meinit_param_remain_size_get(void)
{
    int remain_size=0;
    if(hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_2 || hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_3)    
    {
        if ((me_init_param[0][0] == 0xffffffff) &&
            (me_init_param[0][1] == 0xffffffff)){
            return -1;
        }
        remain_size = sizeof(me_init_param)/sizeof(me_init_param[0]) - meInit_param_get_entry_idx;
        
    }
    else if(hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_4)
    {
        if ((me_init_param_g[0][0] == 0xffffffff) &&
            (me_init_param_g[0][1] == 0xffffffff)){
            return -1;
        }    
        remain_size = sizeof(me_init_param_g)/sizeof(me_init_param_g[0]) - meInit_param_get_entry_idx;
        
    }
//    remain_size = sizeof(me_init_param)/sizeof(me_init_param[0]) - meInit_param_get_entry_idx;
    return remain_size;
}

int btdrv_meinit_param_next_entry_get(uint32_t *addr, uint32_t *val)
{
    if(hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_2 || hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_3)    
    {
        if (meInit_param_get_entry_idx > (sizeof(me_init_param)/sizeof(me_init_param[0])-1))
            return -1;
        *addr = me_init_param[meInit_param_get_entry_idx][0];
        *val = me_init_param[meInit_param_get_entry_idx][1];
        meInit_param_get_entry_idx++;
    }
    else if(hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_4)
    {
        if (meInit_param_get_entry_idx > (sizeof(me_init_param_g)/sizeof(me_init_param_g[0])-1))
            return -1;
        *addr = me_init_param_g[meInit_param_get_entry_idx][0];
        *val = me_init_param_g[meInit_param_get_entry_idx][1];
        meInit_param_get_entry_idx++;    
    }
    return 0;
}

enum
{
    SYNC_IDLE,
    SYNC_64_ORG,
    SYNC_68_ORG,
    SYNC_72_ORG,
    SYNC_64_NEW,
    SYNC_68_NEW,
    SYNC_72_NEW,
};

enum
{
    SYNC_64_BIT,
    SYNC_68_BIT,
    SYNC_72_BIT,
};

uint32_t bt_68_org_set[][2] =
{
   {0xd02201c8,0x148C6042},
   {0xd0350360,0x00000020},
   {0xd03502c4,0x003F0302},

};

#if 1
uint32_t bt_sync_type = SYNC_64_NEW;
void btdrv_sync_config(void)
{
    uint32_t corr_mode = 0x148C6000;
    uint32_t dfe_mode = 0x00000020;
    uint32_t timeinit = 0x003f0302;
    if(bt_sync_type == SYNC_IDLE)
        return;

    switch(bt_sync_type)
    {
        case SYNC_64_ORG:
            corr_mode |=SYNC_64_BIT<<8;
            corr_mode |=0x42;
            break;
        case SYNC_68_ORG:
            corr_mode |=SYNC_68_BIT<<8;
            corr_mode |=0x42;
            break;
         case  SYNC_72_ORG:
            corr_mode |=SYNC_72_BIT<<8;
            corr_mode |=0x46;
            timeinit = 0x003f02ce;
            break;
        case SYNC_64_NEW:
            corr_mode |=SYNC_64_BIT<<8;
            corr_mode |=0x3C;
            dfe_mode |= 0x14;
            break;
        case SYNC_68_NEW:
            corr_mode |=SYNC_68_BIT<<8;
            corr_mode |=0x3C;
            dfe_mode |= 0x14;
            break;
         case  SYNC_72_NEW:
            corr_mode |=SYNC_72_BIT<<8;
            corr_mode |=0x40;
            dfe_mode |= 0x10;
            timeinit = 0x003f02ce;
            break;
    }
    btdrv_write_memory(_32_Bit,0xd02201c8,(uint8_t *)&corr_mode,4);
    btdrv_write_memory(_32_Bit,0xd0350360,(uint8_t *)&dfe_mode,4);
    btdrv_write_memory(_32_Bit,0xd03502c4,(uint8_t *)&timeinit,4);
}
#endif

static bool bt_slave_role;

void btdrv_store_device_role(bool slave)
{
    bt_slave_role = slave;
}

bool btdrv_device_role_is_slave(void)
{
    return bt_slave_role;
}

