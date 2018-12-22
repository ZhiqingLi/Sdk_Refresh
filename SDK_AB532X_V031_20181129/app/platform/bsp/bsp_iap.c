#include "include.h"

//设置苹果的充电电流
void iphone_power_set(u16 *val, u16 *index)
{
    *val = IPHONE_POWER_VAL;
    *index = IPHONE_POWER_INDEX;
}
