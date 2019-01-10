#ifndef __PLAT_ADDR_MAP_BEST1200_H__
#define __PLAT_ADDR_MAP_BEST1200_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef ROM_SIZE
#define ROM_SIZE                                0x00009000
#endif

#ifndef RAM_SIZE
#define RAM_SIZE                                0x00020000
#endif

#include "best1000/plat_addr_map_best1000.h"

#undef ISPI_BASE
#define ISPI_BASE                               SPI_BASE

#ifdef __cplusplus
}
#endif

#endif
