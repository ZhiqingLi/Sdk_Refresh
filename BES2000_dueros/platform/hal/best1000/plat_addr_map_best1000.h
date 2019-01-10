#ifndef __PLAT_ADDR_MAP_BEST1000_H__
#define __PLAT_ADDR_MAP_BEST1000_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ROM_BASE                                0x00000000

#ifndef ROM_SIZE
#define ROM_SIZE                                0x00010000
#endif

#define RAM0_BASE                               0x20000000
#define RAMX0_BASE                              0x10000000
#define RAM1_BASE                               0x20020000
#define RAMX1_BASE                              0x10020000
#define RAM_BASE                                RAM0_BASE
#define RAMX_BASE                               RAMX0_BASE

#ifndef RAM_SIZE
#define RAM_SIZE                                0x00028000
#endif

#define FLASH_BASE                              0x6C000000
#define FLASH_NC_BASE                           0x68000000
#define FLASHX_BASE                             0x0C000000
#define FLASHX_NC_BASE                          0x08000000

#define PSRAM_BASE                              0x64000000
#define PSRAM_NC_BASE                           0x60000000
#define PSRAMX_BASE                             0x1C000000
#define PSRAMX_NC_BASE                          0x18000000

#ifdef FPGA
#define ICACHE_CTRL_BASE                        0x00081000
#else
#define ICACHE_CTRL_BASE                        0x00011000
#endif

#define DCACHE_CTRL_BASE                        0x20029000

#define CMU_BASE                                0x40000000
#define GPIO_BASE                               0x40001000
#define WDT_BASE                                0x40002000
#define PWM_BASE                                0x40003000
#define TIMER0_BASE                             0x40004000
#define I2C0_BASE                               0x40005000
#define SPI_BASE                                0x40006000
#define SPILCD_BASE                             0x40007000
#define UART0_BASE                              0x40008000
#define UART1_BASE                              0x40009000
#define CODEC_BASE                              0x4000A000
#define BTPCM_BASE                              0x4001A000
#define I2S_BASE                                0x4001B000
#define ISPI_BASE                               0x4001E000
#define IOMUX_BASE                              0x4001F000
#define SPDIF0_BASE                             0x40020000

#define SDIO_BASE                               0x40100000
#define SDMMC_BASE                              0x40110000
#define AUDMA_BASE                              0x40120000
#define GPDMA_BASE                              0x40130000
#define FLASH_CTRL_BASE                         0x40140000
#define PSRAM_CTRL_BASE                         0x40150000
#define DPD_RX_BASE                             0x40160000
#define DPD_TX_BASE                             0x40170000
#define USB_BASE                                0x40180000

#define BT_SUBSYS_BASE                          0xA0000000
#define BT_RAM_BASE                             0xC0000000
#define BT_UART_BASE                            0xD0300000
#define BT_CMU_BASE                             0xD0330000

/* For linker scripts */

#define VECTOR_SECTION_SIZE                     208
#define REBOOT_PARAM_SECTION_SIZE               48
#define ROM_BUILD_INFO_SECTION_SIZE             24
#define ROM_EXPORT_FN_SECTION_SIZE              128

/* For ROM system_setup() */

#define DEFAULT_CRYSTAL_FREQ                    26000000

#ifdef __cplusplus
}
#endif

#endif
