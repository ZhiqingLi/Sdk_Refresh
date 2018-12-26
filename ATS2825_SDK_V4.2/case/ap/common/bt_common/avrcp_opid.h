#ifndef AVRCP_OPID_H
#define AVRCP_OPID_H

/* AV/C Panel Commands, Pass Through command operation_id */
#define AVRCP_OPID_SELECT                                           0x00
#define AVRCP_OPID_UP                                               0x01
#define AVRCP_OPID_DOWN                                             0x02
#define AVRCP_OPID_LEFT                                             0x03
#define AVRCP_OPID_RIGHT                                            0x04
#define AVRCP_OPID_RIGHT_UP                                         0x05
#define AVRCP_OPID_RIGHT_DOWN                                       0x06
#define AVRCP_OPID_LEFT_UP                                          0x07
#define AVRCP_OPID_LEFT_DOWN                                        0x08
#define AVRCP_OPID_ROOTMENU                                         0x09
#define AVRCP_OPID_SETUP_MENU                                       0x0a
#define AVRCP_OPID_CONTENTS_MENU                                    0x0b
#define AVRCP_OPID_FAVORITE_MENU                                    0x0c
#define AVRCP_OPID_EXIT                                             0x0d

#define AVRCP_OPID_0                                                0x20
#define AVRCP_OPID_1                                                0x21
#define AVRCP_OPID_2                                                0x22
#define AVRCP_OPID_3                                                0x23
#define AVRCP_OPID_4                                                0x24
#define AVRCP_OPID_5                                                0x25
#define AVRCP_OPID_6                                                0x26
#define AVRCP_OPID_7                                                0x27
#define AVRCP_OPID_8                                                0x28
#define AVRCP_OPID_9                                                0x29
#define AVRCP_OPID_DOT                                              0x2a
#define AVRCP_OPID_ENTER                                            0x2b
#define AVRCP_OPID_CLEAR                                            0x2c

#define AVRCP_OPID_CHANNEL_UP                                       0x30
#define AVRCP_OPID_CHANNEL_DOWN                                     0x31
#define AVRCP_OPID_PREVIOUS_CHANNEL                                 0x32
#define AVRCP_OPID_SOUND_SELECT                                     0x33
#define AVRCP_OPID_INPUT_SELECT                                     0x34
#define AVRCP_OPID_DISPLAY_INFORMATION                              0x35
#define AVRCP_OPID_HELP                                             0x36
#define AVRCP_OPID_PAGE_UP                                          0x37
#define AVRCP_OPID_PAGE_DOWN                                        0x38

#define AVRCP_OPID_POWER                                            0x40
#define AVRCP_OPID_VOLUME_UP                                        0x41
#define AVRCP_OPID_VOLUME_DOWN                                      0x42
#define AVRCP_OPID_MUTE                                             0x43
#define AVRCP_OPID_PLAY                                             0x44
#define AVRCP_OPID_STOP                                             0x45
#define AVRCP_OPID_PAUSE                                            0x46
#define AVRCP_OPID_RECORD                                           0x47
#define AVRCP_OPID_REWIND                                           0x48
#define AVRCP_OPID_FAST_FORWARD                                     0x49
#define AVRCP_OPID_EJECT                                            0x4a
#define AVRCP_OPID_FORWARD                                          0x4b
#define AVRCP_OPID_BACKWARD                                         0x4c

#define AVRCP_OPID_ANGLE                                            0x50
#define AVRCP_OPID_SUBPICTURE                                       0x51
#define AVRCP_OPID_F1                                               0x52
#define AVRCP_OPID_F2                                               0x53
#define AVRCP_OPID_F3                                               0x54
#define AVRCP_OPID_F4                                               0x55
#define AVRCP_OPID_F5                                               0x56
#define AVRCP_OPID_VENDORUNIQUE                                     0x7e

/*---------------------------------------------------------------------------*/
/*                              6.13 Volume Handling                         */
/*---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 Description:
 6.13 An Absolute Volume is represented in one octet. The top bit (bit 7) is reserved for
 future definition. The volume is specified as a percentage of the maximum. The value 0x0
 corresponds to 0%. The value 0x7F corresponds to 100%. Scaling should be applied to
 achieve values between these two.
 -----------------------------------------------------------------------------*/
#define AVRCP_ABSOLUTE_VOLUME_MIN                                   0x00
#define AVRCP_ABSOLUTE_VOLUME_MAX                                   0x7F

#endif
