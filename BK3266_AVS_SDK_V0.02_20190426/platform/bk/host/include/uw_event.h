/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef _DEV_MONITOR_H_
#define _DEV_MONITOR_H_

#include <uw_types.h>

typedef void *mdev_t;
typedef void *j_event_param_h;

#ifdef CONFIG_USER_SPACE
typedef struct {
    uint16_t vendor;
    uint16_t product;
    uint16_t version;

    uint8_t address;
    uint8_t config;
    uint8_t depth;
    uint8_t port;

    uint8_t speed;
    uint8_t self_powered;
    uint16_t power;

    char description[1];
} ju_device_info_t;
#else /* Kernel Space or Monolithic */
typedef struct {
    uint8_t     dev_class;
    uint8_t     subclass;
    uint8_t     protocol;
    uint16_t    vendor;
    uint16_t    product;
    uint16_t    version;

    const char  *name;
    char  *desc;

    uint8_t     address;       /* USB address */
    uint8_t     config;        /* Current configuration # */
    uint8_t     depth;         /* Distance from root hub */
    uint8_t     port;          /* Port number (1..n) in hub */

#define USB_SPEED_LOW           1
#define USB_SPEED_FULL          2
#define USB_SPEED_HIGH          3
    uint8_t     speed;         /* low/full/high speed */
    bool_t      self_powered;  /* Self/Bus powered */
    uint16_t    power;         /* mA the device uses */

    mdev_t      parent;        /* Our parent hub */

    uint8_t    index_manufacturer;  /* Manufacturer string descriptor index */
    uint8_t    index_product;       /* Product string descriptor index */
    uint8_t    index_serial_number; /* Serial number string descriptor index */
} j_dev_desc_t;
#endif

/* Max User/Kernel messages buffer size */
#define J_KU_EVT_MAX_MSG_LEN   256

/* User Kernel Requests */
#define KU_REQ_GET_DEVICE_INFO  1
#define KU_REQ_RESET_DEVICE     2

/* Pack parameters into buffer - 'dest' must be (uint8_t *) */
#define J_KU_EVT_PACK(dest, src, si, total) \
    do { \
        j_memcpy((dest), (src), (si)); \
        (dest) += (si); \
        (total) += (si); \
    } while (0)

#define J_KU_EVT_UNPACK(dest, src, si) \
    do { \
        j_memcpy((dest), (src), (si)); \
        (src) += (si); \
    } while (0)

typedef enum {
    /***********************
     *  Host Notifications *
     ***********************/

    /* HOST -- general host stack notifications */
    EVT_NOTIFY_HOST_CONNECT,
    EVT_NOTIFY_HOST_SUSPEND,
    EVT_NOTIFY_HOST_RESUME,

    /* HOST -- connected device notifications */
    EVT_NEW_DEVICE,
    EVT_NEW_DEVICE_READY,
    EVT_NEW_DEVICE_NO_DRIVER,
    EVT_NEW_DEVICE_ATTACHING,
    EVT_NEW_DEVICE_ATTACH_DENIED,

    EVT_DEVICE_DETACHING,
    EVT_DEVICE_DETACHED,
    EVT_DEVICE_DISCONNECTED,
    EVT_DEVICE_SUSPENDED,
    EVT_DEVICE_RESUMED,
    EVT_DEVICE_CONFIG_SETTING,
    EVT_DEVICE_CONFIG_SET,

    /* HOST -- composite driver notifications */
    EVT_UCOMP_ATTACH_SUBDEV,
    EVT_UCOMP_DETACHING_SUBDEV,
    EVT_UCOMP_DETACHED_SUBDEV,

    /* HOST -- device errors notifications */
    EVT_ERROR_CANNOT_OPEN_EP0,
    EVT_ERROR_CANNOT_ALLOC_ADDR,
    EVT_ERROR_CANNOT_SET_ADDR,
    EVT_ERROR_CANNOT_GET_FIRST_DESC,
    EVT_ERROR_BAD_DEV_DESC,
    EVT_ERROR_BAD_BOS_DESC,
    EVT_ERROR_CANNOT_GET_CONFIG,
    EVT_ERROR_CANNOT_ATTACH,
    EVT_ERROR_CANNOT_SET_CONFIG,
    EVT_ERROR_RESET_FAILED,
    EVT_ERROR_OVERCURRENT,

    /* HOST -- hub errors notifications */
    EVT_ERROR_HUB_TOO_DEEP,
    EVT_ERROR_HUB_GET_DESC,
    EVT_ERROR_HUB_BUSPWR_ATTACHED_TO_BUSPWR,
    EVT_ERROR_HUB_NO_INTERFACE,
    EVT_ERROR_HUB_NO_ENDPOINT,
    EVT_ERROR_HUB_BAD_ENDPOINT,
    EVT_ERROR_HUB_CANNOT_OPEN_PIPE,

    /* HOST -- bus power notifications */
    EVT_NOTIFY_DRIVE_VBUS_ON,
    EVT_NOTIFY_DRIVE_VBUS_OFF,

    /* HOST -- class-specific notifications */
    EVT_NETWORK_IFC_ATTACH,
    EVT_NETWORK_IFC_DETACH,

    /* Block Device notifications */
    EVT_BLOCK_DEV_READY,
    EVT_BLOCK_DEV_REMOVED,
    /* Wi-Fi */
    EVT_ERROR_WIFI_CIPHER,

    /* HOST - Port specific notifications*/
    EVT_PORT_VDK_MOUNT,
    EVT_PORT_VDK_UNMOUNT,

    /*************************
     *  Device Notifications *
     *************************/

    /* DEVICE -- device stack notifications */
    EVT_NOTIFY_DEVICE_CONNECT,
    EVT_NOTIFY_DEVICE_DISCONNECT,
    EVT_NOTIFY_DEVICE_SUSPEND,
    EVT_NOTIFY_DEVICE_RESUME,
    EVT_NOTIFY_DEVICE_RESUME_COMPLETED,
    EVT_NOTIFY_DEVICE_REMOTE_WAKEUP,
    EVT_NOTIFY_DEVICE_CONFIGURED,
    EVT_NOTIFY_DEVICE_UNCONFIGURED,
    EVT_NOTIFY_DEVICE_RESET,

    /*********************
     * OTG Notifications *
     *********************/

    /* OTG -- general notifications */
    EVT_NOTIFY_OTG_IDLE,
    EVT_NOTIFY_OTG_BUSY,

    /***************************
     * Mediacore notifications *
     ***************************/

    EVT_MC_NEW_DEVICE_AUTH_FAILED,

    EVT_LAST
} j_event_t;

result_t j_event_dispatch(j_event_t msg, mdev_t dev, j_event_param_h param);

typedef result_t (*j_event_handler_h)(j_event_t msg, mdev_t dev,
    j_event_param_h param);

result_t j_event_handler_register(j_event_handler_h handler);
void j_event_handler_unregister(j_event_handler_h handler);

/* Host only APIs */
#if defined(CONFIG_JHOST)
result_t j_get_dev_desc(mdev_t dev, j_dev_desc_t *desc);
result_t j_reset_device(mdev_t dev);
char *j_get_string(mdev_t mdev, uint8_t si);
#elif defined(CONFIG_USER_SPACE)
ju_device_info_t *ju_create_device_info(mdev_t dev);
void ju_destroy_device_info(ju_device_info_t *device_info);
result_t ju_reset_device(mdev_t dev);
#endif

/* Debug only APIs */
#ifdef J_DEBUG
const char *j_event_str(j_event_t msg);
# if defined(CONFIG_JHOST)
void j_dump_dev_desc(mdev_t dev);
#elif defined(CONFIG_USER_SPACE)
void ju_dump_device_info(mdev_t dev);
#endif
#endif

#endif

