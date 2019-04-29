/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef _UW_ERRNO_H_
#define _UW_ERRNO_H_

typedef enum {
    UWE_OK              =  0,

    /* General and I/O Errors */
    UWE_NOTSTARTED      =  1,
    UWE_INPROGRESS      =  2,
    UWE_PERM            =  3,
    UWE_NOENT           =  4,
    UWE_IO              =  5,
    UWE_NXIO            =  6,
    UWE_NOMEM           =  7,
    UWE_BUSY            =  8,
    UWE_NODEV           =  9,
    UWE_INVAL           = 10,
    UWE_NOTSUP          = 11,
    UWE_TIMEDOUT        = 12,
    UWE_SUSPENDED       = 13,
    UWE_UNKNOWN         = 14,
    UWE_TEST_FAILED     = 15,
    UWE_STATE           = 16,
    UWE_STALLED         = 17,
    UWE_PARAM           = 18,
    UWE_ABORTED         = 19,
    UWE_SHORT           = 20,
    UWE_WOULDBLOCK      = 21,
    UWE_ALREADY         = 22,
    UWE_EXPIRED         = 23,
    UWE_FULL            = 24,

    /* Network/IPC Errors */
    UWE_DESTADDRREQ     = 41,
    UWE_ADDRNOTAVAIL    = 42,
    UWE_MSGSIZE         = 43,
    UWE_NETDOWN         = 44,
    UWE_NETUNREACH      = 45,
    UWE_NETRESET        = 46,
    UWE_CONNABORTED     = 47,
    UWE_CONNRESET       = 48,
    UWE_ISCONN          = 49,
    UWE_NOTCONN         = 50,
    UWE_CONNREFUSED     = 51,
    UWE_HOSTDOWN        = 52,
    UWE_HOSTUNREACH     = 53,
    UWE_NOLINK          = 54,
    UWE_PROTO           = 55,
    UWE_NOPROTOOPT      = 56,
    UWE_INTR            = 57,
    UWE_PROTOTYPE       = 58
} result_t;

const char *uwe_str(result_t rc);

#endif
