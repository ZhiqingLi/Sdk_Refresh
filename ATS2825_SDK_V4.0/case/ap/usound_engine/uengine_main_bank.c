#include "app_uengine.h"

/******************************************************************************/
/*!
 * \par  Description:
 * \0x00-0x0f转ASCII码
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      uengine_main.c
 * \note
 *******************************************************************************/
void rd_itoa(uint8 data, uint8 pos, uint8 *str)
{
    //uint8 data_low,data_high;
    uint8 data_cur = 0;

    if(pos != 0) {
        data_cur = (uint8)((data & 0xf0) >> 4);
    } else {
        data_cur = (data & 0x0f);
    }

    if(data_cur > 9) {
        data_cur += 55;
    } else {
        data_cur += 48;
    }
    *str = data_cur;
}
/******************************************************************************/
/*!
 * \par  Description:
 * \生成随机的SN并写VM。如果VM里面有SN，则直接使用VM中记录的SN
 * \param[in]    none
 * \param[out]   none
 * \return       none
 * \ingroup      uengine_main.c
 * \note
 *******************************************************************************/

void sn_random(uint8 *buf)
{
    uint8 i = 0;

    uint32 num1, num2 = 0;

    testval_t usb_sn = {0};

    sys_vm_read(&usb_sn, VM_API_FLAG, sizeof(testval_t));

    if(usb_sn.usb_sound_magic == 0x5A5A) {
        num1 = usb_sn.usb_sound_low;

        num2 = usb_sn.usb_sound_high;
    } else {
        num1 = sys_random();

        num2 = sys_random();

        usb_sn.usb_sound_low = num1;

        usb_sn.usb_sound_high = num2;

        usb_sn.usb_sound_magic = 0x5A5A;

        sys_vm_write(&usb_sn, VM_API_FLAG, sizeof(testval_t));
    }

    for(i = 0; i < 8; i++) {
        rd_itoa((uint8)(num1 >> i), 0, &buf[i]);
    }

    for(i = 0; i < 8; i++) {
        rd_itoa((uint8)(num2 >> i), 0, &buf[i + 8]);
    }
}
