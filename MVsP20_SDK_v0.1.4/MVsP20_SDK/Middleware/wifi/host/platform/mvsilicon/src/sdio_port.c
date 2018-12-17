/*
*  (C) Copyright 2014-2016 Shenzhen South Silicon Valley microelectronics co.,limited
*
*  All Rights Reserved
*/


#include <log.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "gpio.h"
#include "irqn.h"

#include "sdio.h"
#include "sdio_port.h"
#include "sdio_def.h"
#include "porting.h"
#include "debug.h"

//#define SDIO_DEFAULT_BLOCK_SIZE  4000//64 //256

static unsigned int sdio_block_size;
void (*gpio_sdio_isr)(void);


__attribute__((section(".driver.isr"))) void GpioInterrupt(void)
//void GpioInterrupt(void)
{
//	GPIO_RegOneBitSet(GPIO_C_OUT,GPIOC6);
	GPIO_INTFlagClear(SDIO_INT_PORT(INT), SDIO_INT_PIN);
    GPIO_INTDisable(SDIO_INT_PORT(INT), SDIO_INT_PIN);
    if (gpio_sdio_isr)
    {
        gpio_sdio_isr();
        //DBG("GPIO INIT 0\n");
    }
    else
    {
    	 //DBG("GPIO INIT 1\n");
    }
//    GPIO_RegOneBitClear(GPIO_C_OUT,GPIOC6);
}

void gpio_init() 
{
    GPIO_RegOneBitClear(SDIO_INT_PORT(OE),SDIO_INT_PIN);
    GPIO_RegOneBitSet(SDIO_INT_PORT(IE),SDIO_INT_PIN);

    GPIO_RegOneBitSet(SDIO_INT_PORT(PU),SDIO_INT_PIN);
    GPIO_RegOneBitSet(SDIO_INT_PORT(PD),SDIO_INT_PIN);

    GPIO_INTEnable(SDIO_INT_PORT(INT), SDIO_INT_PIN, GPIO_POS_EDGE_TRIGGER);
    NVIC_EnableIRQ(GPIO_IRQn);
}

bool gpio_isr_enable(bool enable)
{
    if (enable)
    {
         GPIO_INTEnable(SDIO_INT_PORT(INT), SDIO_INT_PIN, GPIO_POS_EDGE_TRIGGER);
    }
    else
    {
    	GPIO_INTFlagClear(SDIO_INT_PORT(INT), SDIO_INT_PIN);
        GPIO_INTDisable(SDIO_INT_PORT(INT), SDIO_INT_PIN);
    }
}

bool sdio_host_enable_isr(bool enable)
{  
    gpio_isr_enable (enable);
    return true;
}

bool sdio_set_block_size(unsigned int blksize)
{
    unsigned char blk[2];
    unsigned int resp;
    int ret;
    
    if (blksize == 0)
    {
        blksize = SDIO_DEFAULT_BLOCK_SIZE;
    }
    
    blk[0] = (blksize >> 0) & 0xff;
    blk[1] = (blksize >> 8) & 0xff;
    
	ret = mmc_io_rw_direct_host(1, 0, (int)0x110, blk[0], 0);
    if (!ret)
    {
        ret = mmc_io_rw_direct_host(1, 0, (int)0x111, blk[1], 0);
    }

    if (!ret)
    {
        sdio_block_size = blksize;
    }
    else
    {
        SDIO_ERROR("sdio_set_block_size(%x), ret = %d\r\n", blksize, ret);
        return false;
    }
    return true;
}

unsigned int sdio_get_block_size()
{
    return sdio_block_size;
}
bool is_truly_isr()
{
    return TRUE;
}

bool sdio_host_detect_card(void)
{
    printf("sdio_host_detect_card \r\n");
    SD_Init();
    return TRUE;
}

bool sdio_host_init(void (*sdio_isr)(void))
{
    int ret = 0;
    
    SD_Init();
    
    gpio_sdio_isr = sdio_isr;
    gpio_init();

    ret = mmc_io_rw_direct_host(1, 0, 0x4, 0x3, 0);
    if (0 != ret)
    {
        SDIO_ERROR("mmc_io_rw_direct_host ret = %d\r\n", ret);
        return false;
    }
    
    sdio_host_enable_isr(true);

    // output timing
    if (!sdio_writeb_cmd52(REG_OUTPUT_TIMING_REG, SDIO_DEF_OUTPUT_TIMING))
        SDIO_FAIL_RET(0, "sdio_write_byte(0x55, %d)\n", SDIO_DEF_OUTPUT_TIMING);
    LOG_PRINTF("output timing to %d (0x%08x)\n", SDIO_DEF_OUTPUT_TIMING, SDIO_DEF_OUTPUT_TIMING);

	SDIO_TRACE("%-20s : %s\n", "host_int", "ok");
  
    return true;
}


u8	sdio_readb_cmd52(u32 addr)
{
    u8 data;
    int ret;
    
    ret = mmc_io_rw_direct_host(0, 1, addr, 0, &data);
    if (ret != 0)
        SDIO_ERROR("_sdio_drv_creg_read ret = %d\r\n", ret);
    
	SDIO_TRACE("%-20s : 0x%08x, 0x%02x\n", "READ_BYTE", addr, data);
    
    return data;
}

bool sdio_writeb_cmd52(u32 addr, u8 data)
{
    int ret;
	ret = mmc_io_rw_direct_host(1, 1, addr, data, 0);
    if (ret != 0)
    {
        SDIO_ERROR("sdio_drv_creg_write ret = %d\r\n", ret);
        return false;
    }
	SDIO_TRACE("%-20s : 0x%08x, 0x%02x\n", "WRITE_BYTE", addr, data);
    
	return true;
}

bool sdio_read_cmd53(u32 dataPort, u8 *dat, size_t size)
{
    u32 rx_blocks = 0, blksize = 0;
    int ret = 0;

#ifndef NO_NEED_ALIGN_BYTE
    if (((unsigned int)dat & 3) || (size & 3))
    {
        SDIO_ERROR("data and len must be align 4 byte. data = 0x%08x,  size = %d\r\n", (unsigned int)dat, size);
        return false;
    }
#endif
    //DBG("size:%d ",size);
    if (size > SDIO_DEFAULT_BLOCK_SIZE) {
        rx_blocks = (size + SDIO_DEFAULT_BLOCK_SIZE - 1) / SDIO_DEFAULT_BLOCK_SIZE;
        blksize = SDIO_DEFAULT_BLOCK_SIZE;
    } else {
        blksize = size;
        rx_blocks = 1;
    }
    //DBG("%d %d\n",rx_blocks,blksize);
    if(0 != mmc_io_rw_extended(0,1,dataPort, 0,dat,rx_blocks,blksize))
    {
        SDIO_ERROR("sdio_drv_read error \r\n");
        return false;
    }

    SDIO_TRACE("%-20s : 0x%x, 0x%08x, 0x%02x\n", "READ_BLOCK", dataPort, dat, size);
    return true;
}


bool sdio_write_cmd53(u32 dataPort, void *dat, size_t size)
{
  	unsigned int tx_blocks = 0, buflen = 0;
    int ret = 0;

#ifndef NO_NEED_ALIGN_BYTE
    if (((unsigned int)dat & 3) || (size & 3))
    {
        SDIO_ERROR("data and len must be align 4 byte. data = 0x%08x,  size = %d\r\n", (unsigned int)dat, size);
        return false;
    }
#endif

    if (size > SDIO_DEFAULT_BLOCK_SIZE)
    {
        tx_blocks = (size + SDIO_DEFAULT_BLOCK_SIZE - 1) / SDIO_DEFAULT_BLOCK_SIZE;
        buflen = SDIO_DEFAULT_BLOCK_SIZE;
    } 
    else 
    {
        buflen = size;
        tx_blocks = 1;
    }
    
    if (0 != mmc_io_rw_extended(1,1,dataPort,0,dat,tx_blocks, buflen))
    {
            SDIO_ERROR("sdio_drv_write error \r\n");
            return false;
    }

    SDIO_TRACE("%-20s : 0x%x, 0x%p, 0x%02x\n", "WRITE_BLOCK", dataPort, dat, size);

    return true;
}


