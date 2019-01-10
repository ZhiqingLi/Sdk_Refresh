//#include "btapp.h"
#include "dip.h"
#include "me.h"




#define SRC_BT (1)
#define SRC_USB (2)
#define SRC_BT_SANSUMG (0x0075)
#define SRC_BT_APPLE   (0X004c)
#define SRC_USB_APPLE   (0x05AC)

void app_dip_init(void * p_dip_client,void (*A2dpCb)(const DipPnpInfo*,void * ptr));



void app_dip_get_remote_info(BtRemoteDevice *btDevice);
bool app_dip_is_support_msbc(BtRemoteDevice *btDevice);
bool app_dip_is_support_aac(BtRemoteDevice *btDevice);

