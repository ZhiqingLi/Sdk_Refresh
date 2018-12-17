#include  "manager.h"

void welcome_delay_ms(uint8 dly_ms)
{
    uint8 i, j;
    while (dly_ms != 0)
    {
        for (i = 0; i < 8; i++)
        {
            for (j = 0; j < 255; j++)
            {
                ; //do nothing
            }
        }
        dly_ms--;
    }
}

void ft_test_mode(void)
{
    testval_t test_mode = {0};

    sys_vm_read(&test_mode,VM_API_FLAG,sizeof(testval_t));
    if(test_mode.api_flag == 0x01)
    {
        act_writel((act_readl(VD15_DCDC_CTL)&0xffffe7ff),VD15_DCDC_CTL);
        libc_print("FT MODE",0,0);
        test_mode.api_flag = 0x00;
        sys_vm_write(&test_mode,VM_API_FLAG,sizeof(testval_t));
        
        //FT mode        
        act_writel((act_readl(AD_Select1) & (~AD_Select1_GPIOA22_MASK)), AD_Select1);
        welcome_delay_ms(20);
        welcome_delay_ms(10);
        act_writel(act_readl(0xc01b0000) | 0x00000014, 0xc01b0000); //ft test mode
        welcome_delay_ms(20);  
        act_writel((act_readl(WD_CTL) & (~0x10)), WD_CTL);
        while(1)
        {
            ;//nothing 
        }
    }

}

