#ifndef __HAL_IOMUXIP_H__
#define __HAL_IOMUXIP_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"
#include "plat_addr_map.h"
#include "reg_iomuxip_best1000.h"

#define IOMUXIP_REG_BASE IOMUX_BASE

#define iomuxip_read32(b,a) \
     (*(volatile uint32_t *)(b+a))

#define iomuxip_write32(v,b,a) \
     ((*(volatile uint32_t *)(b+a)) = v)

#ifdef __cplusplus
}
#endif

#endif /* __HAL_IOMUXIP_H__ */
