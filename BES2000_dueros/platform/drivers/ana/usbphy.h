#ifndef __USBPHY_H__
#define __USBPHY_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "stdbool.h"
#include "hal_analogif.h"
#include "hal_cmu.h"
#include "plat_addr_map.h"
#include CHIP_SPECIFIC_HDR(usbphy)

#ifdef ISPI_USBPHY_REG
#define usbphy_read(reg,val)            hal_analogif_reg_read(ISPI_USBPHY_REG(reg),val)
#define usbphy_write(reg,val)           hal_analogif_reg_write(ISPI_USBPHY_REG(reg),val)
#else
int usbphy_read(unsigned short reg, unsigned short *val);

int usbphy_write(unsigned short reg, unsigned short val);
#endif

void usbphy_open(void);

void usbphy_sleep(void);

void usbphy_wakeup(void);

#ifdef __cplusplus
}
#endif

#endif

