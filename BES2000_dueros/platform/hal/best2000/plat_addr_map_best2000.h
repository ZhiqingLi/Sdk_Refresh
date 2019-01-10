#ifndef __PLAT_ADDR_MAP_BEST2000_H__
#define __PLAT_ADDR_MAP_BEST2000_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ROM_BASE                                0x00000000

#ifndef ROM_SIZE
#define ROM_SIZE                                0x0000C000
#endif

#define RAMRET_BASE                             0x40090000
#define RAMXRET_BASE                            RAMRET_BASE

#define RAMRET_SIZE                             0x00000400

#define RAM0_BASE                               0x20000000
#define RAMX0_BASE                              0x00200000
#define RAM1_BASE                               0x20020000
#define RAMX1_BASE                              0x00220000
#define RAM_BASE                                RAM0_BASE
#define RAMX_BASE                               RAMX0_BASE

#ifndef RAM_SIZE
#ifdef LARGE_RAM
#if defined(__SBC_FUNC_IN_ROM_VBEST2000__) && !defined(__SBC_FUNC_IN_ROM_VBEST2000_ONLYSBC__)
#define RAM_SIZE                                0x0004b000
#else
#define RAM_SIZE                                0x00054000
#endif
#else
#define RAM_SIZE                                0x00028000
#endif
#endif

#define FLASH_BASE                              0x3C000000
#define FLASH_NC_BASE                           0x38000000
#define FLASHX_BASE                             0x0C000000
#define FLASHX_NC_BASE                          0x08000000

#define PSRAM_BASE                              0x34000000
#define PSRAM_NC_BASE                           0x30000000
#define PSRAMX_BASE                             0x1C000000
#define PSRAMX_NC_BASE                          0x18000000

#define ICACHE_CTRL_BASE                        0x07FFE000
#define DCACHE_CTRL_BASE                        0x2FFFE000

#define CMU_BASE                                0x40000000
#define GPIO_BASE                               0x40001000
#define MCU_WDT_BASE                            0x40002000
#define PWM_BASE                                0x40003000
#define TIMER0_BASE                             0x40004000
#define I2C0_BASE                               0x40005000
#define SPI_BASE                                0x40006000
#define SPILCD_BASE                             0x40007000
#define UART0_BASE                              0x40008000
#define UART1_BASE                              0x40009000
#define BTPCM_BASE                              0x4000A000
#define I2S_BASE                                0x4000B000
#define SPDIF0_BASE                             0x4000C000
#define ISPI_BASE                               0x4000D000
#define UART2_BASE                              0x4000E000
#define SPDIF1_BASE                             0x4000F000
#define GPIOAUX_BASE                            0x40010000
#define TRANSQW_BASE                            0x40011000
#define SPIPHY_BASE                             0x40012000

#define AON_CMU_BASE                            0x40080000
#define AON_GPIO_BASE                           0x40081000
#define AON_WDT_BASE                            0x40082000
#define AON_PWM_BASE                            0x40083000
#define AON_TIMER_BASE                          0x40084000
#define AON_PMU_BASE                            0x40085000
#define AON_GPIOAUX_BASE                        0x40086000
#define AON_IOMUX_BASE                          0x40087000

#define SDIO_BASE                               0x40100000
#define SDMMC_BASE                              0x40110000
#define AUDMA_BASE                              0x40120000
#define GPDMA_BASE                              0x40130000
#define FLASH_CTRL_BASE                         0x40140000
#define PSRAM_CTRL_BASE                         0x40150000
#define USB_BASE                                0x40180000
#define DSD_BASE                                0x401D0000

#define CAMERA_BASE                             0x40200000
#define CODEC_BASE                              0x40300000
#define FFT_BASE                                0x40400000

#define BT_SUBSYS_BASE                          0xA0000000
#define BT_RAM_BASE                             0xC0000000
#define BT_UART_BASE                            0xD0300000
#define BT_CMU_BASE                             0xD0330000

#define WIFI_SUBSYS_BASE                        0x60000000
#define WIFI_TRANSQM_BASE                       0x9000A000

#define IOMUX_BASE                              AON_IOMUX_BASE
#define TRANSQ0_BASE                            TRANSQW_BASE
#define TRANSQ0_PEER_BASE                       WIFI_TRANSQM_BASE
#define TIMER1_BASE                             AON_TIMER_BASE
#define WDT_BASE                                AON_WDT_BASE

/* For linker scripts */

#define VECTOR_SECTION_SIZE                     256
#define REBOOT_PARAM_SECTION_SIZE               64
#define ROM_BUILD_INFO_SECTION_SIZE             40
#define ROM_EXPORT_FN_SECTION_SIZE              128

/* For ROM system_setup() */

#define DEFAULT_CRYSTAL_FREQ                    26000000

#ifdef __cplusplus
}
#endif

#endif
