#ifndef _AP_AUTOTEST_CMD_H
#define _AP_AUTOTEST_CMD_H

#define STUB_TYPE_ATT                   0x04

#define STUB_CMD_ATT_START              0x0400

#define STUB_CMD_ATT_STOP               0x0401

#define STUB_CMD_ATT_PAUSE              0x0402

#define STUB_CMD_ATT_RESUME             0x0403

#define STUB_CMD_ATT_GET_TEST_ID        0x0404

#define STUB_CMD_ATT_GET_TEST_ARG       0x0405

#define STUB_CMD_ATT_REPORT_TRESULT     0x0406

#define STUB_CMD_ATT_PRINT_LOG          0x0407

#define STUB_CMD_ATT_FREAD              0x0408

#define STUB_CMD_ATT_READ_TESTINFO      0x0409

#define STUB_CMD_ATT_WRITE_TESTINFO     0x040a

#define STUB_CMD_ATT_REBOOT_TIMEOUT     0x040b

#define STUB_CMD_ATT_CFO_TX_BEGIN       0x0456

#define STUB_CMD_ATT_CFO_TX_STOP        0x0457

#define STUB_CMD_ATT_FILE_OPEN          0x0480

#define STUB_CMD_ATT_FILE_READ          0x0481

#define STUB_CMD_ATT_FILE_WRITE         0x0482

#define STUB_CMD_ATT_FILE_CLOSE         0x0483

#define STUB_CMD_ATT_NAK                0x04fd

#define STUB_CMD_ATT_ACK                0x04fe

#define STUB_CMD_ATT_POLL               0x04ff

#define STUB_CMD_ATT_HCI_WRITE_CMD      0x0450

#define STUB_CMD_ATT_HCI_EVENT_READ     0x0451

#define STUB_CMD_ATT_RESET_CONTROLLER   0x0452

#define STUB_CMD_ATT_DOWNLOAD_PATCH     0x0453

#define STUB_CMD_ATT_WRITE_TRIM_CAP     0x0454

#define STUB_CMD_ATT_READ_TRIM_CAP      0x0455

#define STUB_CMD_ATT_RETRUN_CFO_VAL     0x0456

#define STUB_CMD_ATT_BER_TX_BEGIN       0x0458

#define STUB_CMD_ATT_BER_TX_STOP        0x0459

#endif

