/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef _CONTROLLERS_H_
#define _CONTROLLERS_H_

/* Hosts types */
#define controller_type_xxx                     0x1000
#define controller_type_isp176x                 0x1001
#define controller_type_isp1362                 0x1002
#define controller_type_isp116x_local           0x1003
#define controller_type_isp116x_pci             0x1103
#define controller_type_ehci_local              0x1004
#define controller_type_ehci_pci                0x1104
#define controller_type_ohci_local              0x1005
#define controller_type_ohci_pci                0x1105
#define controller_type_uhci_local              0x1006
#define controller_type_uhci_pci                0x1106
#define controller_type_ahci_local              0x1007
#define controller_type_ahci_pci                0x1107
#define controller_type_td243hc                 0x1008
#define controller_type_ci13hc                  0x1009
#define controller_type_syn_hshc                0x100A
#define controller_type_td243hc_rev2            0x100B
#define controller_type_musbhshc                0x100C
#define controller_type_sh7723                  0x100D
#define controller_type_xhci_local              0x100E
#define controller_type_xhci_pci                0x110E
#define controller_type_mb9x                    0x100F
#define controller_type_musbmhdrc               0x1010

/* Devices types */
#define controller_type_netchip                 0x2001
#define controller_type_atmel                   0x2002
#define controller_type_tdi_4x                  0x2003
#define controller_type_isp_1582                0x2004
#define controller_type_omapv1030               0x2005
#define controller_type_td243fc                 0x2006
#define controller_type_s3c2413                 0x2007
#define controller_type_str91x                  0x2008
#define controller_type_udphs                   0x2009
#define controller_type_td243fc_rev2            0x2010
#define controller_type_musbhsfc                0x2011
#define controller_type_syn_hsfc                0x2012
#define controller_type_ci13dc                  0x2013
#define controller_type_isp1183                 0x2014
#define controller_type_lpc2378                 0x2015
#define controller_type_net_2272                0x2016
#define controller_type_syn_fsfc                0x2017
/* Dummy value for template DCD */
#define controller_type_dcd_name                0x20FF

/* Transceiver Controllers */
#define controller_type_twl3029                 0x3001

/* OTG Controllers */
#define controller_type_tdi_4x_otg              0x4003
#define controller_type_omapv1030_otg           0x4004
#define controller_type_syn_hs_otg              0x4005

#define CONTROLLER_TYPE_HOST                    1
#define CONTROLLER_TYPE_DEVICE                  2
#define CONTROLLER_TYPE_TRANSCEIVER             3
#define CONTROLLER_TYPE_OTG                     4

#define CONTROLLER_GET_TYPE(ctrl) ((ctrl >> 12) & 0xF)

#define CONTROLLER_IS_HOST(ctrl) \
    (CONTROLLER_GET_TYPE(ctrl) == CONTROLLER_TYPE_HOST)
#define CONTROLLER_IS_DEVICE(ctrl) \
    (CONTROLLER_GET_TYPE(ctrl) == CONTROLLER_TYPE_DEVICE)
#define CONTROLLER_IS_OTG(ctrl) \
    (CONTROLLER_GET_TYPE(ctrl) == CONTROLLER_TYPE_OTG)
#define CONTROLLER_IS_TRANSCEIVER(ctrl) \
        (CONTROLLER_GET_TYPE(ctrl) == CONTROLLER_TYPE_TRANSCEIVER)

#endif

