#ifndef __NORFLASH_CFG_H__
#define __NORFLASH_CFG_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "plat_types.h"
#include "hal_cmu.h"

struct norflash_cfg_struct_t {
    uint8_t neg_phase:1;
    uint8_t pos_neg:1;
    uint8_t cmdquad:1;
    uint8_t samdly:3;

    uint8_t div; /* least 2 */

    uint8_t dualmode:1;
    uint8_t holdpin:1;
    uint8_t wprpin:1;
    uint8_t quadmode:1;
    enum HAL_CMU_FREQ_T mod_clk:4;

    uint8_t spiruen:3;
    uint8_t spirden:3;

    uint8_t dualiocmd;
    uint8_t rdcmd;
    uint8_t frdcmd;
    uint8_t qrdcmd; /* quad io cmd */
};

#ifdef __cplusplus
}
#endif

#endif

