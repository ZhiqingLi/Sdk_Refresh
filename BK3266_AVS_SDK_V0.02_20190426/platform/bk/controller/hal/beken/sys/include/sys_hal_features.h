/***********************************************************************
 *
 * MODULE NAME:    sys_hal_features.c
 * PROJECT CODE:   Bluetooth
 * DESCRIPTION:    Platform Specific Feature Configuration
 * MAINTAINER:     Tom Kerwick
 * CREATION DATE:  10 Jan 2011
 *
 * LICENSE:
 *     This source code is copyright (c) 2011 Ceva Inc.
 *     All rights reserved.
 *
 ***********************************************************************/
#ifndef _PARTHUS_SYS_HAL_FEATURES_H
#define _PARTHUS_SYS_HAL_FEATURES_H

/*
 * Any platform specific configurable features (e.g. UART baud rate)
 * should go in this module.
 */

#define SYS_HAL_PRECALIBRATED_DELAY_MS 10000
#define SYS_HAL_PRECALIBRATED_DELAY_10US 100
#define SYS_HAL_PRECALIBRATED_DELAY_US    10

#define TRA_HCIT_UART_DEFAULT_BAUD_RATE /*230400*/ 115200
#define TRA_HCIT_UART_POLLED_TX_SUPPORTED 1
//#define HCIT_UART_TX_RX_STRESS_TEST

/*
 * Transport plane options
 */

#if (BUILD_TYPE==UNIT_TEST_BUILD)
#define TRA_HCIT_UART_SUPPORTED 1
#define TRA_HCIT_GENERIC_SUPPORTED 1
#define TRA_HCIT_USB_SUPPORTED 0
#define TRA_HCIT_PCMCIA_SUPPORTED 0
#define TRA_HCIT_UART_ONLY_SUPPORTED 1
#elif (BUILD_TYPE==GENERIC_BUILD)
#define TRA_HCIT_UART_SUPPORTED 0
#define TRA_HCIT_GENERIC_SUPPORTED 1
#define TRA_HCIT_USB_SUPPORTED 0
#define TRA_HCIT_PCMCIA_SUPPORTED 0
#define TRA_HCIT_UART_ONLY_SUPPORTED 0
#else // FLASH/DEBUG UART Build
#define TRA_HCIT_UART_SUPPORTED 1
#define TRA_HCIT_GENERIC_SUPPORTED 0
#define TRA_HCIT_USB_SUPPORTED 0
#define TRA_HCIT_PCMCIA_SUPPORTED 0
#define TRA_HCIT_UART_ONLY_SUPPORTED 1
#endif

#define HCIT_RECV_FIFO_TRIGGER 14
#define HCIT_TX_FIFO_LENGTH 120

/*

 * DEFINE OS RELATED CONFIGURATION OPTIONS
 */

#if (BUILD_TYPE==GENERIC_BUILD)
#define SYS_HAL_OS_EVENT_DRIVEN_HOST_CONTROLLER
#endif

#endif /* _PARTHUS_SYS_HAL_FEATURES_H */
