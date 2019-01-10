#include "cmsis_os.h"
#include "hal_trace.h"
#include "hal_sleep.h"
#include "bt_drv_interface.h"
#include "intersyshci.h"
#include "apps.h"
#include "app_factory.h"
#include "app_factory_bt.h"
#include "app_utils.h"
#include "nvrecord.h"
#include "nvrecord_dev.h"
#include "pmu.h"
#include "tgt_hardware.h"

// for bt
extern "C" {
#include "conmgr.h"
#include "btalloc.h"
#include "sys/mei.h"
}

#define APP_FACT_CPU_WAKE_LOCK              HAL_CPU_WAKE_LOCK_USER_3

#ifdef __FACTORY_MODE_SUPPORT__
U8 inquiry_buff[] = {0x01, 0x72, 0x77, 0xb0, 0x18, 0x57, 0x60, 0x01, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x00, 0x00};


static CmgrHandler app_factorymode_cmgrHandler;

static void app_factorymode_bt_inquiry_buff_update(void)
{
    BT_BD_ADDR flsh_dongle_addr;
    int ret = -1;

    ret = nvrec_dev_get_dongleaddr(&flsh_dongle_addr);
    if(0 == ret)
    {
        memcpy((void *)&inquiry_buff[1],(void *)flsh_dongle_addr.addr,BD_ADDR_SIZE);
        DUMP8("0x%02x ", &inquiry_buff[2], BD_ADDR_SIZE);
    }
}

static void app_factorymode_CmgrCallback(CmgrHandler *cHandler,
                              CmgrEvent    Event,
                              BtStatus     Status)
{
    APP_FACTORY_TRACE("%s cHandler:%x Event:%d status:%d", __func__, cHandler, Event, Status);
    if (Event == CMEVENT_DATA_LINK_CON_CNF){
        if (Status == BT_STATUS_SUCCESS){
            APP_FACTORY_TRACE("connect ok");
            app_factorymode_result_set(true);
            CMGR_RemoveDataLink(cHandler);

        }else{
            APP_FACTORY_TRACE("connect failed");
            app_factorymode_result_set(false);
        }
    }

    if (Event == CMEVENT_DATA_LINK_DIS){
        if (Status == BT_STATUS_SUCCESS){
            APP_FACTORY_TRACE("disconnect ok");
        }else{
            APP_FACTORY_TRACE("disconnect failed");
        }
    }
}

static void app_factorymode_bt_InquiryResult_add(void)
{
    U16          i;
    U8*          p;
    U8 len = 15;
    BOOL rssi = false, extended  = false;
    U8* parm = (U8*)inquiry_buff;


    /* Found one or more devices. Report to clients */
    APP_FACTORY_TRACE("%s len:%d rssi:%d extended:%d", __func__, len, rssi, extended);
    DUMP8("0x%02x ", parm, len);
    Report(("ME: Inquiry result num resp = %d, bdaddr %x %x %x %x %x %x\n",
            parm[0], parm[1], parm[2], parm[3], parm[4], parm[5], parm[6]));
    for (i = 0; i < (U16)(parm[0]*14); i+=14) {
        /* Get the parameters */
        MEC(btEvent).p.inqResult.inqMode = INQ_MODE_NORMAL;
        OS_MemCopy(MEC(btEvent).p.inqResult.bdAddr.addr,parm+1+i,6);
        MEC(btEvent).p.inqResult.psi.psRepMode = parm[7+i];
        MEC(btEvent).p.inqResult.psPeriodMode = parm[8+i];
        p = parm + 9 + i;
        /* PS Mode is no longer provided (parm[9+i]) */
        MEC(btEvent).p.inqResult.psi.psMode = 0;

        /* Reserved parameter not present in RSSI or Extended responses */
        if (!rssi && !extended) {
            p++;
        }

        MEC(btEvent).p.inqResult.classOfDevice =
            (U32)(p[0]) | ((U32)(p[1])<<8) | ((U32)(p[2])<<16);
        p += 3;

        MEC(btEvent).p.inqResult.psi.clockOffset = LEtoHost16(p);
        p += 2;
        Report(("ME:Inquiry result clockOffset = %x\n",
            MEC(btEvent).p.inqResult.psi.clockOffset));

        if (rssi) {
            MEC(btEvent).p.inqResult.inqMode = INQ_MODE_RSSI;
            MEC(btEvent).p.inqResult.rssi = (S8)(*p++);
        } else {
            MEC(btEvent).p.inqResult.rssi = BT_INVALID_RSSI;
        }

        if (extended) {
            MEC(btEvent).p.inqResult.inqMode = INQ_MODE_EXTENDED;
            OS_MemCopy(MEC(btEvent).p.inqResult.extInqResp, p, 240);
            p+= 240;
        } else {
            OS_MemSet(MEC(btEvent).p.inqResult.extInqResp, 0, 240);
        }

        /* Report this one to the clients */
        MEC(btEvent).errCode = 0;
        MEC(btEvent).eType = BTEVENT_INQUIRY_RESULT;
        MeReportResults(BEM_INQUIRY_RESULT);
    }
}

void app_factorymode_bt_create_connect(void)
{
    BtStatus status;
    BT_BD_ADDR *bdAddr = (BT_BD_ADDR*)(inquiry_buff+1);


    status = CMGR_CreateDataLink(&app_factorymode_cmgrHandler, bdAddr);
    APP_FACTORY_TRACE("%s:%d", __func__, status);
}

void app_factorymode_bt_init_connect(void)
{
    CMGR_RegisterHandler(&app_factorymode_cmgrHandler, app_factorymode_CmgrCallback);
    app_factorymode_bt_inquiry_buff_update();
    app_factorymode_bt_InquiryResult_add();

    //    BtStatus status = ME_Inquiry(BT_IAC_GIAC, 8, 0);
    //    APP_FACTORY_TRACE("connect_test status:%d", status);
}


extern osTimerId app_bt_accessmode_timer;
void app_factorymode_bt_signalingtest(APP_KEY_STATUS *status, void *param)
{
    dev_addr_name devinfo;
    APP_FACTORY_TRACE("%s",__func__);
#ifdef __WATCHER_DOG_RESET__
    app_wdt_close();
#endif
    hal_cpu_wake_lock(APP_FACT_CPU_WAKE_LOCK);
    app_stop_10_second_timer(APP_PAIR_TIMER_ID);
    app_stop_10_second_timer(APP_POWEROFF_TIMER_ID);
    osTimerStop(app_bt_accessmode_timer);

    app_status_indication_set(APP_STATUS_INDICATION_TESTMODE);
    pmu_sleep_en(0);
    BESHCI_Close();
    btdrv_hciopen();
    btdrv_sleep_config(0);
//    btdrv_hcioff();
    osDelay(2000);
//    btdrv_testmode_start();
//    btdrv_sleep_config(0);
    btdrv_ins_patch_test_init();
    btdrv_feature_default();
//    btdrv_test_mode_addr_set();

    devinfo.btd_addr = bt_addr;
    devinfo.ble_addr = ble_addr;
    devinfo.localname = BT_LOCAL_NAME;
    nvrec_dev_localname_addr_init(&devinfo);
    btdrv_write_localinfo((char *)devinfo.localname, strlen(devinfo.localname) + 1, devinfo.btd_addr);

    btdrv_enable_dut();
}

//int app_battery_stop(void);
#include "app_battery.h"
void app_factorymode_bt_nosignalingtest(APP_KEY_STATUS *status, void *param)
{
    dev_addr_name devinfo;
    APP_FACTORY_TRACE("%s",__func__);
#ifdef __WATCHER_DOG_RESET__
    app_wdt_close();
#endif
    hal_cpu_wake_lock(APP_FACT_CPU_WAKE_LOCK);
    app_stop_10_second_timer(APP_PAIR_TIMER_ID);
    app_stop_10_second_timer(APP_POWEROFF_TIMER_ID);
    app_status_indication_set(APP_STATUS_INDICATION_TESTMODE1);
    app_battery_stop();
    pmu_sleep_en(0);
    BESHCI_Close();
    btdrv_hciopen();
#ifdef BT_50_FUNCTION

#else    
    btdrv_sleep_config(0);
#endif
//    btdrv_hcioff();
    osDelay(2000);
//    btdrv_testmode_start();
 //   btdrv_sleep_config(0);

#ifdef BT_50_FUNCTION
#else
    btdrv_feature_default();

    devinfo.btd_addr = bt_addr;
    devinfo.ble_addr = ble_addr;
    devinfo.localname = BT_LOCAL_NAME;
    nvrec_dev_localname_addr_init(&devinfo);
    btdrv_write_localinfo((char *)devinfo.localname, strlen(devinfo.localname) + 1, devinfo.btd_addr);	
#endif
    btdrv_hcioff();
    btdrv_uart_bridge_loop();
}
int app_factorymode_bt_xtalcalib_proc(void)
{
    uint32_t capval = 0x80;
    int nRet;

    APP_FACTORY_TRACE("%s",__func__);
    hal_cpu_wake_lock(APP_FACT_CPU_WAKE_LOCK);
//  nvrec_dev_get_xtal_fcap((unsigned int*)&capval);
    APP_FACTORY_TRACE("calib default, capval:%d", capval);
    //while(1){
    capval = 0x80;
    bt_drv_calib_open();
//  bt_drv_calib_rxonly_porc();
    nRet = bt_drv_calib_result_porc(&capval);
    bt_drv_calib_close();
    TRACE("!!!!!!!!!!!!!!!!!!!!!!!!!!!calib ret:%d, capval:%d", nRet, capval);
    //}
    if (!nRet)
        nvrec_dev_set_xtal_fcap((unsigned int)capval);

    return nRet;
}

void app_factorymode_bt_xtalcalib(APP_KEY_STATUS *status, void *param)
{
    APP_FACTORY_TRACE("%s",__func__);
    app_factorymode_bt_xtalcalib_proc();
}

#endif

