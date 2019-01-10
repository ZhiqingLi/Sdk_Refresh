extern "C" {
#include "eventmgr.h"
#include "me.h"
#include "sec.h"
#include "btalloc.h"
#include "a2dp.h"
#include "avrcp.h"
#include "hf.h"
#include "cmsis_os.h"
#include "dip.h"
}

#include "app_dip.h"
#include "btapp.h"
#include "app_bt_func.h"

DipClient* p_dipdev[BT_DEVICE_NUM];
U8 info_index  =BT_DEVICE_NUM-1;


static bool app_dip_has_info(BtRemoteDevice *btDevice,U8* index)
{
    //static 
    for(U8 i=0;i<BT_DEVICE_NUM;i++)
    {

        if(OS_MemCmp(btDevice->bdAddr.addr,BD_ADDR_SIZE, p_dipdev[i]->PnpInfo.bdAddr.addr, BD_ADDR_SIZE))
        {
            *index = i;  
            return true;
        }    
    }
    //get the next index for save info
    info_index = (info_index==BT_DEVICE_NUM-1)?(BT_DEVICE_ID_1):(info_index+1);
    return false;
}
bool app_dip_is_support_aac(BtRemoteDevice *btDevice)
{
    U8 index;
    bool IsSupport = false;
    if(app_dip_has_info(btDevice,&index))
    {
        
        switch (p_dipdev[index]->PnpInfo.VIDSource)
        {
        case SRC_BT:
        {
            switch (p_dipdev[index]->PnpInfo.VID)
                {
                case SRC_BT_SANSUMG:
                    //IsSupport = true;
                    TRACE("GET sunsumg phone");
                    break;
                case SRC_BT_APPLE:
                    IsSupport=true;
                    TRACE("get apple phone");
                break;
                default:
                   TRACE("don't sure phone");
                break;
                }
        }
        break;

        case SRC_USB:
        {
            switch (p_dipdev[index]->PnpInfo.VID)
                {
                case SRC_USB_APPLE:
                    IsSupport= true;
                TRACE("get apple phone");
                break;
                default:
                TRACE("don't sure phone");
                break;
                }
        }
        break;
        default:
           TRACE("not test this case yet.it may get PNP error"); 
        break;
        }
    }
    TRACE("su = %d index=%d",IsSupport,index);
    return IsSupport;
}


//hf will call this fun 
// todo tidy HF profile about MSBC
bool app_dip_is_support_msbc(BtRemoteDevice *btDevice)
{
	TRACE("================================================================>>>>>");
    U8 index;
    bool IsSupport = false;
    if(app_dip_has_info(btDevice,&index))
    {
        
        switch (p_dipdev[index]->PnpInfo.VIDSource)
        {
        case SRC_BT:
        {
            switch (p_dipdev[index]->PnpInfo.VID)
                {
                case SRC_BT_SANSUMG:
                    //IsSupport = true;
                    TRACE("GET sunsumg phone");
                    break;
                case SRC_BT_APPLE:
                    IsSupport=true;
                    TRACE("get apple phone");
                break;
                default:
                   TRACE("don't sure phone");
                break;
                }
        }
        break;

        case SRC_USB:
        {
            switch (p_dipdev[index]->PnpInfo.VID)
                {
                case SRC_USB_APPLE:
                    IsSupport= true;
                TRACE("get apple phone");
                break;
                default:
                TRACE("don't sure phone");
                break;
                }
        }
        break;
        default:
           TRACE("not test this case yet.it may get PNP error"); 
        break;
        }
    }
    TRACE("su = %d index=%d",IsSupport,index);
    return IsSupport;
}

void app_dip_init(void * p_dip_client,void (*A2dpCb)(const DipPnpInfo*,void * ptr))
{
    DIP_init(A2dpCb);
    //Initialize struct of client
    for(U8 i=0;i<BT_DEVICE_NUM;i++)
    {
        p_dipdev[i] =(DipClient*)(p_dip_client+i*sizeof(DipClient));
        OS_MemSet((U8*)p_dipdev[i], 0, sizeof(DipClient));
    }

}
void app_dip_get_remote_info(BtRemoteDevice *btDevice)
{
#if DIP_DEVICE ==XA_ENABLED
    U8 i;
    if(app_dip_has_info(btDevice,&i))
    {
        TRACE("has the info %d",i);
    }else{
        app_bt_dip_QuryService(p_dipdev[info_index],btDevice);
    }
#else
  UNUSED_PARAMETER(btDevice);
#endif
    return;
}

