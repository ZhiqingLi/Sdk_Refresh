#include "driver_beken_includes.h"
#include "app_beken_includes.h"
#include "beken_external.h"
#ifdef CONFIG_IRDA
#define IR_MAP                                 \
{                                               \
    0x0, /* KEY_play   */                        \
    0x5, /* KEY_VOL-   */                           \
    0x6,  /* KEY_VOL+   */                           \
    0x9, /* KEY_PREV   */                           \
    0xa,  /* KEY_NEXT   */                           \
    0x4,  /* KEY_MODE   */                           \
	0x1,  /* KEY_CH-   */                           \
    0x2, /* KEY_CH+   */                           \
	0x8,  /* KEY_0   */                           \
	0xc, /* KEY_1   */                           \
    0xd,  /* KEY_2   */                        \
    0xe, /* KEY_3   */                        \
    0x10,  /* KEY_4   */                         \
    0x11,  /* KEY_5   */                        \
    0x12, /* KEY_6   */                        \
    0x14,  /* KEY_7   */                          \
    0x15,  /* KEY_8   */                        \
    0x16  /* KEY_9   */                        \
}
static uint16 IR_Code[IR_CODE_SIZE] = IR_MAP;
static uint32 Recv_IR_Code = 0xffffffff;

void IrDA_init(void)
{
    unsigned int oldmask = get_spr(SPR_VICMR(0));//read old spr_vicmr
    BK3000_PMU_PERI_PWDS &= ~bit_PMU_IRDA_PWD ;
    BK3000_GPIO_6_CONFIG|= 0x70; // Set GPIO as 2nd function
    (*(volatile unsigned long *)(BK3000_GPIO_BASE_ADDR+0x20*4)) |= (0x1<<6); //gpio_pconf
    BK3000_IRDA_CTRL = (0x3921<<sft_CLK_DIV)|(0<<sft_IRDA_POLARIT)|(1<<sft_IRDA_NEC_EN);
    BK3000_IRDA_INT_MASK = IRDA_END_INT_EN|IRDA_RIGHT_INT_EN|IRDA_REPEAT_INT_EN;	
    set_spr(SPR_VICMR(0), 0x00);  //mask all/low priority interrupt.
    oldmask |= (1<<	VIC_IRDA_ISR_INDEX);
    set_spr(SPR_VICMR(0), oldmask); //recover the old spr_vicmr.
}

uint32 app_get_IRKey()
{
    uint32 tmp,index;
    tmp=Recv_IR_Code;
    /*user code check */
    if((tmp&0xffff)!=IR_UserCode)
        return 0xff;
    /*data check*/	
    if( (tmp&0xff000000) & ((tmp&0xff0000)<<8) )
        return 0xff;
    tmp=(tmp>>16)&0xff;

    for(index=0;index<IR_CODE_SIZE;index++)
    {
        if(tmp==IR_Code[index])
            break;
    }

    if(index==IR_CODE_SIZE)
        index=0xff;

    os_printf("IR_Code is : %x, IR KEY is:%x\r\n",Recv_IR_Code,index);
    return index;
}

DRAM_CODE void IrDA_isr(void)
{
    //unsigned int oldmask = get_spr(SPR_VICMR(0));
    //set_spr(SPR_VICMR(0), oldmask & 0x0);

    static uint16 ValidFlag=0;
    //os_printf("IrDA_isr\r\n");

    if (BK3000_IRDA_INT & IRDA_END_INT_FLAG)
    {
    	if (ValidFlag)
    	{
            ValidFlag = 0x0;
            Recv_IR_Code=BK3000_IRDA_DATA;

            msg_put(MSG_IRDA_RX);
        }
        else
            Recv_IR_Code=0xffffffff;//invalid
			
        BK3000_IRDA_INT |= IRDA_END_INT_FLAG;
    }
    
    if (BK3000_IRDA_INT & IRDA_RIGHT_INT_FLAG)
    {
  	 ValidFlag = 0x1 ;
        BK3000_IRDA_INT |= IRDA_RIGHT_INT_FLAG;
    }
    
    if (BK3000_IRDA_INT & IRDA_REPEAT_INT_FLAG)
    {
        BK3000_IRDA_INT |= IRDA_REPEAT_INT_FLAG;
        msg_put(MSG_IRDA_RX);
    }

   //set_spr(SPR_VICMR(0), oldmask);
}
#endif
// EOF
