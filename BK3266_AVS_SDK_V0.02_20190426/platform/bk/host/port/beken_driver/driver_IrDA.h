#ifndef _DRIVER_IRDA_H_

#define	IR_UserCode 	0xfd02 /*不同遥控器值不同*/
#define	IR_CODE_SIZE 	18

#define _DRIVER_IRDA_H_

#ifdef CONFIG_IRDA
void IrDA_init(void);
DRAM_CODE void IrDA_isr(void);
uint32 app_get_IRKey();
/*顺序不能改变，根据IR_MAP定义*/
enum
{
	IR_KEY_PLAY=0,
	IR_KEY_VOL_MINUS,
	IR_KEY_VOL_PLUS,
	IR_KEY_PREV,
	IR_KEY_NEXT,
	IR_KEY_MODE,
	IR_KEY_CH_MINUS,
	IR_KEY_CH_PLUS,
	IR_KEY_NUM0,
	IR_KEY_NUM1,
	IR_KEY_NUM2,
	IR_KEY_NUM3,
	IR_KEY_NUM4,
	IR_KEY_NUM5,
	IR_KEY_NUM6,
	IR_KEY_NUM7,
	IR_KEY_NUM8,
	IR_KEY_NUM9,
	IR_KEY_END
};
#endif
#endif
