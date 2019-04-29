#ifndef __HAL_PRINT_H__
#define __HAL_PRINT_H__

#include "driver_beken_includes.h"
#include "app_beken_includes.h"
//#include "beken_external.h"

#define DUMP8(str, buf, cnt)        hal_print_dump(str, sizeof(uint8_t), cnt, buf)
#define DUMP16(str, buf, cnt)       hal_print_dump(str, sizeof(uint16_t), cnt, buf)
#define DUMP32(str, buf, cnt)       hal_print_dump(str, sizeof(uint32_t), cnt, buf)

#endif // __HAL_PRINT_H__

