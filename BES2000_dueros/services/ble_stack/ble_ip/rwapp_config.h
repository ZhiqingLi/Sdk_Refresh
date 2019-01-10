#ifndef _RWAPP_CONFIG_H_
#define _RWAPP_CONFIG_H_

/**
 ****************************************************************************************
 * @addtogroup app
 * @brief Application configuration definition
 *
 * @{
 ****************************************************************************************
 */


/*
 * INCLUDE FILES
 ****************************************************************************************
 */

/*
 * DEFINES
 ****************************************************************************************
 */

/******************************************************************************************/
/* -------------------------   BLE APPLICATION SETTINGS      -----------------------------*/
/******************************************************************************************/
// #define CFG_APP_HT
//#define CFG_APP_DATAPATH_SERVER
//#define CFG_APP_HR
//#define CFG_APP_HID
#if defined (__SMARTVOICE__)
#define CFG_APP_SMARTVOICE
#elif defined  (__AMA_VOICE__)
#define CFG_APP_AMA_VOICE
#elif defined  (__DMA_VOICE__)
#define CFG_APP_DMA_VOICE
#else
//#define CFG_APP_DATAPATH_SERVER
#endif

#if defined  (__AMA_VOICE__)
#define CFG_APP_SEC
#endif

#ifdef OTA_ENABLED
#define CFG_APP_OTA
#endif


/// Health Thermometer Application
#if defined(CFG_APP_HT)
#define BLE_APP_HT           1
#else // defined(CFG_APP_HT)
#define BLE_APP_HT           0
#endif // defined(CFG_APP_HT)

#if defined(CFG_APP_HR)
#define BLE_APP_HR           1
#else
#define BLE_APP_HR           0
#endif

/// Data Path Server Application
#if defined(CFG_APP_DATAPATH_SERVER)
#define BLE_APP_DATAPATH_SERVER           1
#else // defined(CFG_APP_DATAPATH_SERVER)
#define BLE_APP_DATAPATH_SERVER           0
#endif // defined(CFG_APP_DATAPATH_SERVER)

/// HID Application
#if defined(CFG_APP_HID)
#define BLE_APP_HID          1
#else // defined(CFG_APP_HID)
#define BLE_APP_HID          0
#endif // defined(CFG_APP_HID)

/// DIS Application
#if defined(CFG_APP_DIS)
#define BLE_APP_DIS          1
#else // defined(CFG_APP_DIS)
#define BLE_APP_DIS          0
#endif // defined(CFG_APP_DIS)

/// Time Application
#if defined(CFG_APP_TIME)
#define BLE_APP_TIME         1
#else // defined(CFG_APP_TIME)
#define BLE_APP_TIME         0
#endif // defined(CFG_APP_TIME)

/// Battery Service Application
#if (BLE_APP_HID)
#define BLE_APP_BATT          1
#else
#define BLE_APP_BATT          0
#endif // (BLE_APP_HID)

/// Security Application
#if (defined(CFG_APP_SEC) || BLE_APP_HID || defined(BLE_APP_AM0))
#define BLE_APP_SEC          1
#else // defined(CFG_APP_SEC)
#define BLE_APP_SEC          0
#endif // defined(CFG_APP_SEC)

/// Smart Voice Application
#if defined(CFG_APP_SMARTVOICE)
#define BLE_APP_SMARTVOICE           1
#else // defined(CFG_APP_SMARTVOICE)
#define BLE_APP_SMARTVOICE           0
#endif // defined(CFG_APP_SMARTVOICE)


/// AMA Voice Application
#if defined(CFG_APP_AMA_VOICE)
#define BLE_APP_AMA_VOICE           1
#else // defined(CFG_APP_AMA)
#define BLE_APP_AMA_VOICE           0
#endif // defined(CFG_APP_AMA)


/// DMA Voice Application
#if defined(CFG_APP_DMA_VOICE)
#define BLE_APP_DMA_VOICE           1
#else // defined(CFG_APP_DMA)
#define BLE_APP_DMA_VOICE           0
#endif // defined(CFG_APP_DMA)

/// OTA Application
#if defined(CFG_APP_OTA)
#define BLE_APP_OTA           1
#else // defined(CFG_APP_OTA)
#define BLE_APP_OTA           0
#endif // defined(CFG_APP_OTA)


/// @} rwapp_config

#endif /* _RWAPP_CONFIG_H_ */
