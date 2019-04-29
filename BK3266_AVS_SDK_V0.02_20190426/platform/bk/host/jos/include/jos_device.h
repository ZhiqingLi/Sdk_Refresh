/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef _JOS_DEVICE_H_
#define _JOS_DEVICE_H_

/* Devices & Drivers Manipulation APIs */

typedef struct j_device_s *j_device_h;
/* Device Attach Arguments */
typedef void *j_device_aa_h;

typedef struct j_driver_s *j_driver_h;

typedef enum {
    J_DEVICE_TYPE_INVALID = -1,
    J_DEVICE_TYPE_CONTROLLER = 0,
    J_DEVICE_TYPE_USB,
    J_DEVICE_TYPE_TTY,
    J_DEVICE_TYPE_BLUETOOTH_UNIT,
    J_DEVICE_TYPE_BLUETOOTH_DEV,
    J_DEVICE_TYPE_BLUETOOTH_SERVICE,
    J_DEVICE_TYPE_CLASS_VIDEO,
    J_DEVICE_TYPE_CLASS_MASS,
    J_DEVICE_TYPE_CLASS_SERIAL,
    J_DEVICE_TYPE_CLASS_USB_AUDIO,      /* Audio class -- USB-specific */
    J_DEVICE_TYPE_CLASS_AUDIO,          /* Audio class -- generic */
    J_DEVICE_TYPE_TR_HID,
    J_DEVICE_TYPE_TR_IPOD,
    J_DEVICE_TYPE_CLASS_HID,
    J_DEVICE_TYPE_CLASS_NCM,
    J_DEVICE_TYPE_CLASS_AOAP,
    J_DEVICE_TYPE_CLASS_SICD
} j_device_type_t;

#ifdef J_DEBUG
const char *j_device_type_str(j_device_type_t type);
#endif

typedef enum {
    UMATCH_NONE,
    UMATCH_GENERIC,
    UMATCH_IFACECLASS_GENERIC,
    UMATCH_IFACECLASS,
    UMATCH_IFACECLASS_IFACESUBCLASS,
    UMATCH_IFACECLASS_IFACESUBCLASS_IFACEPROTO,
    UMATCH_VENDOR_IFACESUBCLASS,
    UMATCH_VENDOR_IFACESUBCLASS_IFACEPROTO,
    UMATCH_VENDOR_PRODUCT_CONF_IFACE,
    UMATCH_VENDOR_PRODUCT_REV_CONF_IFACE,
    UMATCH_DEVCLASS_GENERIC,
    UMATCH_DEVCLASS,
    UMATCH_DEVCLASS_DEVSUBCLASS,
    UMATCH_DEVCLASS_DEVSUBCLASS_DEVPROTO,
    UMATCH_VENDOR_DEVCLASS_DEVPROTO,
    UMATCH_VENDOR,
    UMATCH_VENDOR_PRODUCT,
    UMATCH_VENDOR_PRODUCT_REV
} umatch_t;

typedef enum {
    DRV_IOCTL_NONE,

    /* Host Power Management Related IOCTLs */
    DRV_IOCTL_PWR_CHILD_SUSPEND,
    DRV_IOCTL_PWR_CHILD_RESUME,
    DRV_IOCTL_PWR_HUB_REMOTE_WAKEUP,
    DRV_IOCTL_PWR_DEVICE_REATTACH,

    /* OTG Related IOCTLS */
    /* OTG Controller IOCTLs */
    DRV_IOCTL_DEV_SUSPEND,
    DRV_IOCTL_DEV_RESUME,
    DRV_IOCTL_DEV_RESET,
    DRV_IOCTL_DEV_HNP_ENBL,
    DRV_IOCTL_DEV_HNP_ALT_SUPPORTED,
    DRV_IOCTL_HOST_SUSPEND,
    DRV_IOCTL_HOST_RESUME,
    DRV_IOCTL_HOST_IS_PERIPH_SUPPORTED,
    DRV_IOCTL_HOST_REMOTE_CONNECTED,
    DRV_IOCTL_HOST_REMOTE_DISCONNECTED,
    DRV_IOCTL_HOST_PERIPH_PARTIAL_SUPPORTED,
    DRV_IOCTL_HOST_PERIPH_NOT_SUPPORTED,
    DRV_IOCTL_HOST_HW_CONNECT,
    DRV_IOCTL_HOST_HW_DISCONNECT,
    /* Device Controller IOCTLs */
    DRV_IOCTL_DEV_ENABLE,
    DRV_IOCTL_DEV_DISABLE,
    /* Host Controller IOCTLs */
#define HOST_PORT_ENABLE_NO_RESET 0
#define HOST_PORT_ENABLE_RESET    1
    DRV_IOCTL_HOST_PORT_ENABLE,
    DRV_IOCTL_HOST_PORT_DISABLE,
    DRV_IOCTL_HOST_PORT_SET_POWER,
    DRV_IOCTL_HOST_ENABLE_HNP,
    DRV_IOCTL_HOST_PORT_CHANGE,
    DRV_IOCTL_HOST_PORT_RESET,
    /* Transceiver Controller IOCTLs */
    DRV_IOCTL_TCD_GET_OPS,
    DRV_IOCTL_TCD_SET_CTX,

    /* Electrical Host Controller Tests IOCTLs */
    DRV_IOCTL_SET_ETEST_MODE,

    /* Bluetooth IOCTLs */
    DRV_IOCTL_BT_SERVICE_CONNECT,
    DRV_IOCTL_BT_SERVICE_DISCONNECT,
    DRV_IOCTL_BT_SERVER_START,
    DRV_IOCTL_BT_SERVER_STOP,

    DRV_IOCTL_LAST
} drv_ioctl_t;

#define drv_ioctl_str(n) (\
    (n) == DRV_IOCTL_NONE ? "NONE" :\
    (n) == DRV_IOCTL_PWR_CHILD_SUSPEND ? "PWR_CHILD_SUSPEND" :\
    (n) == DRV_IOCTL_PWR_CHILD_RESUME ? "PWR_CHILD_RESUME" :\
    (n) == DRV_IOCTL_PWR_HUB_REMOTE_WAKEUP ? "PWR_HUB_REMOTE_WAKEUP" :\
    (n) == DRV_IOCTL_PWR_DEVICE_REATTACH ? "PWR_DEVICE_REATTACH" :\
    (n) == DRV_IOCTL_DEV_SUSPEND ? "DEV_SUSPEND" :\
    (n) == DRV_IOCTL_DEV_RESUME ? "DEV_RESUME" :\
    (n) == DRV_IOCTL_DEV_RESET ? "DEV_RESET" :\
    (n) == DRV_IOCTL_DEV_HNP_ENBL ? "DEV_HNP_ENBL" :\
    (n) == DRV_IOCTL_DEV_HNP_ALT_SUPPORTED ? "DEV_HNP_ALT_SUPPORTED" :\
    (n) == DRV_IOCTL_HOST_SUSPEND ? "HOST_SUSPEND" :\
    (n) == DRV_IOCTL_HOST_RESUME ? "HOST_RESUME" :\
    (n) == DRV_IOCTL_HOST_IS_PERIPH_SUPPORTED ? "HOST_IS_PERIPH_SUPPORTED" :\
    (n) == DRV_IOCTL_HOST_REMOTE_CONNECTED ? "HOST_REMOTE_CONNECTED" :\
    (n) == DRV_IOCTL_HOST_REMOTE_DISCONNECTED ? "HOST_REMOTE_DISCONNECTED" :\
    (n) == DRV_IOCTL_HOST_PERIPH_PARTIAL_SUPPORTED ? \
    "HOST_PERIPH_PARTIAL_SUPPORTED" :\
    (n) == DRV_IOCTL_HOST_PERIPH_NOT_SUPPORTED ? "HOST_PERIPH_NOT_SUPPORTED" :\
    (n) == DRV_IOCTL_HOST_HW_CONNECT ? "HOST_HW_CONNECT" :\
    (n) == DRV_IOCTL_HOST_HW_DISCONNECT ? "HOST_HW_DISCONNECT" :\
    (n) == DRV_IOCTL_DEV_ENABLE ? "DEV_ENABLE" :\
    (n) == DRV_IOCTL_DEV_DISABLE ? "DEV_DISABLE" :\
    (n) == DRV_IOCTL_HOST_PORT_ENABLE ? "HOST_PORT_ENABLE" :\
    (n) == DRV_IOCTL_HOST_PORT_DISABLE ? "HOST_PORT_DISABLE" :\
    (n) == DRV_IOCTL_HOST_PORT_SET_POWER ? "HOST_PORT_SET_POWER" :\
    (n) == DRV_IOCTL_HOST_ENABLE_HNP ? "HOST_ENABLE_HNP" :\
    (n) == DRV_IOCTL_HOST_PORT_CHANGE ? "HOST_PORT_CHANGE" :\
    (n) == DRV_IOCTL_HOST_PORT_RESET ? "HOST_PORT_RESET" :\
    (n) == DRV_IOCTL_TCD_GET_OPS ? "TCD_GET_OPS" :\
    (n) == DRV_IOCTL_TCD_SET_CTX ? "TCD_SET_CTX" :\
    (n) == DRV_IOCTL_SET_ETEST_MODE ? "SET_ETEST_MODE" :\
    (n) == DRV_IOCTL_BT_SERVICE_CONNECT ? "BT_SERVICE_CONNECT" :\
    (n) == DRV_IOCTL_BT_SERVICE_DISCONNECT ? "BT_SERVICE_DISCONNECT" :\
    (n) == DRV_IOCTL_BT_SERVER_START ? "BT_SERVER_START" :\
    (n) == DRV_IOCTL_BT_SERVER_STOP ? "BT_SERVER_STOP" :\
    "unknown")


typedef void *owner_cbs_h;
typedef void *creator_methods_h;

typedef result_t (*device_own_t)(j_device_h dev, owner_cbs_h owner_cbs,
    creator_methods_h *creator_methods);
typedef void (*device_disown_t)(j_device_h dev);

result_t j_device_own(j_device_h device, j_driver_h owner_drv_h,
    owner_cbs_h owner_cbs, creator_methods_h *creator_methods);
void j_device_disown(j_device_h device);

j_device_h j_device_get_parent(j_device_h dev);
j_device_h j_device_get_first_child(j_device_h device);
j_device_h j_device_get_next_child(j_device_h device, j_device_h curr_child);

j_device_h j_driver_get_first_owned_device(j_driver_h drv);
j_device_h j_driver_get_next_owned_device(j_device_h dev);

void j_device_set_creator_ctx(j_device_h dev, void *ctx);
void *j_device_get_creator_ctx(j_device_h dev);
void *j_device_get_owner_ctx(j_device_h dev);

char *j_device_get_desc(j_device_h dev);
void j_device_set_desc(j_device_h dev, const char *desc);
void *j_device_get_ivars(j_device_h dev);
void j_device_set_ivars(j_device_h dev, void *ivars);

j_device_h j_device_add(j_device_type_t type, j_device_h parent,
    j_device_aa_h attach_args, device_own_t own, device_disown_t disown);
void j_device_delete(j_device_h device);

j_device_aa_h j_device_get_aa(j_device_h dev);
j_device_type_t j_device_get_type(j_device_h dev);

uint32_t j_device_get_unit(j_device_h dev);
const char *j_device_get_nameunit(j_device_h dev);

result_t j_device_probe_and_attach(j_device_h dev);
bool_t j_device_is_owned(j_device_h dev);
j_driver_h j_device_get_owner(j_device_h device);
result_t j_device_detach(j_device_h dev);

result_t j_device_suspend(j_device_h dev);
result_t j_device_resume(j_device_h dev);
result_t j_device_ioctl(j_device_h dev, drv_ioctl_t ioctl, void *arg);

/* Register a Controller with parameters sent as ivars */
result_t add_generic_controller_ex(os_bus_h bus, int_t id, void **handle,
    void *args);
result_t del_generic_controller_ex(void *handle);

os_bus_h j_controller_get_bus(j_device_h dev);

typedef result_t (*driver_init_t)(void);
typedef void     (*driver_uninit_t)(void);
typedef result_t (*driver_func_t)(j_device_h dev);
typedef umatch_t (*driver_match_t)(j_device_h dev);
typedef result_t (*driver_ioctl_t)(j_device_h dev, drv_ioctl_t ioctl,
    void *arg);

typedef struct {
    driver_match_t              match;
    driver_func_t               attach;
    driver_func_t               detach;
    driver_func_t               suspend;
    driver_func_t               resume;
    driver_ioctl_t              ioctl;
    driver_uninit_t             uninit;
} driver_methods_t;

result_t j_driver_register(j_device_type_t type, driver_methods_t *methods,
    const char *name, uint16_t size, j_driver_h *drv_h);
void j_driver_unregister(j_driver_h drv_h);

#ifdef J_DEBUG
void j_device_assert_type(j_device_h device, j_device_type_t type, char *file,
    int_t line);
#define J_DEVICE_ASSERT_TYPE(device, type) \
    j_device_assert_type(device, type, __FILE__, __LINE__)
#else
#define J_DEVICE_ASSERT_TYPE(device, type) \
    do {} while (0)
#endif

/* Common way to declare a driver */
#define DRIVER_COMMON_DECLARE(type, name, size, init_func, uninit_func)     \
                                                                            \
    void UW_CONCAT(name, _uninit)(void);                                    \
    static j_driver_h UW_CONCAT(name, _drv_h) = NULL;                       \
    static driver_methods_t UW_CONCAT(name, _methods) = {                   \
        UW_CONCAT(name, _probe),                                            \
        UW_CONCAT(name, _attach),                                           \
        UW_CONCAT(name, _detach),                                           \
        UW_CONCAT(name, _suspend),                                          \
        UW_CONCAT(name, _resume),                                           \
        UW_CONCAT(name, _ioctl),                                            \
        UW_CONCAT(name, _uninit),                                           \
    };                                                                      \
                                                                            \
    result_t UW_CONCAT(name, _init)(void)                                   \
    {                                                                       \
        result_t rc;                                                        \
                                                                            \
        if((init_func) != NULL)                                            \
        {                                                                   \
            rc = (init_func)();                                             \
            if(rc)                                                         \
                return rc;                                                  \
        }                                                                   \
                                                                            \
        rc = j_driver_register(type, &UW_CONCAT(name, _methods), #name,     \
            size, &UW_CONCAT(name, _drv_h));                                \
        if(rc && (uninit_func) != NULL)                                    \
            (uninit_func)();                                                \
                                                                            \
        return rc;                                                          \
    }                                                                       \
    void UW_CONCAT(name, _uninit)(void)                                     \
    {                                                                       \
        if((uninit_func) != NULL)                                          \
        {                                                                   \
            (uninit_func)();                                                \
        }                                                                   \
                                                                            \
        j_driver_unregister(UW_CONCAT(name, _drv_h));                       \
    }

/* Declare Bluetooth driver with custom init/uninit function */
#define DRIVER_BLUETOOTH_DECLARE_CUSTOM_INIT(name, init_func, uninit_func)  \
    DRIVER_COMMON_DECLARE(J_DEVICE_TYPE_BLUETOOTH_SERVICE, name,            \
        sizeof(struct UW_CONCAT(name, _softc)),                             \
        (driver_init_t)init_func, (driver_uninit_t)uninit_func)

/* Common way to declare a Bluetooth driver */
#define DRIVER_BLUETOOTH_DECLARE(name)                                      \
    DRIVER_BLUETOOTH_DECLARE_CUSTOM_INIT(name, NULL, NULL)

/* Declare usb driver with custom init/uninit function */
#define DRIVER_USB_DECLARE_CUSTOM_INIT(name, init_func, uninit_func)        \
    DRIVER_COMMON_DECLARE(J_DEVICE_TYPE_USB, name,                          \
        sizeof(struct UW_CONCAT(name, _softc)),                             \
        (driver_init_t)init_func, (driver_uninit_t)uninit_func)

/* Common way to declare a usb driver */
#define DRIVER_USB_DECLARE(name)                                            \
    DRIVER_USB_DECLARE_CUSTOM_INIT(name, NULL, NULL)

/* Declare controller driver with custom init/uninit function */
#define DRIVER_CONTROLLER_DECLARE_CUSTOM_INIT(name, size, init_func,        \
    uninit_func)                                                            \
    DRIVER_COMMON_DECLARE(J_DEVICE_TYPE_CONTROLLER, name, size,             \
        (driver_init_t)init_func, (driver_uninit_t)uninit_func)

/* Common way to declare a controller driver */
#define DRIVER_CONTROLLER_DECLARE(name, size)                               \
    DRIVER_CONTROLLER_DECLARE_CUSTOM_INIT(name, size, NULL, NULL)

#endif

