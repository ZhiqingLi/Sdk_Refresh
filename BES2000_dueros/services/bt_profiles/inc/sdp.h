/****************************************************************************
 *
 * File:
 *     $Id: sdp.h 715 2009-01-31 01:07:23Z khendrix $
 *     $Product: BES Blue SDK v3.x $
 *     $Revision: 715 $
 *
 * Description:
 *     Public types, defines, and prototypes for accessing the
 *     Service Discovery Protocol component. 
 *
 * Copyright 1999-2005 Extended Systems, Inc.
 * Portions copyright BES.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of BES.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of BES.
 *
 * Use of this work is governed by a license granted by BES.
 * This work contains confidential and proprietary information of
 * BES. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#ifndef __SDPH
#define __SDPH

#include "l2cap.h"
#include "me.h"
#include "utils.h"


/*---------------------------------------------------------------------------
 * Service Discovery Protocol (SDP) layer
 *
 *     The SDP layer is a "yellow pages" of services available on a device.
 *     With the SDP API, you can create and store "service records" for
 *     remote devices to find. After this is done, SDP handles
 *     all queries from remote devices automatically.
 *
 *     You can also query for the service records on connected remote
 *     devices. To do this, you create a query token, fill it with
 *     the parameters for your query, and pass it to SDP. SDP then
 *     handles the interactions with the remote device, returns the 
 *     query token to you, and helps you parse the results.
 *
 *     Note: unlike many other layers of the Bluetooth protocol,
 *     the SDP uses big-endian format, so more significant bytes are stored
 *     and transferred before less significant bytes.
 */


/****************************************************************************
 *
 * Types and Constants
 *
 ****************************************************************************/
#if (SDP_CLIENT_SUPPORT == XA_ENABLED) && (SDP_PARSING_FUNCS == XA_ENABLED)

/*---------------------------------------------------------------------------
 * SdpParsingMode type
 *
 *     Indicates the parsing mode when calling SDP_ParseAttributes.
 */
typedef U8 SdpParsingMode;

#define BSPM_BEGINNING   0x00   /* Used to parse the first SDP response 
                                 * packet from the beginning.  This is always
                                 * the first mode used.  This mode uses the 
                                 * SdpQueryToken "results" and "rLen" buffer 
                                 * fields when it start its parsing.
                                 */

#define BSPM_RESUME      0x01   /* Used to parse the same SDP response packet
                                 * from where parsing stopped last.  This mode
                                 * can be used to find an attribute returned
                                 * in multiple SDP records.  This mode uses the
                                 * SdpQueryToken "remainLen" and "remainBuff" 
                                 * buffer fields when it starts its parsing.
                                 */


#define BSPM_CONT_STATE  0x02   /* Use to parse the beginning of a 
                                 * continuation state SDP response packet.
                                 * This mode is only valid after performing an
                                 * SDP query using the BSQM_CONTINUE query mode.
                                 * This mode uses the SdpQueryToken "results" 
                                 * and "rLen" buffer fields when it start its 
                                 * parsing.
                                 */

#define BSPM_NO_SKIP   0x04     /* Combined with BSPM_RESUME or BSPM_CONT_STATE to 
                                 * bypass the default parsing behavior which skips
                                 * the previously parsed attribute Id to access subsequent
                                 * SDP records returned in the SDP query response. When
                                 * used, this parsing method will resume from the starting
                                 * point of the resume or continuation state buffer and
                                 * fine the first attribute Id that matches the parsing
                                 * information. Future instances of the same attribute Id in
                                 * the next SDP record (if one exists) can be performed by 
                                 * not using this parsing method. NOTE: this mode cannot
                                 * be used by itself, but must be combined with BSPM_RESUME
                                 * or BSPM_CONT_STATE.
                                 */

/* End of SdpParsingMode */

#endif /* (SDP_CLIENT_SUPPORT == XA_ENABLED) && (SDP_PARSING_FUNCS == XA_ENABLED) */

/*---------------------------------------------------------------------------
 * SdpQueryMode type
 *
 *     Indicates the mode of a query when calling SDP_Query.
 */
typedef U8 SdpQueryMode;

#define BSQM_FIRST     0x00   /* Use this mode to issue the first packet in 
                               * an SDP query. When using this mode, SDP will 
                               * internally initialize a 1-byte continuation 
                               * state parameter containing 0x00 and will store 
                               * this information in the "contState" and 
                               * "contStateLen" fields in the SdpQueryToken 
                               * structure.
                               */

#define BSQM_CONTINUE  0x01   /* Use this mode to send the next packet in an 
                               * SDP query. An SDP query takes multiple packets
                               * when a response is received containing non-zero 
                               * continuation state information. When specifying 
                               * this mode, you must include the continuation 
                               * state information in the "contState" and 
                               * "contStateLen" fields in the SdpQueryToken 
                               * structure.  If SDP_ParseAttributes was used to
                               * parse the response and BT_STATUS_SDP_CONT_STATE
                               * was returned, this data will be automatically
                               * copied into these SdpQueryToken fields.
                               */

#define BSQM_DONT_CARE 0xFF  /* Used internally. */

/* End of SdpQueryMode */

/*---------------------------------------------------------------------------
 * SdpQueryType type
 *
 *     Indicates the type of a query when calling SDP_Query.
 */
typedef U8 SdpQueryType;

/* SDP REQUEST:
 *
 * Use for a ServiceSearchRequest query. This query should result in a
 * response token of type BSQR_SERVICE_SEARCH_RESP or BSQR_ERROR_RESP.
 *
 * The query parameters must include the following:
 *
 * 1) A Data Element Sequence of UUIDs that specify the service(s)
 * desired. Between 1 and 12 UUIDs must be present.
 *
 * 2) The maximum number of service record handles to return (a 2-byte
 * integer).
 *
 * SDP RESPONSE:
 *
 * A successful response (BSQR_SERVICE_SEARCH_RESP) to a 
 * BSQR_SERVICE_SEARCH_REQ query includes the following parameters:
 *
 * 1) The total number of matching service records (a 2-byte integer).
 *
 * 2) The total number of service record handles (a 2-byte integer)
 * expressed in this response. (Additional service records may require
 * a second query containing the ContinuationState parameter.)
 *
 * 3) An array of 32-bit service record handles.
 *
 * 4) The ContinuationState parameter.
 *
 * An unsuccessful query results in an error response (BSQR_ERROR_RESP)
 * which has the following parameters:
 * 
 * 1) A two-byte ErrorCode 
 * 
 * 2) An ErrorInfo field (currently undefined).
 *
 * Error codes are expressed using the SdpErrorCode type.
 */
#define BSQT_SERVICE_SEARCH_REQ         0x02

/* SDP REQUEST:
 *
 * Use for a ServiceAttributeRequest query. This query should result in
 * a response token of type BQSR_ATTRIB_RESP or BSQR_ERROR_RESP.
 *
 * The query parameters must include the following:
 *
 * 1) The ServiceRecordHandle (a 4-byte integer) that identifies the
 * service to query.
 *
 * 2) The maximum size, in bytes, of attribute data to be returned
 * (a 2-byte integer). Between 7 and 0xFFFF bytes can be specified.
 *
 * 3) A Data Element Sequence of ascending attribute IDs and ranges,
 * specifying the attributes of interest on the remote device.
 * IDs are 16-bit identifiers. Ranges are 32-bit identifiers with the
 * higher 16 bits specifying the first ID in the range, and the lower
 * 16 bits specifying the last ID in the range.    
 *
 * SDP RESPONSE:
 * 
 * A successful response (BSQR_ATTRIB_RESP) to a BSQR_ATTRIB_REQ 
 * query includes the following parameters:
 *
 * 1) The number of bytes in the attribute list parameter (next).
 * This is stored as a 2-byte integer and ranges between 2 and 0xFFFF.
 *
 * 2) A Data Element Sequence of attribute ID and value pairs, ascending
 * by ID. Attributes with non-null values are not included.
 *
 * 3) The ContinuationState parameter.
 *
 * An unsuccessful query results in an error response (BSQR_ERROR_RESP)
 * which has the following parameters:
 * 
 * 1) A two-byte ErrorCode 
 * 
 * 2) An ErrorInfo field (currently undefined).
 *
 * Error codes are expressed using the SdpErrorCode type.
 */
#define BSQT_ATTRIB_REQ                 0x04

/* SDP REQUEST:
 * 
 * Use for a ServiceSearchAttributeRequest query, which searches for
 * services and attributes simultaneously. This query should
 * result in a response token of type BSQR_SERVICE_SEARCH_ATTRIB_RESP
 * or BSQR_ERROR_RESP.
 *
 * The query parameters must include the following:
 *
 * 1) A Data Element Sequence of UUIDs that specify the service(s)
 * desired. Between 1 and 12 UUIDs must be present.
 *
 * 2) The maximum size, in bytes, of attribute data to be returned
 * (a 2-byte integer). Between 7 and 0xFFFF bytes can be specified.
 *
 * 3) A Data Element Sequence of ascending attribute IDs and ranges,
 * specifying the attributes of interest on the remote device.
 * IDs are 16-bit identifiers. Ranges are 32-bit identifiers with the
 * higher 16 bits specifying the first ID in the range, and the lower
 * 16 bits specifying the last ID in the range.    
 *
 * SDP RESPONSE:
 *
 * A successful response (BSQR_SERVICE_SEARCH_ATTRIB_RESP) to a 
 * BSQR_SERVICE_SEARCH_ATTRIB_REQ query includes the following 
 * parameters:
 *
 * 1) The number of bytes in the attribute list parameter (next).
 * This is stored as a 2-byte integer and ranges between 2 and 0xFFFF.
 *
 * 2) A Data Element Sequence, where each element corresponds to a
 * particular service record matching the original pattern.
 * Each element of the sequence is itself a Data Element Sequence of
 * attribute ID and value pairs, ascending by ID.
 *
 * 3) The ContinuationState parameter.
 *
 * An unsuccessful query results in an error response (BSQR_ERROR_RESP)
 * which has the following parameters:
 * 
 * 1) A two-byte ErrorCode 
 * 
 * 2) An ErrorInfo field (currently undefined).
 *
 * Error codes are expressed using the SdpErrorCode type.
 */
#define BSQT_SERVICE_SEARCH_ATTRIB_REQ  0x06

/* End of SdpQueryType */

/*---------------------------------------------------------------------------
 * SdpErrorCode type
 *
 *     Indicates the type of error that caused the query to fail
 *     on the remote device. This is expressed as a parameter in
 *     a BSQR_ERROR_RESP response.
 */
typedef U16 SdpErrorCode;

#define BSEC_BAD_VERSION      0x0001 /* Invalid/unsupported SDP version */
#define BSEC_BAD_HANDLE       0x0002 /* Invalid service record handle */
#define BSEC_BAD_SYNTAX       0x0003 /* Invalid request syntax */
#define BSEC_BAD_PDU_SIZE     0x0004 /* Invalid PDU size */
#define BSEC_BAD_CONTINUATION 0x0005 /* Invalid ContinuationState */
#define BSEC_OUT_OF_RESOURCES 0x0006 /* Insufficient resources to satisfy
                                      * request */
/* End of SdpErrorCode */

/*---------------------------------------------------------------------------
 * SdpDataElemType type
 *
 *     Specifies the type of a Data Element.
 *
 *     Data Elements begin with a single byte that contains both type and
 *     size information. To read the type from this byte, use the
 *     SDP_GetElemType macro.
 *
 *     To create the first byte of a Data Element, bitwise OR the
 *     SdpDataElemType and SdpDataElemSize values into a single byte.
 */
typedef U8 SdpDataElemType;

#define DETD_NIL  0x00 /* Specifies nil, the null type.
                        * Requires a size of DESD_1BYTE, which for this type
                        * means an actual size of 0.
                        */
#define DETD_UINT 0x08 /* Specifies an unsigned integer. Must use size
                        * DESD_1BYTE, DESD_2BYTES, DESD_4BYTES, DESD_8BYTES,
                        * or DESD_16BYTES.
                        */
#define DETD_SINT 0x10 /* Specifies a signed integer. May use size
                        * DESD_1BYTE, DESD_2BYTES, DESD_4BYTES, DESD_8BYTES,
                        * or DESD_16BYTES.
                        */
#define DETD_UUID 0x18 /* Specifies a Universally Unique Identifier (UUID).
                        * Must use size DESD_2BYTES, DESD_4BYTES, or
                        * DESD_16BYTES.
                        */
#define DETD_TEXT 0x20 /* Specifies a text string. Must use sizes
                        * DESD_ADD_8BITS, DESD_ADD_16BITS, or DESD_ADD32_BITS.
                        */
#define DETD_BOOL 0x28 /* Specifies a boolean value. Must use size
                        * DESD_1BYTE.
                        */
#define DETD_SEQ  0x30 /* Specifies a data element sequence. The data contains
                        * a sequence of Data Elements. Must use sizes
                        * DESD_ADD_8BITS, DESD_ADD_16BITS, or DESD_ADD_32BITS.
                        */
#define DETD_ALT  0x38 /* Specifies a data element alternative. The data contains
                        * a sequence of Data Elements. This type is sometimes
                        * used to distinguish between two possible sequences.
                        * Must use size DESD_ADD_8BITS, DESD_ADD_16BITS,
                        * or DESD_ADD_32BITS.
                        */
#define DETD_URL  0x40 /* Specifies a Uniform Resource Locator (URL).
                        * Must use size DESD_ADD_8BITS, DESD_ADD_16BITS,
                        * or DESD_ADD_32BITS.
                        */

#define DETD_MASK 0xf8 /* AND this value with the first byte of a Data
                        * Element to return the element's type.
                        */

/* End of SdpDataElemType */
                        
/*---------------------------------------------------------------------------
 * SdpDataElemSize type
 *
 *     Specifies the size of a Data Element.
 *
 *     Data Elements begin with a single byte that contains both type and
 *     size information. To read the size from this byte, use the
 *     SDP_GetElemSize macro.
 *
 *     To create the first byte of a Data Element, bitwise OR the
 *     SdpDataElemType and SdpDataElemSize values into a single byte.
 *     For example, a standard 16 bit unsigned integer with a value of 0x57
 *     could be encoded as follows:
 * 
 *     U8 val[3] = { DETD_UINT | DESD_2BYTES, 0x00, 0x57 };
 *
 *     The text string "hello" could be encoded as follows:
 *
 *     U8 str[7] = { DETD_TEXT | DESD_ADD_8BITS, 0x05, 'h','e','l','l','o' };
 */
typedef U8 SdpDataElemSize;

#define DESD_1BYTE      0x00 /* Specifies a 1-byte element. However, if
                              * type is DETD_NIL then the size is 0.
                              */
#define DESD_2BYTES     0x01 /* Specifies a 2-byte element. */
#define DESD_4BYTES     0x02 /* Specifies a 4-byte element. */
#define DESD_8BYTES     0x03 /* Specifies an 8-byte element. */
#define DESD_16BYTES    0x04 /* Specifies a 16-byte element. */
#define DESD_ADD_8BITS  0x05 /* The element's actual data size, in bytes,
                              * is contained in the next 8 bits.
                              */
#define DESD_ADD_16BITS 0x06 /* The element's actual data size, in bytes,
                              * is contained in the next 16 bits.
                              */
#define DESD_ADD_32BITS 0x07 /* The element's actual data size, in bytes,
                              * is contained in the next 32 bits.
                              */

#define DESD_MASK       0x07 /* AND this value with the first byte of a Data
                              * Element to return the element's size.
                              */

/* End of SdpDataElemSize */

/*---------------------------------------------------------------------------
 * SdpAttributeId type
 *
 *     Represents an attribute ID.
 *
 *     Attribute are identified by attribute ID. This type includes
 *     many (but not all) of the possible attributes available on
 *     a Bluetooth device.
 *
 *     Higher layer services may use these attributes, but may also
 *     need to define their own. In this case, the service must define
 *     attributes with IDs between 0x0200 through 0xFFFF.
 */
typedef U16 SdpAttributeId;

/* Group: The following attributes are required to be present in all
 * service records on all Bluetooth devices.
 */

/* A 32-bit UINT that uniquely identifies the service record for a
 * particular SDP server.
 */
#define AID_SERVICE_RECORD_HANDLE               0x0000

/* A Data Element Sequence of UUIDs. Each UUID represents a service
 * class supported by the service record. At least one UUID must
 * be present.
 *
 * The SdpServiceClassUuid type represents these UUIDs.
 */
#define AID_SERVICE_CLASS_ID_LIST               0x0001

/* Group: The following attributes are "universal" to all service records,
 * meaning that the same attribute IDs are always used. However, attributes
 * may or may not be present within a service record.
 *
 * See the Bluetooth Core specification, Service Discovery Protocol (SDP)
 * chapter, section 5.1 for more detailed explanations of these attributes.
 */

#define AID_SERVICE_RECORD_STATE                0x0002
#define AID_SERVICE_ID                          0x0003
#define AID_PROTOCOL_DESC_LIST                  0x0004
#define AID_BROWSE_GROUP_LIST                   0x0005
#define AID_LANG_BASE_ID_LIST                   0x0006
#define AID_SERVICE_INFO_TIME_TO_LIVE           0x0007
#define AID_SERVICE_AVAILABILITY                0x0008
#define AID_BT_PROFILE_DESC_LIST                0x0009
#define AID_DOC_URL                             0x000a
#define AID_CLIENT_EXEC_URL                     0x000b
#define AID_ICON_URL                            0x000c
#define AID_ADDITIONAL_PROT_DESC_LISTS          0x000d

/* Group: The following "universal" attribute IDs must be added to
 * the appropriate value from the AID_LANG_BASE_ID_LIST attribute (usually 
 * 0x0100).
 */
#define AID_SERVICE_NAME                        0x0000
#define AID_SERVICE_DESCRIPTION                 0x0001
#define AID_PROVIDER_NAME                       0x0002

/* Personal Area Networking Profile */
#define AID_IP_SUBNET                           0x0200

/* Group: The following attribute applies only to a service record that
 * corresponds to a BrowseGroupDescriptor service.
 */

/* A UUID used to locate services that are part of the browse group. */
#define AID_GROUP_ID                            0x0200

/* Group: The following attributes apply only to the service record that
 * corresponds to the Service Discovery Server itself. Therefore, they
 * are valid only when the AID_SERVICE_CLASS_ID_LIST contains
 * a UUID of SC_SERVICE_DISCOVERY_SERVER.
 */
#define AID_VERSION_NUMBER_LIST                 0x0200
#define AID_SERVICE_DATABASE_STATE              0x0201

/* Group: The following attributes are for use by specific profiles as
 * defined in the profile specification.
 */
#define AID_SERVICE_VERSION                     0x0300

/* Cordless Telephony Profile */
#define AID_EXTERNAL_NETWORK                    0x0301

/* Synchronization Profile */
#define AID_SUPPORTED_DATA_STORES_LIST          0x0301

/* Fax Class 1 */
#define AID_FAX_CLASS_1_SUPPORT                 0x0302

/* GAP Profile */
#define AID_REMOTE_AUDIO_VOL_CONTROL            0x0302

/* Fax Class 2.0 */
#define AID_FAX_CLASS_20_SUPPORT                0x0303

/* Object Push Profile */
#define AID_SUPPORTED_FORMATS_LIST              0x0303

/* Fax Service Class 2 - Manufacturer specific */
#define AID_FAX_CLASS_2_SUPPORT                 0x0304
#define AID_AUDIO_FEEDBACK_SUPPORT              0x0305

/* Bluetooth as WAP requirements */
#define AID_NETWORK_ADDRESS                     0x0306
#define AID_WAP_GATEWAY                         0x0307
#define AID_HOME_PAGE_URL                       0x0308
#define AID_WAP_STACK_TYPE                      0x0309

/* Personal Area Networking Profile */
#define AID_SECURITY_DESC                       0x030A
#define AID_NET_ACCESS_TYPE                     0x030B
#define AID_MAX_NET_ACCESS_RATE                 0x030C
#define AID_IPV4_SUBNET                         0x030D
#define AID_IPV6_SUBNET                         0x030E

/* Imaging Profile */
#define AID_SUPPORTED_CAPABILITIES              0x0310
#define AID_SUPPORTED_FEATURES                  0x0311
#define AID_SUPPORTED_FUNCTIONS                 0x0312
#define AID_TOTAL_IMAGE_DATA_CAPACITY           0x0313

/* Phonebook Access Profile */
#define AID_SUPPORTED_REPOSITORIES              0x0314

/* Message Access Profile */
#define AID_MAS_INSTANCE_ID                     0x0315
#define AID_SUPPORTED_MESSAGE_TYPES             0x0316

/* Basic Printing Profile */
#define AID_SUPPORTED_DOC_FORMATS               0x0350
#define AID_SUPPORTED_CHAR_REPERTOIRES          0x0352
#define AID_SUPPORTED_XHTML_IMAGE_FORMATS       0x0354
#define AID_COLOR_SUPPORTED                     0x0356
#define AID_PRINTER_1284ID                      0x0358
#define AID_DUPLEX_SUPPORTED                    0x035E
#define AID_SUPPORTED_MEDIA_TYPES               0x0360
#define AID_MAX_MEDIA_WIDTH                     0x0362
#define AID_MAX_MEDIA_LENGTH                    0x0364



/*HID PROFILE */

#define AID_HID_PARSERVERSION     0x201
#define AID_HID_DEVICESUBCLASS     0x202
#define AID_HID_CONTRYCODE         0x203
#define AID_HID_VIRTUALCABLE         0x204
#define AID_HID_RECONNECTINITIATE         0x205
#define AID_HID_DESCRIPTORLIST         0x206
#define AID_HID_LANGIDBASELIST         0x207
#define AID_HID_BATTERYPOWER         0x209
#define AID_HID_REMOTEWAKE         0x20A
#define AID_HID_SUPERVISIONTIMEOUT         0x20C
#define AID_HID_NORMALLYCONNECTABLE         0x20D
#define AID_HID_BOOTDEVICE         0x20E
#define AID_HID_SSRHOSTMAXLATENCY         0x20F
#define AID_HID_SSRHOSTMINTIMEOUT         0x210


/* End of SdpAttributeId */


/*---------------------------------------------------------------------------
 * SdpServiceClassUuid type
 *
 *     Represents the UUID associated with a specific service and
 *     profile.
 *
 *     Any number of these UUIDs may be present in the
 *     AID_SERVICE_CLASS_ID_LIST attribute of a service record, and may
 *     appear in the AID_BT_PROFILE_DESC_LIST.
 */
typedef U16 SdpServiceClassUuid;

/* Service Discovery Server service. */
#define SC_SERVICE_DISCOVERY_SERVER             0x1000

/* Browse Group Descriptor service. */
#define SC_BROWSE_GROUP_DESC                    0x1001

/* Public Browse Group service. */
#define SC_PUBLIC_BROWSE_GROUP                  0x1002

/* Serial Port service and profile. */
#define SC_SERIAL_PORT                          0x1101

/* LAN Access over PPP service. */
#define SC_LAN_ACCESS_PPP                       0x1102

/* Dial-up networking service and profile. */
#define SC_DIALUP_NETWORKING                    0x1103

/* IrMC Sync service and Synchronization profile. */
#define SC_IRMC_SYNC                            0x1104

/* OBEX Object Push service and Object Push profile. */
#define SC_OBEX_OBJECT_PUSH                     0x1105

/* OBEX File Transfer service and File Transfer profile. */
#define SC_OBEX_FILE_TRANSFER                   0x1106

/* IrMC Sync service and Synchronization profile
 * (Sync Command Scenario).
 */
#define SC_IRMC_SYNC_COMMAND                    0x1107

/* Headset service and profile. */
#define SC_HEADSET                              0x1108

/* Cordless telephony service and profile. */
#define SC_CORDLESS_TELEPHONY                   0x1109

/* Audio Source */
#define SC_AUDIO_SOURCE                         0x110A

/* Audio Sink */
#define SC_AUDIO_SINK                           0x110B

/* Audio/Video Remote Control Target */
#define SC_AV_REMOTE_CONTROL_TARGET             0x110C

/* Advanced Audio Distribution Profile */
#define SC_AUDIO_DISTRIBUTION                   0x110D

/* Audio/Video Remote Control */
#define SC_AV_REMOTE_CONTROL                    0x110E

/* Video Conferencing Profile */
#define SC_VIDEO_CONFERENCING                   0x110F

/* Intercom service and profile. */
#define SC_INTERCOM                             0x1110

/* Fax service and profile. */
#define SC_FAX                                  0x1111

/* Headset Audio Gateway */
#define SC_HEADSET_AUDIO_GATEWAY                0x1112

/* WAP service */
#define SC_WAP                                  0x1113

/* WAP client service */
#define SC_WAP_CLIENT                           0x1114

/* Personal Area Networking Profile */
#define SC_PANU                                 0x1115

/* Personal Area Networking Profile */
#define SC_NAP                                  0x1116

/* Personal Area Networking Profile */
#define SC_GN                                   0x1117

/* Basic Printing Profile */
#define SC_DIRECT_PRINTING                      0x1118

/* Basic Printing Profile */
#define SC_REFERENCE_PRINTING                   0x1119
    
/* Imaging Profile */
#define SC_IMAGING                              0x111A

/* Imaging Profile */
#define SC_IMAGING_RESPONDER                    0x111B

/* Imaging Profile */
#define SC_IMAGING_AUTOMATIC_ARCHIVE            0x111C

/* Imaging Profile */
#define SC_IMAGING_REFERENCED_OBJECTS           0x111D

/* Handsfree Profile */
#define SC_HANDSFREE                            0x111E

/* Handsfree Audio Gateway */
#define SC_HANDSFREE_AUDIO_GATEWAY              0x111F

/* Basic Printing Profile */
#define SC_DIRECT_PRINTING_REF_OBJECTS          0x1120

/* Basic Printing Profile */
#define SC_REFLECTED_UI                         0x1121

/* Basic Printing Profile */
#define SC_BASIC_PRINTING                       0x1122

/* Basic Printing Profile */
#define SC_PRINTING_STATUS                      0x1123

/* Human Interface Device Profile */
#define SC_HUMAN_INTERFACE_DEVICE               0x1124

/* Hardcopy Cable Replacement Profile */
#define SC_HCR                                  0x1125

/* Hardcopy Cable Replacement Profile */
#define SC_HCR_PRINT                            0x1126

/* Hardcopy Cable Replacement Profile */
#define SC_HCR_SCAN                             0x1127

/* Common ISDN Access / CAPI Message Transport Protocol */
#define SC_ISDN                                 0x1128

/* Video Conferencing Gateway */
#define SC_VIDEO_CONFERENCING_GW                0x1129

/* Unrestricted Digital Information Mobile Termination */
#define SC_UDI_MT                               0x112A

/* Unrestricted Digital Information Terminal Adapter */
#define SC_UDI_TA                               0x112B

/* Audio Video service */
#define SC_AUDIO_VIDEO                          0x112C

/* SIM Access Profile */
#define SC_SIM_ACCESS                           0x112D

/* Phonebook Access Client */
#define SC_PBAP_CLIENT                          0x112E

/* Phonebook Access Server */
#define SC_PBAP_SERVER                          0x112F

/* Phonebook Access Profile Id */
#define SC_PBAP_PROFILE                         0x1130

/* Message Access Server */
#define SC_MAP_SERVER                           0x1132

/* Message Access Notification Server */
#define SC_MAP_NOTIFY_SERVER                    0x1133

/* Message Access Profile */
#define SC_MAP_PROFILE                          0x1134

/* Plug-n-Play service */
#define SC_PNP_INFO                             0x1200

/* Generic Networking service. */
#define SC_GENERIC_NETWORKING                   0x1201

/* Generic File Transfer service. */
#define SC_GENERIC_FILE_TRANSFER                0x1202

/* Generic Audio service. */
#define SC_GENERIC_AUDIO                        0x1203

/* Generic Telephony service. */
#define SC_GENERIC_TELEPHONY                    0x1204

/* UPnP L2CAP based profile. */
#define SC_UPNP_SERVICE                         0x1205

/* UPnP IP based profile. */
#define SC_UPNP_IP_SERVICE                      0x1206

/* UPnP IP based solution using PAN */
#define SC_ESDP_UPNP_IP_PAN                     0x1300

/* UPnP IP based solution using LAP */
#define SC_ESDP_UPNP_IP_LAP                     0x1301

/* UPnP L2CAP based solution */
#define SC_ESDP_UPNP_L2CAP                      0x1302

/* Video Source */
#define SC_VIDEO_SOURCE                         0x1303

/* Video Sink */
#define SC_VIDEO_SINK                           0x1304

/* Video Sink */
#define SC_VIDEO_DISTRIBUTION                   0x1305

/* End of SdpServiceClassUuid */

/*---------------------------------------------------------------------------
 * SdpProtocolUuid type
 *
 *     Represents the UUID associated with a protocol.
 *
 *     Any number of these UUIDs may be present in the
 *     AID_SERVICE_CLASS_ID_LIST attribute of a service record, and may
 *     appear in the AID_BT_PROFILE_DESC_LIST.
 */
typedef U16 SdpProtocolUuid;

/* Service Discovery Protocol */
#define PROT_SDP                     0x0001

/* UDP Protocol */
#define PROT_UDP                     0x0002

/* RFCOMM Protocol */
#define PROT_RFCOMM                  0x0003

/* TCP Protocol */
#define PROT_TCP                     0x0004

/* TCS Binary Protocol */
#define PROT_TCS_BIN                 0x0005

/* TCS-AT Protocol */
#define PROT_TCS_AT                  0x0006

/* OBEX Protocol */
#define PROT_OBEX                    0x0008

/* IP Protocol */
#define PROT_IP                      0x0009

/* FTP Protocol */
#define PROT_FTP                     0x000A

/* HTTP Protocol */
#define PROT_HTTP                    0x000C

/* WSP Protocol */
#define PROT_WSP                     0x000E

/* BNEP Protocol */
#define PROT_BNEP                    0x000F

/* Universal Plug and Play */
#define PROT_UPNP                    0x0010

/* Human Interface Device Profile */
#define PROT_HIDP                    0x0011

/* Hardcopy Cable Replacement Control Channel */
#define PROT_HCR_CONTROL_CHANNEL     0x0012

/* Hardcopy Cable Replacement Data Channel */
#define PROT_HCR_DATA_CHANNEL        0x0014

/* Hardcopy Cable Replacement Notification*/
#define PROT_HCR_NOTIFICATION        0x0016

/* Audio/Video Control Transport Protocol */
#define PROT_AVCTP                   0x0017

/* Audio/Video Distribution Transport Protocol */
#define PROT_AVDTP                   0x0019

/* Audio/Video Control Transport Protocol Browsing Channel*/
#define PROT_AVCTP_BROWSING          0x001B

/* Unrestricted Digital Information Control Plane */
#define PROT_UDI_C                   0x001D

/* L2CAP Protocol */
#define PROT_L2CAP                   0x0100

/* End of SdpProtocolUuid */

/****************************************************************************
 *
 * Data Structures
 *
 ****************************************************************************/

#if SDP_CLIENT_SUPPORT == XA_ENABLED
#if SDP_PARSING_FUNCS == XA_ENABLED
/*---------------------------------------------------------------------------
 * SdpClientParsingStateInfo structure
 *
 *     Contains all the state information stored by the SDP Client when 
 *     parsing an SDP response.  This state information is mainly necessary 
 *     in the case of a fragmented response (continuation state).
 */
typedef struct _SdpClientParsingStateInfo
{
    U8      state;                  /* Parsing state */

    BOOL    noSkip;                 /* Need to skip the Current Attribute List */
    BOOL    curAttribListEmpty;     /* Current Attribute List is empty */
    U32     curAttribListLen;       /* Current Attribute List length */
    U32     attribListsLen;         /* Attribute Lists length */
    U16     prevAttribId;           /* Previous Attribute ID that was processed */

    U16     attribByteCount;        /* Attribute Byte Count length */
    U32     attribListLen;          /* Attribute List length */
    U16     attribId;               /* Attribute ID being processed */
    U32     attribIdLen;            /* Attribute Id List length */
    U32     attribSkipLen;          /* Attribute length being skipped */
    U32     resultValueLen;         /* Result length */

    U8      storedBuff[17];         /* Stored buffer */
    U8      storedLen;              /* Stored buffer length */
    U8      curStoredOff;           /* Offset into the stored buffer */

    U8      flags;                  /* For error checking */
} SdpClientParsingStateInfo;
#endif /* SDP_PARSING_FUNCS == XA_ENABLED */

/*---------------------------------------------------------------------------
 * SdpQueryToken structure
 *
 *     Defines a query and expresses its results.
 *
 *     Pass this structure to SDP_Query to initiate a query and 
 *     SDP_ParseAttributes to parse the query results. The query results
 *     are returned during the callback function with an event of
 *     SDEVENT_QUERY_RSP, SDEVENT_QUERY_ERR, or SDEVENT_QUERY_FAILED.
 */
typedef struct _SdpQueryToken
{
    ListEntry       node;     /* For internal use only. */

    /* Group: The following fields must be filled in before calling
     * SDP_Query.
     */
    BtRemoteDevice *rm;       /* The remote device to query. An ACL
                               * connection must exist with this device
                               * before the query is started. */

    SdpQueryType    type;     /* Type of query to send. */
    
    const U8       *parms;    /* Parameters to be sent with the query.
                               * The caller of SDP_Query must make sure
                               * that this buffer is composed correctly.
                               */
    
    U16             plen;     /* Number of bytes in the "parms" buffer. */

    BtCallBack      callback; /* Function to call with results */

    /* Group: The following fields must be filled in when calling
     * SDP_Query with the BSQM_CONTINUE query mode.  SDP_ParseAttributes
     * will automatically fill in these fields if BT_STATUS_SDP_CONT_STATE 
     * was returned.  However, if SDP_ParseAttributes is not used, these 
     * fields must be modified manually.
     */
    
    U8              contState[17]; /* Continuation State information */
                                

    U8              contStateLen; /* Continuation State length. This value 
                                   * must range from 1-17 bytes.
                                   */

    /* Group: The following fields are filled in by SDP after completion
     * of the query. These fields should not be modified and are only
     * valid during the SDEVENT_QUERY_RSP or SDEVENT_QUERY_ERR
     * callbacks. After the callback returns, the fields are invalid.
     */

    SdpErrorCode    errorCode;  /* Error code if query failed. Invalid if
                                 * the query succeeded.
                                 */

    /* SDP_ParseAttributes uses these values when the BSPM_BEGINNING or
     * BSPM_CONT_STATE parsing modes are used to parse current SDP 
     * response packet.
     */
    U8*             results;    /* Pointer to query result parameters. */
    
    U16             rLen;       /* Number of bytes in results buffer. */    
       
#if SDP_PARSING_FUNCS == XA_ENABLED
    /* Group: The following fields must be filled in before calling
     * SDP_ParseAttributes.
     */
    SdpAttributeId  attribId;     /* Attribute ID to search for */
    U16             uuid;         /* UUID to search for (use 0 if unused) */
    SdpParsingMode  mode;         /* SDP parsing mode (see SdpParsingMode) */

    /* Group: The following fields are filled in by SDP after
     * completion of SDP_ParseAttributes.
     */
    
    /* Valid when BT_STATUS_SUCCESS and possibly BT_STATUS_SDP_CONT_STATE are
     * returned from SDP_ParseAttributes.  Partial results may be returned,
     * and therefore should not be evaluated until the entire result is 
     * received.
     */
    U32             totalValueLen; /* Total length of the SDP result value */ 
    U16             availValueLen; /* Available length of the SDP result value */
    U8             *valueBuff;     /* Pointer to the available SDP result value */

    /* Valid when BT_STATUS_SUCCESS is returned from SDP_ParseAttributes.
     * SDP_ParseAttributes uses these values when the BSPM_RESUME parsing mode 
     * is used to parse the remaining data in the current SDP response packet.
     */
    U8             *remainBuff;    /* Pointer to the remaining results buffer */
    U16             remainLen;     /* Length of the remaining results buffer */

    /* Group: The following field is for internal use only. */
    SdpClientParsingStateInfo parseData;
#endif /* SDP_PARSING_FUNCS == XA_ENABLED */

    /* Group: The following fields are for internal use only. */
    U16             transId;
    EvmTimer        queryTimer;
} SdpQueryToken;
#endif /* SDP_CLIENT_SUPPORT == XA_ENABLED */

#if SDP_SERVER_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 * SdpAttribute structure
 *
 *     Defines an attribute's ID and value. SdpAttribute structures
 *     are stored in a SdpRecord prior to calling the SDP_AddRecord
 *     function. 
 */
typedef struct _SdpAttribute
{
    SdpAttributeId   id;       /* Attribute ID. */

    U16              len;      /* Length of the value buffer */
    
    const U8        *value;    /* An array of bytes that contains the value
                                * of the attribute. The buffer is in
                                * Data Element form (see SdpDataElemType
                                * and SdpDataElemSize).
                                */
    
    /* Group: The following field is for internal use only */
    U16              flags;

} SdpAttribute;


/*---------------------------------------------------------------------------
 * SdpRecord structure
 *
 *     Defines the contents of a service record. Service records are
 *     initialized and provided to SDP_AddRecord.
 */
typedef struct _SdpRecord
{    
    ListEntry       node;    /* For internal use only. */
    
    U8              num;     /* Total number of attributes in "attribs". */

    /* Pointer to an array of attributes.
     *
     * Store the attributes in ascending order by attribute ID, and
     * do not add two attributes with the same ID.
     *
     * Do not include an AID_SERVICE_RECORD_HANDLE attribute. This
     * attribute is handled automatically by SDP.
     */
    SdpAttribute   *attribs;
  
    /* The service class fields of the class of device. Use the values defined
     * in me.h. The device class portion is ignored.
     */
    BtClassOfDevice classOfDevice; 

    /* Group: The following fields are for internal use only. */    
    U32             handle;         /* Service Record Handle */
    U32             recordState;    /* Service Record State */
    U16             flags;          /* Flag to keep track of marked attributes */
    U16             handleFlag;     /* Flag to keep track of service record handle */
    U16             stateFlag;      /* Flag to keep track of service record state */
  
} SdpRecord;
#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */

/****************************************************************************
 *
 * Section: Function Reference
 *
 ****************************************************************************/

#if SDP_SERVER_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 * SDP_AddRecord()
 *
 *     Adds a service record to the SDP database.
 * 
 * Parameters:
 *     record - The record to add
 *
 * Returns:
 *     BT_STATUS_SUCCESS - The record was added successfully.
 *
 *     BT_STATUS_FAILED - The record was already added.
 *
 *     BT_STATUS_INVALID_PARM - The record not added because it was
 *         improperly formed. It may contain attributes with duplicate
 *         IDs, or the attributes are not in ascending order.
 *         (XA_ERROR_CHECK only.)
 */
BtStatus SDP_AddRecord(SdpRecord *record);


/*---------------------------------------------------------------------------
 * SDP_RemoveRecord()
 *
 *     Removes a service record from the SDP database.
 *
 * Parameters:
 *     record - The record to remove
 *
 * Returns:
 *     BT_STATUS_SUCCESS - record is removed successfully.
 *
 *     BT_STATUS_FAILED - record does not exist in database.
 */
BtStatus SDP_RemoveRecord(SdpRecord *record);

/*---------------------------------------------------------------------------
 * SDP_LockRecord()
 *
 *     If no current SDP Queries are in progress and if the SDP record to
 *     update is a valid entry, lock SDP for record updating.  SDP will
 *     refuse any SDP queries via an SDP error response until the record has
 *     been updated (signaled by SDP_UnlockRecord()).  In order to guarantee 
 *     safe SDP record updates, this function should be called prior to 
 *     altering any SDP record.  SDP can only be locked one time, thus
 *     preventing simultaneous record updated by multiple tasks.
 *
 * Parameters:
 *     record - The record to be updated.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - record is valid, SDP is locked successfully.
 *
 *     BT_STATUS_FAILED - record is invalid, or SDP cannot be locked at 
 *          this time.
 *
 *     BT_STATUS_INVALID_PARM - The record is improperly formed. 
 *          (XA_ERROR_CHECK only.)
 */
BtStatus SDP_LockRecord(SdpRecord *record);

/*---------------------------------------------------------------------------
 * SDP_UnlockRecord()
 *
 *     Unlocks SDP from record updating, and once again allows SDP queries 
 *     to be handled normally.  Once this function is called, it is no longer 
 *     valid to alter any SDP record. If the unlocked record has changed, 
 *     its internal fields are updated appropriately. SDP must be locked 
 *     prior to calling this function.
 *
 * Parameters:
 *     record - The record to be updated.
 *
 *     updated - Indicates whether the record has actually changed.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - record is unlocked and updated successfully.
 *
 *     BT_STATUS_FAILED - record is invalid, or SDP is not already locked.
 *
 *     BT_STATUS_INVALID_PARM - The record is improperly formed. 
 *          (XA_ERROR_CHECK only.)
 */
BtStatus SDP_UnlockRecord(SdpRecord *record, BOOL updated);

/*---------------------------------------------------------------------------
 * SDP_CreateEIRServiceClassList()
 * 
 *     Scans the local SDP records and creates a list of Service Class UUIDs
 *     suitable for registering in the Extended Inquiry Response.  A list can
 *     only consist of UUIDs of the same size, therefore, if UUIDs are stored
 *     in different sizes, a unique list must be made for each size.
 * 
 * Parameters
 *     buffer - A memory location where the list of Service Class UUIDs will
 *     be assembled.
 * 
 *     length - Length of the buffer.
 * 
 *     uuidSize - Size of the UUID in bytes.  Must be 2, 4, or 16.
 *
 * Returns:
 *     Length of the data in the buffer.
 */
U8 SDP_CreateEIRServiceClassList(U8 *buffer, U8 length, U8 uuidSize);

#endif /* SDP_SERVER_SUPPORT == XA_ENABLED */

#if SDP_CLIENT_SUPPORT == XA_ENABLED
/*---------------------------------------------------------------------------
 * SDP_Query()
 *
 *     Performs an SDP query of a remote device's SDP database.
 *
 *     All appropriate fields in the SdpQueryToken must be filled in
 *     before calling SDP_Query, and an ACL connection must already be
 *     present with the remote device.
 *
 *     If this function returns BT_STATUS_PENDING, the token will
 *     be returned later to the callback.
 *
 *     If the callback event is SDEVENT_QUERY_RSP, the query succeeded and
 *     result parameters can be found in the results field of the token.
 *     However, the results buffer must be processed during the callback:
 *     the buffer is invalid after the callback returns.  SDP_ParseAttributes
 *     can be used to process the results buffer for specific values and to
 *     retrieve any continuation state information.
 *
 *     If the callback event is SDEVENT_QUERY_ERR, the remote device
 *     responded with an error code. This can be found in the token.
 *
 *     If the callback event is SDEVENT_QUERY_FAILED, an error prevented
 *     successful the query/response process. This can be caused by
 *     link failure, a failure of the remote device to respond, or
 *     if the query exceeds the server's L2CAP MTU size.
 *
 *     A successful query result may be incomplete. This is indicated
 *     when the continuation state information in the results buffer starts
 *     with a non-zero value. To retrieve more results for the same query,
 *     merely resubmit the query, which will have had the SdpQueryToken
 *     "contState" and "contStateLen" fields updated upon the last
 *     SDP_ParseAttributes call (Note: If SDP_ParseAttributes was not used
 *     to parse the response, the "contState" and "contStateLen" fields must
 *     be modified to contain the continuation state information).  If the 
 *     continuation state information in the results buffer starts with 0, 
 *     all results have been sent.
 *
 * Parameters:
 *     token - Describes the query. If this call returns BT_STATUS_PENDING,
 *         do not modify the token until a callback event returns it.
 *
 *     mode - Indicates the query mode. Use BSQM_FIRST for the
 *         initial query. Use BSQM_CONTINUE only if the
 *         ContinuationState from the initial query's result is non-0.
 *
 * Returns:
 *     BT_STATUS_PENDING - The operation has started successfully. The
 *         callback in the token will be sent result.
 *
 *     BT_STATUS_NO_CONNECTION - The operation failed because there is
 *         no ACL connection to the remote device specified in the token.
 *     
 *     BT_STATUS_NO_RESOURCES - The operation failed because the
 *         query was found to exceed the MTU size of the remote device.
 *     
 *     BT_STATUS_BUSY - The operation failed because connection is in park 
 *         mode.
 *
 *     BT_STATUS_INVALID_TYPE - The operation failed because the type
 *         field in the token is invalid. (XA_ERROR_CHECK only.)
 *     
 *     BT_STATUS_INVALID_PARM - The operation failed because the
 *         parms field in the token is not valid. (XA_ERROR_CHECK only.)
 *
 *     BT_STATUS_FAILED - The operation failed because the fields in
 *         the token are not properly set or the token is already
 *         involved in a query. (XA_ERROR_CHECK only.)
 */
BtStatus SDP_Query(SdpQueryToken *token, SdpQueryMode mode);
#endif /* SDP_CLIENT_SUPPORT == XA_ENABLED */

/*---------------------------------------------------------------------------
 * SDP_GetU16()
 *
 *     Reads a 16-bit integer from the SDP results buffer.
 *
 * Parameters:
 *     buff - The location in the results buffer of a 16-bit integer
 *
 * Returns:
 *     A 16-bit value.
 */
U16 SDP_GetU16(U8 *buff);
#define SDP_GetU16(buff) BEtoHost16((buff))


/*---------------------------------------------------------------------------
 * SDP_GetU32()
 *
 *     Reads a 32-bit integer from the SDP results buffer.
 *
 * Parameters:
 *     buff - The location in the results buffer of a 32-bit integer
 *
 * Returns:
 *     A 32-bit value.
 */
U32 SDP_GetU32(U8 *buff);
#define SDP_GetU32(buff) BEtoHost32((buff))

/*---------------------------------------------------------------------------
 * SDP_PutU16()
 *
 *     Writes a 16-bit integer into an SDP buffer.
 *
 * Parameters:
 *     buff - The write location
 *
 *     val  - The integer to write
 */
void SDP_PutU16(U8 *buff, U16 val);
#define SDP_PutU16(buff,val) StoreBE16((buff),(val))


/*---------------------------------------------------------------------------
 * SDP_PutU32()
 *
 *     Writes a 32-bit integer into an SDP buffer.
 *
 * Parameters:
 *     buff - The write location
 *
 *     val  - The integer to write
 */
void SDP_PutU32(U8 *buff, U32 val);
#define SDP_PutU32(buff,val) StoreBE32((buff),(val))


/*---------------------------------------------------------------------------
 * SDP_ParseDataElement()
 *
 *     Determines the offset and number of bytes in a Data Element. Note that
 *     offset plus the return of this function is the total size of the
 *     Data Element.
 *
 * Parameters:
 *     buff - Pointer to a Data Element
 *
 *     offset - On function exit: contains the number of bytes between buff 
 *         and the start of the Data Element's data field.
 *
 * Returns:
 *     The length of the Data Element's data field.
 */
U32 SDP_ParseDataElement(U8 *buff, U16 *offset);
#define SDP_ParseDataElement(buff,offset) SdpParseElement((buff),(offset))


/*---------------------------------------------------------------------------
 * SDP_UuidCmp()
 *
 *     Compares two Bluetooth UUIDs. UUIDs must be 2, 4, or 16 bytes in
 *     length. UUIDs of different lengths can be compared with this function.
 *
 * Parameters:
 *     uuid1 - Pointer to a buffer holding the first UUI
 *
 *     len1 - Number of bytes of the first UUI
 *
 *     uuid2 - Pointer to a buffer holding the second UUI
 *     
 *     len2 - Number of bytes of the second UUI
 *
 * Returns:
 *     TRUE - UUIDs are equal
 *
 *     FALSE - UUIDS are not equal.
 */
BOOL SDP_UuidCmp(U8 *uuid1, U16 len1, U8 *uuid2, U16 len2);
#define SDP_UuidCmp(uuid1,len1,uuid2,len2) SdpUuidCmp((uuid1), (len1), \
                                                      (uuid2), (len2))

/*---------------------------------------------------------------------------
 * SDP_GetElemType()
 *
 *     Returns a Data Element's type.
 *
 * Parameters:
 *     header - The first byte of a Data Element
 *
 * Returns:
 *     The type of the Data Element.
 */
SdpDataElemType SDP_GetElemType(U8 header);
#define SDP_GetElemType(header) ((header) & DETD_MASK)


/*---------------------------------------------------------------------------
 * SDP_GetElemSize()
 *
 *     Returns a Data Element's size descriptor.
 *
 * Parameters:
 *     header - The first byte of a Data Element
 *
 * Returns:
 *     The size characteristic of the Data Element.
 */
SdpDataElemSize SDP_GetElemSize(U8 header);
#define SDP_GetElemSize(header) ((header) & DESD_MASK)


/*---------------------------------------------------------------------------
 * SDP_GetElemTypeP()
 *
 *     Returns a Data Element's type.
 *
 * Parameters:
 *     buffer - Pointer to the beginning of a Data Element
 *
 * Returns:
 *     The type of the Data Element.
 */
SdpDataElemType SDP_GetElemTypeP(U8 *buffer);
#define SDP_GetElemTypeP(buffer) (((buffer)[0]) & DETD_MASK)


/*---------------------------------------------------------------------------
 * SDP_GetElemSizeP()
 *
 *     Returns a Data Element's size descriptor.
 *
 * Parameters:
 *     buffer - Pointer to the beginning of a Data Element
 *
 * Returns:
 *     The size characteristic of the Data Element.
 */
SdpDataElemSize SDP_GetElemSizeP(U8 *buffer);
#define SDP_GetElemSizeP(buffer) (((buffer)[0]) & DESD_MASK)


/*---------------------------------------------------------------------------
 * SDP_VerifyDataElement()
 *
 *     Verifies that a Data Element is properly formed. All size descriptors
 *     in the element are checked to make sure that the complete Data Element
 *     fits into the indicated length.
 *
 *     In the case of Data Element Sequences or Alternates, this
 *     function is called recursively. The parameter "maxLevel" is used
 *     to stop the recursion in the case of bad data. "maxLevel" must
 *     be large enough to handle the complete Data Element. For example,
 *     a simple data element such as DETD_UINT would need a "maxLevel" of 1,
 *     but a Data Element Sequence of UUIDs would need a "maxLevel" of 2.
 * 
 * Parameters:
 *     data - Pointer to a buffer containing a Data Element.
 *
 *     len - Length (in bytes) of the entire data element. The size
 *         of the Data Element must be less than or equal to this parameter
 *         or this function will fail.
 *
 *     maxLevel - The maximum level of recursion. Must be at least 1.
 *
 * Returns:
 *     TRUE - if the data element is properly formed
 *
 *     FALSE - data element not properly formed, or "maxLevel" reached
 *         0 during recursion.
 */
BOOL SDP_VerifyDataElement(U8 *data, U16 len, U8 maxLevel);
#define SDP_VerifyDataElement(data,len,maxLevel) \
             SdpVerifyDataElement(data,len,maxLevel)

#if (SDP_CLIENT_SUPPORT == XA_ENABLED) && (SDP_PARSING_FUNCS == XA_ENABLED)
/*---------------------------------------------------------------------------
 * SDP_ParseAttributes()
 *
 *     Parses an SDP Query response buffer for an Attribute ID and optionally 
 *     a UUID within that Attribute ID.  For example, to get the RFCOMM 
 *     channel from an SDP response you would provide AID_PROTOCOL_DESC_LIST
 *     as the Attribute ID and PROT_RFCOMM as the UUID.  
 *
 *     This parsing mechanism will parse the current SDP record for attributes, 
 *     until instructed to move to the next SDP record, if one exists. The 
 *     switch to parse the next SDP record in the set of attribute lists can 
 *     be done at any point during parsing by merely requesting an attribute Id 
 *     whose value is numerically less than or equal to the previous attribute 
 *     Id that was parsed for. This behavior is used for the switch, since you 
 *     will only have one instance of each attribute Id per SDP record.
 *
 *     The length of the Attribute ID or UUID result value is returned along 
 *     with a pointer to the beginning of the result value. For example, 
 *     imagine the following hex data was returned in an SDP response:
 *     09 00 04 35 05 19 00 03 08 01.  The first 3 bytes indicate the
 *     AID_PROTOCOL_DESC_LIST (0x0004).  0x35 0x05 is a list of 5 elements. The
 *     next 3 bytes indicate the PROT_RFCOMM (0x0003) UUID.  0x08 is a header
 *     describing the result (in this case a 1-byte UINT).  The RFCOMM channel 
 *     is 0x01.  If you requested the RFCOMM channel, you would be returned a 
 *     length of 1 along with a pointer to the RFCOMM channel (0x01).
 *  
 *     This function should be called during the SDEVENT_QUERY_RSP event, 
 *     since the results buffer is only valid during this callback.
 *
 *     In the case of a continuation state, additional SDP Queries must be 
 *     issued during the context of the callback, or the L2CAP connection
 *     may be disconnected, due to a lack of SDP Queries to process.  If the 
 *     L2CAP connection goes down, any continuation state that was returned is
 *     no longer valid and the SDP Queries will have to be reissued.
 *
 *     This function will properly parse ServiceAttribReq and 
 *     ServiceSearchAttribReq queries.
 *
 *     RECOMMENDATIONS
 *     -  Parse attributes in ascending order since this is how they are
 *        returned.  Failure to do so may result in a failure to find an 
 *        attribute, and may force reparsing of the entire set of SDP 
 *        result buffers.  Note that in the case of a continuation state 
 *        where all of the SDP results buffers might not be available, this 
 *        would dictate reissuing the original SDP query/queries in order 
 *        to retrieve the result buffers.
 *
 *     -  The BSPM_RESUME mode facilitates quicker SDP parsing by starting where
 *        the last SDP parsing call finished and also enables the retrieval of
 *        the same attribute from multiple SDP records.  However, if you query for
 *        attributes out of order, you may query for an attribute that you have 
 *        already passed in a previous SDP results buffer.  A failure to find 
 *        an attribute may require the reparsing of the entire set of SDP result
 *        buffers. 
 *
 * Requires:
 *     SDP_PARSING_FUNCS set to XA_ENABLED.
 *
 * Parameters:
 *     token - SDP query token data structure that also contains the SDP
 *         parsing data fields.  Some of this structure must be filled in
 *         prior to use (see SdpQueryToken).
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Parsing was successful and a result value and 
 *         length was returned. 
 * 
 *         If additional unparsed data exists (see the token->remainBuff 
 *         and token->remainLen fields), additional parsing can be issued 
 *         from the point where parsing ended last (see SdpParsingMode).  
 *         However, it should be known that SDP attributes are returned 
 *         in ascending order.  For best results, attempt to parse 
 *         attributes in ascending order.  This will prevent needing to 
 *         reparse the entire response for each attribute.  
 *
 *     BT_STATUS_FAILED - Parsing encountered an error or the attribute
 *         requested was not found in the current SDP record being parsed.
 *         In these cases, the internal parsing state will be restored to 
 *         its status prior to the failed parse.
 *
 *     BT_STATUS_SDP_CONT_STATE - A continuation state exists (see the
 *         token->contState and token->contStateLen fields).  In the 
 *         case that a continuation state occurs in the middle
 *         of parsing the result value, the partial results will be 
 *         returned (see the token->totalValueLen, token->availValueLen,
 *         and token->valueBuff fields).  In this case, the partial results 
 *         must be saved until the rest of the results are retrieved from 
 *         additional SDP queries.  This is the only situation where the
 *         result value fields will be valid during this return code.
 *         Additional SDP Queries may be issued to retrieve the remainder 
 *         of the SDP response.
 *
 *     token - some fields are filled in by SDP (see SdpQueryToken).
 */
BtStatus SDP_ParseAttributes(SdpQueryToken *token);
#endif /* (SDP_CLIENT_SUPPORT == XA_ENABLED) && (SDP_PARSING_FUNCS == XA_ENABLED) */



#if (SDP_CLIENT_SUPPORT == XA_ENABLED)
/*---------------------------------------------------------------------------
 * SDP_KeepClientAlive()
 *
 *     Requests (or releases a request) to keep the SDP client channel
 *     open. Some Bluetooth devices will drop the ACL link if no L2CAP
 *     channel is open. This can cause problems if the local device is
 *     performing activity that does not require an open L2CAP channel,
 *     but does require the ACL to be left open. (For example, outgoing
 *     mode 2 security operations requiring user input can take 30
 *     seconds or more.)
 *
 *     Calling this API with "keepAlive" TRUE will cause the SDP client
 *     channel to be opened or held open, at least until this function
 *     is called again with "keepAlive" set to FALSE.
 *
 * Requires:
 *     SDP_CLIENT_SUPPORT set to XA_ENABLED.
 *
 * Parameters:
 *     remDev - Remote device on which to keep a channel alive.
 *
 *     keepAlive - If TRUE, opens the SDP client channel and/or keeps it
 *         open. If FALSE, allows the SDP client channel to be closed
 *         at an appropriate time.
 */
void SDP_KeepClientAlive(BtRemoteDevice *remDev, BOOL keepAlive);
#endif /* (SDP_CLIENT_SUPPORT == XA_ENABLED) */


/****************************************************************************
 *
 * Section: SDP Macros 
 *
 ****************************************************************************/

/*---------------------------------------------------------------------------
 * SDP_ATTRIBUTE()
 *
 *     Macro that formats an SdpAttribute structure using the supplied Attribute ID 
 *     and Attribute value. This macro is very useful when formatting the
 *     SdpAttribute structures used to form the attributes in an SDP Record.
 *
 * Parameters:
 *     attribId - SdpAttributeId value (see the AID_ values).
 *     attrib - Array containing the attribute value.
 */
#define SDP_ATTRIBUTE(attribId, attrib) \
          { attribId,           /* Attribute ID */          \
            sizeof(attrib),     /* Attribute Size */        \
            attrib,             /* Attribute Value */       \
            0x0000 }            /* Flag - For Internal Use */


/*---------------------------------------------------------------------------
 * SDP_ATTRIBUTE_ARM()
 *
 *     Macro that formats an SdpAttribute structure using the supplied Attribute ID 
 *     and Attribute size. This macro is very useful when formatting the
 *     SdpAttribute structures used to form the attributes in an SDP Record.
 *
 *     This macro is used for ARM compilers that cannot set the attribute value
 *     using a pointer to ROM. The pointer must be set later at run-time after
 *     the structure is copied to RAM.
 *
 * Parameters:
 *     attribId - SdpAttributeId value (see the AID_ values).
 *     attrib - Array containing the attribute value (used to set size only.)
 */
#define SDP_ATTRIBUTE_ARM(attribId, attrib) \
          { attribId,           /* Attribute ID */          \
            sizeof(attrib),     /* Attribute Size */        \
            0,                  /* Attribute Value - to be set later */ \
            0x0000 }            /* Flag - For Internal Use */


/*---------------------------------------------------------------------------
 * SDP_ATTRIB_HEADER_8BIT()
 *
 *     Macro that forms a Data Element Sequence header using the supplied 8-bit
 *     size value.  Data Element Sequence headers are used in SDP Record Attributes
 *     and SDP Queries. Notice that this macro only forms the header portion
 *     of the Data Element Sequence. The actual Data Elements within this
 *     sequence will need to be formed using other SDP macros.
 *
 * Parameters:
 *     size - 8-bit size of the Data Element Sequence.
 */
#define SDP_ATTRIB_HEADER_8BIT(size) \
            DETD_SEQ + DESD_ADD_8BITS,      /* Type & size index 0x35 */ \
            size                            /* 8-bit size */

/*---------------------------------------------------------------------------
 * SDP_ATTRIB_HEADER_16BIT()
 *
 *     Macro that forms a Data Element Sequence header using the supplied 16-bit
 *     size value.  Data Element Sequence headers are used in SDP Record Attributes
 *     and SDP Queries. Notice that this macro only forms the header portion
 *     of the Data Element Sequence. The actual Data Elements within this
 *     sequence will need to be formed using other SDP macros.
 *
 * Parameters:
 *     size - 16-bit size of the Data Element Sequence.
 */
#define SDP_ATTRIB_HEADER_16BIT(size) \
            DETD_SEQ + DESD_ADD_16BITS,      /* Type & size index 0x36 */ \
            (U8)(((size) & 0xff00) >> 8),    /* Bits[15:8] of size */     \
            (U8)((size) & 0x00ff)            /* Bits[7:0] of size */

/*---------------------------------------------------------------------------
 * SDP_ATTRIB_HEADER_32BIT()
 *
 *     Macro that forms a Data Element Sequence header using the supplied 32-bit
 *     size value.  Data Element Sequence headers are used in SDP Record Attributes
 *     and SDP Queries. Notice that this macro only forms the header portion
 *     of the Data Element Sequence. The actual Data Elements within this
 *     sequence will need to be formed using other SDP macros.
 *
 * Parameters:
 *     size - 32-bit size of the Data Element Sequence.
 */
#define SDP_ATTRIB_HEADER_32BIT(size) \
            DETD_SEQ + DESD_ADD_32BITS,         /* Type & size index 0x37 */ \
            (U8)(((size) & 0xff000000) >> 24),  /* Bits[32:24] of size */    \
            (U8)(((size) & 0x00ff0000) >> 16),  /* Bits[23:16] of size */    \
            (U8)(((size) & 0x0000ff00) >> 8),   /* Bits[15:8] of size */     \
            (U8)((size) & 0x000000ff)           /* Bits[7:0] of size */

/*---------------------------------------------------------------------------
 * SDP_ATTRIB_HEADER_ALT_8BIT()
 *
 *     Macro that forms a Data Element Sequence Alternative header using the 
 *     supplied 8-bit size value.  Data Element Sequence Alternative headers 
 *     are used in SDP Record Attributes. Notice that this macro only forms 
 *     the header portion of the Data Element Sequence Alternative. The actual 
 *     Data Element Sequences within this alternative will need to be formed 
 *     using other SDP macros.
 *
 * Parameters:
 *     size - 8-bit size of the Data Element Sequence Alternative.
 */
#define SDP_ATTRIB_HEADER_ALT_8BIT(size) \
            DETD_ALT + DESD_ADD_8BITS,      /* Type & size index 0x35 */ \
            size                            /* 8-bit size */

/*---------------------------------------------------------------------------
 * SDP_ATTRIB_HEADER_ALT_16BIT()
 *
 *     Macro that forms a Data Element Sequence Alternative header using the 
 *     supplied 16-bit size value.  Data Element Sequence Alternative headers 
 *     are used in SDP Record Attributes. Notice that this macro only forms 
 *     the header portion of the Data Element Sequence Alternative. The actual 
 *     Data Element Sequences within this alternative will need to be formed 
 *     using other SDP macros.
 *
 * Parameters:
 *     size - 16-bit size of the Data Element Sequence Alternative.
 */
#define SDP_ATTRIB_HEADER_ALT_16BIT(size) \
            DETD_ALT + DESD_ADD_16BITS,      /* Type & size index 0x36 */ \
            (U8)(((size) & 0xff00) >> 8),    /* Bits[15:8] of size */     \
            (U8)((size) & 0x00ff)            /* Bits[7:0] of size */

/*---------------------------------------------------------------------------
 * SDP_ATTRIB_HEADER_ALT_32BIT()
 *
 *     Macro that forms a Data Element Sequence Alternative header using the 
 *     supplied 32-bit size value.  Data Element Sequence Alternative headers 
 *     are used in SDP Record Attributes. Notice that this macro only forms 
 *     the header portion of the Data Element Sequence Alternative. The actual 
 *     Data Element Sequences within this alternative will need to be formed 
 *     using other SDP macros.
 *
 * Parameters:
 *     size - 32-bit size of the Data Element Sequence Alternative.
 */
#define SDP_ATTRIB_HEADER_ALT_32BIT(size) \
            DETD_ALT + DESD_ADD_32BITS,         /* Type & size index 0x37 */ \
            (U8)(((size) & 0xff000000) >> 24),  /* Bits[32:24] of size */    \
            (U8)(((size) & 0x00ff0000) >> 16),  /* Bits[23:16] of size */    \
            (U8)(((size) & 0x0000ff00) >> 8),   /* Bits[15:8] of size */     \
            (U8)((size) & 0x000000ff)           /* Bits[7:0] of size */

/*---------------------------------------------------------------------------
 * SDP_UUID_16BIT()
 *
 *     Macro that forms a UUID Data Element from the supplied 16-bit UUID value. 
 *     UUID data elements are used in SDP Record Attributes and SDP Queries.
 *
 * Parameters:
 *     uuid - 16-bit UUID value (see the SC_ and PROT_ values).
 */
#define SDP_UUID_16BIT(uuid) \
            DETD_UUID + DESD_2BYTES,         /* Type & size index 0x19 */ \
            (U8)(((uuid) & 0xff00) >> 8),    /* Bits[15:8] of UUID */     \
            (U8)((uuid) & 0x00ff)            /* Bits[7:0] of UUID */

/*---------------------------------------------------------------------------
 * SDP_UUID_32BIT()
 *
 *     Macro that forms a UUID Data Element from the supplied 32-bit UUID value. 
 *     UUID data elements are used in SDP Record Attributes and SDP Queries.
 *
 * Parameters:
 *     uuid - 32-bit UUID value (see the SC_ and PROT_ values for 16-bit 
 *            values). 16-bit UUID values can be converted to 32-bit by 
 *            zero extending the 16-bit value.
 */
#define SDP_UUID_32BIT(uuid) \
            DETD_UUID + DESD_4BYTES,            /* Type & size index 0x1A */ \
            (U8)(((uuid) & 0xff000000) >> 24),  /* Bits[32:24] of UUID */    \
            (U8)(((uuid) & 0x00ff0000) >> 16),  /* Bits[23:16] of UUID */    \
            (U8)(((uuid) & 0x0000ff00) >> 8),   /* Bits[15:8] of UUID */     \
            (U8)((uuid) & 0x000000ff)           /* Bits[7:0] of UUID */

/*---------------------------------------------------------------------------
 * SDP_UUID_128BIT()
 *
 *     Macro that forms a UUID Data Element from the supplied 128-bit UUID value. 
 *     UUID data elements are used in SDP Record Attributes and SDP Queries.
 *
 * Parameters:
 *     uuid - 128-bit UUID value (see the SC_ and PROT_ values for 16-bit 
 *            values). 16-bit UUID values can be converted to 128-bit using 
 *            the following conversion: 128_bit_value = 16_bit_value * 2^96 +
 *            Bluetooth Base UUID.
 */
#define SDP_UUID_128BIT(uuid)                /* UUID must be a 16-byte array */ \
            DETD_UUID + DESD_16BYTES,        /* Type & size index 0x1C */ \
            uuid                             /* 128-bit UUID */

/*---------------------------------------------------------------------------
 * SDP_UINT_8BIT()
 *
 *     Macro that forms a UINT Data Element from the supplied 8-bit UINT value. 
 *     UINT data elements are used in SDP Record Attributes and SDP Queries.
 *
 * Parameters:
 *     uint - 8-bit UINT value.
 */
#define SDP_UINT_8BIT(uint) \
            DETD_UINT + DESD_1BYTE,          /* Type & size index 0x08 */ \
            (U8)(uint)                       /* 8-bit UINT */

/*---------------------------------------------------------------------------
 * SDP_UINT_16BIT()
 *
 *     Macro that forms a UINT Data Element from the supplied 16-bit UINT value. 
 *     UINT data elements are used in SDP Record Attributes and SDP Queries.
 *
 * Parameters:
 *     uint - 16-bit UINT value.
 */
#define SDP_UINT_16BIT(uint) \
            DETD_UINT + DESD_2BYTES,         /* Type & size index 0x09 */ \
            (U8)(((uint) & 0xff00) >> 8),    /* Bits[15:8] of UINT */     \
            (U8)((uint) & 0x00ff)            /* Bits[7:0] of UINT */

/*---------------------------------------------------------------------------
 * SDP_UINT_32BIT()
 *
 *     Macro that forms a UINT Data Element from the supplied 32-bit UINT value. 
 *     UINT data elements are used in SDP Record Attributes and SDP Queries.
 *
 * Parameters:
 *     uint - 32-bit UINT value.
 */
#define SDP_UINT_32BIT(uint) \
            DETD_UINT + DESD_4BYTES,            /* Type & size index 0x0A */ \
            (U8)(((uint) & 0xff000000) >> 24),  /* Bits[31:24] of UINT */    \
            (U8)(((uint) & 0x00ff0000) >> 16),  /* Bits[23:16] of UINT */    \
            (U8)(((uint) & 0x0000ff00) >> 8),   /* Bits[15:8] of UINT */     \
            (U8)((uint) & 0x000000ff)           /* Bits[7:0] of UINT */

/*---------------------------------------------------------------------------
 * SDP_UINT_64BIT()
 *
 *     Macro that forms a UINT Data Element from the supplied 64-bit UINT value. 
 *     UINT data elements are used in SDP Record Attributes and SDP Queries.
 *
 * Parameters:
 *     uint - 64-bit UINT value.
 */
#define SDP_UINT_64BIT(uint)                    /* UINT must be an 8-byte array */ \
            DETD_UINT + DESD_8BYTES,            /* Type & size index 0x0B */ \
            uint                                /* 64-bit UINT */

/*---------------------------------------------------------------------------
 * SDP_UINT_128BIT()
 *
 *     Macro that forms a UINT Data Element from the supplied 128-bit UINT value. 
 *     UINT data elements are used in SDP Record Attributes and SDP Queries.
 *
 * Parameters:
 *     uint - 128-bit UINT value.
 */
#define SDP_UINT_128BIT(uint)                   /* UINT must be a 16-byte array */ \
            DETD_UINT + DESD_16BYTES,           /* Type & size index 0x0C */ \
            uint                                /* 128-bit UINT */

/*---------------------------------------------------------------------------
 * SDP_TEXT_8BIT()
 *
 *     Macro that forms a TEXT Data Element Header from the supplied 8-bit size 
 *     value. TEXT data elements are used in SDP Record Attributes and SDP Queries.
 *     Notice that this macro only forms the header portion of the TEXT Data 
 *     Element. The actual TEXT data within this data element will need to 
 *     be provided separately.
 *
 * Parameters:
 *     size - 8-bit size value.
 */
#define SDP_TEXT_8BIT(size) \
            DETD_TEXT + DESD_ADD_8BITS,      /* Type & size index 0x25 */ \
            (U8)(size)                       /* 8-bit size */

/*---------------------------------------------------------------------------
 * SDP_TEXT_16BIT()
 *
 *     Macro that forms a TEXT Data Element Header from the supplied 16-bit size 
 *     value. TEXT data elements are used in SDP Record Attributes and SDP Queries.
 *     Notice that this macro only forms the header portion of the TEXT Data 
 *     Element. The actual TEXT data within this data element will need to 
 *     be provided separately.
 *
 * Parameters:
 *     size - 16-bit size value.
 */
#define SDP_TEXT_16BIT(size) \
            DETD_TEXT + DESD_ADD_16BITS,      /* Type & size index 0x26 */ \
            (U8)(((size) & 0xff00) >> 8),     /* Bits[15:8] of size */     \
            (U8)((size) & 0x00ff)             /* Bits[7:0] of size */

/*---------------------------------------------------------------------------
 * SDP_TEXT_32BIT()
 *
 *     Macro that forms a TEXT Data Element Header from the supplied 32-bit size 
 *     value. TEXT data elements are used in SDP Record Attributes and SDP Queries.
 *     Notice that this macro only forms the header portion of the TEXT Data 
 *     Element. The actual TEXT data within this data element will need to 
 *     be provided separately.
 *
 * Parameters:
 *     size - 32-bit size value.
 */
#define SDP_TEXT_32BIT(size) \
            DETD_TEXT + DESD_ADD_32BITS,        /* Type & size index 0x27 */ \
            (U8)(((size) & 0xff000000) >> 24),  /* Bits[32:24] of size */    \
            (U8)(((size) & 0x00ff0000) >> 16),  /* Bits[23:16] of size */    \
            (U8)(((size) & 0x0000ff00) >> 8),   /* Bits[15:8] of size */     \
            (U8)((size) & 0x000000ff)           /* Bits[7:0] of size */

/*---------------------------------------------------------------------------
 * SDP_BOOL()
 *
 *     Macro that forms a BOOL Data Element from the supplied 8-bit boolean value. 
 *     BOOL data elements are used in SDP Record Attributes and SDP Queries.
 *
 * Parameters:
 *     value - 8-bit boolean value.
 */
#define SDP_BOOL(value) \
            DETD_BOOL + DESD_1BYTE,          /* Type & size index 0x28 */ \
            (U8)(value)                      /* Boolean value */

/****************************************************************************
 *
 * Internal function prototypes
 *
 ****************************************************************************/

U32 SdpParseElement(const U8* p, U16* offset);
BOOL SdpVerifyDataElement(const U8* data, U16 len, U8 maxLevel);
BOOL SdpUuidCmp(const U8* uuid1, U16 len1, const U8* uuid2, U16 len2);

#endif /* __SDPH */



