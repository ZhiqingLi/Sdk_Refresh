/******************************************************************************

                  版权所有 (C), 2001-2011, 神州数码网络有限公司

 ******************************************************************************
  文 件 名   : spi_slave.c
  版 本 号   : 初稿
  作    者   : qing
  生成日期   : 2019年7月10日
  最近修改   :
  功能描述   : spi从机功能实现
  函数列表   :
  修改历史   :
  1.日    期   : 2019年7月10日
    作    者   : qing
    修改内容   : 创建文件

******************************************************************************/

/*----------------------------------------------*
 * 包含头文件                                   *
 *----------------------------------------------*/
#include "app_config.h"
#include "spim.h"
#include "spi_slave.h"
/*----------------------------------------------*
 * 外部变量说明                                 *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 内部函数原型说明                             *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 全局变量                                     *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 模块级变量                                   *
 *----------------------------------------------*/

/*----------------------------------------------*
 * 常量定义                                     *
 *----------------------------------------------*/
uint8_t send_data[32];
uint8_t recv_data[32];
/*----------------------------------------------*
 * 宏定义                                       *
 *----------------------------------------------*/

#ifdef FUNC_SPI_SLAVE_EN
void Spi_SlaveInit(void)
{
	SPI_SLAVE_SPIS_INIT();
	GpioSpimIoConfig(SPI_SLAVE_PORT_SEL);
	SpiMasterInit(0, SPIM_CLK_DIV_325K);
	//Spi_SendCmdToSlave(MAS_BOT_DON);
	APP_DBG("Spi slave device init;\n");
}

static bool Spi_SlaveWaitSync(void)
{
	bool ret = FALSE;
	uint8_t recv_index;
	
	memset(&recv_data, 0x00, sizeof(recv_data));
	SPI_SLAVE_SPIS_ENABLE();
	WaitMs(5);
	for (recv_index=0; recv_index<12; recv_index++) {
		recv_data[recv_index] = SpiMasterRecvByte();
	}
	SPI_SLAVE_SPIS_DISABLE();

	if (memcmp(recv_data, "SLA+RECV+OK&", 12) == 0) {
		ret = TRUE;
	}
	APP_DBG("recv slave cmd: %s!\n", recv_data);

	return ret;
}

static void Spi_SlaveSendData(uint8_t *data, uint8_t len)
{
	uint8_t send_index;
	TIMER	SpiWaitTimer;
	
	TimeOutSet(&SpiWaitTimer, 20);
	do {
		SPI_SLAVE_SPIS_ENABLE();
		WaitMs(5);
		for (send_index=0; send_index<len; send_index++) {
			SpiMasterSendByte(*data);
			data++;
		}
		SPI_SLAVE_SPIS_DISABLE();
	} while (!Spi_SlaveWaitSync() && !IsTimeOut(&SpiWaitTimer));
}

void Spi_SendCmdToSlave(uint8_t cmd) 
{
	uint8_t Len;
	
	memset(&send_data, 0x00, sizeof(send_data));

	switch (cmd) {
		case MAS_BOT_DON:
			memcpy(&send_data, "MAS+BOT+DON&", 12);
			Len = 12;
			break;

		case MAS_SET_RBT:
			memcpy(&send_data, "MAS+SET+RBT&", 12);
			Len = 12;
			break;

		case MAS_CLR_RBT:
			memcpy(&send_data, "MAS+CLR+RBT&", 12);
			Len = 12;
			break;

		default:
			break;
	}
	
	APP_DBG("send slave cmd: %s!\n", send_data);
	Spi_SlaveSendData(send_data, Len);
}
#endif



