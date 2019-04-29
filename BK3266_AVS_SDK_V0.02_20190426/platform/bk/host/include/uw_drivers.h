/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef _UW_DRIVERS_H_
#define _UW_DRIVERS_H_

#include <jos.h>

/* Host Controller drivers */
result_t ehci_pci_init(void);
result_t ehci_local_init(void);
result_t ohci_pci_init(void);
result_t ohci_local_init(void);
result_t uhci_pci_init(void);
result_t ahci_local_init(void);
result_t isp176x_init(void);
result_t isp116x_pci_init(void);
result_t isp116x_local_init(void);
result_t isp1362_init(void);
result_t td243hc_init(void);
result_t td243hc_rev2_init(void);
result_t ci13hc_init(void);
result_t syn_hshc_init(void);
result_t musbhshc_init(void);
result_t musbmhdrc_init(void);
result_t sh7723_init(void);
result_t xhci_init(void);
result_t xhci_test_init(void);
result_t mb9x_init(void);

/* Device Controller drivers */
result_t netchip_init(void);
result_t atmel_init(void);
result_t tdi_4x_init(void);
result_t isp_1582_init(void);
result_t omapv1030_init(void);
result_t td243fc_init(void);
result_t td243fc_rev2_init(void);
result_t s3c2413_init(void);
result_t str91x_init(void);
result_t udphs_init(void);
result_t syn_fs_init(void);
result_t syn_hsfc_init(void);
result_t musbhsfc_init(void);
result_t ci13dc_init(void);
result_t isp1183_init(void);
result_t lpc2378_init(void);
result_t net_2272_init(void);
result_t dcd_name_init(void);

/* OTG Controllers drivers */
result_t tdi_4x_otg_init(void);
result_t syn_hs_otg_init(void);

/* Class drivers */
result_t uhub_init(void);
result_t ucompdev_init(void);
result_t ugen_init(void);
result_t umass_init(void);
result_t jkbd_init(void);
result_t jhid_dev_init(void);
result_t jhid_api_init(void);
result_t jkeyboard_init(void);
result_t jmouse_init(void);
result_t jjoystick_init(void);
result_t jcdc_ecm_init(void);
result_t jcdc_ncm_init(void);
result_t ncm_app_init(void);
result_t jcdc_qmi_init(void);
result_t jcdc_eem_init(void);
result_t jcdc_acm_init(void);
result_t serial_dev_init(void);
result_t jloopback_init(void);
result_t jprinter_init(void);
result_t jaudio_init(void);
result_t audio_sample_init(void);
result_t audio_loopback_init(void);
result_t jrt_init(void);
result_t jeth_init(void);
result_t jvideo_init(void);
result_t video_sample_init(void);
result_t jsicd_init(void);
result_t jsicd_usb_init(void);
result_t jsicd_ipc_ks_init(void);
result_t jiccd_init(void);
result_t jhwa_data_init(void);
result_t jhwa_rf_init(void);
result_t ubt_init(void);
result_t bt_uart_init(void);
result_t jwcbaf_init(void);
result_t jblock_dev_init(void);
result_t jrcp_init(void);
result_t jaoap_init(void);
result_t aoap_app_init(void);
result_t jauto_install_init(void);

/* Function drivers */
result_t mass_storage_init(void *ctx);
result_t loopback_init(void *ctx);
result_t rcp_init(void *ctx);
result_t fd_test_init(void *ctx);
result_t serial_init(void *ctx);
result_t hid_kbd_init(void *ctx);
result_t cdc_init_acm(void *ctx);
result_t cdc_init_ecm(void *ctx);
result_t cdc_init_eem(void *ctx);
result_t cdc_init_ncm(void *ctx);
result_t cdc_init_obex(void *ctx);
result_t video_init(void *ctx);
result_t dfu_init(void *ctx);
result_t sicd_init(void *ctx);
result_t fd_audio_init(void *ctx);
result_t ccid_init(void *ctx);

result_t windrv_init(void);

/* Transceiver drivers */
result_t twl3029_init(void);

/* Embedded HS host electrical test driver */
result_t jetest_init(void);

/* MediaCore drivers */
result_t mc_usb_audio_init(void);

/* Bluetooth drivers */
result_t bthidev_init(void);
result_t bt_map_init(void);

result_t utest_init(void);

#endif

