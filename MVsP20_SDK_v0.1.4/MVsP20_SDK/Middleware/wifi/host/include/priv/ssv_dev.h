/*
*  (C) Copyright 2014-2016 Shenzhen South Silicon Valley microelectronics co.,limited
*
*  All Rights Reserved
*/


/*
 * This file contains definitions and data structures specific
 * to South Silicon Valley Cabrio 802.11 NIC. It contains the
 * Device Information structure struct cabrio_private..
 */
#ifndef _CABRIO_DEV_H_
#define _CABRIO_DEV_H_
#if 0

/* Hardware Offload Engine ID */
#define M_ENG_CPU                       0x00
#define M_ENG_HWHCI                     0x01
//#define M_ENG_FRAG                    0x02
#define M_ENG_EMPTY                     0x02
#define M_ENG_ENCRYPT                   0x03
#define M_ENG_MACRX                     0x04
#define M_ENG_MIC                       0x05
#define M_ENG_TX_EDCA0                  0x06
#define M_ENG_TX_EDCA1                  0x07
#define M_ENG_TX_EDCA2                  0x08
#define M_ENG_TX_EDCA3                  0x09
#define M_ENG_TX_MNG                    0x0A
#define M_ENG_ENCRYPT_SEC               0x0B
#define M_ENG_MIC_SEC                   0x0C
#define M_ENG_RESERVED_1                0x0D
#define M_ENG_RESERVED_2                0x0E
#define M_ENG_TRASH_CAN                 0x0F
#define M_ENG_MAX                      (M_ENG_TRASH_CAN+1)


/* Software Engine ID: */
#define M_CPU_HWENG                     0x00
#define M_CPU_TXL34CS                   0x01
#define M_CPU_RXL34CS                   0x02
#define M_CPU_DEFRAG                    0x03
#define M_CPU_EDCATX                    0x04
#define M_CPU_RXDATA                    0x05
#define M_CPU_RXMGMT                    0x06
#define M_CPU_RXCTRL                    0x07
#define M_CPU_FRAG                      0x08
#define M_CPU_RXMGMT_ACS                0x09

#define LBYTESWAP(a)  ((((a) & 0x00ff00ff) << 8) | \
    (((a) & 0xff00ff00) >> 8))

#define LONGSWAP(a)   ((LBYTESWAP(a) << 16) | (LBYTESWAP(a) >> 16))

#define MAC_DECITBL1_SIZE               16
#define MAC_DECITBL2_SIZE               9
#endif
#define MAC_REG_WRITE(_r, _v)                  \
        ssv6xxx_drv_write_reg(_r,_v)
#define MAC_REG_READ(_r, _v)                   \
        _v = ssv6xxx_drv_read_reg(_r)
#define MAC_LOAD_FW(_s, _l)                            \
        ssv_hal_download_fw(_s,_l)


//extern u16 sta_deci_tbl[];

/* Access Categories / ACI to AC coding */
enum {
    WMM_AC_BE = 0 /* Best Effort */,
    WMM_AC_BK = 1 /* Background */,
    WMM_AC_VI = 2 /* Video */,
    WMM_AC_VO = 3 /* Voice */,
    WMM_NUM_AC
};


#define SSV_EVENT_SIZE  (768)		// Probe response might be large.
#define SSV_EVENT_COUNT (12)



#define CHIP_ID_SSV6051Q_OLD 0x70000000
#define CHIP_ID_SSV6051Z 0x71000000
#define CHIP_ID_SSV6052Q 0x72000000
#define CHIP_ID_SSV6051Q 0x73000000
#define CHIP_ID_SSV6060P 0x74000000
#define CHIP_ID_SSV6051P 0x75000000
#define CHIP_ID_SSV6030P 0x76000000

/**
 *  The flag definition for c_type (command type) field of PKTInfo:
 *
 *      @ M0_TXREQ:
 *      @ M1_TXREQ
 *      @ M2_TXREQ
 *      @ M0_RXEVENT
 *      @ M1_RXEVENT
 *      @ HOST_CMD
 *      @ HOST_EVENT
 *
 */
#define M0_TXREQ                            0
#define M1_TXREQ                            1
#define M2_TXREQ                            2
#define M0_RXEVENT                          3
#define M2_RXEVENT                          4
#define HOST_CMD                            5
#define HOST_EVENT                          6
#define TEST_CMD                            7
#endif
