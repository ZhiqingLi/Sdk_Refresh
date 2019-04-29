/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef _UW_ARGS_H_
#define _UW_ARGS_H_

typedef enum {
    DEVICE_TYPE_NONE = 0,
    DEVICE_TYPE_MSD,
    DEVICE_TYPE_HID,
    DEVICE_TYPE_SERIAL,
    DEVICE_TYPE_LOOPBACK,
    DEVICE_TYPE_RCP,
    DEVICE_TYPE_TEST,
    DEVICE_TYPE_CDC_ACM,
    DEVICE_TYPE_CDC_ECM,
    DEVICE_TYPE_CDC_EEM,
    DEVICE_TYPE_CDC_NCM,
    DEVICE_TYPE_CDC_OBEX,
    DEVICE_TYPE_VIDEO,
    DEVICE_TYPE_DFU,
    DEVICE_TYPE_CCID,
    DEVICE_TYPE_SICD,
    DEVICE_TYPE_AUDIO,
    DEVICE_TYPE_VENDOR
} j_fd_type_t;

typedef enum {
    DRIVER_TYPE_OHCI_PCI = 0,
    DRIVER_TYPE_OHCI_LOCAL,
    DRIVER_TYPE_MUSBHSHC,
    DRIVER_TYPE_MUSBMHDRC,
    DRIVER_TYPE_SH7723,
    DRIVER_TYPE_UHCI_PCI,
    DRIVER_TYPE_XHCI,
    DRIVER_TYPE_EHCI_PCI,
    DRIVER_TYPE_EHCI_LOCAL,
    DRIVER_TYPE_ISP176X,
    DRIVER_TYPE_ISP116X_PCI,
    DRIVER_TYPE_ISP116X_LOCAL,
    DRIVER_TYPE_ISP1362,
    DRIVER_TYPE_TD243HC,
    DRIVER_TYPE_TD243HC_REV2,
    DRIVER_TYPE_CI13HC,
    DRIVER_TYPE_SYN_HSHC,
    DRIVER_TYPE_MB9X,
    DRIVER_TYPE_XHCI_TEST,
    DRIVER_TYPE_NETCHIP,
    DRIVER_TYPE_ATMEL,
    DRIVER_TYPE_TDI_4X,
    DRIVER_TYPE_ISP_1582,
    DRIVER_TYPE_OMAPV1030,
    DRIVER_TYPE_TD243FC,
    DRIVER_TYPE_S3C2413,
    DRIVER_TYPE_STR91X,
    DRIVER_TYPE_UDPHS,
    DRIVER_TYPE_TD243FC_REV2,
    DRIVER_TYPE_SYN_HSFC,
    DRIVER_TYPE_MUSBHSFC,
    DRIVER_TYPE_CI13DC,
    DRIVER_TYPE_ISP1183,
    DRIVER_TYPE_LPC2378,
    DRIVER_TYPE_NET_2272,
    DRIVER_TYPE_DCD_NAME,
    DRIVER_TYPE_TDI_4X_OTG,
    DRIVER_TYPE_SYN_HS_OTG,
    DRIVER_TYPE_UHUB,
    DRIVER_TYPE_UCOMPDEV,
    DRIVER_TYPE_UGEN,
    DRIVER_TYPE_UMASS,
    DRIVER_TYPE_JHID,
    DRIVER_TYPE_JKEYBOARD,
    DRIVER_TYPE_JMOUSE,
    DRIVER_TYPE_JJOYSTICK,
    DRIVER_TYPE_JCDC_ECM,
    DRIVER_TYPE_JCDC_NCM,
    DRIVER_TYPE_JCDC_NCM_APP,
    DRIVER_TYPE_JCDC_QMI,
    DRIVER_TYPE_JCDC_EEM,
    DRIVER_TYPE_JCDC_ACM,
    DRIVER_TYPE_JSERIAL,
    DRIVER_TYPE_JLOOPBACK,
    DRIVER_TYPE_JAOAP,
    DRIVER_TYPE_AOAP_APP,
    DRIVER_TYPE_UTEST,
    DRIVER_TYPE_JPRINTER,
    DRIVER_TYPE_TWL3029,
    DRIVER_TYPE_JETEST,
    DRIVER_TYPE_JAUDIO,
    DRIVER_TYPE_JAUDIO_SAMPLE,
    DRIVER_TYPE_JAUDIO_LOOPBACK,
    DRIVER_TYPE_JRT,
    DRIVER_TYPE_JETH,
    DRIVER_TYPE_JSICD,
    DRIVER_TYPE_JSICD_USB,
    DRIVER_TYPE_JSICD_IPC_KS,
    DRIVER_TYPE_JVIDEO,
    DRIVER_TYPE_VIDEO_SAMPLE,
    DRIVER_TYPE_JIPOD,
    DRIVER_TYPE_JIPOD_IAP,
    DRIVER_TYPE_JICCD,
    DRIVER_TYPE_JHWA_DATA,
    DRIVER_TYPE_JHWA_RF,
    DRIVER_TYPE_JWCBAF,
    DRIVER_TYPE_UBT,
    DRIVER_TYPE_BT_UART,
    DRIVER_TYPE_BT_HIDEV,
    DRIVER_TYPE_BT_SERDEV,
    DRIVER_TYPE_JBLOCK_DEV,
    DRIVER_TYPE_JRCP,
    DRIVER_TYPE_MC_USB_AUDIO,
    DRIVER_TYPE_WINDRV,
    DRIVER_TYPE_JAUTO_INSTALL,
    DRIVER_TYPE_BT_MAP,
    DRIVER_TYPE_LAST
} j_driver_type_t;

typedef int_t uw_init_opt_t;
/* Initialize USBware host stack */
#define UW_INIT_HOST            (1 << 0)
/* Initialize USBware device stack */
#define UW_INIT_DEVICE          (1 << 1)
/* Initialize USBware both host and device stacks */
#define UW_INIT_HOST_DEVICE     (UW_INIT_HOST | UW_INIT_DEVICE)
/* Initialize USBware OTG stack (includes also host and device stacks) */
#define UW_INIT_OTG             (1 << 2)

typedef result_t (*fd_init_func_t)(void *ctx);

typedef struct {
    j_fd_type_t type;             /* Function driver type */
    uint8_t count;                /* Number of instances of 'type' */
    fd_init_func_t init_func;     /* Optional parameter for vendor specific
                                     function drivers - requires type to be
                                     DEVICE_TYPE_VENDOR */
} fd_instance_t;

typedef struct {
    uint_t self_powered : 1;
    uint_t remote_wakeup: 1;
#ifdef CONFIG_JOTG
    uint_t srp_support: 1;
    uint_t hnp_support: 1;
#endif

    uint8_t max_power; /* In 2mAmp units */

    uint16_t vendor;
    uint16_t product;
    uint16_t release;

    uint8_t dev_class;
    uint8_t dev_sub_class;
    uint8_t dev_protocol;

    char *product_name;
    char *product_manufacturer;
    char *product_serial;
} dev_info_t;

typedef struct {
    uint8_t controller_idx;     /* Zero based */
    bool_t auto_enable;
#ifndef CONFIG_FULL_SPEED_ONLY
    bool_t full_speed_only;
#endif
    dev_info_t *dev_info;
    uint8_t num_of_fd_instances;
    fd_instance_t *fd_instances;
} device_params_t;

typedef struct {
    /* For static memory management */
#ifdef CONFIG_MEMPOOL
    void *mem_buf;
    uint32_t mem_buf_size;
#endif

#ifdef CONFIG_POOL_DMA
    void *dma_vbuf;
    void *dma_pbuf;
    uint32_t dma_buf_size;
#ifdef CONFIG_POOL_DMA_CACHABLE
    void *dma_cachable_vbuf;
    void *dma_cachable_pbuf;
    uint32_t dma_cachable_size;
#endif
#endif

    j_driver_type_t *inactive_drivers;

    uw_init_opt_t init_opt;

    /* Relevant only for device stack */
    uint8_t num_of_devices;
    device_params_t *devices;
} uw_args_t;

uw_args_t *get_uw_args(void);
uw_init_opt_t j_get_init_opt(void);

#endif

