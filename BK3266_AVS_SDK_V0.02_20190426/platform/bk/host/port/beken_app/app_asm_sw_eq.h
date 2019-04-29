#ifndef _TEST_SW_EQ_H_
#define _TEST_SW_EQ_H_

/*
gain = pow(X/20)*16384
eg:func_sw_eq((short *)app_sbc.sbc_output,(short *)app_sbc.sbc_output,512/4,7,0);//5181=-10dB 4618=-11dB

*/
//void func_sw_eq(short *input,short *output,int len,int eq_cnt,uint32 index);//len: 1=L+R=4bytes
/*
//updata coef
typedef struct _aud_pre_equ_para_s
{
    int    a[2];
    int    b[3];
}__PACKED_POST__ aud_pre_equ_para_t;
extern SAMPLE_ALIGN aud_pre_equ_para_t tbl_eq_coef[];
//keyword: tbl_eq_coef
*/



#endif

