/******************************************************************************
 *                               US212A
 *                            Module: SPI_NOR DRIVER
 *                 Copyright(c) 2003-2011 Actions Semiconductor,
 *                            All Rights Reserved.
 * History:
 *      <author>         <time>             <version>             <desc>
 *      wuyufan      2012-12-6 11:19:48          1.0              build this file
 ******************************************************************************/
/*!
 * \file     spi_nor_id_tbl.c
 * \brief
 * \author   wuyufan
 * \version  1.0
 * \date     2012-12-6
 *******************************************************************************/
#include "spi_nor_flashid.h"

#pragma __PRQA_IGNORE_START__
const nor_chipinfo_t nor_chip_tbl[] =
{
    /*
     * .chipid      = chip id
     * .capacity    = nor capacity
     * .block_erase = erase region
     * .name        = flash name
     */

    /* ------------------------------ AMIC ------------------------------ */
    //A25L40PU
    {
        .chipid = {AMIC_ID, 0x20, 0x13, 0xff},        
        .capacity = NOR_CAP_512K,
        .block_erase =
        {
            {ERASE_BLOCK_4K, 2},
            {ERASE_BLOCK_8K, 1},
            {ERASE_BLOCK_16K, 1},
            {ERASE_BLOCK_32K, 1},
            {ERASE_BLOCK_64K, 7},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 1,
        .reserved = {0},     
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "A25L40PU",
#endif
    }, 

    //A25L80P
    {
        .chipid = {AMIC_ID, 0x20, 0x14, 0xff},
        .capacity = NOR_CAP_1M,
        .block_erase =
        {
            {ERASE_BLOCK_4K, 2},
            {ERASE_BLOCK_8K, 1},
            {ERASE_BLOCK_16K, 1},
            {ERASE_BLOCK_32K, 1},
            {ERASE_BLOCK_64K, 15},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 1,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "A25L80P",
#endif
    },

    //A25L16PT
    {
        .chipid = {AMIC_ID, 0x20, 0x25, 0xff},
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 31},
            {ERASE_BLOCK_32K, 1},
            {ERASE_BLOCK_16K, 1},
            {ERASE_BLOCK_8K, 1},
            {ERASE_BLOCK_4K, 2},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 2,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "A25L16PT",
#endif
    },

    //A25L16PU
    {
        .chipid = {AMIC_ID, 0x20, 0x15, 0xff},
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_4K, 2},
            {ERASE_BLOCK_8K, 1},
            {ERASE_BLOCK_16K, 1},
            {ERASE_BLOCK_32K, 1},
            {ERASE_BLOCK_64K, 31},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 1,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "A25L16PU",
#endif
    },

    //A25L040
    {
        .chipid = {AMIC_ID, 0x30, 0x13, 0xff},        
        .capacity = NOR_CAP_512K,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 8},           
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},      
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "A25L040",
#endif
    },     

    //A25L080
    {
        .chipid = {AMIC_ID, 0x30, 0x14, 0xff},
        .capacity = NOR_CAP_1M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 16},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "A25L080",
#endif
    },

    //A25L016
    {
        .chipid = {AMIC_ID, 0x30, 0x15, 0xff},
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "A25L016",
#endif
    },

    //A25L032
    {
        .chipid = {AMIC_ID, 0x30, 0x16, 0xff},
        .capacity = NOR_CAP_4M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 64},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "A25L032",
#endif
    },

    //A25LQ16
    {
        .chipid = {AMIC_ID, 0x40, 0x15, 0xff},        
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},           
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},      
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "A25LQ16",
#endif
    },
    

    //A25LQ032
    {
        .chipid = {AMIC_ID, 0x40, 0x16, 0xff},
        .capacity = NOR_CAP_4M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 64},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "A25LQ032",
#endif
    },

    //A25LQ064
    {
        .chipid = {AMIC_ID, 0x40, 0x17, 0xff},        
        .capacity = NOR_CAP_8M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 128},           
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},       
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "A25LQ64",
#endif
    },    
    

    /* ------------------------------ ATMEL ------------------------------ */
    //AT25DF041A
    {
        .chipid = {ATMEL_ID, 0x44, 0x01, 0xff},        
        .capacity = NOR_CAP_512K,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 8},           
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},      
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "AT25DF041A",
#endif
    }, 

    //AT25DF081
    {
        .chipid = {ATMEL_ID, 0x45, 0x02, 0xff},
        .capacity = NOR_CAP_1M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 16},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "AT25DF081",
#endif
    },

    //AT25DF081A
    {
        .chipid = {ATMEL_ID, 0x45, 0x01, 0xff},
        .capacity = NOR_CAP_1M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 16},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "AT25DF081A",
#endif
    },

    //AT25DF161
    {
        .chipid = {ATMEL_ID, 0x46, 0x02, 0xff},
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "AT25DF161",
#endif
    },

    //AT25DF321
    {
        .chipid = {ATMEL_ID, 0x47, 0x00, 0xff},
        .capacity = NOR_CAP_4M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 64},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "AT25DF321",
#endif
    },

    //AT25DF321A
    {
        .chipid = {ATMEL_ID, 0x47, 0x01, 0xff},
        .capacity = NOR_CAP_4M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 64},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "AT25DF321A",
#endif
    },

    //AT25DF641(A)
    {
        .chipid = {ATMEL_ID, 0x48, 0x00, 0xff},
        .capacity = NOR_CAP_8M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 128},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "AT25DF641(A)",
#endif
    },

    //AT25FS040
    {
        .chipid = {ATMEL_ID, 0x66, 0x04, 0xff},        
        .capacity = NOR_CAP_512K,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 8},           
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},        
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "AT25FS040",
#endif
    }, 

    //AT26F004
    {
        .chipid = {ATMEL_ID, 0x04, 0x00, 0xff},        
        .capacity = NOR_CAP_512K,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 8},           
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},       
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "AT26F004",
#endif
    },     


    //AT25DQ161
    {
        .chipid = {ATMEL_ID, 0x86, 0x00, 0xff},
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "AT25DQ161",
#endif
    },

    //AT26DF081A
    {
        .chipid = {ATMEL_ID, 0x45, 0x01, 0xff},
        .capacity = NOR_CAP_1M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 16},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "AT26DF081A",
#endif
    },

    //AT26DF161
    {
        .chipid = {ATMEL_ID, 0x46, 0x00, 0xff},
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "AT26DF161",
#endif
    },

    //AT26DF161A
    {
        .chipid = {ATMEL_ID, 0x46, 0x01, 0xff},
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "AT26DF161A",
#endif
    },
    
    /* ------------------------------ EMST ------------------------------ */
    //F25L08
    {
        .chipid = {EMST_ID, 0x20, 0x14, 0xff},
        .capacity = NOR_CAP_1M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 16},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "F25L08",
#endif
    },

    //F25L16
    {
        .chipid = {EMST_ID, 0x20, 0x15, 0xff},
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "F25L16",
#endif
    },

    //F25L32PA
    {
        .chipid = {EMST_ID, 0x20, 0x16, 0xff},
        .capacity = NOR_CAP_4M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 64},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "F25L32PA",
#endif
    },

    /* ------------------------------ EON ------------------------------ */
    //EN25B40
    {
        .chipid = {EON_ID, 0x20, 0x13, 0xff},        
        .capacity = NOR_CAP_512K,
        .block_erase =
        {
            {ERASE_BLOCK_4K, 2},
            {ERASE_BLOCK_8K, 1},
            {ERASE_BLOCK_16K, 1},
            {ERASE_BLOCK_32K, 1},
            {ERASE_BLOCK_64K, 7},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 1,
        .reserved = {0},       
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "EN25B40",
#endif
    },

    //EN25B80
    {
        .chipid = {EON_ID, 0x20, 0x14, 0xff},
        .capacity = NOR_CAP_1M,
        .block_erase =
        {
            {ERASE_BLOCK_4K, 2},
            {ERASE_BLOCK_8K, 1},
            {ERASE_BLOCK_16K, 1},
            {ERASE_BLOCK_32K, 1},
            {ERASE_BLOCK_64K, 15},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 1,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (EON_SR_SRP_MASK | EON_SR_BP2_MASK | EON_SR_BP1_MASK | EON_SR_BP0_MASK),
        .block_protect = 
        {
            {(EON_SR_BP2_MASK | EON_SR_BP1_MASK), NOR_WP_512K}, 
            {(EON_SR_BP2_MASK | EON_SR_BP0_MASK), NOR_WP_64K},  
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "EN25B80",
#endif
    },

    //EN25B16
    {
        .chipid = {EON_ID, 0x20, 0x15, 0xff},
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_4K, 2},
            {ERASE_BLOCK_8K, 1},
            {ERASE_BLOCK_16K, 1},
            {ERASE_BLOCK_32K, 1},
            {ERASE_BLOCK_64K, 31},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 1,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (EON_SR_SRP_MASK | EON_SR_BP2_MASK | EON_SR_BP1_MASK | EON_SR_BP0_MASK),
        .block_protect = 
        {
            {(EON_SR_BP2_MASK | EON_SR_BP1_MASK), NOR_WP_1M}, 
            {(EON_SR_BP2_MASK | EON_SR_BP0_MASK), NOR_WP_64K},  
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "EN25B16",
#endif
    },

    //EN25B32
    {
        .chipid = {EON_ID, 0x20, 0x16, 0xff},
        .capacity = NOR_CAP_4M,
        .block_erase =
        {
            {ERASE_BLOCK_4K, 2},
            {ERASE_BLOCK_8K, 1},
            {ERASE_BLOCK_16K, 1},
            {ERASE_BLOCK_32K, 1},
            {ERASE_BLOCK_64K, 63},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 1,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (EON_SR_SRP_MASK | EON_SR_BP2_MASK | EON_SR_BP1_MASK | EON_SR_BP0_MASK),
        .block_protect = 
        {
            {(EON_SR_BP2_MASK | EON_SR_BP1_MASK), NOR_WP_2M}, 
            {(EON_SR_BP2_MASK | EON_SR_BP0_MASK), NOR_WP_64K}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "EN25B32",
#endif
    },

    //EN25B64
    {
        .chipid = {EON_ID, 0x20, 0x17, 0xff},
        .capacity = NOR_CAP_8M,
        .block_erase =
        {
            {ERASE_BLOCK_4K, 2},
            {ERASE_BLOCK_8K, 1},
            {ERASE_BLOCK_16K, 1},
            {ERASE_BLOCK_32K, 1},
            {ERASE_BLOCK_64K, 127},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 1,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (EON_SR_SRP_MASK | EON_SR_BP2_MASK | EON_SR_BP1_MASK | EON_SR_BP0_MASK),
        .block_protect = 
        {
            {(EON_SR_BP2_MASK | EON_SR_BP1_MASK), NOR_WP_4M},  
            {(EON_SR_BP2_MASK | EON_SR_BP0_MASK), NOR_WP_64K},  
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "EN25B64",
#endif
    },

    //EN25F40
    {
        .chipid = {EON_ID, 0x31, 0x13, 0xff},        
        .capacity = NOR_CAP_512K,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 8},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},         
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "EN25F40",
#endif
    }, 

    //EN25F80
    {
        .chipid = {EON_ID, 0x31, 0x14, 0xff},
        .capacity = NOR_CAP_1M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 16},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (EON_SR_SRP_MASK | EON_SR_BP2_MASK | EON_SR_BP1_MASK | EON_SR_BP0_MASK),
        .block_protect = 
        {
            {(EON_SR_BP2_MASK | EON_SR_BP0_MASK), NOR_WP_1M_128},  
            {0, 0},  
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "EN25F80",
#endif
    },

    //EN25F16
    {
        .chipid = {EON_ID, 0x31, 0x15, 0xff},
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (EON_SR_SRP_MASK | EON_SR_BP2_MASK | EON_SR_BP1_MASK | EON_SR_BP0_MASK),
        .block_protect = 
        {
            {(EON_SR_BP2_MASK | EON_SR_BP0_MASK), NOR_WP_1M},  
            {(EON_SR_BP1_MASK), NOR_WP_2M_128},   
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "EN25F16",
#endif
    },

    //EN25F32
    {
        .chipid = {EON_ID, 0x31, 0x16, 0xff},
        .capacity = NOR_CAP_4M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 64},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (EON_SR_SRP_MASK | EON_SR_BP3_MASK | 
                EON_SR_BP2_MASK | EON_SR_BP1_MASK | EON_SR_BP0_MASK),
        .block_protect = 
        {
            {(EON_SR_BP2_MASK | EON_SR_BP1_MASK), NOR_WP_2M},  
            {(EON_SR_BP1_MASK), NOR_WP_4M_128}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "EN25F32",
#endif
    },

    //EN25F64
    {
        .chipid = {EON_ID, 0x31, 0x17, 0xff},        
        .capacity = NOR_CAP_8M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 128},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},         
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "EN25F64",
#endif
    },      

    //EN25Q40
    {
        .chipid = {EON_ID, 0x30, 0x13, 0xff},        
        .capacity = NOR_CAP_512K,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 8},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 1,
        .reserved = {0},          
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "EN25Q40",
#endif
    }, 

    //EN25Q80
    {
        .chipid = {EON_ID, 0x30, 0x14, 0xff},
        .capacity = NOR_CAP_1M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 16},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (EON_SR_SRP_MASK | EON_SR_BP3_MASK | 
            EON_SR_BP2_MASK | EON_SR_BP1_MASK | EON_SR_BP0_MASK),
        .block_protect = 
        {
            {(EON_SR_BP2_MASK | EON_SR_BP0_MASK), NOR_WP_1M_128}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "EN25Q80",
#endif
    },

    //EN25Q16
    {
        .chipid = {EON_ID, 0x30, 0x15, 0xff},
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (EON_SR_SRP_MASK | EON_SR_BP3_MASK | 
            EON_SR_BP2_MASK | EON_SR_BP1_MASK | EON_SR_BP0_MASK),
        .block_protect = 
        {
            {(EON_SR_BP1_MASK), NOR_WP_2M_128}, 
            {(EON_SR_BP2_MASK | EON_SR_BP0_MASK), NOR_WP_1M}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "EN25Q16",
#endif
    },

    //EN25Q32
    {
        .chipid = {EON_ID, 0x30, 0x16, 0xff},
        .capacity = NOR_CAP_4M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 64},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (EON_SR_SRP_MASK | EON_SR_BP3_MASK | 
            EON_SR_BP2_MASK | EON_SR_BP1_MASK | EON_SR_BP0_MASK),
        .block_protect = 
        {
            {(EON_SR_BP1_MASK), NOR_WP_4M_128}, 
            {(EON_SR_BP2_MASK | EON_SR_BP1_MASK), NOR_WP_2M},  
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "EN25Q32",
#endif
    },

    //EN25Q64
    {
        .chipid = {EON_ID, 0x30, 0x17, 0xff},
        .capacity = NOR_CAP_8M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 128},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (EON_SR_SRP_MASK | EON_SR_BP3_MASK | 
            EON_SR_BP2_MASK | EON_SR_BP1_MASK | EON_SR_BP0_MASK),
        .block_protect = 
        {
            {(EON_SR_BP1_MASK), NOR_WP_8M_128}, 
            {0, 0},  
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "EN25Q64",
#endif
    },

    //EN25Q128
    {
        .chipid = {EON_ID, 0x30, 0x18, 0xff},
        .capacity = NOR_CAP_16M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 256},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (EON_SR_SRP_MASK | EON_SR_BP3_MASK | 
            EON_SR_BP2_MASK | EON_SR_BP1_MASK | EON_SR_BP0_MASK),
        .block_protect = 
        {
            {(EON_SR_BP1_MASK), NOR_WP_16M_128},
            {0, 0}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "EN25Q128",
#endif
    },

    //EN25QH16
    {
        .chipid = {EON_ID, 0x70, 0x15, 0xff},
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (EON_SR_SRP_MASK | EON_SR_BP3_MASK | 
            EON_SR_BP2_MASK | EON_SR_BP1_MASK | EON_SR_BP0_MASK),
        .block_protect = 
        {
            {(EON_SR_BP3_MASK | EON_SR_BP2_MASK | EON_SR_BP0_MASK), NOR_WP_1M}, 
            {(EON_SR_BP3_MASK | EON_SR_BP2_MASK), NOR_WP_512K},  
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "EN25QH16",
#endif
    },

    //EN25QH32
    {
        .chipid = {EON_ID, 0x70, 0x16, 0xff},
        .capacity = NOR_CAP_4M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 64},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (EON_SR_SRP_MASK | EON_SR_BP3_MASK | 
            EON_SR_BP2_MASK | EON_SR_BP1_MASK | EON_SR_BP0_MASK),
        .block_protect = 
        {
            {(EON_SR_BP3_MASK | EON_SR_BP2_MASK | EON_SR_BP1_MASK), NOR_WP_2M}, 
            {(EON_SR_BP3_MASK | EON_SR_BP2_MASK | EON_SR_BP0_MASK), NOR_WP_1M},  
            {(EON_SR_BP3_MASK | EON_SR_BP2_MASK), NOR_WP_512K}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "EN25QH32",
#endif
    },

    {
        .chipid = {EON_ID, 0x51, 0x14, 0xff},
        .capacity = NOR_CAP_1M,
        .block_erase =
        {
            {   ERASE_BLOCK_64K, 16},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved =  {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "EN25T80",
#endif
    },

    //EN25QH64
    {
        .chipid = {EON_ID, 0x70, 0x17, 0xff},        
        .capacity = NOR_CAP_8M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 128},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (EON_SR_SRP_MASK | EON_SR_BP3_MASK | 
            EON_SR_BP2_MASK | EON_SR_BP1_MASK | EON_SR_BP0_MASK),
        .block_protect = 
        {
            {(EON_SR_BP3_MASK | EON_SR_BP2_MASK | EON_SR_BP1_MASK), NOR_WP_2M}, 
            {(EON_SR_BP3_MASK | EON_SR_BP2_MASK | EON_SR_BP0_MASK), NOR_WP_1M},
            {(EON_SR_BP3_MASK | EON_SR_BP2_MASK), NOR_WP_512K}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},          
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "EN25QH64",
#endif
    }, 

    //EN25QH128
    {
        .chipid = {EON_ID, 0x70, 0x18, 0xff},        
        .capacity = NOR_CAP_16M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 256},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (EON_SR_SRP_MASK | EON_SR_BP3_MASK | 
            EON_SR_BP2_MASK | EON_SR_BP1_MASK | EON_SR_BP0_MASK),
        .block_protect = 
        {
            {(EON_SR_BP3_MASK | EON_SR_BP2_MASK | EON_SR_BP1_MASK), NOR_WP_2M}, 
            {(EON_SR_BP3_MASK | EON_SR_BP2_MASK | EON_SR_BP0_MASK), NOR_WP_1M},
            {(EON_SR_BP3_MASK | EON_SR_BP2_MASK), NOR_WP_512K}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},         
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "EN25QH128",
#endif
    }, 

    //EN25T80
    {
        .chipid = {EON_ID, 0x51, 0x14, 0xff},
        .capacity = NOR_CAP_1M,
        .block_erase =
        {
            {   ERASE_BLOCK_64K, 16},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0}, 
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "EN25T80",
#endif
    },  

    //EN25T16
    {
        .chipid = {EON_ID, 0x51, 0x15, 0xff},
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {   ERASE_BLOCK_64K, 32},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (EON_SR_SRP_MASK | EON_SR_BP3_MASK | 
            EON_SR_BP2_MASK | EON_SR_BP1_MASK | EON_SR_BP0_MASK),
        .block_protect = 
        {
            {(EON_SR_BP1_MASK), NOR_WP_2M_128}, 
            {(EON_SR_BP2_MASK | EON_SR_BP0_MASK), NOR_WP_1M},  
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0}, 
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "EN25T16",
#endif
    },      

    //EN25S40
    {
        .chipid = {EON_ID, 0x38, 0x13, 0xff},        
        .capacity = NOR_CAP_512K,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 8},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},       
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "EN25S40",
#endif
    }, 

    //EN25S80
    {
        .chipid = {EON_ID, 0x38, 0x14, 0xff},        
        .capacity = NOR_CAP_1M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 16},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (EON_SR_SRP_MASK | EON_SR_BP3_MASK | 
            EON_SR_BP2_MASK | EON_SR_BP1_MASK | EON_SR_BP0_MASK),
        .block_protect = 
        {
            {(EON_SR_BP3_MASK | EON_SR_BP1_MASK), NOR_WP_1M_128}, 
            {(EON_SR_BP3_MASK | EON_SR_BP2_MASK), NOR_WP_512K}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},        
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "EN25S80",
#endif
    },   

    //EN25S16
    {
        .chipid = {EON_ID, 0x38, 0x15, 0xff},        
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (EON_SR_SRP_MASK | EON_SR_BP3_MASK | 
            EON_SR_BP2_MASK | EON_SR_BP1_MASK | EON_SR_BP0_MASK),
        .block_protect = 
        {
            {(EON_SR_BP1_MASK), NOR_WP_2M_128}, 
            {(EON_SR_BP2_MASK | EON_SR_BP0_MASK), NOR_WP_1M},  
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},          
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "EN25S16",
#endif
    },

    //EN25S32
    {
        .chipid = {EON_ID, 0x38, 0x16, 0xff},        
        .capacity = NOR_CAP_4M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 64},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (EON_SR_SRP_MASK | EON_SR_BP3_MASK | 
            EON_SR_BP2_MASK | EON_SR_BP1_MASK | EON_SR_BP0_MASK),
        .block_protect = 
        {
            {(EON_SR_BP1_MASK), NOR_WP_4M_128}, 
            {(EON_SR_BP2_MASK | EON_SR_BP1_MASK), NOR_WP_2M}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},          
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "EN25S32",
#endif
    }, 

    //EN25S64
    {
        .chipid = {EON_ID, 0x38, 0x17, 0xff},        
        .capacity = NOR_CAP_8M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 128},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (EON_SR_SRP_MASK | EON_SR_BP3_MASK | 
            EON_SR_BP2_MASK | EON_SR_BP1_MASK | EON_SR_BP0_MASK),
        .block_protect = 
        {
            {(EON_SR_BP0_MASK), NOR_WP_8M_128}, 
            {(EON_SR_BP2_MASK | EON_SR_BP1_MASK), NOR_WP_4M}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},        
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "EN25S64",
#endif
    }, 

    /* ------------------------------ FIDELIX ------------------------- */
    //FM25Q16
    {
        .chipid = {FIDELIX_ID, 0x32, 0x15, 0xff},        
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},        
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "FM25Q16",
#endif
    },     
    
    /* ------------------------------ GD ------------------------------ */
    //GD25D40
    {
        .chipid = {GIGADEVICE_ID, 0x30, 0x13, 0xff},        
        .capacity = NOR_CAP_512K,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 8},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},         
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "GD25D40",
#endif
    }, 

    //GD25D80
    {
        .chipid = {GIGADEVICE_ID, 0x30, 0x14, 0xff},
        .capacity = NOR_CAP_1M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 16},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "GD25D80",
#endif
    },

    //GD25D16
    {
        .chipid = {GIGADEVICE_ID, 0x30, 0x15, 0xff},
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "GD25D16",
#endif
    },

    //GD25D32
    {
        .chipid = {GIGADEVICE_ID, 0x30, 0x16, 0xff},
        .capacity = NOR_CAP_4M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 64},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "GD25D32",
#endif
    },

    //GD25D64
    {
        .chipid = {GIGADEVICE_ID, 0x30, 0x17, 0xff},
        .capacity = NOR_CAP_8M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 128},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "GD25D64",
#endif
    },

    //GD25D128
    {
        .chipid = {GIGADEVICE_ID, 0x30, 0x18, 0xff},
        .capacity = NOR_CAP_16M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 256},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "GD25D128",
#endif
    },

    //GD25Q40
    {
        .chipid = {GIGADEVICE_ID, 0x40, 0x13, 0xff},        
        .capacity = NOR_CAP_512K,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 8},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},       
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "GD25Q40",
#endif
    },     

    //GD25Q80
    {
        .chipid = {GIGADEVICE_ID, 0x40, 0x14, 0xff},
        .capacity = NOR_CAP_1M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 16},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = 
        ( 
              GD_SR_CMP_MASK  | GD_SR_SRP0_MASK 
            | GD_SR_SRP1_MASK | GD_SR_BP0_MASK 
            | GD_SR_BP1_MASK  | GD_SR_BP2_MASK
            | GD_SR_BP3_MASK  | GD_SR_BP4_MASK
        ),        
        .block_protect = 
        {
            {(GD_SR_BP3_MASK | GD_SR_BP2_MASK),  NOR_WP_512K}, //512KB
            {0, 0},    
            {0, 0},
            {0, 0},  
            {0, 0},            
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "GD25Q80",
#endif
    },

    //GD25Q16
    {
        .chipid = {GIGADEVICE_ID, 0x40, 0x15, 0xff},
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_16bit),
        .wp_clr_mask = 
        ( 
              GD_SR_CMP_MASK  | GD_SR_SRP0_MASK 
            | GD_SR_SRP1_MASK | GD_SR_BP0_MASK 
            | GD_SR_BP1_MASK  | GD_SR_BP2_MASK
            | GD_SR_BP3_MASK  | GD_SR_BP4_MASK
        ),        
        .block_protect = 
        {
            {(GD_SR_CMP_MASK | GD_SR_BP1_MASK),  NOR_WP_2M_128}, //1920KB
            {(GD_SR_CMP_MASK | GD_SR_BP2_MASK | GD_SR_BP0_MASK),  NOR_WP_1M},  //1MB  
            {0, 0},
            {0, 0},  
            {0, 0},            
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "GD25Q16",
#endif
    },

    //GD25Q32
    {
        .chipid = {GIGADEVICE_ID, 0x40, 0x16, 0xff},
        .capacity = NOR_CAP_4M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 64},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_16bit),
        .wp_clr_mask = 
        ( 
              GD_SR_CMP_MASK  | GD_SR_SRP0_MASK 
            | GD_SR_SRP1_MASK | GD_SR_BP0_MASK 
            | GD_SR_BP1_MASK  | GD_SR_BP2_MASK
            | GD_SR_BP3_MASK  | GD_SR_BP4_MASK
        ), 
        .block_protect = 
        {
            {(GD_SR_CMP_MASK | GD_SR_BP1_MASK), NOR_WP_4M_128}, //3968KB
            {(GD_SR_CMP_MASK | GD_SR_BP2_MASK | GD_SR_BP0_MASK), NOR_WP_3M}, //3MB  
            {(GD_SR_CMP_MASK | GD_SR_BP2_MASK | GD_SR_BP1_MASK), NOR_WP_2M}, //2MB
            {0, 0},  
            {0, 0},            
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "GD25Q32",
#endif
    },

    //GD25Q64
    {
        .chipid = {GIGADEVICE_ID, 0x40, 0x17, 0xff},
        .capacity = NOR_CAP_8M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 128},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_16bit),
        .wp_clr_mask = 
        ( 
              GD_SR_CMP_MASK  | GD_SR_SRP0_MASK 
            | GD_SR_SRP1_MASK | GD_SR_BP0_MASK 
            | GD_SR_BP1_MASK  | GD_SR_BP2_MASK
            | GD_SR_BP3_MASK  | GD_SR_BP4_MASK
        ), 
        .block_protect = 
        {
            {(GD_SR_CMP_MASK | GD_SR_BP0_MASK), NOR_WP_8M_128}, //8064KB
            {(GD_SR_CMP_MASK | GD_SR_BP2_MASK), NOR_WP_7M}, //7MB  
            {(GD_SR_CMP_MASK | GD_SR_BP2_MASK | GD_SR_BP0_MASK), NOR_WP_6M}, //6MB
            {(GD_SR_CMP_MASK | GD_SR_BP2_MASK | GD_SR_BP1_MASK), NOR_WP_4M}, //4MB 
            {(GD_SR_BP3_MASK | GD_SR_BP2_MASK | GD_SR_BP0_MASK), NOR_WP_2M}, //2MB            
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "GD25Q64",
#endif
    },

    //GD25Q128
    {
        .chipid = {GIGADEVICE_ID, 0x40, 0x18, 0xff},
        .capacity = NOR_CAP_16M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 256},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_16bit),
        .wp_clr_mask = 
        ( 
              GD_SR_CMP_MASK  | GD_SR_SRP0_MASK 
            | GD_SR_SRP1_MASK | GD_SR_BP0_MASK 
            | GD_SR_BP1_MASK  | GD_SR_BP2_MASK
            | GD_SR_BP3_MASK  | GD_SR_BP4_MASK
        ), 
        .block_protect = 
        {
            {(GD_SR_CMP_MASK | GD_SR_BP0_MASK), NOR_WP_16M_256}, //16128KB
            {(GD_SR_CMP_MASK | GD_SR_BP2_MASK | GD_SR_BP1_MASK), NOR_WP_8M}, //8MB  
            {(GD_SR_BP3_MASK | GD_SR_BP2_MASK | GD_SR_BP0_MASK), NOR_WP_4M}, //4MB
            {(GD_SR_BP3_MASK | GD_SR_BP2_MASK ), NOR_WP_2M}, //2MB 
            {(GD_SR_BP3_MASK | GD_SR_BP1_MASK | GD_SR_BP0_MASK), NOR_WP_1M}, //1MB            
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "GD25Q128",
#endif
    },

   //GD25T80
    {
        .chipid = {GIGADEVICE_ID, 0x31, 0x14, 0xff},        
        .capacity = NOR_CAP_1M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 16},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},        
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "GD25T80",
#endif
    },          

    /* ------------------------------ Macronix ------------------------------ */
    //MX25L4005
    {
        .chipid = {MACRONIX_ID, 0x20, 0x13, 0xff},        
        .capacity = NOR_CAP_512K,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 8},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},       
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "MX25L4005",
#endif
    }, 

    //MX25L8005
    {
        .chipid = {MACRONIX_ID, 0x20, 0x14, 0xff},
        .capacity = NOR_CAP_1M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 16},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (MX_SR_SRP_MASK | MX_SR_BP3_MASK | MX_SR_BP2_MASK 
            | MX_SR_BP1_MASK | MX_SR_BP0_MASK),
        .block_protect = 
        {
            {(MX_SR_BP3_MASK | MX_SR_BP2_MASK | MX_SR_BP0_MASK), NOR_WP_1M_128}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "MX25L8005",
#endif
    },

    //MX25L1605
    {
        .chipid = {MACRONIX_ID, 0x20, 0x15, 0xff},
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (MX_SR_SRP_MASK | MX_SR_BP3_MASK | MX_SR_BP2_MASK 
            | MX_SR_BP1_MASK | MX_SR_BP0_MASK),
        .block_protect = 
        {
            {(MX_SR_BP3_MASK | MX_SR_BP2_MASK | MX_SR_BP0_MASK), NOR_WP_2M_128}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "MX25L1605",
#endif
    },

    //MX25L1635D
    {
        .chipid = {MACRONIX_ID, 0x24, 0x15, 0xff},
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (MX_SR_SRP_MASK | MX_SR_BP3_MASK | MX_SR_BP2_MASK 
            | MX_SR_BP1_MASK | MX_SR_BP0_MASK),
        .block_protect = 
        {
            {(MX_SR_BP3_MASK | MX_SR_BP2_MASK | MX_SR_BP0_MASK), NOR_WP_2M_128}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "MX25L1635D",
#endif
    },

    //MX25L1635E
    {
        .chipid = {MACRONIX_ID, 0x25, 0x15, 0xff},
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (MX_SR_SRP_MASK | MX_SR_BP3_MASK | MX_SR_BP2_MASK 
            | MX_SR_BP1_MASK | MX_SR_BP0_MASK),
        .block_protect = 
        {
            {(MX_SR_BP3_MASK | MX_SR_BP2_MASK | MX_SR_BP0_MASK), NOR_WP_2M_128}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "MX25L1635E",
#endif
    },

    //MX
    {
        .chipid = {MACRONIX_ID, 0x28, 0x15, 0xff},
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "MX",
#endif
    },

    //MX25L13206E
    //特别注意:C2 20 16这个ID表有好几款同类型的flash，可保护区域并不相同，需要注意区分
    //MX25L3275E,MX25L3235E也是这个ID
    {
        .chipid = {MACRONIX_ID, 0x20, 0x16, 0xff},
        .capacity = NOR_CAP_4M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 64},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (MX_SR_SRP_MASK | MX_SR_BP3_MASK | MX_SR_BP2_MASK 
            | MX_SR_BP1_MASK | MX_SR_BP0_MASK),
        .block_protect = 
        {
            {(MX_SR_BP3_MASK | MX_SR_BP2_MASK | MX_SR_BP0_MASK), NOR_WP_4M_128}, 
            {(MX_SR_BP3_MASK | MX_SR_BP0_MASK), NOR_WP_2M},  
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "MX25L13206E",
#endif
    },

    //MX25L13235D
    {
        .chipid = {MACRONIX_ID, 0x5E, 0x16, 0xff},
        .capacity = NOR_CAP_4M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 64},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (MX_SR_SRP_MASK | MX_SR_BP3_MASK | MX_SR_BP2_MASK 
            | MX_SR_BP1_MASK | MX_SR_BP0_MASK),
        .block_protect = 
        {
            {(MX_SR_BP3_MASK | MX_SR_BP2_MASK | MX_SR_BP0_MASK), NOR_WP_4M_128}, 
            {(MX_SR_BP3_MASK | MX_SR_BP0_MASK), NOR_WP_2M},  
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "MX25L13235D",
#endif
    },

    //MX25L6405
    {
        .chipid = {MACRONIX_ID, 0x20, 0x17, 0xff},
        .capacity = NOR_CAP_8M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 128},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (MX_SR_SRP_MASK | MX_SR_BP3_MASK | MX_SR_BP2_MASK 
            | MX_SR_BP1_MASK | MX_SR_BP0_MASK),
        .block_protect = 
        {
            {(MX_SR_BP3_MASK | MX_SR_BP2_MASK | MX_SR_BP1_MASK), NOR_WP_8M_128}, 
            {(MX_SR_BP3_MASK | MX_SR_BP0_MASK), NOR_WP_4M}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "MX25L6405",
#endif
    },

    //MX25L12805
    {
        .chipid = {MACRONIX_ID, 0x20, 0x18, 0xff},
        .capacity = NOR_CAP_16M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 256},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "MX25L12805",
#endif
    },

    //MX25U1635
    {
        .chipid = {MACRONIX_ID, 0x25, 0x35, 0xff},        
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (MX_SR_SRP_MASK | MX_SR_BP3_MASK | MX_SR_BP2_MASK 
            | MX_SR_BP1_MASK | MX_SR_BP0_MASK),
        .block_protect = 
        {
            {(MX_SR_BP3_MASK | MX_SR_BP2_MASK | MX_SR_BP0_MASK), NOR_WP_2M_128}, 
            {(MX_SR_BP3_MASK | MX_SR_BP1_MASK), NOR_WP_1M}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},       
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "MX25U1635",
#endif
    }, 
    
    //MX25U3235
    {
        .chipid = {MACRONIX_ID, 0x25, 0x36, 0xff},        
        .capacity = NOR_CAP_4M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 64},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (MX_SR_SRP_MASK | MX_SR_BP3_MASK | MX_SR_BP2_MASK 
            | MX_SR_BP1_MASK | MX_SR_BP0_MASK),
        .block_protect = 
        {
            {(MX_SR_BP3_MASK | MX_SR_BP2_MASK | MX_SR_BP0_MASK), NOR_WP_4M_128}, 
            {(MX_SR_BP3_MASK | MX_SR_BP0_MASK), NOR_WP_2M}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},      
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "MX25U3235",
#endif
    }, 

    //MX25U6435
    {
        .chipid = {MACRONIX_ID, 0x25, 0x37, 0xff},        
        .capacity = NOR_CAP_8M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 128},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (MX_SR_SRP_MASK | MX_SR_BP3_MASK | MX_SR_BP2_MASK 
            | MX_SR_BP1_MASK | MX_SR_BP0_MASK),
        .block_protect = 
        {
            {(MX_SR_BP3_MASK | MX_SR_BP2_MASK | MX_SR_BP0_MASK), NOR_WP_8M_128}, 
            {(MX_SR_BP3_MASK), NOR_WP_4M}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},       
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "MX25U6435",
#endif
    },     

    /* ------------------------------ Numonyx ------------------------------ */
    //M25PE40
    {
        .chipid = {NUMONYX_ID, 0x80, 0x13, 0xff},        
        .capacity = NOR_CAP_512K,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 8},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},     
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "M25PE40",
#endif
    },  

    //M25PE80
    {
        .chipid = {NUMONYX_ID, 0x80, 0x14, 0xff},
        .capacity = NOR_CAP_1M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 16},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "M25PE80",
#endif
    },

    //M25PE16
    {
        .chipid = {NUMONYX_ID, 0x80, 0x15, 0xff},
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "M25PE16",
#endif
    },

    //M45PE40
    {
        .chipid = {NUMONYX_ID, 0x40, 0x13, 0xff},        
        .capacity = NOR_CAP_512K,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 8},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},       
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "M45PE40",
#endif
    },  

    //M45PE80
    {
        .chipid = {NUMONYX_ID, 0x40, 0x14, 0xff},        
        .capacity = NOR_CAP_1M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 16},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},       
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "M45PE80",
#endif
    },  

    //M45PE16
    {
        .chipid = {NUMONYX_ID, 0x40, 0x15, 0xff},        
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},       
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "M45PE16",
#endif
    },      

    //N25Q016
    {
        .chipid = {NUMONYX_ID, 0xBA, 0x15, 0xff},        
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (NOR_SR_SRP_MASK | NM_SR_TB_MASK | NOR_SR_BP2_MASK
            | NOR_SR_BP1_MASK | NOR_SR_BP0_MASK | NM_SR_TB_MASK),
        .block_protect = 
        {
            {(NM_SR_TB_MASK | NOR_SR_BP2_MASK | NOR_SR_BP0_MASK), NOR_WP_1M}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},       
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "N25Q016",
#endif
    },  

    //N25Q032
    {
        .chipid = {NUMONYX_ID, 0xBA, 0x16, 0xff},        
        .capacity = NOR_CAP_4M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 64},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (NOR_SR_SRP_MASK | NM_SR_TB_MASK | NOR_SR_BP2_MASK
            | NOR_SR_BP1_MASK | NOR_SR_BP0_MASK | NM_SR_TB_MASK),
        .block_protect = 
        {
            {(NM_SR_TB_MASK | NOR_SR_BP2_MASK | NOR_SR_BP1_MASK), NOR_WP_1M}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},       
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "N25Q032",
#endif
    },

    //N25Q064
    {
        .chipid = {NUMONYX_ID, 0xBA, 0x17, 0xff},
        .capacity = NOR_CAP_8M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 128},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (NOR_SR_SRP_MASK | NM_SR_TB_MASK | NOR_SR_BP2_MASK
            | NOR_SR_BP1_MASK | NOR_SR_BP0_MASK | NOR_SR_BP3_MASK | NM_SR_TB_MASK),
        .block_protect = 
        {
            {(NM_SR_TB_MASK | NOR_SR_BP2_MASK | NOR_SR_BP1_MASK | NOR_SR_BP0_MASK), NOR_WP_4M}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "N25Q064",
#endif
    },


    //N25Q0128
    {
        .chipid = {NUMONYX_ID, 0xBA, 0x18, 0xff},        
        .capacity = NOR_CAP_16M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 256},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},       
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "N25Q0128",
#endif
    }, 

    /* ------------------------------ Nantronics ------------------------------ */     
    //N25S40
    {
        .chipid = {NANTRONIC_ID, 0x30, 0x13, 0xff},        
        .capacity = NOR_CAP_512K,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 8},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},      
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "N25S40",
#endif
    },   

    //N25S80
    {
        .chipid = {NANTRONIC_ID, 0x30, 0x14, 0xff},        
        .capacity = NOR_CAP_1M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 16},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (NOR_SR_SRP_MASK | NOR_SR_BP3_MASK | NOR_SR_BP2_MASK
            | NOR_SR_BP1_MASK | NOR_SR_BP0_MASK),
        .block_protect = 
        {
            {(NOR_SR_BP3_MASK | NOR_SR_BP0_MASK), NOR_WP_127K}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},       
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "N25S80",
#endif
    },   

    //N25S16
    {
        .chipid = {NANTRONIC_ID, 0x30, 0x15, 0xff},        
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (NOR_SR_SRP_MASK | NOR_SR_BP3_MASK | NOR_SR_BP2_MASK
            | NOR_SR_BP1_MASK | NOR_SR_BP0_MASK),
        .block_protect = 
        {
            {(NOR_SR_BP3_MASK | NOR_SR_BP2_MASK | NOR_SR_BP0_MASK), NOR_WP_2M_128},  
            {0, 0}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},      
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "N25S16",
#endif
    },    

    /* ------------------------------ PMC ------------------------------ */
    //PM25LV080B
    {
        .chipid = {PMC_ID, 0x7F, 0x13, 0xff},
        .capacity = NOR_CAP_1M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 16},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "PM25LV080B",
#endif
    },

    //PM25LV016B
    {
        .chipid = {PMC_ID, 0x7F, 0x14, 0xff},
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "PM25LV016B",
#endif
    },

    //IS25LQ080
    {
        .chipid = {PMC_ID, 0x13, 0x44, 0xff},
        .capacity = NOR_CAP_1M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 16},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "IS25LQ080",
#endif
    },  

    //IS25LQ016
    {
        .chipid = {PMC_ID, 0x14, 0x45, 0xff},
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "IS25LQ016",
#endif
    },     

    /* ------------------------------ Sanyo ------------------------------ */
    //LF25FW203A
    {
        .chipid = {SANYO_ID, 0x16, 0x00, 0xff},
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "LF25FW203A",
#endif
    },

    /* ------------------------------ Spansion ------------------------------ */
    //S25FL004A
    {
        .chipid = {SPANSION_ID, 0x02, 0x12, 0xff},        
        .capacity = NOR_CAP_512K,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 8},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},        
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "S25FL004A",
#endif
    }, 

    //S25FL008A
    {
        .chipid = {SPANSION_ID, 0x02, 0x13, 0xff},
        .capacity = NOR_CAP_1M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 16},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "S25FL008A",
#endif
    },

    //S25FL016A
    {
        .chipid = {SPANSION_ID, 0x02, 0x14, 0xff},
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "S25FL016A",
#endif
    },

    //S25FL032A
    {
        .chipid = {SPANSION_ID, 0x02, 0x15, 0xff},
        .capacity = NOR_CAP_4M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 64},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "S25FL032A",
#endif
    },

    //S25FL064A
    {
        .chipid = {SPANSION_ID, 0x02, 0x16, 0xff},
        .capacity = NOR_CAP_8M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 128},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "S25FL064A",
#endif
    },

    //S25FL032A
    {
        .chipid = {SPANSION_ID, 0x02, 0x15, 0xff},
        .capacity = NOR_CAP_4M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 64},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "S25FL032A",
#endif
    },

    //S25FL064A
    {
        .chipid = {SPANSION_ID, 0x02, 0x16, 0xff},        
        .capacity = NOR_CAP_8M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 128},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},       
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "S25FL064A",
#endif
    },   

    //S25FL204
    {
        .chipid = {SPANSION_ID, 0x40, 0x13, 0xff},        
        .capacity = NOR_CAP_512K,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 8},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},     
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "S25FL204",
#endif
    }, 

    //S25FL208
    {
        .chipid = {SPANSION_ID, 0x40, 0x14, 0xff},        
        .capacity = NOR_CAP_1M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 16},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (NOR_SR_SRP_MASK | NOR_SR_BP3_MASK | NOR_SR_BP2_MASK
            | NOR_SR_BP1_MASK | NOR_SR_BP0_MASK),
        .block_protect = 
        {
            {(NOR_SR_BP3_MASK | NOR_SR_BP0_MASK), NOR_WP_127K}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},       
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "S25FL208",
#endif
    }, 

    //S25FL216
    {
        .chipid = {SPANSION_ID, 0x40, 0x15, 0xff},        
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (NOR_SR_SRP_MASK | NOR_SR_BP3_MASK | NOR_SR_BP2_MASK
            | NOR_SR_BP1_MASK | NOR_SR_BP0_MASK),
        .block_protect = 
        {
            {(NOR_SR_BP3_MASK | NOR_SR_BP2_MASK | NOR_SR_BP0_MASK), NOR_WP_2M_128}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},        
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "S25FL216",
#endif
    },      

    /* ------------------------------ SST ------------------------------ */
    //SST25LF080A
    {
        .chipid = {SST_ID, 0x80, 0xff, 0xff},
        .capacity = NOR_CAP_1M,
        .block_erase =
        {
            {ERASE_BLOCK_32K, 32},
        },
        .dma_write = FALSE,
        .write_cmd = SPI_NOR_SEQUENTIAL_PROGRAM_2,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_32K,
        .erase_mode = 3,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "SST25LF080A",
#endif
    },

    //SST25VF040
    {
        .chipid = {SST_ID, 0x25, 0x8D, 0xff},
        .capacity = NOR_CAP_512K,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 8},
        },
        .dma_write = FALSE,
        .write_cmd = SPI_NOR_AAI_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "SST25VF040",
#endif
    },

    //SST25VF080B
    {
        .chipid = {SST_ID, 0x25, 0x8E, 0xff},
        .capacity = NOR_CAP_1M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 16},
        },
        .dma_write = FALSE,
        .write_cmd = SPI_NOR_AAI_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "SST25VF080B",
#endif
    },

    //SST25VF016
    {
        .chipid = {SST_ID, 0x25, 0x41, 0xff},
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},
        },
        .dma_write = FALSE,
        .write_cmd = SPI_NOR_AAI_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "SST25VF016",
#endif
    },

    //SST25VF032B
    {
        .chipid = {SST_ID, 0x25, 0x4A, 0xff},
        .capacity = NOR_CAP_4M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 64},
        },
        .dma_write = FALSE,
        .write_cmd = SPI_NOR_AAI_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "SST25VF032B",
#endif
    },

    //SST25VF064C
    {
        .chipid = {SST_ID, 0x25, 0x4B, 0xff},
        .capacity = NOR_CAP_8M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 128},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "SST25VF064C",
#endif
    },

    /* ------------------------------ ST ------------------------------ */
    //M25P40
    {
        .chipid = {NUMONYX_ID, 0x20, 0x13, 0xff},        
        .capacity = NOR_CAP_512K,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 8},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},       
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "M25P40",
#endif
    },  

    //M25P80
    {
        .chipid = {ST_ID, 0x20, 0x14, 0xff},
        .capacity = NOR_CAP_1M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 16},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "M25P80",
#endif
    },

    //M25P16
    {
        .chipid = {ST_ID, 0x20, 0x15, 0xff},
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "M25P16",
#endif
    },

    //M25P32
    {
        .chipid = {ST_ID, 0x20, 0x16, 0xff},
        .capacity = NOR_CAP_4M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 64},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "M25P32",
#endif
    },

    //M25P64
    {
        .chipid = {ST_ID, 0x20, 0x17, 0xff},
        .capacity = NOR_CAP_8M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 128},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "M25P64",
#endif
    },

    //M25P128
    {
        .chipid = {ST_ID, 0x20, 0x18, 0xff},
        .capacity = NOR_CAP_16M,
        .block_erase =
        {
            {ERASE_BLOCK_256K, 64},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "M25P128",
#endif
    },

    //M25PX80
    {
        .chipid = {ST_ID, 0x71, 0x14, 0xff},        
        .capacity = NOR_CAP_1M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 16},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (NOR_SR_SRP_MASK | NM_SR_TB_MASK | NOR_SR_BP2_MASK
            | NOR_SR_BP1_MASK | NOR_SR_BP0_MASK),
        .block_protect = 
        {
            {(NM_SR_TB_MASK | NOR_SR_BP1_MASK | NOR_SR_BP0_MASK), NOR_WP_256K}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},      
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "M25PX80",
#endif
    },  
    

    //M25PX16
    {
        .chipid = {ST_ID, 0x71, 0x15, 0xff},
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (NOR_SR_SRP_MASK | NM_SR_TB_MASK | NOR_SR_BP2_MASK
            | NOR_SR_BP1_MASK | NOR_SR_BP0_MASK),
        .block_protect = 
        {
            {(NM_SR_TB_MASK | NOR_SR_BP2_MASK), NOR_WP_1M}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "M25PX16",
#endif
    },

    //M25PX32
    {
        .chipid = {ST_ID, 0x71, 0x16, 0xff},
        .capacity = NOR_CAP_4M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 64},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (NOR_SR_SRP_MASK | NM_SR_TB_MASK | NOR_SR_BP2_MASK
            | NOR_SR_BP1_MASK | NOR_SR_BP0_MASK),
        .block_protect = 
        {
            {(NM_SR_TB_MASK | NOR_SR_BP2_MASK), NOR_WP_1M}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "M25PX32",
#endif
    },

    //M25PX64
    {
        .chipid = {ST_ID, 0x71, 0x17, 0xff},
        .capacity = NOR_CAP_8M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 128},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "M25PX64",
#endif
    },

    /* ------------------------------ Winbond ------------------------------ */
    //W25Q40
    {
        .chipid = {WINBOND_NEX_ID, 0x40, 0x13, 0xff},        
        .capacity = NOR_CAP_512K,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 8},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},        
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "W25Q40",
#endif
    },

    //W25Q80
    {
        .chipid = {WINBOND_NEX_ID, 0x40, 0x14, 0xff},
        .capacity = NOR_CAP_1M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 16},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (WB_SR_SRP_MASK | WB_SR_TB_MASK 
            | WB_SR_BP2_MASK | WB_SR_BP1_MASK | WB_SR_BP0_MASK),
        .block_protect = 
        {
            {(WB_SR_TB_MASK | WB_SR_BP2_MASK), NOR_WP_512K}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "W25Q80",
#endif
    },

    //W25Q16
    {
        .chipid = {WINBOND_NEX_ID, 0x40, 0x15, 0xff},
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (WB_SR_SRP_MASK | WB_SR_TB_MASK 
            | WB_SR_BP2_MASK | WB_SR_BP1_MASK | WB_SR_BP0_MASK),
        .block_protect = 
        {
            {(WB_SR_TB_MASK | WB_SR_BP2_MASK | WB_SR_BP0_MASK), NOR_WP_1M},  
            {(WB_SR_TB_MASK | WB_SR_BP2_MASK), NOR_WP_512K},  
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "W25Q16",
#endif
    },

    //W25Q32
    {
        .chipid = {WINBOND_NEX_ID, 0x40, 0x16, 0xff},
        .capacity = NOR_CAP_4M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 64},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (WB_SR_SRP_MASK | WB_SR_TB_MASK 
            | WB_SR_BP2_MASK | WB_SR_BP1_MASK | WB_SR_BP0_MASK),
        .block_protect = 
        {
            {(WB_SR_TB_MASK | WB_SR_BP2_MASK | WB_SR_BP1_MASK), NOR_WP_2M},
            {(WB_SR_TB_MASK | WB_SR_BP2_MASK | WB_SR_BP0_MASK), NOR_WP_1M}, 
            {(WB_SR_TB_MASK | WB_SR_BP2_MASK), NOR_WP_512K},
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "W25Q32",
#endif
    },

    //W25Q64
    {
        .chipid = {WINBOND_NEX_ID, 0x40, 0x17, 0xff},
        .capacity = NOR_CAP_8M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 128},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (WB_SR_SRP_MASK | WB_SR_TB_MASK 
            | WB_SR_BP2_MASK | WB_SR_BP1_MASK | WB_SR_BP0_MASK),
        .block_protect = 
        {
            //由于Q64多种型号具有同样ID,而某些flash不具有CMP位，不能设置此保护域
            //{(WB_SR_CMP_MASK | WB_SR_BP0_MASK), NOR_WP_8M_128}, 
            //{(WB_SR_CMP_MASK | WB_SR_BP2_MASK | WB_SR_BP1_MASK), NOR_WP_4M},
            {(WB_SR_TB_MASK | WB_SR_BP2_MASK | WB_SR_BP1_MASK), NOR_WP_4M},
            {(WB_SR_TB_MASK | WB_SR_BP2_MASK | WB_SR_BP0_MASK), NOR_WP_2M}, 
            {(WB_SR_TB_MASK | WB_SR_BP2_MASK), NOR_WP_1M}, 
            {(WB_SR_TB_MASK | WB_SR_BP1_MASK | WB_SR_BP0_MASK), NOR_WP_512K},
            {0, 0},
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "W25Q64",
#endif
    },

    //W25Q128
    {
        .chipid = {WINBOND_NEX_ID, 0x40, 0x18, 0xff},
        .capacity = NOR_CAP_16M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 256},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "W25Q128",
#endif
    },

    //W25Q40.W
    {
        .chipid = {WINBOND_NEX_ID, 0x50, 0x13, 0xff},        
        .capacity = NOR_CAP_512K,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 8},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},        
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "W25Q40.W",
#endif
    },

    //W25Q80.W
    {
        .chipid = {WINBOND_NEX_ID, 0x50, 0x14, 0xff},        
        .capacity = NOR_CAP_1M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 16},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},      
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "W25Q80.W",
#endif
    },

    //W25Q16.W
    {
        .chipid = {WINBOND_NEX_ID, 0x50, 0x15, 0xff},        
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},        
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "W25Q16.W",
#endif
    }, 

    //W25Q32.W
    {
        .chipid = {WINBOND_NEX_ID, 0x50, 0x16, 0xff},        
        .capacity = NOR_CAP_4M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 64},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},        
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "W25Q32.W",
#endif
    },  

    //W25Q64.W
    {
        .chipid = {WINBOND_NEX_ID, 0x50, 0x17, 0xff},        
        .capacity = NOR_CAP_8M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 128},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},        
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "W25Q64.W",
#endif
    }, 

    //W25Q128.W
    {
        .chipid = {WINBOND_NEX_ID, 0x50, 0x18, 0xff},        
        .capacity = NOR_CAP_16M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 256},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},        
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "W25Q128.W",
#endif
    }, 

    //W25X40
    {
        .chipid = {WINBOND_NEX_ID, 0x30, 0x13, 0xff},        
        .capacity = NOR_CAP_512K,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 8},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},        
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "W25X40",
#endif
    },    

    //W25X80
    {
        .chipid = {WINBOND_NEX_ID, 0x30, 0x14, 0xff},
        .capacity = NOR_CAP_1M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 16},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (WB_SR_SRP_MASK | WB_SR_TB_MASK 
            | WB_SR_BP2_MASK | WB_SR_BP1_MASK | WB_SR_BP0_MASK),
        .block_protect = 
        {
            {(WB_SR_TB_MASK | WB_SR_BP2_MASK ), NOR_WP_512K}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "W25X80",
#endif
    },

    //W25X16
    {
        .chipid = {WINBOND_NEX_ID, 0x30, 0x15, 0xff},
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (WB_SR_SRP_MASK | WB_SR_TB_MASK 
            | WB_SR_BP2_MASK | WB_SR_BP1_MASK | WB_SR_BP0_MASK),
        .block_protect = 
        {
            {(WB_SR_TB_MASK | WB_SR_BP2_MASK | WB_SR_BP0_MASK), NOR_WP_1M}, 
            {(WB_SR_TB_MASK | WB_SR_BP2_MASK), NOR_WP_512K}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "W25X16",
#endif
    },

    //W25X32
    {
        .chipid = {WINBOND_NEX_ID, 0x30, 0x16, 0xff},
        .capacity = NOR_CAP_4M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 64},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (WB_SR_SRP_MASK | WB_SR_TB_MASK 
            | WB_SR_BP2_MASK | WB_SR_BP1_MASK | WB_SR_BP0_MASK),
        .block_protect = 
        {
            {(WB_SR_TB_MASK | WB_SR_BP2_MASK | WB_SR_BP1_MASK), NOR_WP_2M}, 
            {(WB_SR_TB_MASK | WB_SR_BP2_MASK | WB_SR_BP0_MASK), NOR_WP_1M}, 
            {(WB_SR_TB_MASK | WB_SR_BP2_MASK), NOR_WP_512K},  
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "W25X32",
#endif
    },

    //W25X64
    {
        .chipid = {WINBOND_NEX_ID, 0x30, 0x17, 0xff},
        .capacity = NOR_CAP_8M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 128},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (WB_SR_SRP_MASK | WB_SR_TB_MASK 
            | WB_SR_BP2_MASK | WB_SR_BP1_MASK | WB_SR_BP0_MASK),
        .block_protect = 
        {
            {(WB_SR_TB_MASK | WB_SR_BP2_MASK | WB_SR_BP1_MASK), NOR_WP_4M}, 
            {(WB_SR_TB_MASK | WB_SR_BP2_MASK | WB_SR_BP0_MASK), NOR_WP_2M}, 
            {(WB_SR_TB_MASK | WB_SR_BP2_MASK), NOR_WP_1M},  
            {(WB_SR_TB_MASK | WB_SR_BP1_MASK | WB_SR_BP0_MASK), NOR_WP_512K}, 
            {0, 0},             
        },        
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "W25X64",
#endif
    },

    //MX25R1635
    {
        .chipid = {WINBOND_NEX_ID, 0x60, 0x15, 0xff},
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "MX25R1635",
#endif
    },  

    //MK25Q32
    {
        .chipid = {WINBOND_NEX_ID, 0x70, 0x16, 0xff},
        .capacity = NOR_CAP_4M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 64},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "MK25Q32",
#endif
    },
    

    /* -------------------------- shagnhai fudan micro --------------------------- */
    //FM25F04
    {
        .chipid = {FD_MICRO_ID, 0x31, 0x13, 0xff},
        .capacity = NOR_CAP_512K,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 8},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "FM25F04",
#endif
    },

    //FM25Q32
    {
        .chipid = {FD_MICRO_ID, 0x40, 0x16, 0xff},
        .capacity = NOR_CAP_4M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "FM25Q32",
#endif
    },

    /* -------------------------- zhuhai Berg micro(Bo Guan) --------------------------- */
    //BG25Q40A
    {
        .chipid = {ZH_BERG_ID, 0x40, 0x13, 0xff},        
        .capacity = NOR_CAP_512K,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 8},           
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},          
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "BG25Q40A",
#endif
    },

    //BQ25Q80A
    {
        .chipid = {ZH_BERG_ID, 0x40, 0x14, 0xff},
        .capacity = NOR_CAP_1M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 16},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "BQ25Q80A",
#endif
    },

    //BQ25Q16A
    {
        .chipid = {ZH_BERG_ID, 0x40, 0x15, 0xff},
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "BQ25Q16A",
#endif
    },

    //BG25Q32A
    {
        .chipid = {ZH_BERG_ID, 0x40, 0x16, 0xff},        
        .capacity = NOR_CAP_4M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 64},           
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},         
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "BG25Q32A",
#endif
    },      

    /* -------------------------- Fremont Micro Device(FMD) --------------------------- */
    //FT25H16
    {
        .chipid = {FMD_ID, 0x40, 0x15, 0xff},
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_16bit),
        .wp_clr_mask = (FMD_SR_SRP_MASK | FMD_SR_BP4_MASK | FMD_SR_BP3_MASK |
            FMD_SR_BP2_MASK | FMD_SR_BP1_MASK | FMD_SR_BP0_MASK | FMD_SR_CMP_MASK),
        .block_protect = 
        {
            {(FMD_SR_CMP_MASK | FMD_SR_BP1_MASK), NOR_WP_2M_128}, 
            {(FMD_SR_BP3_MASK | FMD_SR_BP2_MASK), NOR_WP_512K}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "FT25H16",
#endif
    },

    /* -------------------------- GD costdown norflash --------------------------- */
    //MD25D40
    {
        .chipid = {GIGADEVICE_ID2, 0x40, 0x13, 0xff},        
        .capacity = NOR_CAP_512K,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 8},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},        
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "MD25D40",
#endif
    }, 
    
    //MD25D80
    {
        .chipid = {GIGADEVICE_ID2, 0x40, 0x14, 0xff},        
        .capacity = NOR_CAP_1M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 16},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (GD_SR_SRP0_MASK | GD_SR_BP2_MASK 
            | GD_SR_BP1_MASK | GD_SR_BP0_MASK),
        .block_protect = 
        {
            {(GD_SR_BP2_MASK | GD_SR_BP0_MASK), NOR_WP_1M_128}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},        
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "MD25D80",
#endif
    }, 
    
    //MD25D16
    {
        .chipid = {GIGADEVICE_ID2, 0x40, 0x15, 0xff},        
        .capacity = NOR_CAP_2M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 32},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .feature_mask = (REG_ATTR_WP | REG_ATTR_SR_8bit),
        .wp_clr_mask = (GD_SR_SRP0_MASK | GD_SR_BP2_MASK 
            | GD_SR_BP1_MASK | GD_SR_BP0_MASK),
        .block_protect = 
        {
            {(GD_SR_BP2_MASK | GD_SR_BP0_MASK), NOR_WP_2M_128}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0}, 
            {0, 0},             
        },        
        .reserved = {0},         
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "MD25D16",
#endif
    },  
    
    //MD25D32
    {
        .chipid = {GIGADEVICE_ID2, 0x40, 0x16, 0xff},        
        .capacity = NOR_CAP_4M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 64},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},         
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "MD25D32",
#endif
    },  
    
    //MD25D64
    {
        .chipid = {GIGADEVICE_ID2, 0x40, 0x17, 0xff},        
        .capacity = NOR_CAP_4M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 128},            
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},         
#ifdef  SUPPORT_SHOW_NORFLASH_NAME  
        .name = "MD25D64",
#endif
    },     

    /* ------------------------------ Generic ------------------------------ */
    //Unknown chip
    {
        .chipid = {0x11, 0x11, 0x11, 0x11},
        .capacity = NOR_CAP_4M,
        .block_erase =
        {
            {ERASE_BLOCK_64K, 64},
        },
        .dma_write = TRUE,
        .write_cmd = SPI_NOR_PAGE_PROGRAM,
        .erase_cmd = SPI_NOR_BLOCK_ERASE_64K,
        .erase_mode = 0,
        .reserved = {0},
#ifdef  SUPPORT_SHOW_NORFLASH_NAME
        .name = "Unknown chip",
#endif
    }
};
#pragma __PRQA_IGNORE_END__
