/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#include <jos.h>
#include "uw_args.h"

#ifdef CONFIG_JSLAVE
static fd_instance_t fd_instances[] = {
#ifdef CONFIG_FD_MASS
    { DEVICE_TYPE_MSD, CONFIG_FD_MASS, NULL },
#endif
#ifdef CONFIG_FD_LOOPBACK
    { DEVICE_TYPE_LOOPBACK, CONFIG_FD_LOOPBACK, NULL },
#endif
#ifdef CONFIG_FD_RCP
    { DEVICE_TYPE_RCP, CONFIG_FD_RCP, NULL },
#endif
#ifdef CONFIG_FD_TEST
    { DEVICE_TYPE_TEST, CONFIG_FD_TEST, NULL },
#endif
#ifdef CONFIG_FD_SERIAL
    { DEVICE_TYPE_SERIAL, CONFIG_FD_SERIAL, NULL },
#endif
#ifdef CONFIG_FD_HID_KEYBOARD
    { DEVICE_TYPE_HID, CONFIG_FD_HID_KEYBOARD, NULL },
#endif
#ifdef CONFIG_FD_CDC_ACM
    { DEVICE_TYPE_CDC_ACM, CONFIG_FD_CDC_ACM, NULL },
#endif
#ifdef CONFIG_FD_CDC_ECM
    { DEVICE_TYPE_CDC_ECM, CONFIG_FD_CDC_ECM, NULL },
#endif
#ifdef CONFIG_FD_CDC_EEM
    { DEVICE_TYPE_CDC_EEM, CONFIG_FD_CDC_EEM, NULL },
#endif
#ifdef CONFIG_FD_CDC_NCM
    { DEVICE_TYPE_CDC_NCM, CONFIG_FD_CDC_NCM, NULL },
#endif
#ifdef CONFIG_FD_CDC_OBEX
    { DEVICE_TYPE_CDC_OBEX, CONFIG_FD_CDC_OBEX, NULL },
#endif
#ifdef CONFIG_FD_VIDEO
    { DEVICE_TYPE_VIDEO, CONFIG_FD_VIDEO, NULL },
#endif
#ifdef CONFIG_FD_DFU
    { DEVICE_TYPE_DFU, CONFIG_FD_DFU, NULL },
#endif
#ifdef CONFIG_FD_SICD
    { DEVICE_TYPE_SICD, CONFIG_FD_SICD, NULL },
#endif
#ifdef CONFIG_FD_VENDOR
    { DEVICE_TYPE_VENDOR, CONFIG_FD_VENDOR, loopback_init },
#endif
#ifdef CONFIG_FD_AUDIO
    { DEVICE_TYPE_AUDIO, CONFIG_FD_AUDIO, NULL },
#endif
#ifdef CONFIG_FD_CCID
    { DEVICE_TYPE_CCID, CONFIG_FD_CCID, NULL },
#endif
    { DEVICE_TYPE_NONE, 0, NULL }
};

static dev_info_t dev_info = {
    /* self_powered */         1,
#ifdef CONFIG_REMOTE_WAKEUP_TIMEOUT
    /* remote_wakeup */        1,
#else
    /* remote_wakeup */        0,
#endif
#ifdef CONFIG_JOTG
    /* srp_support */          1,
    /* hnp_support */          1,
#endif
    /* max_power */            50,
    /* vendor */               0x09D9,
    /* product */              0,
    /* release */              0x0001,
#if(defined(CONFIG_FD_CDC_RNDIS) || defined(CONFIG_FD_RNDIS_CE_PDD))
    /* dev_class */            2,
#else
    /* dev_class */            0,
#endif
    /* dev_sub_class */        0,
    /* dev_protocol */         0,
    /* product_name */         "JUSB Stack",
    /* product_manufacturer */ "Jungo",
    /* product_serial */       "0123456789ABCDEF"
};

static device_params_t dev_params[] = {
    {
        /* controller_idx */      0,
#ifdef CONFIG_SKIP_DEVICE_AUTO_ENABLE
        /* auto_enable */         0,
#else
        /* auto_enable */         1,
#endif
#ifndef CONFIG_FULL_SPEED_ONLY
        /* full_speed_only */     0,
#endif
        /* dev_info */            &dev_info,
        /* num_of_fd_instances */
        sizeof(fd_instances) / sizeof(fd_instance_t) - 1,
        /* fd_instances */        fd_instances
#ifdef CONFIG_JSLAVE_DUAL_CORES
    },
    {
        /* controller_idx */      1,
#ifdef CONFIG_SKIP_DEVICE_AUTO_ENABLE
        /* auto_enable */         0,
#else
        /* auto_enable */         1,
#endif
#ifndef CONFIG_FULL_SPEED_ONLY
        /* full_speed_only */     0,
#endif
        /* dev_info */            &dev_info,
        /* num_of_fd_instances */
        sizeof(fd_instances) / sizeof(fd_instance_t) - 1,
        /* fd_instances */        fd_instances
#endif
    }
};
#endif

/* Note - When compiling OTG and device, i.e. there is an OTG controller and a
 * separated Device controller, the init_opt should contain both the UW_INIT_OTG
 * bit and the UW_INIT_DEVICE bit, and the auto_enable flag in the
 * device_params for the OTG controller should be 0 */
#if defined(CONFIG_JOTG)
    #define _init_opt UW_INIT_OTG
#elif defined(CONFIG_JHOST) && defined(CONFIG_JSLAVE)
    #define _init_opt UW_INIT_HOST_DEVICE
#elif defined(CONFIG_JHOST)
    #define _init_opt UW_INIT_HOST
#else
    #define _init_opt UW_INIT_DEVICE
#endif

#ifdef CONFIG_MEMPOOL
#   ifndef CONFIG_MEMPOOL_SIZE
#       define CONFIG_MEMPOOL_SIZE (200 * 1024UL)
#   endif
#   ifdef CONFIG_MEMPOOL_EXTERN
        extern uint8_t _mempool[CONFIG_MEMPOOL_SIZE];
#   elif defined (CONFIG_MEMPOOL_ADDRESS)
        #define _mempool CONFIG_MEMPOOL_ADDRESS
#   else
        static uint8_t _mempool[CONFIG_MEMPOOL_SIZE];
#   endif
#else
    #define CONFIG_MEMPOOL_SIZE 0
    #define _mempool NULL
#endif

#ifdef CONFIG_POOL_DMA
#    if defined(CONFIG_POOL_DMA_PADDRESS) && \
       defined(CONFIG_POOL_DMA_VADDRESS) && \
       defined(CONFIG_POOL_DMA_SIZE)
#        define _pdma_pool CONFIG_POOL_DMA_PADDRESS
#        define _vdma_pool CONFIG_POOL_DMA_VADDRESS
#        define _dma_pool_size CONFIG_POOL_DMA_SIZE
#    elif defined(CONFIG_POOL_DMA_PADDRESS) || \
       defined(CONFIG_POOL_DMA_VADDRESS) || \
       defined(CONFIG_POOL_DMA_SIZE)
#        error "Please define CONFIG_POOL_DMA_PADDRESS, " \
             "CONFIG_POOL_DMA_VADDRESS and CONFIG_POOL_DMA_SIZE"
#    else
#        define _pdma_pool NULL
#        define _vdma_pool NULL
#        define _dma_pool_size 0
#     endif
#     ifdef CONFIG_POOL_DMA_CACHABLE
#         if defined(CONFIG_POOL_DMA_CACHABLE_PADDRESS) && \
            defined(CONFIG_POOL_DMA_CACHABLE_VADDRESS) && \
            defined(CONFIG_POOL_DMA_CACHABLE_SIZE)
#             define _vdma_cachable CONFIG_POOL_DMA_CACHABLE_VADDRESS
#             define _pdma_cachable CONFIG_POOL_DMA_CACHABLE_PADDRESS
#             define _dma_cachable_size CONFIG_POOL_DMA_CACHABLE_SIZE
#         elif defined(CONFIG_POOL_DMA_CACHABLE_PADDRESS) || \
            defined(CONFIG_POOL_DMA_CACHABLE_VADDRESS) || \
            defined(CONFIG_POOL_DMA_CACHABLE_SIZE)
#             error "Please define CONFIG_POOL_DMA_CACHABLE_PADDRESS, " \
                  "CONFIG_POOL_DMA_CACHABLE_VADDRESS and " \
                  "CONFIG_POOL_DMA_CACHABLE_SIZE"
#         else
#             define _vdma_cachable NULL
#             define _pdma_cachable NULL
#             define _dma_cachable_size 0
#         endif
#     endif
#endif

static uw_args_t uw_args = {
#ifdef CONFIG_MEMPOOL
    (void *)_mempool,
    CONFIG_MEMPOOL_SIZE,
#endif
#ifdef CONFIG_POOL_DMA
    (void *)_vdma_pool, (void *)_pdma_pool, _dma_pool_size,
#endif
#ifdef CONFIG_POOL_DMA_CACHABLE
    (void *)_vdma_cachable, (void *)_pdma_cachable, _dma_cachable_size,
#endif
    NULL, /* inactive_drivers */
    _init_opt,
#ifdef CONFIG_JSLAVE
#ifdef CONFIG_JSLAVE_DUAL_CORES
    2,
#else
    1,
#endif
    dev_params
#else
    0,
    NULL
#endif
};

#ifdef CONFIG_JSLAVE
#define FD_MASS_PRODUCT_ID              0x0001
#define FD_HID_KEYBOARD_PRODUCT_ID      0x0002
#define FD_SERIAL_PRODUCT_ID            0x0004
#define FD_LOOPBACK_PRODUCT_ID          0x0008
#define FD_CCID_PRODUCT_ID              0x0010
#define FD_CDC_ACM_PRODUCT_ID           0x0020
#define FD_CDC_ECM_PRODUCT_ID           0x0040
#define FD_CDC_OBEX_PRODUCT_ID          0x0080
#define FD_VIDEO_PRODUCT_ID             0x0100
#define FD_SICD_PRODUCT_ID              0x0200
#define FD_AUDIO_PRODUCT_ID             0x0400
#define FD_CDC_NCM_PRODUCT_ID           0x0800
#define FD_DFU_PRODUCT_ID               0x1000
#define FD_MASS_AUTOINSTALL_PRODUCT_ID  0x2000
#define FD_CDC_ACM_RNDIS_PRODUCT_ID     0x4000
#define FD_CDC_EEM_PRODUCT_ID           0x8000

/* Choose 0x0004 for Test Device PID. Note cannot compile test device with
 * Serial Function Driver */
#define FD_TEST_PRODUCT_ID              0x0004
# if defined(CONFIG_FD_SERIAL) && defined(CONFIG_FD_TEST)
# error Cannot run both functions (test device and serial)
#endif

/**
 * Function name:  get_product_id
 * Description: Calculate a product id according to the specific configuration
 * Parameters:
 *
 * Return value: product id
 * Scope: local
 **/
static uint16_t get_product_id(void)
{
    uint16_t product_id = 0;
    #ifdef CONFIG_FD_MASS
        #ifdef CONFIG_AUTOINSTALL
            product_id |= FD_MASS_AUTOINSTALL_PRODUCT_ID;
        #else
            product_id |= FD_MASS_PRODUCT_ID;
        #endif
    #endif
    #ifdef CONFIG_FD_HID_KEYBOARD
        product_id |= FD_HID_KEYBOARD_PRODUCT_ID;
    #endif
    #ifdef CONFIG_FD_SERIAL
        product_id |= FD_SERIAL_PRODUCT_ID;
    #endif
    #ifdef CONFIG_FD_LOOPBACK
        product_id |= FD_LOOPBACK_PRODUCT_ID;
    #endif
    #ifdef CONFIG_FD_CDC_ACM
        product_id |= FD_CDC_ACM_PRODUCT_ID;
    #endif
    #if(defined(CONFIG_FD_CDC_RNDIS) || defined(CONFIG_FD_RNDIS_CE_PDD))
        product_id |= FD_CDC_ACM_RNDIS_PRODUCT_ID;
    #endif
    #ifdef CONFIG_FD_CDC_EEM
        product_id |= FD_CDC_EEM_PRODUCT_ID;
    #endif
    #ifdef CONFIG_FD_CDC_ECM
        product_id |= FD_CDC_ECM_PRODUCT_ID;
    #endif
    #ifdef CONFIG_FD_CDC_NCM
        product_id |= FD_CDC_NCM_PRODUCT_ID;
    #endif
    #ifdef CONFIG_FD_CDC_OBEX
        product_id |= FD_CDC_OBEX_PRODUCT_ID;
    #endif
    #ifdef CONFIG_FD_TEST
        product_id |= FD_TEST_PRODUCT_ID;
    #endif
    #ifdef CONFIG_FD_VIDEO
        product_id |= FD_VIDEO_PRODUCT_ID;
    #endif
    #ifdef CONFIG_FD_SICD
        product_id |= FD_SICD_PRODUCT_ID;
    #endif
    #ifdef CONFIG_FD_DFU
        product_id |= FD_DFU_PRODUCT_ID;
    #endif
    #ifdef CONFIG_FD_AUDIO
        product_id |= FD_AUDIO_PRODUCT_ID;
    #endif
    #ifdef CONFIG_FD_CCID
        product_id |= FD_CCID_PRODUCT_ID;
    #endif

    return product_id;
}
#endif

uw_args_t *get_uw_args(void)
{
#ifdef CONFIG_JSLAVE
    dev_info.product = get_product_id();
#endif
    return &uw_args;
}

