#include "ccd_inner.h"

ccd_op_i ccd_op_table[CCD_CMD_MAX] =
{
    (ccd_op_i)pa_op_poweron,
    (ccd_op_i)pa_op_powerdown,
    (ccd_op_i)pa_op_set_volume,
    (ccd_op_i)pa_op_get_volume,
    (ccd_op_i)pa_op_set_clock,
};

