#ifndef __USB_DEV_DESC_H__
#define __USB_DEV_DESC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"

const uint8_t * cdc_deviceDesc(uint8_t type);

const uint8_t *cdc_stringDesc(uint8_t index);

const uint8_t * uaud_dev_desc(uint8_t type);

const uint8_t *uaud_string_desc(uint8_t index);

#ifdef __cplusplus
}
#endif

#endif

