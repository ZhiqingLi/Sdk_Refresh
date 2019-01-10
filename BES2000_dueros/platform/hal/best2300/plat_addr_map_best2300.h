#ifndef __PLAT_ADDR_MAP_BEST2300_H__
#define __PLAT_ADDR_MAP_BEST2300_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ROM_BASE                                0x00000000

#ifndef ROM_SIZE
#define ROM_SIZE                                0x0000C000
#endif

#define RAMRET_BASE                             0x200A0000
#define RAMXRET_BASE                            0x002A0000

#ifndef RAMRET_SIZE
#define RAMRET_SIZE                             0x00020000
#endif

#define RAM0_BASE                               0x20000000
#define RAMX0_BASE                              0x00200000
#define RAM1_BASE                               0x20040000
#define RAMX1_BASE                              0x00240000
#define RAM2_BASE                               0x20080000
#define RAMX2_BASE                              0x00280000
#define RAM_BASE                                RAM0_BASE
#define RAMX_BASE                               RAMX0_BASE

#ifndef RAM_SIZE
#ifdef LARGE_RAM
#define RAM_SIZE                                0x000C0000
#else
#define RAM_SIZE                                0x000A0000
#endif
#endif

#if defined(ROM_BUILD) && defined(MCU_SLEEP_POWER_DOWN)
#undef RAM_BASE
#undef RAMX_BASE
#undef RAM_SIZE
#define RAM_BASE                                RAMRET_BASE
#define RAMX_BASE                               RAMXRET_BASE
#define RAM_SIZE                                RAMRET_SIZE
#endif

#define FLASH_BASE                              0x3C000000
/* NEVER ACCESS UNCACHED ADDRESS due to cache logic bug */
/* #define FLASH_NC_BASE                           0x38000000 */
#define FLASH_NC_BASE                           FLASH_BASE
#define FLASHX_BASE                             0x0C000000
/* NEVER ACCESS UNCACHED ADDRESS due to cache logic bug */
/* #define FLASHX_NC_BASE                          0x08000000 */
#define FLASHX_NC_BASE                          FLASHX_BASE

#define ICACHE_CTRL_BASE                        0x07FFE000
/* No data cache */

#define CMU_BASE                                0x40000000
#define MCU_WDT_BASE                            0x40001000
#define MCU_TIMER0_BASE                         0x40002000
#define MCU_TIMER1_BASE                         0x40003000
#define MCU_TIMER2_BASE                         0x40004000
#define I2C0_BASE                               0x40005000
#define I2C1_BASE                               0x40006000
#define SPI_BASE                                0x40007000
#define SPILCD_BASE                             0x40008000
#define ISPI_BASE                               0x40009000
#define SPIPHY_BASE                             0x4000A000
#define UART0_BASE                              0x4000B000
#define UART1_BASE                              0x4000C000
#define UART2_BASE                              0x4000D000
#define BTPCM_BASE                              0x4000E000
#define I2S_BASE                                0x4000F000
#define SPDIF0_BASE                             0x40010000

#define AON_CMU_BASE                            0x40080000
#define AON_GPIO_BASE                           0x40081000
#define AON_WDT_BASE                            0x40082000
#define AON_PWM_BASE                            0x40083000
#define AON_TIMER_BASE                          0x40084000
#define AON_PMU_BASE                            0x40085000
#define AON_IOMUX_BASE                          0x40086000

#define SDMMC_BASE                              0x40110000
#define AUDMA_BASE                              0x40120000
#define GPDMA_BASE                              0x40130000
#define FLASH_CTRL_BASE                         0x40140000
#define BT_DUMP_BASE                            0x40150000
#define I2C_SLAVE_BASE                          0x40160000
#define SENSOR_HUB_BASE                         0x40170000
#define USB_BASE                                0x40180000

#define CODEC_BASE                              0x40300000
#define FFT_BASE                                0x40400000

#define BT_SUBSYS_BASE                          0xA0000000
#define BT_RAM_BASE                             0xC0000000
#define BT_UART_BASE                            0xD0300000
#define BT_CMU_BASE                             0xD0330000

#define IOMUX_BASE                              AON_IOMUX_BASE
#define GPIO_BASE                               AON_GPIO_BASE
#define PWM_BASE                                AON_PWM_BASE
#ifdef MCU_SLEEP_POWER_DOWN
#define TIMER0_BASE                             AON_TIMER_BASE
#else
#define TIMER0_BASE                             MCU_TIMER0_BASE
#endif
#define TIMER1_BASE                             MCU_TIMER1_BASE
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
