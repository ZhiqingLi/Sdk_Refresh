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

///////////////////ins  patch ..////////////////////////////////////



const uint32_t bes2300_patch0_ins_data[] =
{
    0x68134a04,
    0x3300f423,
    0x46206013,
    0xfa48f628,
    0xbcbdf618,
    0xd02200a4
};


const BTDRV_PATCH_STRUCT bes2300_ins_patch0 =
{
    0,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_patch0_ins_data),
    0x0001ed88,
    0xbb3af1e7,
    0xc0006400,
    (uint8_t *)bes2300_patch0_ins_data
};/////test mode


static const uint32_t best2300_ins_patch_config[] =
{
    1,
    (uint32_t)&bes2300_ins_patch0,        
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
    const BTDRV_PATCH_STRUCT *ins_patch_p;
    if(hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_0 || hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_1)
    {
        for(uint8_t i=0;i<best2300_ins_patch_config[0];i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)best2300_ins_patch_config[i+1];
            if(ins_patch_p->patch_state ==BTDRV_PATCH_ACT)
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)best2300_ins_patch_config[i+1]);
        }
    }
}

///////////////////data  patch ..////////////////////////////////////


static const uint32_t best2300_data_patch_config[] =
{
    0,
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

    for(uint8_t i=0;i<best2300_data_patch_config[0];i++)
    {
        data_patch_p = (BTDRV_PATCH_STRUCT *)best2300_data_patch_config[i+1];
        if(data_patch_p->patch_state == BTDRV_PATCH_ACT)
            btdrv_data_patch_write((BTDRV_PATCH_STRUCT *)best2300_data_patch_config[i+1]);
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


static const uint32_t ins_patch_2300_config_testmode[] =
{
    0,
};


void btdrv_ins_patch_test_init(void)
{
    return;
    
    const BTDRV_PATCH_STRUCT *ins_patch_p;

    btdrv_patch_en(0);

    for(uint8_t i=0;i<ins_patch_2300_config_testmode[0];i++)
    {
        ins_patch_p = (BTDRV_PATCH_STRUCT *)ins_patch_2300_config_testmode[i+1];
        if(ins_patch_p->patch_state ==BTDRV_PATCH_ACT)
            btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)ins_patch_2300_config_testmode[i+1]);
    }    

    btdrv_patch_en(1);
}




////////////////////////////////bt 5.0 /////////////////////////////////

const BTDRV_PATCH_STRUCT bes2300_50_ins_patch0 =
{
    0,
    BTDRV_PATCH_ACT,
    0,
    0x00056138,
    0xe0042300,
    0,
    NULL
};//disable reslove to



const uint32_t bes2300_50_patch1_ins_data[] =
{
    0x0f02f013,
    0xf008bf12,
    0x79330303,
    0x0303ea08,
    0x300bf889,
    0xf01378b3,
    0xbf120f01,
    0x0303f008,
    0xea0878f3,
    0xf8890303,
    0xbf00300a,
    0xbf6ff659,
};


const BTDRV_PATCH_STRUCT bes2300_50_ins_patch1 =
{
    1,
    BTDRV_PATCH_INACT,
    sizeof(bes2300_50_patch1_ins_data),
    0x0005f6e8,
    0xb88af1a6,
    0xc0005800,
    (uint8_t *)bes2300_50_patch1_ins_data
};


const uint32_t bes2300_50_patch2_ins_data[] =
{
    0xf01378cb,
    0xd1080f04,
    0xf013790b,
    0xd1040f04,
    0xf003788b,
    0xf6590202,
    0x2211bf16,
    0xbf99f659,
};


const BTDRV_PATCH_STRUCT bes2300_50_ins_patch2 =
{
    2,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_50_patch2_ins_data),
    0x0005f680,
    0xb8def1a6,
    0xc0005840,
    (uint8_t *)bes2300_50_patch2_ins_data
};



const BTDRV_PATCH_STRUCT bes2300_50_ins_patch3 =
{
    3,
    BTDRV_PATCH_INACT,
    0,
    0x000587e8,
    0xbf00bf00,
    0,
    NULL
};


const uint32_t bes2300_50_patch4_ins_data[] =
{
    0x23014A03,
    0x223E8013,
    0xBF002314,
    0xB81EF653,
    0xC0005884,
    0x00000000,
};


const BTDRV_PATCH_STRUCT bes2300_50_ins_patch4 =
{
    4,
    BTDRV_PATCH_ACT,
    sizeof(bes2300_50_patch4_ins_data),
    0x000588B8,
    0xbFDAf1aC,
    0xc0005870,
    (uint8_t *)bes2300_50_patch4_ins_data
};

const BTDRV_PATCH_STRUCT bes2300_50_ins_patch5 =
{
    5,
    BTDRV_PATCH_ACT,
    0,
    0x0006b410,
    0xbf00e003,
    0,
    NULL
};


static const uint32_t best2300_50_ins_patch_config[] =
{
    6,
    (uint32_t)&bes2300_50_ins_patch0,
    (uint32_t)&bes2300_50_ins_patch1,
    (uint32_t)&bes2300_50_ins_patch2,
    (uint32_t)&bes2300_50_ins_patch3,
    (uint32_t)&bes2300_50_ins_patch4,
    (uint32_t)&bes2300_50_ins_patch5,

    
    
    
};

void btdrv_ins_patch_init_50(void)
{
    const BTDRV_PATCH_STRUCT *ins_patch_p;
    if(hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_0 || hal_get_chip_metal_id() == HAL_CHIP_METAL_ID_1)
    {
        for(uint8_t i=0;i<best2300_50_ins_patch_config[0];i++)
        {
            ins_patch_p = (BTDRV_PATCH_STRUCT *)best2300_50_ins_patch_config[i+1];
            if(ins_patch_p->patch_state ==BTDRV_PATCH_ACT)
                btdrv_ins_patch_write((BTDRV_PATCH_STRUCT *)best2300_50_ins_patch_config[i+1]);
        }
    }
}

