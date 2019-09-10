#include "include.h"

#if EQ_MODE_EN

const eq_param music_eq_tbl[MUSIC_EQ_TBL_LEN] = {
    {&RES_BUF_EQ_NORMAL_EQ,     &RES_LEN_EQ_NORMAL_EQ}, //normal
    {&RES_BUF_EQ_POP_EQ,        &RES_LEN_EQ_POP_EQ},    //pop
    {&RES_BUF_EQ_ROCK_EQ,       &RES_LEN_EQ_ROCK_EQ},   //rock
    {&RES_BUF_EQ_JAZZ_EQ,       &RES_LEN_EQ_JAZZ_EQ},   //jazz
    {&RES_BUF_EQ_CLASSIC_EQ,    &RES_LEN_EQ_CLASSIC_EQ},//classic
    {&RES_BUF_EQ_COUNTRY_EQ,    &RES_LEN_EQ_COUNTRY_EQ},//country
};

#else

const eq_param music_eq_tbl[MUSIC_EQ_TBL_LEN] = {
    {&RES_BUF_EQ_NORMAL_EQ,     &RES_LEN_EQ_NORMAL_EQ}, //normal
};

#endif

