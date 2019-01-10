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

/////2200 VER A   patch

const BTDRV_PATCH_STRUCT bes2200_ins_patch0 =
{
    0,
    BTDRV_PATCH_ACT,
    0,
    0x00000604,
    0x4770bf00,
    0,
    NULL
};



const uint32_t bes2200_patch1_ins_data[] =
{
	0x3301d00c,
	0x4378f023,
	0xd007459e,
	0xf0233301,
	0x459e4378,
	0xbf00d002,
	0xbcaaf605,
	0xbca3f605
};

const BTDRV_PATCH_STRUCT bes2200_ins_patch1 =
{
    1,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch1_ins_data),
    0x0000b3bc,
    0xbc60f1fa, 
    0xc0005c80,
    (uint8_t *)bes2200_patch1_ins_data
};//mtk



const BTDRV_PATCH_STRUCT bes2200_ins_patch2 =
{
    2,
#ifdef __TWS_RECONNECT_USE_BLE__
    BTDRV_PATCH_ACT,
#else
    BTDRV_PATCH_INACT,
#endif
    0,
    0x00039458,
    0x2a1fe002,
    0,
    NULL
};//adv faster




const BTDRV_PATCH_STRUCT bes2200_ins_patch3 =
{
    3,
    BTDRV_PATCH_ACT,
    0,
    0x00012750,
    0xf001bf00,
    0,
    NULL
};//preferrate rate





const BTDRV_PATCH_STRUCT bes2200_ins_patch4 =
{
    4,
    BTDRV_PATCH_ACT,
    0,
    0x0001f788,
    0xbf002624,
    0,
    NULL
};//clk offset even


const uint32_t bes2200_patch5_ins_data[] =
{
	0xf5fa4631,
	0xebc4fe17,
	0x4b0517c4,
	0x0787eb03,	
	0xf6238d38,	
	0x4630fffd,	
	0xf8b6f610,	
	0xbfbff619,
	0xc0004a68,
};

const BTDRV_PATCH_STRUCT bes2200_ins_patch5 =
{
    5,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch5_ins_data),
    0x0001fc48,
    0xb832f1e6, 
    0xc0005cb0,
    (uint8_t *)bes2200_patch5_ins_data
};//sniff not accept: RemoveSniff & restore_previous_state



const uint32_t bes2200_patch6_ins_data[] =
{
	0xd0072b02,
	0x3302b143,
	0xd1012b0e,
	0xbf00230b,
	0xbb98f5ff,
	0xbb85f5ff,
	0xbb87f5ff,
};


const BTDRV_PATCH_STRUCT bes2200_ins_patch6 =
{
    6,
    BTDRV_PATCH_INACT,
    sizeof(bes2200_patch6_ins_data),
    0x000053f8,
    0xbc72f200,
    0xc0005ce0,
    (uint8_t *)bes2200_patch6_ins_data
};//3dh3


const uint32_t bes2200_patch7_ins_data[] =
{
	0x3092f886,
	0x3002f898,
	0x308ef886,
	0x3003f898,
	0x308ff886,
	0x462a4641,
	0x0f04f851,
	0xc2036849,
	0xbc08f615,
};


const BTDRV_PATCH_STRUCT bes2200_ins_patch7 =
{
    7,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch7_ins_data),
    0x0001b530,
    0xbbe6f1ea,
    0xc0005d00,
    (uint8_t *)bes2200_patch7_ins_data
};//remote feature


const BTDRV_PATCH_STRUCT bes2200_ins_patch8 =
{
    8,
    BTDRV_PATCH_ACT,
    0,
    0x00022108,
    0xbf00bf00,
    0,
    NULL
};//clk offset even


const uint32_t bes2200_patch9_ins_data[] =
{
	0x68584b04,
	0xf5fdb108,
	0xf5fdfd7b,
	0xf603fd71,
	0xbf00ff1b,
	0xc00047b0,
};


const BTDRV_PATCH_STRUCT bes2200_ins_patch9 =
{
    9,
    BTDRV_PATCH_INACT,
    sizeof(bes2200_patch9_ins_data),
    0x00009b74,
    0xb8dcf1fc,
    0xc0005d30,
    (uint8_t *)bes2200_patch9_ins_data
};//eir buf  


const uint32_t bes2200_patch10_ins_data[] =
{
	0xd80d2826,
	0xd0082820,	
	0xd0062824,
	0xd0042825,
	0xd0022826,
	0xd0002800,
	0x2006e000,
	0xbf32f61b,
	0xbf4af61b,
};


const BTDRV_PATCH_STRUCT bes2200_ins_patch10 =
{
    10,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch10_ins_data),
    0x00021bd0,
    0xb8bef1e4,
    0xc0005d50,
    (uint8_t *)bes2200_patch10_ins_data
};//disconnect process


const BTDRV_PATCH_STRUCT bes2200_ins_patch11 =
{
    11,
    BTDRV_PATCH_ACT,
    0,
    0x00013494,
    0xbf00e01d,
    0,
    NULL
};//clk update




const BTDRV_PATCH_STRUCT bes2200_ins_patch12 =
{
    12,
    BTDRV_PATCH_ACT,
    0,
    0x00021040,
    0xe0012001,
    0,
    NULL
};//pref_rate_change_patch(new_state=1)


#if 0
const BTDRV_PATCH_STRUCT bes2200_ins_patch13 =
{
    13,
    BTDRV_PATCH_INACT,
    0,
    0x00011b2c,
    0xbf00e004,
    0,
    NULL
};//test mode
#else

const uint32_t bes2200_patch13_ins_data[] =
{
	0xf4206820,
	0xf4200004,	
	0x60204000,
	0x4c022000,
	0x0025f884,
	0xbedcf60b,
	0xc0004958,

};


const BTDRV_PATCH_STRUCT bes2200_ins_patch13 =
{
    13,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch13_ins_data),
    0x00011b2c,
    0xb928f1f4,
    0xc0005d80,
    (uint8_t *)bes2200_patch13_ins_data
};//test mode

#endif


const BTDRV_PATCH_STRUCT bes2200_ins_patch14 =
{
    14,
    BTDRV_PATCH_ACT,
    0,
    0x0003fc38,
    0x80130302,
    0,
    NULL
};//test mode ble


#if 0
const uint32_t bes2200_patch15_ins_data[] =
{
    0x0304f443,
    0x4300f443,
    0x4b0b6013,
    0xf023681b,
    0xf4237380,
    0xf89d43f0,
    0xf0822034,
    0x09170255,
    0x020ff002,
    0x23c2ea43,
    0xd1012f02,
    0x7380f043,
    0x60134a01,
    0xbe5cf60c,
    0xd02201b4


};




const BTDRV_PATCH_STRUCT bes2200_ins_patch15 =
{
    15,
    BTDRV_PATCH_INACT,
    sizeof(bes2200_patch15_ins_data),
    0x00012a28,
    0xb9baf1f3,
    0xc0005da0,
    (uint8_t *)bes2200_patch15_ins_data
};//test mode 
#else

const uint32_t bes2200_patch15_ins_data[] =
{
    0x0304f443,
    0x4300f443,
    0x4b026013,
    0x601a2200,
    0xbe6ef60c,
    0xc0005db8,
    0x000000ff,
};




const BTDRV_PATCH_STRUCT bes2200_ins_patch15 =
{
    15,
    BTDRV_PATCH_INACT,
    sizeof(bes2200_patch15_ins_data),
    0x00012a28,
    0xb9baf1f3,
    0xc0005da0,
    (uint8_t *)bes2200_patch15_ins_data
};//test mode 
#endif

const uint32_t bes2200_patch16_ins_data[] =
{
    0xbf006813,
    0x0304f443,
    0x4300f443,
    0x4b0b6013,
    0xf023681b,
    0xf4237380,
    0xf89d43f0,
    0xf0822034,
    0x09170255,
    0x020ff002,
    0x23c2ea43,
    0xd1012f02,
    0x7380f043,
    0x60134a01,
    0xbe3cf60c,
    0xd02201b4


};


const BTDRV_PATCH_STRUCT bes2200_ins_patch16 =
{
    16,
    BTDRV_PATCH_INACT,
    sizeof(bes2200_patch16_ins_data),
    0x000129d8,
    0xba00f1f3,
    0xc0005ddc,
    (uint8_t *)bes2200_patch16_ins_data
};//test mode 


#if 1

const uint32_t bes2200_patch17_ins_data[] =
{
    0x537ff423,
    0x0338f023,
    0xbe26f600

};



const BTDRV_PATCH_STRUCT bes2200_ins_patch17 =
{
    17,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch17_ins_data),
    0x00006a70,
    0xb9d6f1ff,
    0xc0005e20,
    (uint8_t *)bes2200_patch17_ins_data
};//test mode 


#elif 0
const uint32_t bes2200_patch17_ins_data[] =
{
    0x885b4b07,
    0xd8052bb0,
    0x53fff423,
    0x0318f023,
    0xbe22f600,
    0x537ff423,
    0x0338f023,
    0xbe1cf600,
    0xc0005be4,
};



const BTDRV_PATCH_STRUCT bes2200_ins_patch17 =
{
    17,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch17_ins_data),
    0x00006a70,
    0xb9d6f1ff,
    0xc0005e20,
    (uint8_t *)bes2200_patch17_ins_data
};//test mode

#else
const uint32_t bes2200_patch17_ins_data[] =
{
    0x68194b0b,
    0xd80b2920,
    0x60193101,
    0x302cf8b8,
    0x537ff423,
    0x0338f023,
    0x5300f443,
    0xbe1cf600,
    0x302cf8b8,
    0x537ff423,
    0x0338f023,
    0xbe14f600,
    0xc0005db8
};



const BTDRV_PATCH_STRUCT bes2200_ins_patch17 =
{
    17,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch17_ins_data),
    0x00006a70,
    0xb9d6f1ff,
    0xc0005e20,
    (uint8_t *)bes2200_patch17_ins_data
};//test mode

#endif

const BTDRV_PATCH_STRUCT bes2200_ins_patch18 =
{
    18,
    BTDRV_PATCH_INACT,
    0,
    0x00006d78,
    0xbf00bf00,
    0,
    NULL
};//test mode ble


static const uint32_t bes2200_ins_patch_config[] =
{
    19,
    (uint32_t)&bes2200_ins_patch0,
    (uint32_t)&bes2200_ins_patch1,
    (uint32_t)&bes2200_ins_patch2,
    (uint32_t)&bes2200_ins_patch3,
    (uint32_t)&bes2200_ins_patch4,
    (uint32_t)&bes2200_ins_patch5,
    (uint32_t)&bes2200_ins_patch6,
    (uint32_t)&bes2200_ins_patch7,
    (uint32_t)&bes2200_ins_patch8,
    (uint32_t)&bes2200_ins_patch9,
    (uint32_t)&bes2200_ins_patch10,
    (uint32_t)&bes2200_ins_patch11,
    (uint32_t)&bes2200_ins_patch12,
    (uint32_t)&bes2200_ins_patch13,
    (uint32_t)&bes2200_ins_patch14,
    (uint32_t)&bes2200_ins_patch15,
    (uint32_t)&bes2200_ins_patch16,
    (uint32_t)&bes2200_ins_patch17,
    (uint32_t)&bes2200_ins_patch18,
    
};





const BTDRV_PATCH_STRUCT bes2200_ins_patch0_2 =
{
    0,
    BTDRV_PATCH_ACT,
    0,
    0x000005c4,
    0x4770bf00,
    0,
    NULL
};


const BTDRV_PATCH_STRUCT bes2200_ins_patch1_2 =
{
    1,
    BTDRV_PATCH_ACT,
    0,
    0x000229e4,
    0xe0012001,
    0,
    NULL
};//pref_rate_change_patch(new_state=1)



#if 0

const BTDRV_PATCH_STRUCT bes2200_ins_patch2_2 =
{
    2,
    BTDRV_PATCH_ACT,
    0,
    0x000417cc,
    0x80130302,
    0,
    NULL
};//test mode ble
#elif 0
const uint32_t bes2200_patch2_ins_data_2[] =
{
	0x2b047823,
	0x7820d004,
	0xfc74f604,
	0xbcabf60e,
	0xbf00bd38,
};

const BTDRV_PATCH_STRUCT bes2200_ins_patch2_2 =
{
    2,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch2_ins_data_2),
    0x00014eb0,
    0xbb4ef1f1,
    0xc0006550,
    (uint8_t *)bes2200_patch2_ins_data_2
};//stop sniffer sco
#else
const uint32_t bes2200_patch2_ins_data_2[] =
{
	0x2b047823,
	0x7820d008,
	0xfc74f604,
	0xf6067820,
	0xbf00fab1,
	0xbcaef60e,
	0xbf00bd38,
};

const BTDRV_PATCH_STRUCT bes2200_ins_patch2_2 =
{
    2,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch2_ins_data_2),
    0x00014eb0,
    0xbb4ef1f1,
    0xc0006550,
    (uint8_t *)bes2200_patch2_ins_data_2
};//stop sniffer sco mix patch7



#endif


#if 0
const uint32_t bes2200_patch3_ins_data_2[] =
{
	0xf4206820,
	0xf4200004,	
	0x60204000,
	0x4c022000,
	0x0025f884,
	0xbecaf60b,
	0xc00049c0,
};


const BTDRV_PATCH_STRUCT bes2200_ins_patch3_2 =
{
    3,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch3_ins_data_2),
    0x00011d08,
    0xb93af1f4,
    0xc0005f80,
    (uint8_t *)bes2200_patch3_ins_data_2
};//test mode 1
#endif

const uint32_t bes2200_patch3_ins_data_2[] =
{
	0xf4038ca3,
	0x84a34380,	
	0xd9012f01,
	0xbc3ff5ff,
	0xbc4cf5ff,
};


const BTDRV_PATCH_STRUCT bes2200_ins_patch3_2 =
{
    3,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch3_ins_data_2),
    0x00005808,
    0xbbbaf200,
    0xc0005f80,
    (uint8_t *)bes2200_patch3_ins_data_2
};//sco silence patch1



#if 0
const uint32_t bes2200_patch4_ins_data_2[] =
{
    0x0304f443,
    0x4300f443,
    0x4b026013,
    0x601a2200,
    0xbe86f60c,
    0xc0005fb8,   //count
    0x000000ff,
};




const BTDRV_PATCH_STRUCT bes2200_ins_patch4_2 =
{
    4,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch4_ins_data_2),
    0x00012c58,
    0xb9a2f1f3,
    0xc0005fa0,
    (uint8_t *)bes2200_patch4_ins_data_2
};//test mode  no white
#endif


const uint32_t bes2200_patch4_ins_data_2[] =
{
    0xf4238cab,
    0xb29b4380,
    0xd1012b00,
    0xbcc1f604,
    0xbd39f604,
};


const BTDRV_PATCH_STRUCT bes2200_ins_patch4_2 =
{
    4,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch4_ins_data_2),
    0x0000a92c,
    0xbb38f1fb,
    0xc0005fa0,
    (uint8_t *)bes2200_patch4_ins_data_2
};//sco silence patch2


#if 0
const uint32_t bes2200_patch5_ins_data_2[] =
{
    0xbf006813,
    0x0304f443,
    0x4300f443,
    0x4b026013,
    0x601a2200,
    0xbe74f60c,
    0xc0005fb8,   //count
};




const BTDRV_PATCH_STRUCT bes2200_ins_patch5_2 =
{
    5,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch5_ins_data_2),
    0x00012c08,
    0xb9daf1f3,
    0xc0005fc0,
    (uint8_t *)bes2200_patch5_ins_data_2
};//test mode  whiten
#endif


const uint32_t bes2200_patch5_ins_data_2[] =
{
    0xd1052a00,
    0xf4228c8a,
    0xb2924280,
    0xbf00b112,
    0xbc83f5ff,
    0xbc84f5ff,
};




const BTDRV_PATCH_STRUCT bes2200_ins_patch5_2 =
{
    5,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch5_ins_data_2),
    0x000058d0,
    0xbb76f200,
    0xc0005fc0,
    (uint8_t *)bes2200_patch5_ins_data_2
}; ////sco silence patch3

#if 0
const uint32_t bes2200_patch6_ins_data_2[] =
{
    0x68194b0b,
    0xd80b2930,
    0x60193101,
    0x8d8b9909,
    0x537ff423,
    0x0338f023,
    0x5300f443,
    0xbe2df600,
    0x8d8b9909,
    0x537ff423,
    0x0338f023,
    0xbe25f600,
    0xc0005fb8
};

const BTDRV_PATCH_STRUCT bes2200_ins_patch6_2 =
{
    6,
    BTDRV_PATCH_INACT,
    sizeof(bes2200_patch6_ins_data_2),
    0x00006c50,
    0xb9c6f1ff,
    0xc0005fe0,
    (uint8_t *)bes2200_patch6_ins_data_2
};//test mode silence

const BTDRV_PATCH_STRUCT bes2200_ins_patch7_2 =
{
    7,
    BTDRV_PATCH_INACT,
    0,
    0x00006f5c,
    0xbf00bf00,
    0,
    NULL
};//test mode ble

#else
const uint32_t bes2200_patch6_ins_data_2[] =
{
    0x7b9b4b04,
	0x0f02f013,
	0x4620d102,
	0xfe64f60d,
	0xbc05f615,
	0xc0004bf0
};

const BTDRV_PATCH_STRUCT bes2200_ins_patch6_2 =
{
    6,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch6_ins_data_2),
    0x0001b7f8,
    0xbbf2f1ea,
    0xc0005fe0,
    (uint8_t *)bes2200_patch6_ins_data_2
};//sniffer sco start filter 


#if 0

const BTDRV_PATCH_STRUCT bes2200_ins_patch7_2 =
{
    7,
    BTDRV_PATCH_ACT,
    0,
    0x00014ebc,
    0xb802f000,
    0,
    NULL
};//sco tx silence

#else
const BTDRV_PATCH_STRUCT bes2200_ins_patch7_2 =
{
    7,
    BTDRV_PATCH_INACT,
    0,
    0x0000aca4,
    0xbf00e00e,
    0,
    NULL
};//acl flow off

#endif


#endif

const uint32_t bes2200_patch8_ins_data_2[] =
{
    0xf8832404,
    0x7cd9406d,
    0xbf000209,
    0xbe22f60d
};



const BTDRV_PATCH_STRUCT bes2200_ins_patch8_2 =
{
    8,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch8_ins_data_2),
    0x00013c70,
    0xb9d6f1f2,
    0xc0006020,
    (uint8_t *)bes2200_patch8_ins_data_2
};//sco pending 




const uint32_t bes2200_patch9_ins_data_2[] =
{
    0x20014b02,
    0x9b066018,
    0xbaa4f60d,
    0xc0006050,
    0x00000000,
};



const BTDRV_PATCH_STRUCT bes2200_ins_patch9_2 =
{
    9,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch9_ins_data_2),
    0x00013590,
    0xbd56f1f2,
    0xc0006040,
    (uint8_t *)bes2200_patch9_ins_data_2
};//sco start


const uint32_t bes2200_patch10_ins_data_2[] =
{
    0xf5fb219c,
    0x4b04f837,
    0x2b00681b,
    0xf60dd001,
    0xf614f961,
    0xbf00ba6a,
    0xc0006050
};

const BTDRV_PATCH_STRUCT bes2200_ins_patch10_2 =
{
    10,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch10_ins_data_2),
    0x0001a544,
    0xbd8cf1eb,
    0xc0006060,
    (uint8_t *)bes2200_patch10_ins_data_2

};//sco start


#if 0

const uint32_t bes2200_patch11_ins_data_2[] =
{
    0x21004a04,
    0x49026011,
    0xbf002206,
    0xbab6f60d,
    0xc0004bf8,
    0xc0006050,
};

const BTDRV_PATCH_STRUCT bes2200_ins_patch11_2 =
{
    11,
    BTDRV_PATCH_INACT,
    sizeof(bes2200_patch11_ins_data_2),
    0x00013618,
    0xbd42f1f2,
    0xc00060a0,
    (uint8_t *)bes2200_patch11_ins_data_2

};//sco start
#else
const BTDRV_PATCH_STRUCT bes2200_ins_patch11_2 =
{
    11,
    BTDRV_PATCH_ACT,
    0,
    0x00014dd4,
    0xbf00e00d,
    0,
    NULL

}; ///bd address exchange
#endif


#if 0
const uint32_t bes2200_patch12_ins_data_2[] =
{
    0x88094903,
    0xf8a34b03,
    0x669a1066,
    0xbc6ef60d,
    0xc00002d6,
    0xc0004bf0,
};

const BTDRV_PATCH_STRUCT bes2200_ins_patch12_2 =
{
    12,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch12_ins_data_2),
    0x000139a0,
    0xbb8ef1f2,
    0xc00060c0,
    (uint8_t *)bes2200_patch12_ins_data_2

};//sco start
#else

const BTDRV_PATCH_STRUCT bes2200_ins_patch12_2 =
{
    12,
    BTDRV_PATCH_ACT,
    0,
    0x0001f568,
    0xbf00e7ef,
    0,
    NULL

}; 


#endif

const uint32_t bes2200_patch13_ins_data_2[] =
{
    0x68014805,
    0xd0052900,
    0x60012100,
    0xbf002003,
    0xfff0f5fa,
    0xba3df5fe,
    0xc00060fc,
    0x00000000
};

const BTDRV_PATCH_STRUCT bes2200_ins_patch13_2 =
{
    13,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch13_ins_data_2),
    0x00004560,
    0xbdbef201,
    0xc00060e0,
    (uint8_t *)bes2200_patch13_ins_data_2

};//dbg pending



const uint32_t bes2200_patch14_ins_data_2[] =
{
    0x21012003,
    0xffdef5fa,
    0xbd702002,
};

const BTDRV_PATCH_STRUCT bes2200_ins_patch14_2 =
{
    14,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch14_ins_data_2),
    0x0000222c,
    0xbf70f203,
    0xc0006110,
    (uint8_t *)bes2200_patch14_ins_data_2

};//dbg pending




const BTDRV_PATCH_STRUCT bes2200_ins_patch15_2 =
{
    15,
    BTDRV_PATCH_ACT,
    0,
    0x000236f4,
    0xf7f14628,
    0,
    NULL
};//send done



const uint32_t bes2200_patch16_ins_data_2[] =
{
    0x41f0e92d, 
    0x0a044606,      	
    0x4b2a4a29,      	
    0xeb037c1b,      	
    0xeb021343, 	
    0xf8931303, 	
    0xeb047041, 	
    0xeb021344, 	
    0xf8931303, 	
    0xf8938041, 	
    0xb943303d, 	
    0xf60b4638,      	
    0x4605fe7b,   
    0xf60d4638,      	
    0xb281f859,   
    0x4638e023,      	
    0xfe72f60b, 
    0x46404605,      	
    0xfe6ef60b,    
    0xf0201a28,      	
    0x46384578, 	
    0xf84af60d,    
    0x46404607,      	
    0xf846f60d,    
    0xb2801a38,      	
    0xf411b281,      	
    0xd0034f00, 	
    0x2171f200,  	
    0x3501b289,      	
    0xf240b20a,      	
    0x429a2371, 	
    0xf2a1dd03,      	
    0xb2892171, 	
    0xeb043d01,      	
    0x4b091244, 	
    0x1302eb03,  	
    0x462a4620,      	
    0x303df893,  	
    0xfbecf622,    
    0xf60f4630,      	
    0x4630fdd3,   
    0xf5fa219e,      	
    0xe8bdff81,   
    0xbf0081f0, 	
    0xc0004cf4, 	 
    0xc0004bf0, 	 
};

const BTDRV_PATCH_STRUCT bes2200_ins_patch16_2 =
{
    16,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch16_ins_data_2),
    0x00014a18,
    0xbb86f1f1,
    0xc0006128,
    (uint8_t *)bes2200_patch16_ins_data_2

};//send done



const BTDRV_PATCH_STRUCT bes2200_ins_patch17_2 =
{
    17,
    BTDRV_PATCH_ACT,
    0,
    0x0001ecb0,
    0x21054628,
    0,
    NULL

};  //MAX SLOT



const uint32_t bes2200_patch18_ins_data_2[] =
{
    0x02097cd9,
    0x20004b02,
    0xf60d6698,
    0xbf00bd1b,
    0xc0004bf0
};

const BTDRV_PATCH_STRUCT bes2200_ins_patch18_2 =
{
    18,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch18_ins_data_2),
    0x00013C30,
    0xbadef1f2,
    0xc00061F0,
    (uint8_t *)bes2200_patch18_ins_data_2

};//dbg pending

const BTDRV_PATCH_STRUCT bes2200_ins_patch19_2 =
{
    19,
    BTDRV_PATCH_ACT,
    0,
    0x000149d4,
    0xbf00e001,
    0,
    NULL

};  //ignore force rx error


#if 0
const uint32_t bes2200_patch20_ins_data_2[] =
{
    0xf60d4630,
    0x4b02f80b,
    0x0346eb03,
    0xb99ef60e,
    0xc0003af0
};

const BTDRV_PATCH_STRUCT bes2200_ins_patch20_2 =
{
    20,
    BTDRV_PATCH_INACT,
    sizeof(bes2200_patch20_ins_data_2),
    0x00014558,
    0xbe5af1f1,
    0xc0006210,
    (uint8_t *)bes2200_patch20_ins_data_2

};//force rx error
#elif 0

const uint32_t bes2200_patch20_ins_data_2[] =
{
    0x01604e06,
    0xeb061903,
    0x22001303,
    0x2040f883,
    0xf0400220,
    0xbf000039,
    0xbc87f60e,
    0xc0004cf4
};

const BTDRV_PATCH_STRUCT bes2200_ins_patch20_2 =
{
    20,
    BTDRV_PATCH_INACT,
    sizeof(bes2200_patch20_ins_data_2),
    0x00014b34,
    0xbb6cf1f1,
    0xc0006210,
    (uint8_t *)bes2200_patch20_ins_data_2

};//reason=0
#else

const BTDRV_PATCH_STRUCT bes2200_ins_patch20_2 =
{
    20,
    BTDRV_PATCH_ACT,
    0,
    0x00007f44,
    0xe00dd104,
    0,
    NULL

}; //page in esco
#endif



const BTDRV_PATCH_STRUCT bes2200_ins_patch21_2 =
{
    21,
    BTDRV_PATCH_ACT,
    0,
    0x0002c3b4,
    0x60302004,
    0,
    NULL

};  //esco w

#if 0
const uint32_t bes2200_patch22_ins_data_2[] =
{
    0x68094901,
    0xbd9af60d,
    0xc000623c,
    0x00000001,
};

const BTDRV_PATCH_STRUCT bes2200_ins_patch22_2 =
{
    22,
    BTDRV_PATCH_INACT,
    sizeof(bes2200_patch22_ins_data_2),
    0x00013d68,
    0xba62f1f2,
    0xc0006230,
    (uint8_t *)bes2200_patch22_ins_data_2

};//start sco slave
#else
const BTDRV_PATCH_STRUCT bes2200_ins_patch22_2 =
{
    22,
    BTDRV_PATCH_ACT,
    0,
    0x00023174,
    0x508B011B,
    0,
    NULL

}; //detch timeout
#endif

#if 0

const uint32_t bes2200_patch23_ins_data_2[] =
{
    0x68094909,
    0xd00729ff,
    0xd10542a9,
    0xf803214f,
    0x42831b01,
    0xe005d1fa,
    0x1f01f812,
    0x1b01f803,
    0xd1f94283,
    0xbcd8f606,
    0xc000626c,
    0x000000ff
};

const BTDRV_PATCH_STRUCT bes2200_ins_patch23_2 =
{
    23,
    BTDRV_PATCH_INACT,
    sizeof(bes2200_patch23_ins_data_2),
    0x0000cc0c,
    0xbb18f1f9,
    0xc0006240,
    (uint8_t *)bes2200_patch23_ins_data_2

};//fx 2480

const uint32_t bes2200_patch23_ins_data_2[] =
{
    0x601c4b01,
    0xbf00e7fe,
    0xc000623c,
    0xabcd0000,
};

const BTDRV_PATCH_STRUCT bes2200_ins_patch23_2 =
{
    23,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch23_ins_data_2),
    0x00004474,
    0xbedcf201,
    0xc0006230,
    (uint8_t *)bes2200_patch23_ins_data_2

};//platform reset,dump error

#else

const BTDRV_PATCH_STRUCT bes2200_ins_patch23_2 =
{
    23,
    BTDRV_PATCH_ACT,
    0,
    0x0001349c,
    0xbdf88360,
    0,
    NULL,

};//kill redundant CONNECT_UPDATE_TO timer(can stop)

#endif

#if 0
const uint32_t bes2200_patch24_ins_data_2[] =
{
    0x6809490c,
    0xd00c29ff,
    0xd10a42a9,
    0x4a084b07,
    0x68516810,
    0x60596018,
    0x811a8912,
    0xf6052000,
    0xbf00fe13,
    0x01694b05,
    0xbd9ef60f,
    0xc0004067,
    0xc0004071,
    0xc00062a8,
    0x000000ff,
    0xc0004cf4
};


const BTDRV_PATCH_STRUCT bes2200_ins_patch24_2 =
{
    24,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch24_ins_data_2),
    0x00015dd4,
    0xba4cf1f0,
//    0x01694b21,
    0xc0006270,
    (uint8_t *)bes2200_patch24_ins_data_2

};//afh
#else
const uint32_t bes2200_patch24_ins_data_2[] =
{
0x4b14b43b,      
0x4298681b,      
0x4b13d11e,      
0x2bff681b,      
0x4a12d01a,      
0xf8904410,      
0xeb000169, 
0xeb020180, 
0x44130141, 
0x3169f893,  
0x0383eb03,  
0x0343eb02,  
0x5f5bf853,  
0xf8c1685a,      
0xf8c1505b, 
0x891b205f, 
0x3063f8a1,  
0xfe00f605,
0xf606bc3b,      
0xf60ffbe5,
0xbf00bd97,
0xc00062d0, 	
0xc00062d4, 	
0xc000400c, 	
0x000000ff,  
0x000000ff,
};


const BTDRV_PATCH_STRUCT bes2200_ins_patch24_2 =
{
    24,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch24_ins_data_2),
    0x00015dec,
    0xba40f1f0,
    0xc0006270,
    (uint8_t *)bes2200_patch24_ins_data_2

};//afh

#endif



#if 0
const BTDRV_PATCH_STRUCT bes2200_ins_patch25_2 =
{
    25,
    BTDRV_PATCH_ACT,
    0,
    0x0001ebac,
    0xeb04bf00,
    0,
    NULL

};////max slot
#else

const BTDRV_PATCH_STRUCT bes2200_ins_patch25_2 =
{
    25,
    BTDRV_PATCH_ACT,
    0,
    0x0001eb84,
    0x4620bf00,
    0,
    NULL

};////max slot
#endif

#if 0
const BTDRV_PATCH_STRUCT bes2200_ins_patch26_2 =
{
    26,
    BTDRV_PATCH_INACT,
    0,
    0x00007aac,
    0x2bfe9802,
    0,
    NULL

};///2slave
#else
const BTDRV_PATCH_STRUCT bes2200_ins_patch26_2 =
{
    26,
    BTDRV_PATCH_ACT,
    0,
    0x000121e4,
    0xf0331c72,
    0,
    NULL

};///lmp buff
#endif


#if 0
const uint32_t bes2200_patch27_ins_data_2[] =
{
    0xfaf0f5fa,
    0xb8b4f619,
};

const BTDRV_PATCH_STRUCT bes2200_ins_patch27_2 =
{
    27,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch27_ins_data_2),
    0x0001f3f8,
    0xbf62f1e6,
    0xc00062c0,
    (uint8_t *)bes2200_patch27_ins_data_2

};
#elif 0

const uint32_t bes2200_patch27_ins_data_2[] =
{
    0xfae0f5fa,
    0xb8a4f619,
};

const BTDRV_PATCH_STRUCT bes2200_ins_patch27_2 =
{
    27,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch27_ins_data_2),
    0x0001f3f8,
    0xbf72f1e6,
    0xc00062e0,
    (uint8_t *)bes2200_patch27_ins_data_2

};//unsniff

#elif 0

const BTDRV_PATCH_STRUCT bes2200_ins_patch27_2 =
{
    27,
    BTDRV_PATCH_INACT,
    0,
    0x0001f3d4,
    0xbf00e012,
    0,
    NULL

};///unsniff accept all
#elif 0
const uint32_t bes2200_patch27_ins_data_2[] =
{
    0xd0012800,
    0x60184b02,
    0xf5fa4628,
    0xbd70faeb,
    0xc0006484,
    0x00000000
};

const BTDRV_PATCH_STRUCT bes2200_ins_patch27_2 =
{
    27,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch27_ins_data_2),
    0x00027d1c,
    0xbba8f1de,
    0xc0006470,
    (uint8_t *)bes2200_patch27_ins_data_2

};//unsniff
#else
const uint32_t bes2200_patch27_ins_data_2[] =
{
    0xd0032b00,
    0x21054630,
    0xfe2cf5fa,
    0xb928f619,
};

const BTDRV_PATCH_STRUCT bes2200_ins_patch27_2 =
{
    27,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch27_ins_data_2),
    0x0001f6c4,
    0xbed4f1e6,
    0xc0006470,
    (uint8_t *)bes2200_patch27_ins_data_2

};//role switch



#endif

#if 0
////this patch's address should move to bigger address
const uint32_t bes2200_patch28_ins_data_2[] =
{
    0x68094910,
    0xd01b29ff,
    0xf0014640,
    0x4290027f,
    0xf001d116,
    0x2a000280,
    0x9902d009,
    0xf4428c8a,
    0x848a4280,
    0xf4428cca,
    0x84ca4280,
    0x9902e008,
    0xf4228c8a,
    0x848a4280,
    0xf4228cca,
    0x84ca4280,
    0xb963f600,
    0xc0006318,
    0x000000ff,
};

const BTDRV_PATCH_STRUCT bes2200_ins_patch28_2 =
{
    28,
    BTDRV_PATCH_INACT,
    0,
    0x000065d0,
    0xbe7ef1ff,
    0xc00062d0,
    (uint8_t *)bes2200_patch28_ins_data_2

};

#elif 0

const uint32_t bes2200_patch28_ins_data_2[] =
{
    0xf8c34b21,
    0x605ce000,
    0x60de609d,
    0xbf00611f,
    0x61584640,
    0x61984648,
    0x61d84650,
    0x62184658,
    0x62584660,
    0x629a9a00,
    0x62da9a01,
    0x631a9a02,
    0x635a9a03,
    0x639a9a04,
    0x63da9a05, 
    0x641a9a06,
    0x645a9a07,
    0x649a9a08,
    0x64da9a09,
    0x651a9a0a,
    0x655a9a0b,
    0x659a9a0c,
    0x65da9a0d, 
    0x661a9a0e,
    0x665a9a0f,
    0x669a9a10, 
    0x68124a04,
    0x4a0466da,
    0x671a6812,
    0x68124a03,
    0xbf00675a,
    0xe000ed28,
    0xe000ed38,
    0xe000ed2c,
    0xc00064cc,
    0x00000001,
    0x00000002,
    0x00000003,
    0x00000004,
    0x00000005,
    0x00000006,
    0x00000007,
    0x00000008,
    0x00000009,
    0x00000010,
    0x00000011,
    0x00000012,
    0x00000013,
    0x00000014,
    0x00000015,
    0x00000016,
    0x00000017,
    0x00000018,
    0x00000019,
    0x00000020,
    0x00000021,
    0x00000022,
    0x00000023,
    0x00000024,
    0x00000025,
    0x00000026,
    0x00000027,
    0x00000028,
    0x00000029,
    0x00000030, 
};//dump REG val when bus fault

const BTDRV_PATCH_STRUCT bes2200_ins_patch28_2 =
{
    28,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch28_ins_data_2),
    0x00004400,
    0xb81ef202,
    0xc0006440,
    (uint8_t *)bes2200_patch28_ins_data_2

};

#elif 0


const uint32_t bes2200_patch28_ins_data_2[] =
{
	0xd10d428c,
	0x4b07b409,
	0x4658681b,
	0x28031ac0,
	0xf110dc02,
	0xda020f03,
	0xf601bc09,
	0xbc09bbef,
	0xbbf5f601,
	0xc0006468,
	0x00000000,	
};


const BTDRV_PATCH_STRUCT bes2200_ins_patch28_2 =
{
    28,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch28_ins_data_2),
    0x00007c38,
    0xbc02f1fe,
    0xc0006440,
    (uint8_t *)bes2200_patch28_ins_data_2

};
#else

const uint32_t bes2200_patch28_ins_data_2[] =
{
	0x2a188099,
	0x4b04d904,
	0x2b00681b,
	0xbf00d002,
	0xbd40f600,
	0xbd1df600,
	0xc000645c,
	0x00000000,	
};


const BTDRV_PATCH_STRUCT bes2200_ins_patch28_2 =
{
    28,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch28_ins_data_2),
    0x00006e8c,
    0xbad8f1ff,
    0xc0006440,
    (uint8_t *)bes2200_patch28_ins_data_2

};


#endif

#if 0
const uint32_t bes2200_patch29_ins_data_2[] =
{
    0x781b4b14,
    0xb410b323,
    0x681b4b13,
    0x33fff113,
    0x2301bf18,
    0xf103490f,
    0xf8510208,
    0x440b2022,
    0xeb017d59,
    0x4c0d0141,
    0x1141eb04,
    0x7e596b0c,
    0x1b13b161,
    0x4378f023,
    0xd9064298,
    0xfbb01ac0,
    0x3303f0f1,
    0x3000fb01,
    0x1cd8e000,
    0x4b04f85d,
    0xbf004770,
    0xc00048ac,
    0xc000490c,
    0xd0210190,
    0xf0234403,
    0x46184378,
    0xffcaf7ff,
    0xbf0063a0,
    0xbe9cf63c,
           
};


const BTDRV_PATCH_STRUCT bes2200_ins_patch29_2 =
{
    29,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch29_ins_data_2),
    0x000430cc,
    0xb95cf1c3,
    0xc0006328,
    (uint8_t *)bes2200_patch29_ins_data_2

};//adv




const uint32_t bes2200_patch30_ins_data_2[] =
{
    0x4378f023,
    0xf7ff4618,
    0x63a0ffbf,
    0xbf45f63c,
};


const BTDRV_PATCH_STRUCT bes2200_ins_patch30_2 =
{
    30,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch30_ins_data_2),
    0x00043234,
    0xb8b4f1c3,
    0xc00063a0,
    (uint8_t *)bes2200_patch30_ins_data_2

};//ble scan
#else
const uint32_t bes2200_patch29_ins_data_2[] =
{
    0x4b2ab5f0,
    0xb1fb781b,
    0x681b4b29,
    0x33fff113,
    0x2301bf18,
    0xf1034a25,
    0xf8520108,
    0x44131021,
    0xeb027d5a,
    0x4c230242,
    0x1242eb04,
    0x7e5a6b14,
    0x1b0bb152,
    0x4378f023,
    0xd90f4298,
    0xfbb01ac0,
    0x3303f0f2,
    0x3000fb02,
    0x491c4c1b,
    0xf04f4b1c,
    0x26ff3eff,
    0xc070f8df,
    0xe0014d1a,
    0xe7f31cd8,
    0x2f04f854,
    0xea02b1c2,
    0x2a80020c,
    0x680fd114,
    0x443a6a0a,
    0x1bd2681f,
    0x4278f022,
    0xd9044290,
    0x2f091a87,
    0x2601bf98,
    0x1a17e003,
    0xbf982f09,
    0x45962601,
    0x4696bf28,
    0x33603104,
    0xd1df42ab,
    0xbf082e01,
    0x0008f1ae,
    0xbf00bdf0,
    0xc00048ac,
    0xc000490c,
    0xd0210190,
    0xc0003b00,
    0xc0004820,
    0xd02101c0,
    0xd0210340,
    0x08007ff0,
    0xf0234403,
    0x46184378,
    0xff94f7ff,
    0xbf0063a0,
    0xbe78f63c,

           
};


const BTDRV_PATCH_STRUCT bes2200_ins_patch29_2 =
{
    29,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch29_ins_data_2),
    0x000430cc,
    0xb980f1c3,
    0xc0006304,
    (uint8_t *)bes2200_patch29_ins_data_2

};//adv




const uint32_t bes2200_patch30_ins_data_2[] =
{
    0x4378f023,
    0xf7ff4618,
    0x63a0ff85,
    0xbf1df63c,
};


const BTDRV_PATCH_STRUCT bes2200_ins_patch30_2 =
{
    30,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch30_ins_data_2),
    0x00043234,
    0xb8dcf1c3,
    0xc00063f0,
    (uint8_t *)bes2200_patch30_ins_data_2

};//ble scan

#endif



#if 0
const uint32_t bes2200_patch31_ins_data_2[] =
{
    0x1404eb03,
    0x21044802,
    0xffe6f5fc,
    0xb830f620,
    0xc00063d4,
    0x30313233
};


const BTDRV_PATCH_STRUCT bes2200_ins_patch31_2 =
{
    31,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch31_ins_data_2),
    0x0002642c,
    0xbfc8f1df,
    0xc00063c0,
    (uint8_t *)bes2200_patch31_ins_data_2

};
#elif 0

const uint32_t bes2200_patch31_ins_data_2[] =
{
    0x46594806,
    0xf94af63e,
    0xd0052800,
    0xf6214620,
    0xbf00f8dd,
    0xb943f605,
    0xb97af605,
    0xc00063e0,
    0x00000000
};


const BTDRV_PATCH_STRUCT bes2200_ins_patch31_2 =
{
    31,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch31_ins_data_2),
    0x0000b658,
    0xbeb2f1fa,
    0xc00063c0,
    (uint8_t *)bes2200_patch31_ins_data_2

};//acl tx check
#else
const uint32_t bes2200_patch31_ins_data_2[] =
{
    0x46594806,
    0xf922f63e,
    0xd0052800,
    0xf6214620,
    0xbf00f8b5,
    0xb91bf605,
    0xb952f605,
    0xc0006430,
    0x00000000
};


const BTDRV_PATCH_STRUCT bes2200_ins_patch31_2 =
{
    31,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch31_ins_data_2),
    0x0000b658,
    0xbedaf1fa,
    0xc0006410,
    (uint8_t *)bes2200_patch31_ins_data_2

};//acl tx check

#endif




static const uint32_t bes2200_ins_patch_config_2[] =
{
    32,
    (uint32_t)&bes2200_ins_patch0_2,
    (uint32_t)&bes2200_ins_patch1_2,
    (uint32_t)&bes2200_ins_patch2_2,
    (uint32_t)&bes2200_ins_patch3_2,
    (uint32_t)&bes2200_ins_patch4_2,
    (uint32_t)&bes2200_ins_patch5_2,
    (uint32_t)&bes2200_ins_patch6_2,
    (uint32_t)&bes2200_ins_patch7_2,
    (uint32_t)&bes2200_ins_patch8_2,
    (uint32_t)&bes2200_ins_patch9_2,
    (uint32_t)&bes2200_ins_patch10_2,
    (uint32_t)&bes2200_ins_patch11_2,
    (uint32_t)&bes2200_ins_patch12_2,
    (uint32_t)&bes2200_ins_patch13_2,
    (uint32_t)&bes2200_ins_patch14_2,
    (uint32_t)&bes2200_ins_patch15_2,
    (uint32_t)&bes2200_ins_patch16_2,
    (uint32_t)&bes2200_ins_patch17_2,
    (uint32_t)&bes2200_ins_patch18_2,
    (uint32_t)&bes2200_ins_patch19_2,
    (uint32_t)&bes2200_ins_patch20_2,
    (uint32_t)&bes2200_ins_patch21_2,
    (uint32_t)&bes2200_ins_patch22_2,
    (uint32_t)&bes2200_ins_patch23_2,
    (uint32_t)&bes2200_ins_patch24_2,
    (uint32_t)&bes2200_ins_patch25_2,
    (uint32_t)&bes2200_ins_patch26_2,
    (uint32_t)&bes2200_ins_patch27_2,
    (uint32_t)&bes2200_ins_patch28_2,
    (uint32_t)&bes2200_ins_patch29_2,
    (uint32_t)&bes2200_ins_patch30_2,
    (uint32_t)&bes2200_ins_patch31_2,
    
    
    
};


void btdrv_ins_patch_write(const BTDRV_PATCH_STRUCT *ins_patch_p)
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
    const BTDRV_PATCH_STRUCT *ins_patch_p;


    if(hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_2)
    {
        for(uint8_t i=0;i<bes2200_ins_patch_config_2[0];i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)bes2200_ins_patch_config_2[i+1];
            if(ins_patch_p->patch_state ==BTDRV_PATCH_ACT)
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)bes2200_ins_patch_config_2[i+1]);
        }    
    }
    else if(hal_get_chip_metal_id() <= HAL_CHIP_METAL_ID_1)
    {
        for(uint8_t i=0;i<bes2200_ins_patch_config[0];i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)bes2200_ins_patch_config[i+1];
            if(ins_patch_p->patch_state ==BTDRV_PATCH_ACT)
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)bes2200_ins_patch_config[i+1]);
        }
    }
}




///////////////////data  patch ..////////////////////////////////////



static const uint32_t bes2200_dpatch0_data[] =
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

static const BTDRV_PATCH_STRUCT bes2200_data_patch0 =
{
    0,
    BTDRV_PATCH_INACT,
    sizeof(bes2200_dpatch0_data),
    0x00004da8,
    0xc0005c30,
    0xc0005c30,
    (uint8_t *)&bes2200_dpatch0_data
};


static const uint32_t bes2200_data_patch_config[] =
{
    1,
    (uint32_t)&bes2200_data_patch0,

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



void btdrv_data_patch_init(void)
{
    const BTDRV_PATCH_STRUCT *data_patch_p;

    for(uint8_t i=0;i<bes2200_data_patch_config[0];i++)
    {
        data_patch_p = (BTDRV_PATCH_STRUCT *)bes2200_data_patch_config[i+1];
        if(data_patch_p->patch_state == BTDRV_PATCH_ACT)
            btdrv_data_patch_write((BTDRV_PATCH_STRUCT *)bes2200_data_patch_config[i+1]);
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



const BTDRV_PATCH_STRUCT bes2200_ins_patch18_testmode =
{
    18,
    BTDRV_PATCH_ACT,
    0,
    0x00006d78,
    0xbf00bf00,
    0,
    NULL
};//test mode ble


uint32_t ins_patch_2200_config_testmode[] =
{
    1,
    (uint32_t)&bes2200_ins_patch18_testmode,
    
};



const BTDRV_PATCH_STRUCT bes2200_ins_patch7_testmode_2 =
{
    7,
    BTDRV_PATCH_INACT,
    0,
#if 1
    0x00006f58,
    0x990220bf,
#else
    0x00006f5c,
    0xbf00bf00,
#endif
    0,
    NULL
};//test mode ble


const uint32_t bes2200_patch3_ins_data_tesmode_2[] =
{
	0xf4206820,
	0xf4200004,	
	0x60204000,
	0x4c022000,
	0x0025f884,
	0xbecaf60b,
	0xc00049c0,
};


const BTDRV_PATCH_STRUCT bes2200_ins_patch3_testmode_2 =
{
    3,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch3_ins_data_tesmode_2),
    0x00011d08,
    0xb93af1f4,
    0xc0005f80,
    (uint8_t *)bes2200_patch3_ins_data_tesmode_2
};//test mode 1



const uint32_t bes2200_patch4_ins_data_testmode_2[] =
{
    0x0304f443,
    0x4300f443,
    0x4b026013,
    0x601a2200,
    0xbe86f60c,
    0xc0005fb8,   //count
    0x000000ff,
};




const BTDRV_PATCH_STRUCT bes2200_ins_patch4_testmode_2 =
{
    4,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch4_ins_data_testmode_2),
    0x00012c58,
    0xb9a2f1f3,
    0xc0005fa0,
    (uint8_t *)bes2200_patch4_ins_data_testmode_2
};//test mode  no white



const uint32_t bes2200_patch5_ins_data_testmode_2[] =
{
    0xbf006813,
    0x0304f443,
    0x4300f443,
    0x4b026013,
    0x601a2200,
    0xbe74f60c,
    0xc0005fb8,   //count
};




const BTDRV_PATCH_STRUCT bes2200_ins_patch5_testmode_2 =
{
    5,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch5_ins_data_testmode_2),
    0x00012c08,
    0xb9daf1f3,
    0xc0005fc0,
    (uint8_t *)bes2200_patch5_ins_data_testmode_2
};//test mode  whiten


const uint32_t bes2200_patch6_ins_data_testmode_2[] =
{
    0x68194b0b,
    0xd80b2930,
    0x60193101,
    0x8d8b9909,
    0x537ff423,
    0x0338f023,
    0x5300f443,
    0xbe2df600,
    0x8d8b9909,
    0x537ff423,
    0x0338f023,
    0xbe25f600,
    0xc0005fb8
};



const BTDRV_PATCH_STRUCT bes2200_ins_patch6_testmode_2 =
{
    6,
    BTDRV_PATCH_ACT,
    sizeof(bes2200_patch6_ins_data_testmode_2),
    0x00006c50,
    0xb9c6f1ff,
    0xc0005fe0,
    (uint8_t *)bes2200_patch6_ins_data_testmode_2
};//test mode silence



const BTDRV_PATCH_STRUCT bes2200_ins_patch2_testmode_2 =
{
    2,
    BTDRV_PATCH_ACT,
    0,
    0x000417cc,
    0x80130302,
    0,
    NULL
};//test mode ble


static const uint32_t ins_patch_2200_config_testmode_2[] =
{
    6,
    (uint32_t)&bes2200_ins_patch7_testmode_2,
    (uint32_t)&bes2200_ins_patch3_testmode_2,
    (uint32_t)&bes2200_ins_patch4_testmode_2,
    (uint32_t)&bes2200_ins_patch5_testmode_2,
    (uint32_t)&bes2200_ins_patch6_testmode_2,
    (uint32_t)&bes2200_ins_patch2_testmode_2,

    
};


void btdrv_ins_patch_test_init(void)
{
    const BTDRV_PATCH_STRUCT *ins_patch_p;

    btdrv_patch_en(0);

    if(hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_2)
    {
        for(uint8_t i=0;i<ins_patch_2200_config_testmode_2[0];i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)ins_patch_2200_config_testmode_2[i+1];
            if(ins_patch_p->patch_state ==BTDRV_PATCH_ACT)
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)ins_patch_2200_config_testmode_2[i+1]);
        }            
    }
    else if(hal_get_chip_metal_id() <= HAL_CHIP_METAL_ID_1)
    {
        for(uint8_t i=0;i<ins_patch_2200_config_testmode[0];i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)ins_patch_2200_config_testmode[i+1];
            if(ins_patch_p->patch_state ==BTDRV_PATCH_ACT)
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)ins_patch_2200_config_testmode[i+1]);
        }    
    }
    btdrv_patch_en(1);
}

void btdrv_dynamic_patch_moble_disconnect_reason_hacker(uint16_t hciHandle)
{
    uint32_t disconnect_reason_address = 0xC0004D34 +(hciHandle-0x80)*0x210;
    TRACE("%s:hdl=%x reason=%d",__func__,hciHandle,*(uint8_t *)(disconnect_reason_address));
    *(uint8_t *)(disconnect_reason_address) = 0x0;
    TRACE("After op,reason=%d",*(uint8_t *)(disconnect_reason_address));
}

void btdrv_dynamic_patch_sco_status_clear(void)
{
    ASSERT(bes2200_patch9_ins_data_2[3] == 0xc0006050, "FUCK PATCH!!!");
    TRACE("%s =%d",__func__,*(uint32_t *)0xc0006050);
    *(uint32_t *)0xc0006050 = 0x0;//sco_status Variable in patch
    TRACE("After op, sco_status_in_patch =%d",*(uint32_t *)0xc0006050);
}

