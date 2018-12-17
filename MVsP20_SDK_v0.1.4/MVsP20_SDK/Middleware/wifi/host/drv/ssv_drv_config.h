/*
*  (C) Copyright 2014-2016 Shenzhen South Silicon Valley microelectronics co.,limited
*
*  All Rights Reserved
*/


#ifndef _SSV_DRV_CONFIG_H_
#define _SSV_DRV_CONFIG_H_
#include "porting.h"
#ifndef STRING2
#define STRING2(x)	#x
#endif
#ifndef STRING
#define STRING(x)	STRING2(x)
#endif

//Interface select move to porting.h
//#define	SDRV_INCLUDE_SIM	1
//#define SDRV_INCLUDE_UART	0
//#define SDRV_INCLUDE_USB	0
//#define SDRV_INCLUDE_SDIO	1
//#ifdef _WIN32
//#define SDRV_INCLUDE_SPI	1
//#else
//#define SDRV_INCLUDE_SPI	0
//#endif
//#pragma message("========== ssv drv module config =============")
//#pragma message("SDRV_INCLUDE_SIM  = " STRING(SDRV_INCLUDE_SIM))
//#pragma message("SDRV_INCLUDE_UART = " STRING(SDRV_INCLUDE_UART))
//#pragma message("SDRV_INCLUDE_USB  = " STRING(SDRV_INCLUDE_USB))
//#pragma message("SDRV_INCLUDE_SDIO = " STRING(SDRV_INCLUDE_SDIO))
//#pragma message("SDRV_INCLUDE_SPI  = " STRING(SDRV_INCLUDE_SPI))
//#pragma message("===============================================")


#define DRV_NAME_SIM		"sim"
#define DRV_NAME_UART		"uart"
#define DRV_NAME_USB		"usb"
#define DRV_NAME_SDIO		"sdio"
#define DRV_NAME_SPI		"spi"

//#ifndef MAX_RECV_BUF		
//#define MAX_RECV_BUF		4096	// must be <= MAX_BUF
//#endif

#ifndef MAX_BUF
#define MAX_BUF				256
#endif

#endif /* _SSV_DRV_CONFIG_H_ */
