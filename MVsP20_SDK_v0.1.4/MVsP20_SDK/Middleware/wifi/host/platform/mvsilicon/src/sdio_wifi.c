#include "rtos.h"
#include "sdio_wifi.h"
#include "delay.h"
#include "irqn.h"
#include "debug.h"
#include "sdio_port.h"
#include "dma.h"

#define  ENOMEM       12  /* Out of memory */
#define NULL 							0
#define SDIO_STATIC_FLAGS               ((uint32_t)0x000005FF)
#define SDIO_CMD0TIMEOUT                ((uint32_t)0x00002710)
#define SDIO_FIFO_Address               ((uint32_t)0x40012C80)

/* Mask for errors Card Status R1 (OCR Register) */
#define SD_OCR_ADDR_OUT_OF_RANGE        ((uint32_t)0x80000000)
#define SD_OCR_ADDR_MISALIGNED          ((uint32_t)0x40000000)
#define SD_OCR_BLOCK_LEN_ERR            ((uint32_t)0x20000000)
#define SD_OCR_ERASE_SEQ_ERR            ((uint32_t)0x10000000)
#define SD_OCR_BAD_ERASE_PARAM          ((uint32_t)0x08000000)
#define SD_OCR_WRITE_PROT_VIOLATION     ((uint32_t)0x04000000)
#define SD_OCR_LOCK_UNLOCK_FAILED       ((uint32_t)0x01000000)
#define SD_OCR_COM_CRC_FAILED           ((uint32_t)0x00800000)
#define SD_OCR_ILLEGAL_CMD              ((uint32_t)0x00400000)
#define SD_OCR_CARD_ECC_FAILED          ((uint32_t)0x00200000)
#define SD_OCR_CC_ERROR                 ((uint32_t)0x00100000)
#define SD_OCR_GENERAL_UNKNOWN_ERROR    ((uint32_t)0x00080000)
#define SD_OCR_STREAM_READ_UNDERRUN     ((uint32_t)0x00040000)
#define SD_OCR_STREAM_WRITE_OVERRUN     ((uint32_t)0x00020000)
#define SD_OCR_CID_CSD_OVERWRIETE       ((uint32_t)0x00010000)
#define SD_OCR_WP_ERASE_SKIP            ((uint32_t)0x00008000)
#define SD_OCR_CARD_ECC_DISABLED        ((uint32_t)0x00004000)
#define SD_OCR_ERASE_RESET              ((uint32_t)0x00002000)
#define SD_OCR_AKE_SEQ_ERROR            ((uint32_t)0x00000008)
#define SD_OCR_ERRORBITS                ((uint32_t)0xFDFFE008)

/* Masks for R6 Response */
#define SD_R6_GENERAL_UNKNOWN_ERROR     ((uint32_t)0x00002000)
#define SD_R6_ILLEGAL_CMD               ((uint32_t)0x00004000)
#define SD_R6_COM_CRC_FAILED            ((uint32_t)0x00008000)

#define SD_VOLTAGE_WINDOW_SD            ((uint32_t)0x80100000)
#define SD_HIGH_CAPACITY                ((uint32_t)0x40000000)
#define SD_STD_CAPACITY                 ((uint32_t)0x00000000)
#define SD_CHECK_PATTERN                ((uint32_t)0x000001AA)

#define SD_MAX_VOLT_TRIAL               ((uint32_t)0x0000FFFF)
#define SD_ALLZERO                      ((uint32_t)0x00000000)

#define SD_WIDE_BUS_SUPPORT             ((uint32_t)0x00040000)
#define SD_SINGLE_BUS_SUPPORT           ((uint32_t)0x00010000)
#define SD_CARD_LOCKED                  ((uint32_t)0x02000000)
#define SD_CARD_PROGRAMMING             ((uint32_t)0x00000007)
#define SD_CARD_RECEIVING               ((uint32_t)0x00000006)
#define SD_DATATIMEOUT                  ((uint32_t)0x000FFFFF)
#define SD_0TO7BITS                     ((uint32_t)0x000000FF)
#define SD_8TO15BITS                    ((uint32_t)0x0000FF00)
#define SD_16TO23BITS                   ((uint32_t)0x00FF0000)
#define SD_24TO31BITS                   ((uint32_t)0xFF000000)
#define SD_MAX_DATA_LENGTH              ((uint32_t)0x01FFFFFF)

#define SD_HALFFIFO                     ((uint32_t)0x00000008)
#define SD_HALFFIFOBYTES                ((uint32_t)0x00000020)

/* Command Class Supported */
#define SD_CCCC_LOCK_UNLOCK             ((uint32_t)0x00000080)
#define SD_CCCC_WRITE_PROT              ((uint32_t)0x00000040)
#define SD_CCCC_ERASE                   ((uint32_t)0x00000020)

/* Following commands are SD Card Specific commands.
   SDIO_APP_CMD should be sent before sending these commands. */
#define SDIO_SEND_IF_COND               ((uint32_t)0x00000008)

#define SDIO_INIT_CLK_DIV               ((uint8_t)0x80)

#if 1
#define SDIO_TRANSFER_CLK_DIV		    ((uint8_t)0x0)   //45m/(x+2)
#define ENABLE_DATA_WIDE 				0
#else
#define SDIO_TRANSFER_CLK_DIV		    ((uint8_t)0x6)   //45m/(x+2)
#define ENABLE_DATA_WIDE 	1			//4bit模式下时钟不能太高，否则会出错，CLK_DIV = 6是速率跟1bit模式差不多
#endif


struct sdio_cccr cccr;
//struct sdio_func func;

//static wait_event_t sd_waitq;
static OsMutex sd_mutex;
typedef unsigned short              u16;
typedef unsigned int                u32;
typedef unsigned char                u8;
static uint8_t convert_from_bytes_to_power_of_two(uint16_t NumberOfBytes);
int mmc_send_relative_addr( u16 *rca);
int sdio_read_cccr(void);
int sdio_read_common_cis(void);
int sdio_enable_wide(void);
int mmc_sdio_switch_hs(int enable);
int mmc_send_if_cond(u32 ocr);
#include "sdio.h"
#include "gpio.h"
extern void SdioGetCmdResp(uint8_t *RespBuf, uint8_t RespBufLen);
/*uint8_t SDIO_DmaChannelMap[27] =
{
		255,//PERIPHERAL_ID_SPIS_RX = 0,	//0
		255,//PERIPHERAL_ID_SPIS_TX,		//1
		0,//PERIPHERAL_ID_SD_RX,			//2
		1,//PERIPHERAL_ID_SD_TX,			//3
		255,//PERIPHERAL_ID_SPIM0_RX,		//4
		255,//PERIPHERAL_ID_SPIM0_TX,		//5
		255,//PERIPHERAL_ID_SPIM1_RX,		//6
		255,//PERIPHERAL_ID_SPIM1_TX,		//7
		255,//PERIPHERAL_ID_UART0_RX,		//8
		255,//PERIPHERAL_ID_UART1_RX,		//9
		255,//PERIPHERAL_ID_UART2_RX,		//10
		255,//PERIPHERAL_ID_TIMER3,			//11
		255,//PERIPHERAL_ID_TIMER4,			//12
		255,//PERIPHERAL_ID_I2S_TX,			//13
		255,//PERIPHERAL_ID_I2S_RX,			//14
		255,//PERIPHERAL_ID_SARADC,			//15
		255,//PERIPHERAL_ID_UART0_TX,		//16
		255,//PERIPHERAL_ID_UART1_TX,		//17
		255,//PERIPHERAL_ID_UART2_TX,		//18
		255,//PERIPHERAL_ID_TIMER5,			//19
		255,//PERIPHERAL_ID_TIMER6,			//20
		255,//PERIPHERAL_ID_TIMER7,			//21
		255,//PERIPHERAL_ID_TIMER8,			//22
		255,//PERIPHERAL_ID_LACC_SRC,		//23
		255,//PERIPHERAL_ID_LACC_DST,		//24
		255,//PERIPHERAL_ID_LACC_DSTRD,     //25
		255,//PERIPHERAL_ID_SOFTWARE,		//26
};*/

/*wifi*/
void enable_sdio_int()
{
}


int mmc_go_idle()
{
	SDIO_CmdSend(SDIO_GO_IDLE_STATE, 0, 20);
	return SD_OK;
}

/*
 * 函数名：SD_SelectDeselect
 * 描述  ：Selects od Deselects the corresponding card
 * 输入  ：-addr 选择卡的地址
 * 输出  ：-SD_Error SD卡错误代码
 *         成功时则为 SD_OK
 * 调用  ：外部调用
 */
SD_Error SD_SelectDeselect(uint32_t addr)
{
	uint8_t Respons[4];
	if(SDIO_CmdSend(SDIO_SEL_DESEL_CARD,addr,10) == NO_ERR)
	{
		/*
		07 00 00 1E 00 A1
		SDIO_CCCR_CCCR:2
		SDIO_CCCR_CAPS:13
		SDIO_CCCR_POWER:1
		SDIO_CCCR_SPEED:1
		*/
		SdioGetCmdResp(Respons,4);
		//printf("%02X %02X %02X %02X\r\n",Respons[0],Respons[1],Respons[2],Respons[3]);
		return SD_OK;
	}
	else
	{
		//printf("CMD_SDIO_SEL_DESEL_CARD error\n");
		return !SD_OK;
	}
}


int mmc_send_if_cond(u32 ocr)
{
	SDIO_CmdSend(SD_SEND_IF_COND, ocr, 20);
	return SD_OK;
}

static uint8_t convert_from_bytes_to_power_of_two(uint16_t NumberOfBytes)
{
	uint8_t count = 0;

	while (NumberOfBytes != 1)
	{
		NumberOfBytes >>= 1;
		count++;
	}
	return (count);
}

#include "delay.h"
int mmc_send_io_op_cond(u32 ocr, u32 *rocr)
{
	int i;
	u32 response;
	SD_Error errorstatus = SD_OK;
	uint8_t Respons[6];
    int ret;
	for (i = 2000; i; i--)
	{
		//ret = SdioSendCommand(SD_IO_SEND_OP_COND,ocr,100);
		SDIO_CmdSend(SD_IO_SEND_OP_COND,ocr,100);
//        printf("========mmc_send_io_op_cond== %d ==\r\n",ret);
//		SdioGetCmdResp(Respons,5);
		SdioGetCmdResp(Respons,5);

//		printf("mmc_send_io_op_cond:%02X %02X %02X %02X %02X %02X\n",Respons[0],Respons[1],Respons[2],Respons[3],Respons[4],Respons[5]);
		//sleep(10);
		//if (Respons[0] & MMC_CARD_BUSY)
			break;
		OS_MsDelay(1000);
		//WaitMs(1000);
	}
	if (i == 0)
	{
		
		errorstatus = SD_ERROR;
		goto end;
	}
	if (rocr)
		*rocr = response;

end:
	if(errorstatus != SD_OK)
		printf("mmc_send_io_op_cond err:%x\n",errorstatus);

	return errorstatus;
}



__attribute__((section(".driver.isr")))  int mmc_io_rw_direct_host(int write, unsigned fn,
		unsigned addr, u8 in, u8 *out)
{
	SD_Error errorstatus = SD_OK;
	uint8_t ResBuf[6];
	
	uint32_t SDIO_Argument;

	SDIO_Argument = write ? 0x80000000 : 0x00000000;
	SDIO_Argument |= fn << 28;
	SDIO_Argument |= (write && out) ? 0x08000000 : 0x00000000;
	SDIO_Argument |= addr << 9;
	SDIO_Argument |= in;
	OS_MutexLock(sd_mutex);
	//DBG("CMD52:%d %d..",write,addr);
	if(SDIO_CmdSend(SD_IO_RW_DIRECT,SDIO_Argument,100) != NO_ERR)
	{
		errorstatus = !SD_OK;
		//printf("CMD_SD_IO_RW_DIRECT_ERROE\n");
		goto end;
	}
	SdioGetCmdResp(ResBuf,6);

	if (ResBuf[0] & R5_ERROR)
	{
		errorstatus = SD_ILLEGAL_CMD;
	}
	if (ResBuf[0] & R5_FUNCTION_NUMBER)
	{
		errorstatus = SD_SDIO_UNKNOWN_FUNCTION;
	}
	if (ResBuf[0] & R5_OUT_OF_RANGE)
	{
		errorstatus = SD_CMD_OUT_OF_RANGE;
	}
	//printf(">>>>%02X %02X %02X %02X %02X %02X\n",ResBuf[0],ResBuf[1],ResBuf[2],ResBuf[3],ResBuf[4],ResBuf[5]);
	if (out)
		*out = ResBuf[4] & 0xff;

end:	
	OS_MutexUnLock(sd_mutex);
	//DBG("END\n");
	return errorstatus;
}


int mmc_io_rw_direct(int write, unsigned fn, unsigned addr, u8 in, u8 *out)
{
	return mmc_io_rw_direct_host(write, fn, addr, in, out);
}

#define DCTRL_CLEAR_MASK         ((uint32_t)0xFFFFFF08)


#define CCR_ENABLE_Reset        ((uint32_t)0xFFFFFFFE)
extern void woal_sdio_interrupt(struct sdio_func *func);
//DECLARE_MONITOR_ITEM("SDIO INT CNT", sdio_int_cnt);

uint32_t fffff= 1;
int falage = 0;
int read_f = 0;
int readcount;
uint8_t* readbuffer;



int mmc_send_relative_addr( u16 *rca)
{
	uint8_t ResBuf[6];
	if (rca == 0)
		return SD_ERROR;
	if(SDIO_CmdSend(SDIO_SET_REL_ADDR,0,100) == NO_ERR)
	{
		SdioGetCmdResp(ResBuf,5);
		*rca = ResBuf[1];
		*rca = *rca<<8;
		*rca = *rca + ResBuf[2];
		return SD_OK;
	}
	printf("CMD_SDIO_SET_REL_ADDR ERROR\n");
	return SD_ERROR;
}

int sdio_read_cccr()
{
	int ret;
	int cccr_vsn;
	unsigned char data;

	memset(&cccr, 0, sizeof(struct sdio_cccr));
	ret = mmc_io_rw_direct(0, 0, SDIO_CCCR_CCCR, 0, &data);
	if (ret != SD_OK)
		goto out;

	cccr_vsn = data & 0x0f;
//	printf("SDIO_CCCR_CCCR:%x\n", cccr_vsn);

	if (cccr_vsn > SDIO_CCCR_REV_1_20)
	{
		printf("unrecognised CCCR structure version %d\n", cccr_vsn);
		return SD_ERROR;
	}

	cccr.sdio_vsn = (data & 0xf0) >> 4;

	ret = mmc_io_rw_direct(0, 0, SDIO_CCCR_CAPS, 0, &data);
	if (ret != SD_OK)
		goto out;

//	printf("SDIO_CCCR_CAPS:%x\n", data);
	if (data & SDIO_CCCR_CAP_SMB)
		cccr.multi_block = 1;
	if (data & SDIO_CCCR_CAP_LSC)
		cccr.low_speed = 1;
	if (data & SDIO_CCCR_CAP_4BLS)
		cccr.wide_bus = 1;

	if (cccr_vsn >= SDIO_CCCR_REV_1_10)
	{
		ret = mmc_io_rw_direct(0, 0, SDIO_CCCR_POWER, 0, &data);

//		printf("SDIO_CCCR_POWER:%x\n", data);
		if (ret != SD_OK)
			goto out;

		if (data & SDIO_POWER_SMPC)
			cccr.high_power = 1;
	}

	if (cccr_vsn >= SDIO_CCCR_REV_1_20)
	{
		ret = mmc_io_rw_direct(0, 0, SDIO_CCCR_SPEED, 0, &data);

//		printf("SDIO_CCCR_SPEED:%x\n", data);
		if (ret != SD_OK)
			goto out;

		if (data & SDIO_SPEED_SHS)
			cccr.high_speed = 1;
	}

out:
	if (ret != SD_OK)
		printf("sdio_read_cccr err\n");
	return ret;
}

int mmc_sdio_switch_hs(int enable)
{
	int ret;
	u8 speed;

	ret = mmc_io_rw_direct(0, 0, SDIO_CCCR_SPEED, 0, &speed);
	if (ret != SD_OK)
	{
		printf("mmc_sdio_switch_hs err:%d\n", ret);
		return ret;
	}
	if (enable)
		speed |= SDIO_SPEED_EHS;
	else
		speed &= ~SDIO_SPEED_EHS;

	ret = mmc_io_rw_direct(1, 0, SDIO_CCCR_SPEED, speed, NULL);
	if (ret != SD_OK)
		printf("mmc_sdio_switch_hs err1:%d\n", ret);
	else
		printf("mmc_sdio_switch_hs ok\n");

	return ret;
}

int sdio_enable_wide()
{
}

struct cis_tpl
{
	unsigned char code;
	unsigned char min_size;
	//	tpl_parse_t *parse;
};

struct sdio_func_tuple
{
	unsigned char code;
	unsigned char size;
	unsigned char data[128];
};

//struct sdio_func_tuple func_tuple[10];

static int sdio_read_cis(int f_n)
{
	int ret;
	struct sdio_func_tuple *this;
	unsigned i, ptr = 0, tuple_cnt = 0;
	unsigned char tpl_code, tpl_link;
	/*
	 * Note that this works for the common CIS (function number 0) as
	 * well as a function's CIS * since SDIO_CCCR_CIS and SDIO_FBR_CIS
	 * have the same offset.
	 */
	//printf("sdio_read_cis start\n");
	for (i = 0; i < 3; i++)
	{
		unsigned char x;
		//sleep(50);
		ret = mmc_io_rw_direct(0, f_n,
				SDIO_FBR_BASE(f_n) + SDIO_FBR_CIS + i, 0, &x);
		if (ret)
			return ret;
		ptr |= x << (i * 8);
	}

//	p_info("read_cis,fn:%d,addr:%d\n", f_n, ptr);
	
	do
	{
		//sleep(50);
		ret = mmc_io_rw_direct(0, f_n, ptr++, 0, &tpl_code);
		//printf(">>>>>>>>>>>>>>>>>>>>>>>ggg>>>>>>>>>>>>>>>read_cis,ret:%x,addr:%x\n", ret, ptr);
		if (ret)
			break;

		/* 0xff means we're done */
		if (tpl_code == 0xff)
			break;

		/* null entries have no link field or data */
		if (tpl_code == 0x00)
			continue;

		ret = mmc_io_rw_direct(0, f_n, ptr++, 0, &tpl_link);
		//printf(">>>>>>>>>>>>>>>>>>>>kk>>>>>>>>>>>>>>>>>>read_cis,ret:%x,addr:%x\n", ret, ptr);
		if (ret)
			break;

		/* a size of 0xff also means we're done */
		if (tpl_link == 0xff)
			break;

//		p_info("tpl code:%x,size:%d\n", tpl_code, tpl_link);


		if (tuple_cnt > 9 || tpl_link > 128)
		{
//			p_dbg("tuple_cnt over\n");
			break;
		}
//		func_tuple[tuple_cnt].size = tpl_link;
		
	//	this = (struct sdio_func_tuple *)mem_malloc(sizeof(*this) + tpl_link);
		if (!this)
			return -ENOMEM;

		for (i = 0; i < tpl_link; i++)
		{
			ret = mmc_io_rw_direct(0, f_n,
					ptr + i, 0, &this->data[i]);
			if (ret)
				break;
		}

//		dump_hex("cis", this->data, i);

		if (ret)
		{
		//	mem_free(this);
			break;
		}
		if(tpl_code == CISTPL_VERS_1)
//			p_info("%s\n", this->data + 2);
		
	//	mem_free(this);

		ptr += tpl_link;
		tuple_cnt += 1;
	}
	while (!ret);

	if (tpl_link == 0xff)
		ret = 0;

	return ret;
}

int sdio_read_common_cis()
{
	return sdio_read_cis(NULL);
}
/*wifi*/
int SD_Init(void)
{
    int i=0;
	int errorstatus = SD_OK;
	int ocr;
	u16 rca;
#if 0	
	sd_waitq = init_event();
	sd_mutex = mutex_init(__FUNCTION__);
	
	SDIO_PDN_HIGH;
	SDIO_RESET_LOW;
#endif
	OS_MutexInit(&sd_mutex);


//	DMA_ChannelAllocTableSet(SDIO_DmaChannelMap);

	GPIO_RegBitsClear(SDIO_PORT(PU),	SDIO_PIN);
	GPIO_RegBitsClear(SDIO_PORT(PD),	SDIO_PIN);
	GPIO_RegBitsSet(SDIO_PORT(DS),		SDIO_PIN);
	GPIO_RegBitsSet(SDIO_PORT(OUTDS),	SDIO_PIN);

	GPIO_SDIOConfig(SDIO_BUS_PORT);
	SDIO_ClkEnable();
	SDIO_ClkSet(6);
	SDIO_Init();
	SDIO_ClkEnable();
	SDIO_ClkSet(6);
//	SDIO_IntEn(SDIO_INT_DAT,1);

//	void SDIO_IntEn(SDIO_INT_TYPE IntType, bool En);
//
//	bool SDIO_GetIntFlag(SDIO_INT_TYPE IntType);
//
//	void SDIO_ClearIntFlag(SDIO_INT_TYPE IntType);

	//while(1);
	DBG("sdio start init \n");
again:
    OS_MsDelay(500);
	errorstatus = mmc_send_io_op_cond(0, (u32*)&ocr);
	if (errorstatus != SD_OK)
	{
        printf("1111 \r\n");

         goto out;
	}
#if 1
	errorstatus = mmc_send_io_op_cond(ocr, (u32*)&ocr);
	if (errorstatus != SD_OK)
	{
		goto out;
	}
	errorstatus = mmc_send_relative_addr( &rca);
	if (errorstatus != SD_OK)
	{
		goto out;
	}
	errorstatus = SD_SelectDeselect(rca << 16);
	if (errorstatus != SD_OK)
	{
		goto out;
	}
	errorstatus = sdio_read_cccr();
	if (errorstatus != SD_OK)
	{
		goto out;
	}
//	sdio_read_common_cis();
	errorstatus = mmc_sdio_switch_hs(1);
	if (errorstatus != SD_OK)
	{
		goto out;
	}
#if ENABLE_DATA_WIDE
	errorstatus = sdio_enable_wide();
	if (errorstatus != SD_OK)
	{
		goto out;
	}
#endif
	
out:
	if (errorstatus != SD_OK)
	{
//		printf("SDIO card init failed! \r\n");
        OS_MsDelay(1000);
		goto again;
	}
	if(SDIO_DEFAULT_BLOCK_SIZE != 64)
		sdio_set_block_size(SDIO_DEFAULT_BLOCK_SIZE);
	DBG("SDIO设备初始化OK\n");
	//SDIO_ClkEn(0);
	SDIO_ClkSet(0);
	(*(volatile unsigned long *) 0x40021008) = 1;//SDIO 设置到30M
#endif    
	return errorstatus;

}

//uint8_t sdiointflage;
//__attribute__((section(".driver.isr"))) void SDIO_Interrupt1()
//{
////	SDIO_ClkEn(1);
////	SDIO_ClearIntFlag(SDIO_INT_DAT);
////	SDIO_RecvBlockClearDoneFlag();
////	*(uint32_t *)0x4000102C |= 0x20;
////	sdiointflage = 1;
//	//DBG("##################\n");
//	GPIO_RegOneBitClear(GPIO_C_OUT,GPIOC1);
//	GPIO_RegOneBitSet(GPIO_C_OUT,GPIOC1);
//}

#define     REG_DMAC_S0_SRC_ADDR           (*(volatile unsigned long *) 0x40008014)
#define     REG_DMAC_S0_DST_ADDR           (*(volatile unsigned long *) 0x40008018)
#define     REG_DMAC_S0_TOTAL_LEN          (*(volatile unsigned long *) 0x40008004)
#define     REG_DMAC_S0_INT_ST             (*(volatile unsigned long *) 0x4000801C)
#define     MASK_DMAC_S0_DINT              (0x1)


#define     REG_DMAC_S1_SRC_ADDR           (*(volatile unsigned long *) 0x40008034)
#define     REG_DMAC_S1_DST_ADDR           (*(volatile unsigned long *) 0x40008038)
#define     REG_DMAC_S1_TOTAL_LEN          (*(volatile unsigned long *) 0x40008024)
#define     REG_DMAC_S1_INT_ST             (*(volatile unsigned long *) 0x4000803c)
#define     MASK_DMAC_S1_DINT              (0x1)


#define     REG_DMAC_EN_CTRL               (*(volatile unsigned long *) 0x40008100)
#define     REG_SDIO_DAT_TXRXLEN           (*(volatile unsigned long *) 0x40001044)
#define     REG_SDIO_CTRL1                 (*(volatile unsigned long *) 0x4000102C)
#define     REG_SDIO_CTRL2                 (*(volatile unsigned long *) 0x40001030)


#define     REG_SDIO_CTRL4                 (*(volatile unsigned long *) 0x40001034)
#define     MASK_SDIO_DAT_TX_DONE          (0x2)

int mmc_io_rw_extended(int write, unsigned fn,
		unsigned addr, int incr_addr, u8 *buf, unsigned blocks, unsigned blksz)
{

	SD_Error errorstatus = SD_OK;
	uint32_t TotalNumberOfBytes;
	uint32_t SDIO_Argument = 0;
	OS_MutexLock(sd_mutex);

	TotalNumberOfBytes = blocks * blksz;

	SDIO_Argument = write ? 0x80000000 : 0x00000000;
	SDIO_Argument |= fn << 28;
	SDIO_Argument |= incr_addr ? 0x04000000 : 0x00000000;
	SDIO_Argument |= addr << 9;
	if (blocks == 1 && blksz < SDIO_DEFAULT_BLOCK_SIZE)
		SDIO_Argument |= /*(blksz == 256) ? 0 : */blksz;	/* byte mode */
	else
		SDIO_Argument |= 0x08000000 | blocks;		/* block mode */
	read_f = 0;
	if(!write)//r
	{
		if (blocks == 1 && blksz < SDIO_DEFAULT_BLOCK_SIZE)
		{
			REG_DMAC_EN_CTRL &= ~(1<<0);
			REG_DMAC_S0_SRC_ADDR = 0x4000104C;
			REG_DMAC_S0_TOTAL_LEN = TotalNumberOfBytes;
			REG_DMAC_EN_CTRL |= 1<<0;
			REG_SDIO_DAT_TXRXLEN = TotalNumberOfBytes;
			REG_SDIO_CTRL1 |= 0x08;

			REG_DMAC_S0_DST_ADDR = buf;
			REG_DMAC_EN_CTRL |= 1<<0;
			REG_SDIO_CTRL1 &= ~0x04;
			REG_SDIO_CTRL1 |= 0x04;

			SDIO_CmdSend(SD_IO_RW_EXTENDED,SDIO_Argument,100);
			while(!SDIO_RXIsDone());

			REG_SDIO_CTRL1 &= ~0x20;
			REG_DMAC_S0_INT_ST |= 0x100;
		}
		else
		{
			uint32_t i;
			REG_DMAC_EN_CTRL &= ~(1<<0);//dma disable
			REG_DMAC_S0_SRC_ADDR = 0x4000104C;//dma rx src
			REG_DMAC_S0_TOTAL_LEN = SDIO_DEFAULT_BLOCK_SIZE;//dma len
			REG_DMAC_EN_CTRL |= 1<<0;
			REG_SDIO_DAT_TXRXLEN = SDIO_DEFAULT_BLOCK_SIZE;//sdio len
			REG_SDIO_CTRL1 |= 0x08;//DAT_DIRECTION rx

			REG_DMAC_S0_DST_ADDR = buf;
			REG_DMAC_EN_CTRL |= 1<<0;
			REG_SDIO_CTRL1 &= ~0x04;
			REG_SDIO_CTRL1 |= 0x04;

			SDIO_CmdSend(SD_IO_RW_EXTENDED,SDIO_Argument,100);

			while(!SDIO_RXIsDone());
			REG_SDIO_CTRL1 &= ~0x20;//.DAT_START_EN = 0;
			REG_DMAC_S0_INT_ST |= 0x100;

			for(i=1;i<blocks;i++)
			{
				REG_DMAC_EN_CTRL &= ~(1<<0);
				REG_DMAC_S0_DST_ADDR = buf + blksz*i;
				REG_DMAC_S0_TOTAL_LEN = SDIO_DEFAULT_BLOCK_SIZE;//dma len
				REG_DMAC_EN_CTRL |= 1<<0;
				REG_SDIO_CTRL1 &= ~0x04;//.DAT_START_EN = 0;
				REG_SDIO_CTRL1 |= 0x04;// .DAT_START_EN = 1;

				REG_SDIO_CTRL1 &= ~0x20;
				REG_SDIO_CTRL1 |= 0x20;
				REG_SDIO_CTRL2 &= ~0x4000;
				while(!SDIO_RXIsDone());
				REG_DMAC_S0_INT_ST |= 0x100;
				REG_SDIO_CTRL1 &= ~0x04;//.DAT_START_EN = 0;

			}
		}
		*(uint32_t *)0x4000102C = 0x29;
		SDIO_RXEnd();
	}
	else
	{
		uint32_t i;
		SDIO_CmdSend(SD_IO_RW_EXTENDED,SDIO_Argument,100);
		if (blocks == 1 && blksz < SDIO_DEFAULT_BLOCK_SIZE)
		{
			REG_DMAC_EN_CTRL &= ~(1<<1);//dma disable
			REG_DMAC_S1_DST_ADDR = 0x40001048;
			REG_DMAC_S1_TOTAL_LEN = TotalNumberOfBytes;//dma len
			REG_DMAC_S1_SRC_ADDR = buf;
			REG_DMAC_EN_CTRL |= 1<<1;

			REG_SDIO_DAT_TXRXLEN = TotalNumberOfBytes;//sdio len
			REG_SDIO_CTRL1 &= ~0x08;//DAT_DIRECTION rx
			REG_SDIO_CTRL1 &= ~0x04;
			REG_SDIO_CTRL1 |= 0x04;
			while(!SDIO_TXIsDone());
			REG_DMAC_S1_INT_ST |= 0x100;
			REG_SDIO_CTRL1 &= ~0x04;
		}
		else
		{
			REG_DMAC_EN_CTRL &= ~(1<<1);//dma disable
			REG_DMAC_S1_DST_ADDR = 0x40001048;
			REG_DMAC_S1_TOTAL_LEN = SDIO_DEFAULT_BLOCK_SIZE;//dma len
			//REG_DMAC_S1_SRC_ADDR = buf;
			REG_DMAC_EN_CTRL |= 1<<1;

			REG_SDIO_DAT_TXRXLEN = SDIO_DEFAULT_BLOCK_SIZE;//sdio len
			REG_SDIO_CTRL1 &= ~0x08;//DAT_DIRECTION rx

			for(i = 0; i < blocks; i++)
			{
				REG_DMAC_EN_CTRL &= ~(1<<1);
				REG_DMAC_S1_SRC_ADDR = buf + SDIO_DEFAULT_BLOCK_SIZE*i;
				REG_DMAC_EN_CTRL |= 1<<1;

				REG_SDIO_CTRL1 &= ~0x04;
				REG_SDIO_CTRL1 |= 0x04;
				while(!SDIO_TXIsDone());
				REG_DMAC_S1_INT_ST |= 0x100;
				REG_SDIO_CTRL1 &= ~0x04;
			}
		}
		SDIO_TXEnd();
	}
	OS_MutexUnLock(sd_mutex);
	return (errorstatus == SD_OK)?0:-1;
}
