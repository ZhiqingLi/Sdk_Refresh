#include "plat_types.h"
#include "hal_i2c.h"
#include "hal_uart.h"
#include "hal_chipid.h"
#include "bt_drv.h"


extern void btdrv_write_memory(uint8_t wr_type,uint32_t address,const uint8_t *value,uint8_t length);

///enable m4 patch func
#define BTDRV_PATCH_EN_REG   0xe0002000

//set m4 patch remap adress
#define BTDRV_PATCH_REMAP_REG    0xe0002004

////instruction patch compare src address
#define BTDRV_PATCH_INS_COMP_ADDR_START   0xe0002008

#define BTDRV_PATCH_INS_REMAP_ADDR_START   0xc0000100

////data patch compare src address
#define BTDRV_PATCH_DATA_COMP_ADDR_START   0xe0002088

#define BTDRV_PATCH_DATA_REMAP_ADDR_START   0xc0000180



#define BTDRV_PATCH_ACT   0x1
#define BTDRV_PATCH_INACT   0x0



typedef struct
{
    uint8_t patch_index;   //patch position
    uint8_t patch_state;   //is patch active
    uint16_t patch_length;     ///patch length 0:one instrution replace  other:jump to ram to run more instruction
    uint32_t patch_remap_address;   //patch occured address
    uint32_t patch_remap_value;      //patch replaced instuction
    uint32_t patch_start_address;    ///ram patch address for lenth>0
    uint8_t *patch_data;                  //ram patch date for length >0

}BTDRV_PATCH_STRUCT;





uint8_t patch_count;

/////version g   patch

const BTDRV_PATCH_STRUCT verg_ins_patch0 =
{
    0,
    BTDRV_PATCH_ACT,
    0,
    0x000005f8,
    0x4770bf00,
    0,
    NULL
};



const uint32_t verg_patch1_ins_data[] =
{
	0x3301d00c,
	0x4378f023,
	0xd007459e,
	0xf0233301,
	0x459e4378,
	0xbf00d002,
	0xb974f605,
	0xb96df605
};

const BTDRV_PATCH_STRUCT verg_ins_patch1 =
{
    1,
    BTDRV_PATCH_ACT,
    sizeof(verg_patch1_ins_data),
    0x0000ac70,
    0xbf96f1fa,
    0xc0005ba0,
    (uint8_t *)verg_patch1_ins_data
};//mtk


//tws
const uint32_t verg_patch2_ins_data[] =
{
	0x461ebf28,
	0xbf284281,
	0x480f461c,
	0x28016800,
	0x480bd113,
	0xf4106800,
	0xd00e6f80,
	0x68004809,
	0x005af3c0,
	0x010ef1a0,
	0xd8064559,
	0x45583003,
	0x461ebf28,
	0xbf284558,
	0x3301461c,
	0xb972f607,
	0xd02201b4,
	0xd022007c,
	0xc00001e4,
	0x00000000,
};

const BTDRV_PATCH_STRUCT verg_ins_patch2=
{
    2,
    BTDRV_PATCH_INACT,
    sizeof(verg_patch2_ins_data),
    0x000074b0,
    0xbe72f1f8,
    0xc0000198,
    (uint8_t *)verg_patch2_ins_data
};//tws trigger


const BTDRV_PATCH_STRUCT verg_ins_patch3 =
{
    3,
    BTDRV_PATCH_INACT,
    0,
    0x00006428,
    0x8C0Bbf00,
    0,
    NULL
};//test mode for test max and min edr relative power meanwhile


const BTDRV_PATCH_STRUCT verg_ins_patch4 =
{
    4,
#ifdef __TWS_RECONNECT_USE_BLE__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    0,
    0x000371b8,
    0x2a1fe002,
    0,
    NULL
};//adv faster


const BTDRV_PATCH_STRUCT verg_ins_patch5 =
{
    5,
    BTDRV_PATCH_ACT,
    0,
    0x00029298,
    0xbf002000,
    0,
    NULL
};//HUAWEI d_esco is odd problem(g ver)


const uint32_t verg_patch6_ins_data[] =
{
    0xd1092800,
    0x781b9b15,
    0x0f01f013,
    0xe005d100,
    0xbf002404,
    0xbbf3f624,
    0xbbc8f624,
    0xbbdef624,
};
const BTDRV_PATCH_STRUCT verg_ins_patch6 =
{
    6,
    BTDRV_PATCH_ACT,
    sizeof(verg_patch6_ins_data),
    0x00029894,
    0xb998f1dc,
    0xc0005bc8,
    (uint8_t *)verg_patch6_ins_data
};//HUAWEI d_esco is odd problem(g ver)


const BTDRV_PATCH_STRUCT verg_ins_patch7 =
{
    7,
    BTDRV_PATCH_INACT,
    0,
    0x0002878c,
    0xe018bf00,
    0,
    NULL
};//tws call retransmission problem(g ver)

const uint32_t verg_patch8_ins_data[] =
{
	0x601e4b05,
	0xf5fe20ff,
	0x4a04fb15,
	0xf0236813,
	0x60130301,
	0xbf04f608,
	0xd0220120,
	0xd0221004
};


const BTDRV_PATCH_STRUCT verg_ins_patch8 =
{
    8,
    BTDRV_PATCH_ACT,
    sizeof(verg_patch8_ins_data),
    0x0000ea18,
    0xb8f2f1f7,
    0xc0005c00,
    (uint8_t *)verg_patch8_ins_data
};//remove sco


const BTDRV_PATCH_STRUCT verg_ins_patch9 =
{
    9,
    BTDRV_PATCH_ACT,
    0,
    0x00011d40,
    0xf001bf00,
    0,
    NULL
};//preferrate rate


const BTDRV_PATCH_STRUCT verg_ins_patch10 =
{
    10,
    BTDRV_PATCH_ACT,
    0,
    0x000177f4,
    0xf894bf00,
    0,
    NULL
};//data overflow


#if 0
const BTDRV_PATCH_STRUCT verg_ins_patch11 =
{
    11,
    BTDRV_PATCH_ACT,
    0,
    0x0000ac70,
    0x8118f000,
    0,
    NULL
};//

#else
const BTDRV_PATCH_STRUCT verg_ins_patch11 =
{
    11,
    BTDRV_PATCH_ACT,
    0,
    0x0001dd60,
    0xbf002624,
    0,
    NULL
};//clk offset even
#endif


const uint32_t verg_patch12_ins_data[] =
{
	0xf5fa4631,
	0xebc4fe39,
	0x4b0517c4,
	0x0787eb03,
	0xf6218d38,
	0x4630ffb5,
	0xfeacf60e,
	0xbad3f618,
	0xc00049ac,
};

const BTDRV_PATCH_STRUCT verg_ins_patch12 =
{
    12,
    BTDRV_PATCH_ACT,
    sizeof(verg_patch12_ins_data),
    0x0001e220,
    0xbd1ef1e7,
    0xc0005c60,
    (uint8_t *)verg_patch12_ins_data
};//sniff not accept: RemoveSniff & restore_previous_state

const uint32_t verg_patch13_ins_data[] =
{
	0xd0072b02,
	0x3302b143,
	0xd1012b0e,
	0xbf00230b,
	0xb878f5ff,
	0xb865f5ff,
	0xb867f5ff,
};


const BTDRV_PATCH_STRUCT verg_ins_patch13 =
{
    13,
    BTDRV_PATCH_INACT,
    sizeof(verg_patch13_ins_data),
    0x00004d68,
    0xbf92f200,
    0xc0005c90,
    (uint8_t *)verg_patch13_ins_data
};//3dh3


const uint32_t verg_patch14_ins_data[] =
{
	0x3092f886,
	0x3002f898,
	0x308ef886,
	0x3003f898,
	0x308ff886,
	0x462a4641,
	0x0f04f851,
	0xc2036849,
	0xbf54f613,
};


const BTDRV_PATCH_STRUCT verg_ins_patch14 =
{
    14,
    BTDRV_PATCH_ACT,
    sizeof(verg_patch14_ins_data),
    0x00019b78,
    0xb89af1ec,
    0xc0005cb0,
    (uint8_t *)verg_patch14_ins_data
};//remote feature


const uint32_t verg_patch15_ins_data[] =
{
	0xd80d2826,
	0xd0082820,	
	0xd0062824,
	0xd0042825,
	0xd0022826,
	0xd0002800,
	0x2006e000,
	0xba66f61a,
	0xba7ef61a,
};

 
const BTDRV_PATCH_STRUCT verg_ins_patch15 =
{
    15,
    BTDRV_PATCH_ACT,
    sizeof(verg_patch15_ins_data),
    0x000201c8,
    0xbd8af1e5, 
    0xc0005ce0,
    (uint8_t *)verg_patch15_ins_data
};//disconnect process


uint32_t ins_patch_config_3[] =
{
    16,
    (uint32_t)&verg_ins_patch0,
    (uint32_t)&verg_ins_patch1,
    (uint32_t)&verg_ins_patch2,
    (uint32_t)&verg_ins_patch3,
    (uint32_t)&verg_ins_patch4,
    (uint32_t)&verg_ins_patch5,
    (uint32_t)&verg_ins_patch6,
    (uint32_t)&verg_ins_patch7,
    (uint32_t)&verg_ins_patch8,
    (uint32_t)&verg_ins_patch9,
    (uint32_t)&verg_ins_patch10,
    (uint32_t)&verg_ins_patch11,
    (uint32_t)&verg_ins_patch12,
    (uint32_t)&verg_ins_patch13,
    (uint32_t)&verg_ins_patch14,    
    (uint32_t)&verg_ins_patch15,    
    
};


//wesco to 4
const BTDRV_PATCH_STRUCT verd_ins_patch0 =
{
    0,
    BTDRV_PATCH_ACT,
    0,
    0x000284b0,
    0x6832bf00,
    0,
    NULL
};

//wesco to bigger 4
const BTDRV_PATCH_STRUCT verd_ins_patch1 =
{
    1,
#ifdef __BT_ONE_BRING_TWO__
    BTDRV_PATCH_INACT,
#else
    BTDRV_PATCH_ACT,
#endif
    0,
    0x000284d8,
    0xBF00bf00,
    0,
    NULL
};


//btcore disable wakeup
const BTDRV_PATCH_STRUCT verd_ins_patch2 =
{
    2,
    BTDRV_PATCH_ACT,
    0,
    0x000005f8,
    0x4770bf00,
    0,
    NULL
};


const uint32_t verd_patch3_ins_data[] =
{
    0x681b4b08,
    0x5f80f413,
    0x68134a07,
    0x0318f023,
    0xf043bf08,
    0x60130310,
    0x7a1b4b04,
    0xbf002bff,
    0xbba2f600,
    0xd0220124,
    0xd0224004,
    0xc00042cc,
};

////sco miss data, pcm output zero
// TODO:over hci is not available
const BTDRV_PATCH_STRUCT verd_ins_patch3 =
{
    3,
#if defined (SPEECH_RX_PLC)
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(verd_patch3_ins_data),
    0x00006d84,
    0xbc4cf1ff,
    0xc0006620,
    (uint8_t *)verd_patch3_ins_data
};


//ld_sco_alloc_port
const uint32_t verd_patch4_ins_data[] =
{
   0xbf004d09,
   0x5054f895,
   0x0f01f015,
   0x8005f000,
   0x0f02f015,
   0x800af040,
   0x2500e002,
   0xbe48f608,
   0xbf002501,
   0xbe44f608,
   0xc0005250,
   0xbe8bf608
};
const BTDRV_PATCH_STRUCT verd_ins_patch4 =
{
    4,
#if 1// defined( __BT_ONE_BRING_TWO__) && defined(__BT_REAL_ONE_BRING_TWO__)
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(verd_patch4_ins_data),
    0x0000efd8,
    0xbb3ef1f7,
    0xc0006658,
    (uint8_t *)verd_patch4_ins_data
};


//audio_disable
const uint32_t verd_patch5_ins_data[] =
{
   0x4b07d10c,
   0x2b00781b,
   0x8006f040,
   0xfa0ef62f,
   0xbf00bd38,
   0xb829f62f,
   0xb82cf62f,
   0xb820f62f,
   0xc0005250,
};

const BTDRV_PATCH_STRUCT verd_ins_patch5 =
{
    5,
#if 1//defined( __BT_ONE_BRING_TWO__) && defined(__BT_REAL_ONE_BRING_TWO__)
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(verd_patch5_ins_data),
    0x000356f4,
    0xbfd2f1d0,
    0xc000669c,
    (uint8_t *)verd_patch5_ins_data
};

const uint32_t verd_patch6_ins_data[]  =
{
    0x4b12b40f,
    0x2a01681a,
    0x681bd01c,
    0xd0052b02,
    0x2201bc0f,
    0x20e0f883,
    0xbe19f5fe,
    0x4a0d4b0c,
    0x6812681b,
    0xf3c34053,
    0x2b000309,
    0x4a0ad0f0,
    0x68114b07,
    0x4a07681b,
    0x0309f3c3,
    0x2381ea43,
    0xe7e56013,
    0x601a2202,
    0xbf00e7f1,
    0xd0217210,
    0xd02101c8,
    0xd0217214,
    0xd0220020,
};

const BTDRV_PATCH_STRUCT verd_ins_patch6 =
{
    6,
#ifdef BT_XTAL_SYNC
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(verd_patch6_ins_data),
    0x00005310,
    0xb9daf201,
    0xc00066c8,
    (uint8_t *)verd_patch6_ins_data
};// ljh add :bit offset and bt clock  sync

const uint32_t verd_patch7_ins_data[]  =
{
    0xbf00d90c,
    0x3025f859,
    0x30c5f893,
    0x9806b933,
    0xfd5af5fa,
    0xd0012805,
    0xba17f632,
    0xba3af632
};

const BTDRV_PATCH_STRUCT verd_ins_patch7 =
{
    7,
    BTDRV_PATCH_INACT,
    sizeof(verd_patch7_ins_data),
    0x00038b74,
    0xbddaf1cd,
    0xc000672c,
    (uint8_t *)verd_patch7_ins_data
};// ljh add :ble channel map bug

const BTDRV_PATCH_STRUCT verd_ins_patch8 =
{
    8,
    BTDRV_PATCH_ACT,
    0,
    0x00017558,
    0xf894bf00,
    0,
    NULL
};//solve data overflow

const uint32_t verd_patch9_ins_data[] =
{
    0x6f00f1b2,
    0xf412d004,
    0xd0037fc0,
    0xb998f607,
    0xbce0f607,
    0xb992f607,
};

const BTDRV_PATCH_STRUCT verd_ins_patch9 =
{
    9,
    BTDRV_PATCH_ACT,
    sizeof(verd_patch9_ins_data),
    0x0000da88,
    0xbe64f1f8,
    0xc0006754,
    (uint8_t *)verd_patch9_ins_data
};//sniff patch 1

const uint32_t verd_patch10_ins_data[] =
{
    0x2b019b0f,
    0x9b09d908,
    0xd1052b07,
    0x68194b07,
    0x29013101,
    0xe002dc05,
    0x21004b04,
    0xf6006019,
    0x2100bf7c,
    0xf6006019,
    0xbf00bf51,
    0xa0206840,
    0x00000000
};

const BTDRV_PATCH_STRUCT verd_ins_patch10 =
{
    10,
    BTDRV_PATCH_ACT,
    sizeof(verd_patch10_ins_data),
    0x00007630,
    0xb89ef1ff,
    0xc0006770,
    (uint8_t *)verd_patch10_ins_data
};//sniff patch 2

// role switch
const uint32_t verd_patch11_ins_data[] =
{
	0xbf004b05,
	0x0787eb03,
	0x2044f887,
	0x203df897,
	0x203ef887,
	0xbb43f617,
	0xc000541c,
};

const BTDRV_PATCH_STRUCT verd_ins_patch11 =
{
    11,
    BTDRV_PATCH_ACT,
    sizeof(verd_patch11_ins_data),
    0x0001de3c,
    0xbcb4f1e8,
    0xc00067a8,
    (uint8_t *)verd_patch11_ins_data
};//not accept


#if 0
const uint32_t verd_patch12_ins_data[] =
{
	0x31e2f896,
	0xb167b16b,
	0x13c4ebc4,
	0xeb024a09,
	0xf8930383,
	0xb12111d0,
	0xf8832101,
	0x4b0311d1,
	0x23006019,
	0x31e2f886,
	0xbcc1f60e,
	0xc00067f8,
	0x00000000,
	0xc000541c,
};

const BTDRV_PATCH_STRUCT verd_ins_patch12 =
{
    12,
    BTDRV_PATCH_INACT,
    sizeof(verd_patch12_ins_data),
    0x00015170,
    0xbb2af1f1,
    0xc00067c8,
    (uint8_t *)verd_patch12_ins_data
};//fail1

#else
const uint32_t verd_patch12_ins_data[] =
{
    0xf5fa4610,	
    0x2826fd13,		
    0xbf00d80e,	
    0xd0082820,		
    0xd0062824,		
    0xd0042825,		
    0xd0022826,		
    0xd0002800,		
    0x2006e000,	
    0xbb9bf619,	
    0xbbb3f619,	
};

const BTDRV_PATCH_STRUCT verd_ins_patch12 =
{
    12,
    BTDRV_PATCH_ACT,
    sizeof(verd_patch12_ins_data),
    0x0001ff1c,
    0xbc54f1e6,
    0xc00067c8,
    (uint8_t *)verd_patch12_ins_data
};//disconnext buffree


#endif

const uint32_t verd_patch13_ins_data[] =
{
	0x4b012200,
	0xbdf8601a,
	0xc00067f8,
};

const BTDRV_PATCH_STRUCT verd_ins_patch13 =
{
    13,
    BTDRV_PATCH_INACT,
    sizeof(verd_patch13_ins_data),
    0x000151e4,
    0xbb0ef1f1,
    0xc0006804,
    (uint8_t *)verd_patch13_ins_data
};//fail 2

const uint32_t verd_patch14_ins_data[] =
{
	0x4b04b933,
	0xb91b681b,
	0xbf004620,
	0xb8e4f60d,
	0xb8e6f60d,
	0xc00067f8,
};

const BTDRV_PATCH_STRUCT verd_ins_patch14 =
{
    14,
    BTDRV_PATCH_INACT,
    sizeof(verd_patch14_ins_data),
    0x000139e8,
    0xbf14f1f2,
    0xc0006814,
    (uint8_t *)verd_patch14_ins_data
};//fail3

//dual sco
const uint32_t verd_patch15_ins_data[] =
{
    0xd0052b00,
    0x681b4b03,
    0xd1012b00,
    0xbe9df606,
    0xbf00bdf0,
    0xc00052b0,
};

const BTDRV_PATCH_STRUCT verd_ins_patch15 =
{
    15,
    BTDRV_PATCH_INACT,
    sizeof(verd_patch15_ins_data),
    0x0000d574,
    0xb95cf1f9,
    0xc0006830,
    (uint8_t *)verd_patch15_ins_data
};//rx win

const uint32_t verd_patch16_ins_data[] =
{
    0xd1194297,
    0x68124a0f,
    0xfa072701,
    0x4297f703,
    0x4a0bd014,
    0xd1072b00,
    0x6a127e57,
    0xf022443a,
    0x4f074278,
    0xe007623a,
    0x6a527e97,
    0xf022443a,
    0x4f034278,
    0xbf00627a,
    0xb8e2f607,
    0xb8caf607,
    0xc0005250,
    0xc00052b0
};

const BTDRV_PATCH_STRUCT verd_ins_patch16 =
{
    16,
    BTDRV_PATCH_ACT,
    sizeof(verd_patch16_ins_data),
    0x00007210,
    0xbf16f1f8,
    0xc0000040,
    (uint8_t *)verd_patch16_ins_data
};// master/slave sco

const uint32_t verd_patch17_ins_data[] =
{
	0xd015459e,
	0xf0233301,
	0x459e4378,
	0x3301d010,
	0x4378f023,
	0xd00b459e,
	0xf0233301,
	0x459e4378,
	0x3301d006,
	0x4378f023,
	0xd001459e,
	0xbe59f60a,
	0xbe52f60a,
};

const BTDRV_PATCH_STRUCT verd_ins_patch17 =
{
    17,
    BTDRV_PATCH_ACT,
    sizeof(verd_patch17_ins_data),
    0x0000ab18,
    0xbab8f1f5,
    0xc000008c,
    (uint8_t *)verd_patch17_ins_data
};//mtk

//remove sco/esco
const BTDRV_PATCH_STRUCT verd_ins_patch18 =
{
    18,
    BTDRV_PATCH_ACT,
    0,
    0x000195cc,
    0x78eb782a,
    0,
    NULL,
};// remove sco link 1

const BTDRV_PATCH_STRUCT verd_ins_patch19=
{
    19,
    BTDRV_PATCH_ACT,
    0,
    0x00019578,
    0x78eb782a,
    0,
    NULL,
};// remove esco link 1

#if 0
const uint32_t verd_patch20_ins_data[] =
{
	0xb12b681b,
	0xbf008818,
	0x0380f042,
	0xd0094298,
	0x689b4b06,
	0x881bb123,
	0x0280f042,
	0xd0034293,
	0xb8d3f626,
	0xb92ff626,
	0xb92ff626,
	0xc00005c4,
};

const BTDRV_PATCH_STRUCT verd_ins_patch20=
{
    20,
    BTDRV_PATCH_ACT,
    sizeof(verd_patch20_ins_data),
    0x00026314,
    0xbf24f1d9,
    0xc0000160,
    (uint8_t *)verd_patch20_ins_data
};// remove sco link 2
#else
const uint32_t verd_patch20_ins_data[] =
{
	0xb12b681b,
	0xbf008818,
	0x0380f042,
	0xd0094298,
	0x689b4b06,
	0x881bb123,
	0x0280f042,
	0xd0034293,
	0xba99f624,
	0xbaf5f624,
	0xbaf5f624,
	0xc00005c4,
};

const BTDRV_PATCH_STRUCT verd_ins_patch20=
{
    20,
    BTDRV_PATCH_ACT,
    sizeof(verd_patch20_ins_data),
    0x00026314,
    0xbd5ef1db,
    0xc0001dd4,
    (uint8_t *)verd_patch20_ins_data
};// remove sco link 2

#endif
//tws
const uint32_t verd_patch21_ins_data[] =
{
	0x461fbf28,
	0xbf284281,
	0x480f461c,
	0x28016800,
	0x480bd114,
	0xf4106800,
	0xd00f6f80,
	0x68004809,
	0x005af3c0,
	0x010ef1a0,
	0xd8074559,
	0x45583003,
	0x461fbf28,
	0xbf284558,
	0x3301461c,
	0xb8ecf607,
	0xd02201b4,
	0xd022007c,
	0xc00001e4,
	0x00000000,
};

const BTDRV_PATCH_STRUCT verd_ins_patch21=
{
    21,
    BTDRV_PATCH_INACT,
    sizeof(verd_patch21_ins_data),
    0x000073a4,
    0xbef8f1f8,
    0xc0000198,
    (uint8_t *)verd_patch21_ins_data
};//tws trigger

const BTDRV_PATCH_STRUCT verd_ins_patch22 =
{
    22,
    BTDRV_PATCH_ACT,
    0,
    0x00006374,
    0x8C0Bbf00,
    0,
    NULL
};//test mode for test max and min edr relative power meanwhile

const BTDRV_PATCH_STRUCT verd_ins_patch23 =
{
    23,
#ifdef __TWS_RECONNECT_USE_BLE__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    0,
    0x00036e44,
    0x2a1fe002,
    0,
    NULL
};//adv faster


const uint32_t verd_patch_test_ins_data[] =
{
	0xd8092809,
	0x68204c09,
	0x0004f420,
	0x4000f420,
	0xbf006020,
	0xbf72f610,
	0xbf68f610,
	0xd0220000,
};

const BTDRV_PATCH_STRUCT verd_ins_patchtest =
{
    16,
    BTDRV_PATCH_ACT,
    sizeof(verd_patch_test_ins_data),
    0x00010f28,
    0xb88af1ef,
    0xc0000040,
    (uint8_t *)verd_patch_test_ins_data
};//test mode for write seqn dsb reg



const BTDRV_PATCH_STRUCT verd_ins_patch24=
{
    24,
    BTDRV_PATCH_ACT,
    0,
    0x0003bde4,
    0xbf00e003,
    0,
    NULL
};//disable ll timer



#if 0
const BTDRV_PATCH_STRUCT verd_ins_patch25 =
{
    25,
    BTDRV_PATCH_INACT,
    0,
    0x00028fdc,
    0xbf002000,
    0,
    NULL
};//HUAWEI d_esco is odd problem(f ver)
#else


const uint32_t verd_patch25_ins_data[] =
{
	0x3092f886,
	0x3002f898,
	0x308ef886,
	0x3003f898,
	0x308ff886,
	0x462a4641,
	0x0f04f851,
	0xc2036849,
	0xbd1af617,
};


const BTDRV_PATCH_STRUCT verd_ins_patch25 =
{
    25,
    BTDRV_PATCH_ACT,
    sizeof(verd_patch25_ins_data),
    0x000198dc,
    0xbad4f1e8,
    0xc0001e88,
    (uint8_t *)verd_patch25_ins_data
};//remote feature

#endif


#if 0
const uint32_t verd_patch26_ins_data[] =
{
	0xd0072b02,
	0x3302b143,
	0xd1012b0e,
	0xbf00230b,
	0xbfa0f602,
	0xbf8df602,
	0xbf8ff602,
};


const BTDRV_PATCH_STRUCT verd_ins_patch26 =
{
    26,
    BTDRV_PATCH_INACT,
    sizeof(verd_patch26_ins_data),
    0x00004d38,
    0xb86af1fd,
    0xc0001e10,
    (uint8_t *)verd_patch26_ins_data
};//3dh3
#else

const uint32_t verd_patch26_ins_data[] =
{
    0xeb034b05,
    0x695b0380,
    0x2f00f413,
    0xbf00d102,
    0xbbabf60b,
    0xbdfcf60b,
    0xc00044e4
};


const BTDRV_PATCH_STRUCT verd_ins_patch26 =
{
    26,
    BTDRV_PATCH_ACT,
    sizeof(verd_patch26_ins_data),
    0x0000d574,
    0xbc4cf1f4,
    0xc0001e10,
    (uint8_t *)verd_patch26_ins_data
};//rx win limited

#endif

const BTDRV_PATCH_STRUCT verd_ins_patch27 =
{
    27,
    BTDRV_PATCH_INACT,
    0,
    0x000284d4,
    0xe017bf00,
    0,
    NULL
};//tws call retransmission problem(f ver)


const uint32_t verd_patch28_ins_data[] =
{
	0x601e4b05,
	0xf60220ff,
	0x4a04f9dd,
	0xf0236813,
	0x60130301,
	0xbd0af60c,
	0xd0220120,
	0xd0221004
};


const BTDRV_PATCH_STRUCT verd_ins_patch28 =
{
    28,
    BTDRV_PATCH_ACT,
    sizeof(verd_patch28_ins_data),
    0x0000e864,
    0xbaecf1f3,
    0xc0001e40,
    (uint8_t *)verd_patch28_ins_data
};//remove sco disable pll


const BTDRV_PATCH_STRUCT verd_ins_patch29 =
{
    29,
    BTDRV_PATCH_ACT,
    0,
    0x00011b34,
    0xf001bf00,
    0,
    NULL
};//preferred rate


const BTDRV_PATCH_STRUCT verd_ins_patch30 =
{
    30,
    BTDRV_PATCH_ACT,
    0,
    0x0001dac4,
    0xbf002624,
    0,
    NULL
};//slot offset


const uint32_t verd_patch31_ins_data[] =
{
	0xf5fe4631,
	0xebc4fd39,
	0x4b0517c4,
	0x0787eb03,
	0xf6258d38,
	0x4630fd5b,
	0xfca0f612,
	0xb881f61c,
	0xc000541c,
};

const BTDRV_PATCH_STRUCT verd_ins_patch31 =
{
    31,
    BTDRV_PATCH_ACT,
    sizeof(verd_patch31_ins_data),
    0x0001df7c,
    0xbf70f1e3,
    0xc0001e60,
    (uint8_t *)verd_patch31_ins_data
};//sniff not accept: RemoveSniff & restore_previous_state


uint32_t ins_patch_config_2[] =
{
    32,
    (uint32_t)&verd_ins_patch0,
    (uint32_t)&verd_ins_patch1,
    (uint32_t)&verd_ins_patch2,
    (uint32_t)&verd_ins_patch3,
    (uint32_t)&verd_ins_patch4,
    (uint32_t)&verd_ins_patch5,
    (uint32_t)&verd_ins_patch6,
    (uint32_t)&verd_ins_patch7,
    (uint32_t)&verd_ins_patch8,
    (uint32_t)&verd_ins_patch9,
    (uint32_t)&verd_ins_patch10,
    (uint32_t)&verd_ins_patch11,
    (uint32_t)&verd_ins_patch12,
    (uint32_t)&verd_ins_patch13,
    (uint32_t)&verd_ins_patch14,
    (uint32_t)&verd_ins_patch15,
    (uint32_t)&verd_ins_patch16,
    (uint32_t)&verd_ins_patch17,
    (uint32_t)&verd_ins_patch18,
    (uint32_t)&verd_ins_patch19,
    (uint32_t)&verd_ins_patch20,
    (uint32_t)&verd_ins_patch21,
    (uint32_t)&verd_ins_patch22,
    (uint32_t)&verd_ins_patch23,
    (uint32_t)&verd_ins_patch24,
    (uint32_t)&verd_ins_patch25,
    (uint32_t)&verd_ins_patch26,
    (uint32_t)&verd_ins_patch27,
    (uint32_t)&verd_ins_patch28,
    (uint32_t)&verd_ins_patch29,
    (uint32_t)&verd_ins_patch30,
    (uint32_t)&verd_ins_patch31,


};

///ins_patch12   ins_patch1  ins_patch13    for disable extwakeup
///ins_patch19  for switch instant
///ins_patch20 for afh interval
////ins_patch22  ins_patch23 for esco retrans window
//ins_patch24   for data over pcm

#if 0
const uint32_t patch0_ins_data[] =
{
    0xeb034b05,
    0x695a0384,
    0x6f00f1b2,
    0xbf00d102,
    0xba15f607,
    0xbd3df607,
    0xc0004320,
};


const BTDRV_PATCH_STRUCT ins_patch0 =
{
    0,
    BTDRV_PATCH_ACT,
    sizeof(patch0_ins_data),
    0x0000d938,
    0xbde2f1f8,
    0xc0006500,
    (uint8_t *)patch0_ins_data
};///ld_lowpowit_manage------>LD_LID_SNIFFSLEEP1
#else

const uint32_t patch0_ins_data[] =
{
    0xeb034b06,
    0x695a0384,
    0x6f00f1b2,
    0xf412d002,
    0xd0017fc0,
    0xba23f607,
    0xbd4bf607,
    0xc0004320,
};


const BTDRV_PATCH_STRUCT ins_patch0 =
{
    0,
    BTDRV_PATCH_ACT,
    sizeof(patch0_ins_data),
    0x0000d938,
    0xbdd2f1f8,
    0xc00064e0,
    (uint8_t *)patch0_ins_data
};///ld_lowpowit_manage------>LD_LID_SNIFFSLEEP1

#endif


const BTDRV_PATCH_STRUCT ins_patch1 =
{
    1,
    BTDRV_PATCH_ACT,
    0,
    0x000005dc,
    0X4770BF00,
    0,
    NULL
};


const uint32_t patch2_ins_data[] =
{
    0x20a4f883,
    0xf8934b03,
    0x32012121,
    0x2121f883,
    0xbfecf602,
    0xc0004320,
};

const BTDRV_PATCH_STRUCT ins_patch2 =
{
    2,
    BTDRV_PATCH_ACT,
    sizeof(patch2_ins_data),
    0x00009508,
    0xb80af1fd,
    0xc0006520,
    (uint8_t *)patch2_ins_data
};//ld_access_mconnect_pend_rx:ld_env.master_cnt++;



const BTDRV_PATCH_STRUCT ins_patch3 =
{
    3,
    BTDRV_PATCH_ACT,
    0,
    0x00009ae8,
    0xbf004770,
    0,
    NULL
};//ld_access_setup_complete



const uint32_t patch4_ins_data[] =
{
    0x3044f88a,
    0xf8934b03,
    0x30010120,
    0x0120f883,
    0xb9aaf603,
    0xc0004320,
};

const BTDRV_PATCH_STRUCT ins_patch4 =
{
    4,
    BTDRV_PATCH_ACT,
    sizeof(patch4_ins_data),
    0x000098a4,
    0xbe4cf1fc,
    0xc0006540,
    (uint8_t *)patch4_ins_data
};//ld_access_sconnect_pend_rx:ld_env.slave_cnt++;



const BTDRV_PATCH_STRUCT ins_patch5 =
{
    5,
    BTDRV_PATCH_INACT,
    0,
    0x00017a7c,
    0Xbf00bf00,
    0,
    NULL
};////lc_op_scatt_start_ind_handler://lc_scatt_improv_set(dest_id);





const BTDRV_PATCH_STRUCT ins_patch6 =
{
    6,
    BTDRV_PATCH_ACT,
    0,
    0x0000aa78,
    0Xb2d3bf00,
    0,
    NULL
};


const uint32_t patch7_ins_data[] =
{
    0x3000f8dc,
    0xf8de4453,
    0x455bb020,
    0x800af0c0,
    0xebcb4403,
    0xf0230303,
    0xf8de4b78,
    0x459b3000,
    0xba8ff604,
    0xbb76f604,
};

const BTDRV_PATCH_STRUCT ins_patch7 =
{
    7,
    BTDRV_PATCH_ACT,
    sizeof(patch7_ins_data),
    0x0000aa8c,
    0xbd68f1fb,
    0xc0006560,
    (uint8_t *)patch7_ins_data
};//////patch7&8:mode change ??



const uint32_t patch8_ins_data[] =
{
	0xd1092b01,
	0x44234b08,
	0xf0037b1b,
	0x7ba90302,
	0x0102f021,
	0x73a94319,
	0xf3002b00,
	0xf60a8002,
	0xf60ab802,
	0xbf00b88a,
	0xc000512c,
};

const BTDRV_PATCH_STRUCT ins_patch8 =
{
    8,
    BTDRV_PATCH_ACT,
    sizeof(patch8_ins_data),
    0x000105b0,
    0xbfeef1f5,
    0xc0006590,
    (uint8_t *)patch8_ins_data
};  ////patch9:  ld_switch_start: case LD_LID_SM_SWITCH2




const BTDRV_PATCH_STRUCT ins_patch9 =
{
    9,
    BTDRV_PATCH_ACT,
    0,
    0x0000fb80,
    0xbf00e003,
    0,
    NULL
};////patch11 don't prepare fhs at switch req


const uint32_t patch10_ins_data[] =
{
    0x44234b06,
    0x4b067a18,
    0xfb01216c,
    0xf60a3104,
    0xf006fe21,
    0xf609020f,
    0xbf00bbcb,
    0xc000512c,
    0xd02139a0,
};


const BTDRV_PATCH_STRUCT ins_patch10 =
{
    10,
    BTDRV_PATCH_ACT,
    sizeof(patch10_ins_data),
    0x0000fd6c,
    0xbc28f1f6,
    0xc00065c0,
    (uint8_t *)patch10_ins_data
};////patch12 don't prepare fhs at switch req

const uint32_t patch11_ins_data[] =
{
    0xd007428a,
    0xeb024a05,
    0x69520281,
    0x7fc0f412,
    0xbf00d002,
    0xbcf7f607,
    0xbddef607,
    0xc0004320,
};


const BTDRV_PATCH_STRUCT ins_patch11 =
{
    11,
    BTDRV_PATCH_ACT,
    sizeof(patch11_ins_data),
    0x0000dff0,
    0xbafef1f8,
    0xc00065f0,
    (uint8_t *)patch11_ins_data
};///ld_lowpowit_sniff_management:(ld_env.lid_type[LinkId] & (LD_LID_SNIFF |LD_LID_SNIFFSLEEP )

#if 0
BTDRV_PATCH_STRUCT ins_patch12 =
{
    12,
    BTDRV_PATCH_ACT,
    0,
    0x00031eb0,
    0x22ff213c,
    0,
    NULL
};////hci timeout
#endif

const uint32_t patch12_ins_data[] =
{
    0x4280f44f,
    0x601a4b01,
    0xbf004770,
    0xe000e180,
};

const BTDRV_PATCH_STRUCT ins_patch12 =
{
    12,
    BTDRV_PATCH_ACT,
    sizeof(patch12_ins_data),
    0x0000f86c,
    0xbed8f1f6,
    0xc0006620,
    (uint8_t *)patch12_ins_data
};////sleep disable ext wakeup


const uint32_t patch13_ins_data[] =
{
    0x4280f44f,
    0x601a4b01,
    0xb922f609,
    0xe000e100,
};

const BTDRV_PATCH_STRUCT ins_patch13 =
{
    13,
    BTDRV_PATCH_ACT,
    sizeof(patch13_ins_data),
    0x0000f888,
    0xbedaf1f6,
    0xc0006640,
    (uint8_t *)patch13_ins_data
};////wakeup enable ext wakeup


const BTDRV_PATCH_STRUCT ins_patch14 =
{
    14,
    BTDRV_PATCH_ACT,
    0,
    0x0003430c,
    0xbf00bf00,
    0,
    NULL
};////disable sync overflow report


//hci_eif_tx_done
const BTDRV_PATCH_STRUCT ins_patch15 =
{
    15,
    BTDRV_PATCH_ACT,
    0,
    0x0002ffd8,
    0xb133685b,
    0,
    NULL
};

//hci_tx_done
const BTDRV_PATCH_STRUCT ins_patch16 =
{
    16,
    BTDRV_PATCH_ACT,
    0,
    0x00031efc,
    0x685b4b05,
    0,
    NULL
};

//intersys_msg_write
const BTDRV_PATCH_STRUCT ins_patch17 =
{
    17,
    BTDRV_PATCH_ACT,
    0,
    0x0000465c,
    0xf82cf000,
    0,
    NULL
};


const uint32_t patch18_ins_data[] =
{
    0x3b04681a,
    0xf013681b,
    0xd10d0f80,
    0x681a4b0a,
    0x0280f042,
    0xf240601a,
    0x4a087177,
    0x681a6011,
    0x0280f022,
    0xbd08601a,
    0x0f07f012,
    0xbf00d002,
    0xbee2f62e,
    0xbee2f62e,
    0xd0225000,
    0xd0225004,
};


const BTDRV_PATCH_STRUCT ins_patch18 =
{
    18,
    BTDRV_PATCH_ACT,
    sizeof(patch18_ins_data),
    0x00035450,
    0xb906f1d1,
    0xc0006660,
    (uint8_t *)patch18_ins_data
};

#if 0
const BTDRV_PATCH_STRUCT ins_patch19 =
{
    19,
    BTDRV_PATCH_INACT,
    0,
    0x0002fa24,
    0xe0002464,
    0,
    NULL
};//role switch instant


const uint32_t patch20_ins_data[] =
{
    0x00eaf24e,
    0xf44f2138,
    0xbf0062fa,
    0xbe74f626,
};


const BTDRV_PATCH_STRUCT ins_patch20 =
{
    20,
    BTDRV_PATCH_INACT,
    sizeof(patch20_ins_data),
    0x0002d3a0,
    0xb986f1d9,
    0xc00066b0,
    (uint8_t *)patch20_ins_data
};///afh timer
#else
const uint32_t patch19_ins_data[] =
{
	0x31e2f896,
	0xb167b13b,
	0x13c4ebc4,
	0xeb024a09,
	0xf8930383,
	0xb12111d0,
	0xf8832101,
	0x4b0311d1,
	0x23006019,
	0x31e2f886,
	0xbf33f614,
	0x00000090,
	0x00000000,
	0xc0005254,
};

const BTDRV_PATCH_STRUCT ins_patch19 =
{
    19,
    BTDRV_PATCH_ACT,
    sizeof(patch19_ins_data),
    0x00014eec,
    0xb8b8f1eb,
    0xc0000060,
    (uint8_t *)patch19_ins_data
};///fail 1

const uint32_t patch20_ins_data[] =
{
	0x4b012200,
	0xbdf8601a,
	0xc0000090,
};

const BTDRV_PATCH_STRUCT ins_patch20 =
{
    20,
    BTDRV_PATCH_ACT,
    sizeof(patch20_ins_data),
    0x00014f26,
    0xb8bbf1eb,
    0xc00000a0,
    (uint8_t *)patch20_ins_data
};///fail2

#endif
const uint32_t patch21_ins_data[] =
{
    0x12c4ebc4,
    0xeb034b0c,
    0xf8930382,
    0xb97b303d,
    0x14c4ebc4,
    0xeb004809,
    0x30040084,
    0xf6292123,
    0xb128fa97,
    0x40f1f24e,
    0x462a4629,
    0xf9a4f5fa,
    0xf60d4628,
    0xbd70ff31,
    0xc0005254,
    0xc00052bc,
};


const BTDRV_PATCH_STRUCT ins_patch21 =
{
    21,
    BTDRV_PATCH_ACT,
    sizeof(patch21_ins_data),
    0x00014e40,
    0xbc44f1f1,
    0xc00066cc,
    (uint8_t *)patch21_ins_data
};


//wesco
const BTDRV_PATCH_STRUCT ins_patch22 =
{
    22,
    BTDRV_PATCH_ACT,
    0,
    0x0002821c,
    0x6832bf00,
    0,
    NULL
};

//wesco
const BTDRV_PATCH_STRUCT ins_patch23 =
{
    23,
    BTDRV_PATCH_ACT,
    0,
    0x00028244,
    0xBF00bf00,
    0,
    NULL
};

const uint32_t patch24_ins_data[] =
{
    0x681b4b08,
    0x68124a08,
    0x5f80f412,
    0x0318f023,
    0xf043bf08,
    0x4a030310,
    0x4b046013,
    0x2bff7a1b,
    0xba80f600,
    0xd0224004,
    0xd0220124,
    0xc0004108,
};

////sco miss data, pcm output zero
// TODO:over hci is not available
const BTDRV_PATCH_STRUCT ins_patch24 =
{
    24,
#if defined (SPEECH_RX_PLC)
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(patch24_ins_data),
    0x00006c30,
    0xbd6ef1ff,
    0xc0006710,
    (uint8_t *)patch24_ins_data
};


#if 0
const uint32_t patch25_ins_data[] =
{
    0x4b13b40f,
    0x2a01681a,
    0x681bd01e,
    0xd0052b02,
    0x2201bc0f,
    0x20e0f883,
    0xbd2bf5fe,
    0x4a0e4b0d,
    0x6812681b,
    0xf3c34053,
    0x2b000309,
    0x4a0bd0f0,
    0x68114b08,
    0xf5a2681b,
    0x3a0c420e,
    0x0309f3c3,
    0x2381ea43,
    0xe7e36013,
    0x601a2202,
    0xbf00e7ef,
    0xd0217210,
    0xd02101c8,
    0xd0217214,
    0xd0220020,
};

const BTDRV_PATCH_STRUCT ins_patch25 =
{
    25,
#ifdef BT_XTAL_SYNC
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    sizeof(patch25_ins_data),
    0x000051bc,
    0xbac8f201,
    0xc0006750,
    (uint8_t *)patch25_ins_data
};

#else

const uint32_t patch25_ins_data[]  =
{
	0xbf004b05,
	0x0787eb03,
	0x2044f887,
	0x203df897,
	0x203ef887,
	0xbd75f61d,
	0xc0005254,
};

const BTDRV_PATCH_STRUCT ins_patch25 =
{
    25,

    BTDRV_PATCH_ACT,
    sizeof(patch25_ins_data),
    0x0001de3c,
    0xb948f1e2,
    0xc00000d0,
    (uint8_t *)patch25_ins_data
};//not accept

#endif

const uint32_t patch26_ins_data[]  =
{
	0xbf00d90c,
	0x3025f859,
	0x30c5f893,
	0x9806b933,
	0xfd5af5fa,
	0xd0012805,
	0xbeb5f631,
	0xbed8f631
};

const BTDRV_PATCH_STRUCT ins_patch26 =
{
    26,
    BTDRV_PATCH_INACT,
    sizeof(patch26_ins_data),
    0x0003853c,
    0xb93cf1ce,
    0xc00067b8,
    (uint8_t *)patch26_ins_data
};// ljh add :ble channel map bug



////rx win
const BTDRV_PATCH_STRUCT ins_patch27 =
{
    27,
    BTDRV_PATCH_ACT,
    0,
    0x0000643c,
    0x23059a02,
    0,
    NULL
};


const uint32_t patch28_ins_data[] =
{
    0xd0082b00,
    0xeb034b04,
    0x695b0380,
    0x2f00f413,
    0xf606d101,
    0xbdf0be22,
    0xc0004320
};

const BTDRV_PATCH_STRUCT ins_patch28 =
{
    28,
    BTDRV_PATCH_ACT,
    sizeof(patch28_ins_data),
    0x0000d434,
    0xb9d4f1f9,
    0xc00067e0,
    (uint8_t *)patch28_ins_data
};//rxwin




const uint32_t patch29_ins_data[] =
{
    0x2b019b0f,
    0x9b09d908,
    0xd1052b07,
    0x68194b07,
    0x29013101,
    0xe002dc05,
    0x21004b04,
    0xf6006019,
    0x2100be88,
    0xf6006019,
    0xbf00be5d,
    0xa0206840,
    0x00000000
};

const BTDRV_PATCH_STRUCT ins_patch29 =
{
    29,
    BTDRV_PATCH_ACT,
    sizeof(patch29_ins_data),
    0x000074e8,
    0xb992f1ff,
    0xc0006810,
    (uint8_t *)patch29_ins_data
};//sniff patch

const uint32_t patch30_ins_data[] =
{
	0x4b03b904,
	0xb901681b,
	0xbb6af613,
	0xbb6bf613,
	0xc0000090,
};

const BTDRV_PATCH_STRUCT ins_patch30 =
{
    30,
    BTDRV_PATCH_ACT,
    sizeof(patch30_ins_data),
    0x0001378e,
    0xb8bbf1eb,
    0xc00000b4,
    (uint8_t *)patch30_ins_data
};///fail3

const uint32_t patch31_ins_data[] =
{
	0xd10745e6,
	0xbf003302,
	0x4378f023,
	0xd101459e,
	0xbdfcf60a,
	0xbdeef60a,
};

const BTDRV_PATCH_STRUCT ins_patch31 =
{
    31,
    BTDRV_PATCH_ACT,
    sizeof(patch31_ins_data),
    0x0000a9f8,
    0xbb22f1f5,
    0xc0000040,
    (uint8_t *)patch31_ins_data
};//dual sco

#if 0
const uint32_t patch29_ins_data[] =
{
    0xf40a9307,
    0xf5b34398,
    0xd0086f40,
    0xd1042f02,
    0x444b4b04,
    0xf8832200,
    0xf5ff21ea,
    0xf5ffbb4d,
    0xbf00bab2,
    0xc00044a0,
};

const BTDRV_PATCH_STRUCT ins_patch29 =
{
    29,
    BTDRV_PATCH_ACT,
    sizeof(patch29_ins_data),
    0x00005d78,
    0xbd42f200,
    0xc0006800,
    (uint8_t *)patch29_ins_data
};//master poll


const uint32_t patch30_ins_data[] =
{
    0xdc022b05,
    0xf5ff4b02,
    0xf5ffbdb5,
    0xbf00bdb8,
    0xc00044a0,
};

const BTDRV_PATCH_STRUCT ins_patch30 =
{
    30,
    BTDRV_PATCH_ACT,
    sizeof(patch30_ins_data),
    0x000063a0,
    0xba46f200,
    0xc0006830,
    (uint8_t *)patch30_ins_data
};//master poll

#endif


uint32_t ins_patch_config_1[] =
{
    32,
    (uint32_t)&ins_patch0,
    (uint32_t)&ins_patch1,
    (uint32_t)&ins_patch2,
    (uint32_t)&ins_patch3,
    (uint32_t)&ins_patch4,
    (uint32_t)&ins_patch5,
    (uint32_t)&ins_patch6,
    (uint32_t)&ins_patch7,
    (uint32_t)&ins_patch8,
    (uint32_t)&ins_patch9,
    (uint32_t)&ins_patch10,
    (uint32_t)&ins_patch11,
    (uint32_t)&ins_patch12,
    (uint32_t)&ins_patch13,
    (uint32_t)&ins_patch14,
    (uint32_t)&ins_patch15,
    (uint32_t)&ins_patch16,
    (uint32_t)&ins_patch17,
    (uint32_t)&ins_patch18,
    (uint32_t)&ins_patch19,
    (uint32_t)&ins_patch20,
    (uint32_t)&ins_patch21,
    (uint32_t)&ins_patch22,
    (uint32_t)&ins_patch23,
    (uint32_t)&ins_patch24,
    (uint32_t)&ins_patch25,
    (uint32_t)&ins_patch26,
    (uint32_t)&ins_patch27,
    (uint32_t)&ins_patch28,
    (uint32_t)&ins_patch29,
    (uint32_t)&ins_patch30,
    (uint32_t)&ins_patch31,
};


void btdrv_ins_patch_write(BTDRV_PATCH_STRUCT *ins_patch_p)
{
    uint32_t remap_addr;
    uint8_t i=0;
    remap_addr =   ins_patch_p->patch_remap_address | 1;
     btdrv_write_memory(_32_Bit,(BTDRV_PATCH_INS_REMAP_ADDR_START + ins_patch_p->patch_index*4),
                                                (uint8_t *)&ins_patch_p->patch_remap_value,4);
    if(ins_patch_p->patch_length != 0)  //have ram patch data
    {
        for( ;i<(ins_patch_p->patch_length-1)/128;i++)
        {
                    btdrv_write_memory(_32_Bit,ins_patch_p->patch_start_address+i*128,
                        (ins_patch_p->patch_data+i*128),128);

        }

        btdrv_write_memory(_32_Bit,ins_patch_p->patch_start_address+i*128,ins_patch_p->patch_data+i*128,
            ins_patch_p->patch_length%128);
    }

    btdrv_write_memory(_32_Bit,(BTDRV_PATCH_INS_COMP_ADDR_START + ins_patch_p->patch_index*4),
                                                (uint8_t *)&remap_addr,4);

}

void btdrv_ins_patch_init(void)
{
    BTDRV_PATCH_STRUCT *ins_patch_p;

    if(hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_2 || hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_3)
    {
        for(uint8_t i=0;i<ins_patch_config_2[0];i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)ins_patch_config_2[i+1];
            if(ins_patch_p->patch_state ==BTDRV_PATCH_ACT)
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)ins_patch_config_2[i+1]);
        }
    }
    else if(hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_4)
    {
        for(uint8_t i=0;i<ins_patch_config_3[0];i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)ins_patch_config_3[i+1];
            if(ins_patch_p->patch_state ==BTDRV_PATCH_ACT)
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)ins_patch_config_3[i+1]);
        }
    }
    else
    {
        for(uint8_t i=0;i<ins_patch_config_1[0];i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)ins_patch_config_1[i+1];
            if(ins_patch_p->patch_state ==BTDRV_PATCH_ACT)
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)ins_patch_config_1[i+1]);
        }
    }
}




///////////////////data  patch ..////////////////////////////////////
uint32_t dpatch0_data[] =
{
    0x00360011,
    0x0,
    0x0,
    0x0,
    0x16f,
    0x0,
    0x2a7,
    0
};



BTDRV_PATCH_STRUCT data_patch0 =
{
    0,
    BTDRV_PATCH_ACT,
    sizeof(dpatch0_data),
    0x00004d78,
    0xc0001eb0,
    0xc0001eb0,
    (uint8_t *)&dpatch0_data
};


uint32_t data_patch_config_d[] =
{
    1,
    (uint32_t)&data_patch0,

};


uint32_t dpatch0_data_g[] =
{
    0x00360011,
    0x0,
    0x0,
    0x0,
    0x16f,
    0x0,
    0x2a7,
    0
};

BTDRV_PATCH_STRUCT data_patch0_g =
{
    0,
    BTDRV_PATCH_ACT,
    sizeof(dpatch0_data_g),
    0x00004da8,
    0xc0005c30,
    0xc0005c30,
    (uint8_t *)&dpatch0_data_g
};


uint32_t data_patch_config_g[] =
{
    1,
    (uint32_t)&data_patch0_g,

};


void btdrv_data_patch_write(const BTDRV_PATCH_STRUCT *d_patch_p)
{
    uint32_t remap_addr;
    uint8_t i=0;

    remap_addr = d_patch_p->patch_remap_address |1;
    btdrv_write_memory(_32_Bit,(BTDRV_PATCH_DATA_COMP_ADDR_START + d_patch_p->patch_index*4),
                                        (uint8_t *)&remap_addr,4);
    btdrv_write_memory(_32_Bit,(BTDRV_PATCH_DATA_REMAP_ADDR_START + d_patch_p->patch_index*4),
                                        (uint8_t *)&d_patch_p->patch_remap_value,4);

    if(d_patch_p->patch_length != 0)  //have ram patch data
    {
        for( ;i<(d_patch_p->patch_length-1)/128;i++)
        {
                    btdrv_write_memory(_32_Bit,d_patch_p->patch_start_address+i*128,
                        (d_patch_p->patch_data+i*128),128);

        }

        btdrv_write_memory(_32_Bit,d_patch_p->patch_start_address+i*128,d_patch_p->patch_data+i*128,
            d_patch_p->patch_length%128);
    }

}

void btdrv_testmode_data_overide(void)
{
    btdrv_data_patch_write(&verd_ins_patchtest);
}


void btdrv_data_patch_init(void)
{
    BTDRV_PATCH_STRUCT *data_patch_p;
    if(hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_2 || hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_3)
    {
        for(uint8_t i=0;i<data_patch_config_d[0];i++)
        {
            data_patch_p = (BTDRV_PATCH_STRUCT *)data_patch_config_d[i+1];
            if(data_patch_p->patch_state == BTDRV_PATCH_ACT)
                btdrv_data_patch_write((BTDRV_PATCH_STRUCT *)data_patch_config_d[i+1]);
        }
    }
    else if(hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_4)
    {
        for(uint8_t i=0;i<data_patch_config_g[0];i++)
        {
            data_patch_p = (BTDRV_PATCH_STRUCT *)data_patch_config_g[i+1];
            if(data_patch_p->patch_state == BTDRV_PATCH_ACT)
                btdrv_data_patch_write((BTDRV_PATCH_STRUCT *)data_patch_config_g[i+1]);
        }
    }
}


//////////////////////////////patch enable////////////////////////


void btdrv_patch_en(uint8_t en)
{
    uint32_t value[2];

    //set patch enable
    value[0] = 0x2f02 | en;
    //set patch remap address  to 0xc0000100
    value[1] = 0x20000100;
    btdrv_write_memory(_32_Bit,BTDRV_PATCH_EN_REG,(uint8_t *)&value,8);
}

void btdrv_ins_patch_test_init(void)
{
}

