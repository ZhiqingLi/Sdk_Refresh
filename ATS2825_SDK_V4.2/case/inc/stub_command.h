#ifndef _STUB_COMMAND_H
#define _STUB_COMMAND_H

#define STUB_CMD_OPEN                   (0xff00)

#define STUB_CMD_BTT_POLL               (0x0100)

#define STUB_CMD_BTT_ACK                (0x1ffe)

#define STUB_CMD_BTT_HCI_WRITE          (0x1f01)

#define STUB_CMD_BTT_HCI_READ           (0x1f02)

#define STUB_CMD_BTT_RESET_CONTROLLER   (0x1f03)

#define STUB_CMD_BTT_CLOSE              (0x0f00)

#define STUB_CMD_BTT_CLOSE_ACK          (0x8f00)

#define STUB_CMD_BTT_DOWNLOAD_PATCH     (0x1f04)

#define STUB_CMD_RESET_HOST             (0x1f05)

#define STUB_CMD_BTT_READ_TRIM_CAP      (0x1f86)

#define STUB_CMD_BTT_WRITE_TRIM_CAP     (0x1f06)

//旧版ASET工具命令字
#define STUB_CMD_ASET_POLL              (0x3100)

#define STUB_CMD_ASET_DOWNLOAD_DATA     (0x3200)

#define STUB_CMD_ASET_UPLOAD_DATA       (0x3400)

#define STUB_CMD_ASET_READ_VOL          (0x3500)

//新版ASET工具命令字

//Read Staus
#define STUB_CMD_ASET_READ_STATUS       (0x0300)

#define STUB_CMD_ASET_WRITE_STATUS      (0x0380)

#define STUB_CMD_ASET_WRITE_APPLICATION_PROPERTIES   (0x0390)//向工具端写应用的属性


//Read Volume
#define STUB_CMD_ASET_READ_VOLUME       (0x0301)

#define STUB_CMD_ASET_WRITE_VOLUME      (0x0381)

#define STUB_CMD_ASET_READ_EQ_DATA      (0x0302)

#define STUB_CMD_ASET_WRITE_EQ_DATA     (0x0382)

#define STUB_CMD_ASET_READ_VBASS_DATA   (0x0303)

#define STUB_CMD_ASET_WRITE_VBASS_DATA  (0x0383)

#define STUB_CMD_ASET_READ_TE_DATA      (0x0304)

#define STUB_CMD_ASET_WRITE_TE_DATA     (0x0384)

#define STUB_CMD_ASET_READ_SURR_DATA    (0x0305)

#define STUB_CMD_ASET_WRITE_SURR_DATA   (0x0385)

#define STUB_CMD_ASET_READ_LIMITER_DATA (0x0306)

#define STUB_CMD_ASET_WRITE_LIMITER_DATA (0x0386)

#define STUB_CMD_ASET_READ_SPKCMP_DATA  (0x0307)

#define STUB_CMD_ASET_WRITE_SPKCMP_DATA (0x0387)

#define STUB_CMD_ASET_READ_MDRC_DATA    (0x0308)

#define STUB_CMD_ASET_WRITE_MDRC_DATA   (0x0388)

#define STUB_CMD_ASET_READ_MDRC2_DATA    (0x0309)

#define STUB_CMD_ASET_WRITE_MDRC2_DATA   (0x0389)




#define STUB_CMD_ASET_READ_SEE_DATA    (0x030C)

#define STUB_CMD_ASET_WRITE_SEE_DATA   (0x038C)

#define STUB_CMD_ASET_READ_SEW_DATA    (0x030D)

#define STUB_CMD_ASET_WRITE_SEW_DATA   (0x038D)

#define STUB_CMD_ASET_READ_SD_DATA    (0x030E)

#define STUB_CMD_ASET_WRITE_SD_DATA   (0x038E)



#define STUB_CMD_ASET_READ_LFRC_DATA_BASE    (0x0311)

#define STUB_CMD_ASET_WRITE_LFRC_DATA_BASE   (0x0391)

#define STUB_CMD_ASET_READ_LFRC1_DATA    (STUB_CMD_ASET_READ_LFRC_DATA_BASE)

#define STUB_CMD_ASET_WRITE_LFRC1_DATA   (STUB_CMD_ASET_WRITE_LFRC_DATA_BASE)

#define STUB_CMD_ASET_READ_LFRC2_DATA    (STUB_CMD_ASET_READ_LFRC_DATA_BASE+1)

#define STUB_CMD_ASET_WRITE_LFRC2_DATA   (STUB_CMD_ASET_WRITE_LFRC_DATA_BASE+1)

#define STUB_CMD_ASET_READ_LFRC3_DATA    (STUB_CMD_ASET_READ_LFRC_DATA_BASE+2)

#define STUB_CMD_ASET_WRITE_LFRC3_DATA   (STUB_CMD_ASET_WRITE_LFRC_DATA_BASE+2)

#define STUB_CMD_ASET_READ_LFRC4_DATA    (STUB_CMD_ASET_READ_LFRC_DATA_BASE+3)

#define STUB_CMD_ASET_WRITE_LFRC4_DATA   (STUB_CMD_ASET_WRITE_LFRC_DATA_BASE+3)



#define STUB_CMD_ASET_READ_RFRC_DATA_BASE    (0x0315)

#define STUB_CMD_ASET_WRITE_RFRC_DATA_BASE   (0x0395)


#define STUB_CMD_ASET_READ_RFRC1_DATA    (STUB_CMD_ASET_READ_RFRC_DATA_BASE)

#define STUB_CMD_ASET_WRITE_RFRC1_DATA   (STUB_CMD_ASET_WRITE_RFRC_DATA_BASE)

#define STUB_CMD_ASET_READ_RFRC2_DATA    (STUB_CMD_ASET_READ_RFRC_DATA_BASE+1)

#define STUB_CMD_ASET_WRITE_RFRC2_DATA   (STUB_CMD_ASET_WRITE_RFRC_DATA_BASE+1)

#define STUB_CMD_ASET_READ_RFRC3_DATA    (STUB_CMD_ASET_READ_RFRC_DATA_BASE+2)

#define STUB_CMD_ASET_WRITE_RFRC3_DATA   (STUB_CMD_ASET_WRITE_RFRC_DATA_BASE+2)

#define STUB_CMD_ASET_READ_RFRC4_DATA    (STUB_CMD_ASET_READ_RFRC_DATA_BASE+3)

#define STUB_CMD_ASET_WRITE_RFRC4_DATA   (STUB_CMD_ASET_WRITE_RFRC_DATA_BASE+3)


#define STUB_CMD_ASET_READ_MAIN_SWITCH   (0x0319)

#define STUB_CMD_ASET_READ_EQ2_DATA      (0x0321)
#define STUB_CMD_ASET_READ_EQ3_DATA      (0x0322)


#define STUB_CMD_ASET_ACK               (0x03fe)


//兼容标准音效流程增加的宏定义
#define STUB_CMD_AUDIOPP_SELECT                             (0x030f)

#define STUB_CMD_ASET_READ_MDRC_DATA_STANDARD_MODE          (0x032a)

#define STUB_CMD_ASET_READ_MDRC2_DATA_STANDARD_MODE         (0x0329)

#define STUB_CMD_ASET_READ_COMPRESSOR_DATA_STANDARD_MODE    (0x032b)


//ATT工具命令字

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

#define STUB_CMD_ATT_CFO_TX_BEGIN       0x0456

#define STUB_CMD_ATT_CFO_TX_STOP        0x0457

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

//waves工具命令字
//小机成功接收Maxx Audio Tuning工具发来的数据后，回复给PC的命令字
#define STUB_CMD_WAVES_ASET_ACK                         (0x07fe)

#define STUB_CMD_WAVES_ASET_READ_COEFF_PROPERTY         (0x0740)

#define STUB_CMD_WAVES_ASET_READ_COEFF_CONTENT         (0x0741)

#endif
