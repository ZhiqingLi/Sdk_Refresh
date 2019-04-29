/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */

#ifndef _BT_AT_TYPES_H_
#define _BT_AT_TYPES_H_

#define CLI_NUMBER_MAX  32
#define CLI_SATYPE_MAX  32
#define CLI_ALPHA_MAX   32

/* AT Command Codes */
enum {
    AT_COMMAND_VGM = 0,         /* Gain of microphone */
    AT_COMMAND_VGS,             /* Gain of speaker */
    
    /* HSP-Specific AT Command Codes */
    AT_COMMAND_BUTTON_PRESSED,  /* HS Button pressed */

    /* HFP-Specific AT Command Codes */
    AT_COMMAND_ATA,             /* Answer call */
    AT_COMMAND_ATD,             /* Dial - place a call by dialing a number */
    AT_COMMAND_BLDN,            /* Get Bluetooth last dialed number */
    AT_COMMAND_BRSF,            /* Bluetooth retrieve supported feature */
    AT_COMMAND_BTRH_READ,       /* Get Bluetooth response and hold status */
    AT_COMMAND_BTRH_SET,        /* Set Bluetooth response and hold feature */
    AT_COMMAND_BVRA,            /* Enable/disable Bluetooth voice recognition */
    AT_COMMAND_CCWA,            /* Enable/disable Call Waiting notification */
    AT_COMMAND_CHLD,            /* Set call-hold and multi-party handling */
    AT_COMMAND_CHLD_TEST,       /* Get call-hold and multi-party services */
    AT_COMMAND_CHUP,            /* Call hang up - terminate active call */
    AT_COMMAND_CIND_READ,       /* Get current AG indicators status */
    AT_COMMAND_CIND_TEST,       /* Get supported AG indicators mapping */
    AT_COMMAND_CLCC,            /* Get list of current calls */
    AT_COMMAND_CLIP,            /* Enable/disable CLI notification */
    AT_COMMAND_CMER,            /* Enable/disable events reporting */
    AT_COMMAND_CNUM,            /* Get subscriber number information */
    AT_COMMAND_COPS_READ,       /* Get network operator string */
    AT_COMMAND_COPS_SET,        /* Set network operator string format */
    AT_COMMAND_VTS,                  /* Transmit DTMF code */ 
    AT_COMMAND_BAC,             /* For hfp 1.6 or later*/
    AT_COMMAND_BCC,             /* For hfp 1.6 or later*/
    AT_COMMAND_BCS,             /* For hfp 1.6 or later*/
    AT_COMMAND_NREC,
    AT_COMMAND_XAPL,
    AT_COMMAND_IPHONEACCEV,
#ifdef CONFIG_PRODUCT_TEST_INF
    AT_COMMAND_BKCMD,
#endif    
	AT_COMMAND_END	
};
typedef uint8_t at_cmd_code_t;

/* AT Result Codes */
enum {
    AT_RESULT_OK = 100,         /* Command execution acknowledgment */
    AT_RESULT_ERROR,            /* Error indication */

    AT_RESULT_RING,             /* Incoming call indication */
    AT_RESULT_VGM,              /* Gain of microphone */
    AT_RESULT_VGS,              /* Gain of speaker */

    /* HFP-Specific AT Result Codes */
    AT_RESULT_BRSF,             /* AG supported features */
    AT_RESULT_BSIR,             /* In-band ring tone setting */
    AT_RESULT_BTRH,             /* Bluetooth response and hold state */
    AT_RESULT_BVRA,             /* Voice recognition activation */
    AT_RESULT_CCWA,             /* Call Waiting notification */
    AT_RESULT_CHLD_TEST,        /* Call-hold & multi-party handling services */
    AT_RESULT_CIEV,             /* AG indicator events reporting (status) */
    AT_RESULT_CIND_READ,        /* AG indicators status */
    AT_RESULT_CIND_TEST,        /* Supported AG indicators mapping */
    AT_RESULT_CLCC,             /* List of current calls */
    AT_RESULT_CLIP,             /* Calling Line ID (CLI) notification */
    AT_RESULT_CME_ERROR,        /* Extended AG audio error */
    AT_RESULT_CNUM,             /* Subscriber number information */
    AT_RESULT_COPS,              /* Network operator string */
    AT_RESULT_XAPL,
    AT_RESULT_BCS,
#ifdef CONFIG_PRODUCT_TEST_INF
    AT_RESULT_BKCMD,
#endif
    AT_RESULT_END
};
typedef uint8_t at_result_code_t;

#define AT_CODE_INVALID 0xff
typedef int8_t at_result_bcs_t;
/* AT_RESULT_VGM (+VGM) Result Data -- Microphone Gain */
typedef int8_t at_result_vgm_t;   /* 0--15 */

/* AT_RESULT_VGS (+VGS) Result Data -- Speaker Gain */
typedef int8_t at_result_vgs_t;   /* 0--15 */

/* AT_COMMAND_BRSF (AT+BRSF) Command HFP HF Supported Feature Bits */
#define HF_EC_NR_FUNCTION                       (1 << 0)
#define HF_CALL_WAITING_AND_3WAY_CALLING        (1 << 1)
#define HF_CLI_PRESENTATION_CAPABILITY          (1 << 2)
#define HF_VOICE_RECOGNITION_ACTIVATION         (1 << 3)
#define HF_REMOTE_VOLUME_CONTROL                (1 << 4)
#define HF_ENHANCED_CALL_STATUS                 (1 << 5)
#define HF_ENHANCED_CALL_CONTROL                (1 << 6)
#define HF_CODEC_NEGOTIATION                    (1 << 7)
#define HF_HF_INDICATORS                        (1 << 8)
#define HF_ESCO_S4_T2_SUPPORTED                 (1 << 9)


/* HFP AG Supported Feature Bits */
#define AG_3WAY_CALLING                         (1 << 0)
#define AG_EC_NR_FUNCTION                       (1 << 1)
#define AG_VOICE_RECOGNITION_FUNCTION           (1 << 2)
#define AG_IN_BAND_RING_TONE_CAPABILITY         (1 << 3)
#define AG_ATTACH_A_NUMBER_TO_A_VOICE_TAG       (1 << 4)
#define AG_ABILITY_TO_REJECT_A_CALL             (1 << 5)
#define AG_ENHANCED_CALL_STATUS                 (1 << 6)
#define AG_ENHANCED_CALL_CONTROL                (1 << 7)
#define AG_EXTENDED_ERROR_RESULT_CODES          (1 << 8)

#define AG_CODEC_NEGOTIATION                    (1 << 9)
#define AG_HF_INDICATORS                        (1 << 10)
#define AG_ESCO_S4_T2_SUPPORTED                 (1 << 11)

/* AT_RESULT_BRSF (+BRSF) Result Data - AG Supported Features Bitmask */
typedef int16_t at_result_brsf_t;

/* AT_RESULT_BSIR (+BSIR) Result Data -- AG In-Band Ring Tone Setting */
typedef int8_t at_result_bsir_t;  /* 0 - AG provides non in-band ring tone
                                   * 1 - AG provides an in-band ring tone  */

/* AT_COMMAND_BTRH_SET (AT+BTRH=<n> Set) Command -
   AG Response and Hold Feature States to Set */
typedef enum {
    AT_BTRH_HOLD_INCOMING   = 0,    /* Put incoming call on hold */
    AT_BTRH_ACCEPT_HELD     = 1,    /* Accept held incoming call */
    AT_BTRH_REJECT_HELD     = 2     /* Reject held incoming call */
} at_cmd_btrh_t;

/* AT_RESULT_BTRH (+BTRH) Result Data -- Current AG Response and Hold State */
typedef int8_t at_result_btrh_t;  /* 0 - Incoming call is put on hold in the AG
                                     1 - Held incoming call is accepted in the
                                         AG
                                     2 - Held incoming call is rejected in the
                                   *     AG */

/* AT_RESULT_BVRA (+BVRA) Result Data -- AG Voice Recognition Function State */
typedef int8_t at_result_bvra_t; /* 0 - Voice recognition is disabled in the AG.
                                    1 - Voice recognition is enabled in the AG.
                                  */

/* AT_RESULT_CCWA (+CCWA) Result Data - Call Waiting Notification Information */
typedef struct {
    char    number[CLI_NUMBER_MAX]; /* Phone number, as a double-quoted string
                                     */
    int16_t type;                   /* Phone number format integer; -1 if N/A */
    int16_t info_class;             /* Information-class bitmask:
                                       1 << 0 - Voice (telephony)
                                       1 << 1 - Data
                                       1 << 2 - Fax (facsimile services)
                                       1 << 3 - Short message service
                                       1 << 4 - Data circuit sync
                                       1 << 5 - Data circuit async
                                       1 << 6 - Dedicated packet access
                                       1 << 7 - Dedicated PAD access
                                     */
    char    alpha[CLI_ALPHA_MAX];   /* Alphanumeric representation of the
                                       phone number ("number"), corresponding
                                       to the phone-book entry */
    int8_t  cli_validity;           /* CLI validity (-1 if N/A):
                                       0 - CLI is valid.
                                       1 - CLI has been withheld by the
                                           originator.
                                       2 - CLI is not available due to
                                           interworking problems or limitations
                                           of the originating network.
                                     */
    char    subaddr[CLI_SATYPE_MAX];/* Subaddress string */
    int8_t  satype;                 /* Subaddress integer format; -1 if N/A */
    int8_t  priority;               /* Incoming-call eMLPP priority-level digit;
                                       -1 if N/A
                                     */
} at_result_ccwa_t;

/* AT_COMMAND_CHLD (AT+CHLD=<n>) Command Values -- Call-Hold and Multi-Party
 * Handling (Three-Way Call Handling) Actions */
typedef enum {
    AT_CHLD_RELEASE_ALL_HELD               = 0, /* Release all held calls or set
                                                   UDUB for a waiting call */
    AT_CHLD_RELEASE_ALL_ACTIVE_ACCEPT_HELD = 1, /* Release all active calls and
                                                   accept the other call */
    AT_CHLD_RELEASE_INDICATED_ACTIVE       = 1, /* Release the specified active
                                                   call */
    AT_CHLD_HOLD_ALL_ACTIVE_ACCEPT_HELD    = 2, /* Place all active calls on
                                                   hold and accept other call */
    AT_CHLD_HOLD_ALL_EXCEPT_INDICATED      = 2, /* Request private consultation
                                                   mode with the specified call
                                                 */
    AT_CHLD_MULTIPARTY                     = 3, /* Add held call to conversation
                                                   -- place all calls on hold
                                                   except the specified call */
    AT_CHLD_EXPLICIT_CALL_TRANSFER         = 4  /* Connect 2 calls & disconnect
                                                   subscriber from both calls
                                                   (Explicit Call Transfer) */
} at_cmd_chld_t;

/* AG-Available Call-Hold and Multi-Party (Three-Way Call) Service Bits */
#define AG_CHLD_0       (1 << 0)    /* Release all held calls or set UDUB for
                                       a waiting call */
#define AG_CHLD_1       (1 << 1)    /* Release all active calls and accept the
                                       other (held/waiting) call */
#define AG_CHLD_1x      (1 << 2)    /* Release the specified active call */
#define AG_CHLD_2       (1 << 3)    /* Place all active calls on hold and accept
                                       the other (held/waiting) call */
#define AG_CHLD_2x      (1 << 4)    /* Request private consultation mode with
                                       the specified call -- place all calls on
                                       hold except the specified call */
#define AG_CHLD_3       (1 << 5)    /* Add a held call to the conversation */
#define AG_CHLD_4       (1 << 6)    /* Connect the two calls and disconnect the
                                       subscriber from both calls
                                       (Explicit Call Transfer) */

/* AT_RESULT_CHLD_TEST (+CHLD) Result Data -- a Bitmask of Call-Hold and
 * Multi-Party (Three-Way Call) Services available in the AG */
typedef int8_t at_result_chld_t;

/* AG Call-Indicator Codes */
/* The comments list the status values supported for each indicator. */
enum {
    CIND_SERVICE = 0,       /* Service availability indicator:
                               0 - Implies no home/roam network service
                               1 - Implies available home/roam network service
                             */
    CIND_CALL,              /* Standard call status indicator:
                               0 - There are no calls in progress.
                               1 - There is at least one call in progress.
                             */
    CIND_CALLSETUP,         /* Bluetooth propriety call-setup status indicator:
                               0 - Not currently in call set-up
                               1 - Ongoing incoming call process
                               2 - Ongoing outgoing call set-up
                               3 - Remote party being alerted in outgoing call
                             */
    CIND_CALLHELD,          /* Bluetooth process call-hold status indicator:
                               0 - No held calls
                               1 - Call is placed on hold, or active/held calls
                                   are swapped; the AG has both an active and a
                                   held call.
                               2 - Call on hold, no active call
                             */
    CIND_SIGNAL,            /* Signal strength indicator: 0--5 */
    CIND_ROAM,              /* Roaming status indicator:
                               0 - Roaming is not active
                               1 - Roaming is active
                             */
    CIND_BATTCHG,           /* Battery charge indicator: 0--5 */
    /* The following codes are not covered by the HFP specification: */
    CIND_MESSAGE,           /* Message indicator:
                               0 - No message received
                               1 - Message received
                             */
    CIND_BATTERYWARNING,    /* Battery warning indicator:
                               0 - No battery warning
                               1 - Battery warning
                            */
    CIND_CHARGERCONNECTED,  /* Charger connection indicator:
                               0 - Charger is not connected
                               1 - Charger is connected
                             */
    CIND_LAST
};
typedef uint8_t cind_code_t;

#define ag_cind_2_str(cind_code) (                              \
    (cind_code) == CIND_SERVICE ? "service" :                   \
    (cind_code) == CIND_CALL ? "call" :                         \
    (cind_code) == CIND_CALLSETUP ? "callsetup" :               \
    (cind_code) == CIND_CALLSETUP ? "call_setup":               \
    (cind_code) == CIND_CALLHELD ? "callheld" :                 \
    (cind_code) == CIND_SIGNAL ? "signal" :                     \
    (cind_code) == CIND_ROAM ? "roam" :                         \
    (cind_code) == CIND_BATTCHG ? "battchg" :                   \
    (cind_code) == CIND_MESSAGE ? "message" :                   \
    (cind_code) == CIND_BATTERYWARNING ? "batterywarning" :     \
    (cind_code) == CIND_CHARGERCONNECTED ? "chargerconnected" : \
    "unknown")

/* AT_RESULT_CIEV (+CIEV: 3,0) Result Data -- Standard Indicator Status */
typedef struct {
    cind_code_t cind_code;  /* AG indicator code */
    int8_t      val;        /* Current status of the AG indicator */
} at_result_ciev_t;

/* AT_RESULT_CIND_READ (+CIND: 1,1,2,0) Result Data -- AG Indicators Status */
typedef struct {
    int8_t      data[CIND_LAST];    /* Array of the current states of the AG
                                       indicators; -1 if N/A (no indicator)  */
} at_result_cind_t;

/* AG Indicator Mapping to Corresponding Order-Index and Value Ranges
 */
typedef struct {
    uint8_t     index;  /* Indicator index */
    int8_t      min;    /* Minimum indicator status value; -1 if N/A */
    int8_t      max;    /* Maximum indicator status value; -1 if N/A */
} cind_map_t;

/* AT_RESULT_CIND_TEST (+CIND Test) Result Data -- AG-Supported Indicators
 * Mapping */
typedef struct {
    cind_map_t  data[CIND_LAST];
} at_result_cind_map_t;

/* AT_RESULT_CLCC (+CLCC) Result Data -- Standard List of Current Calls */
typedef struct {
    int8_t      call_id;    /* Call identification number (one-based) */
    int8_t      dir;        /* Call direction:
                               0 - Outgoing call (mobile originated - MO)
                               1 - Incoming call (mobile terminated -MT)
                             */
    int8_t      stat;       /* Call status:
                               0 - Active
                               1 - Held
                               2 - Dialing (MO call)
                               3 - Alerting (MO call)
                               4 - Incoming (MT call)
                               5 - Waiting (MT call)
                               6 - Call held by Response and Hold
                             */
    int8_t      mode;       /* Call mode:
                               0 - Voice
                               1 - Data
                               2 - Fax
                               The following modes are defined in the 3GPP TS
                               spec, and are not part of the HFP Spec:
                               3 - Voice followed by data, voice mode
                               4 - Alternating voice/data, voice mode
                               5 - Alternating voice/fax, voice mode
                               6 - Voice followed by data, data mode
                               7 - Alternating voice/data, data mode
                               8 - Alternating voice/fax, fax mode
                               9 - Unknown
                             */
    int8_t      mpty;       /* 0 - Call is not a member of a multi-party
                                   (conference) call
                               1 - Call is a member of multi-party (conference)
                                   call
                             */
    char        number[CLI_NUMBER_MAX]; /* Optional phone number, as a
                                         * double-quoted string */
    int16_t     type;       /* Optional address integer format; -1 if N/A */
    char        alpha[CLI_ALPHA_MAX];   /* Alphanumeric representation of the
                                           phone number ("number"),
                                           corresponding to the phone-book entry
                                        */
    int8_t  priority;       /* Incoming-call eMLPP priority-level digit;
                               -1 if N/A
                             */
} at_result_clcc_t;

/* AT_RESULT_CLIP (+CLIP) Result Data --
 * Calling Line Identification (CLI) Notification Data */
typedef struct {
    char    number[CLI_NUMBER_MAX]; /* Phone number, as a double-quoted string
                                     */
    int16_t type;                   /* Phone number format integer; -1 if N/A */
    char    subaddr[CLI_SATYPE_MAX];/* Subaddress string */
    int8_t  satype;                 /* Subaddress integer format; -1 if N/A */
    char    alpha[CLI_ALPHA_MAX];   /* Alphanumeric representation of the
                                       phone number ("number"), corresponding
                                       to the phone-book entry */
    int8_t  cli_validity;           /* CLI validity (-1 if N/A):
                                       0 - CLI is valid.
                                       1 - CLI has been withheld by the
                                           originator.
                                       2 - CLI is not available due to
                                           interworking problems or limitations
                                           of the originating network.
                                     */
} at_result_clip_t;

/* AT_RESULT_CME_ERROR (+CME ERROR) Result Data -- Extended AG Audio Error Code
 */
#define CME_ERROR_AG_FAILURE                                    0
#define CME_ERROR_NO_CONNECTION_TO_PHONE                        1
#define CME_ERROR_OPERATION_NOT_ALLOWED                         3
#define CME_ERROR_OPERATION_NOT_SUPPORTED                       4
#define CME_ERROR_PH_SIM_PIN_REQUIRED                           5
#define CME_ERROR_SIM_NOT_INSERTED                              10
#define CME_ERROR_SIM_PIN_REQUIRED                              11
#define CME_ERROR_SIM_PUK_REQUIRED                              12
#define CME_ERROR_SIM_FAILURE                                   13
#define CME_ERROR_SIM_BUSY                                      14
#define CME_ERROR_INCORRECT_PASSWORD                            16
#define CME_ERROR_SIM_PIN2_REQUIRED                             17
#define CME_ERROR_SIM_PUK2_REQUIRED                             18
#define CME_ERROR_MEMORY_FULL                                   20
#define CME_ERROR_INVALID_INDEX                                 21
#define CME_ERROR_MEMORY_FAILURE                                23
#define CME_ERROR_TEXT_STRING_TOO_LONG                          24
#define CME_ERROR_INVALID_CHARS_IN_TEXT_STR                     25
#define CME_ERROR_DIAL_STRING_TOO_LONG                          26
#define CME_ERROR_INVALID_CHARS_IN_DIAL_STR                     27
#define CME_ERROR_NO_NETWORK_SERVICE                            30
#define CME_ERROR_EMERGENCY_CALLS_ONLY                          32
typedef uint8_t at_result_cme_error_t;

/* AT_RESULT_CNUM (+CNUM) Result Data -- Subscriber Number Information */
typedef struct {
    char    number[CLI_NUMBER_MAX]; /* Phone number, as a double-quoted string
                                    */
    int16_t type;                   /* Phone number format integer; -1 if N/A */
} at_result_cnum_t;
typedef struct {
    char    name[CLI_NUMBER_MAX]; /* Phone name, as a string                                   */
    int16_t para;                   /* Phone parameter format integer; -1 if N/A */
    
} at_result_xapl_t;
#define COPS_OPERATOR_MAX       16
/* AT_RESULT_COPS (+COPS) Result Data -- Network Operator */
typedef struct {
    char name[COPS_OPERATOR_MAX];   /* Network operator name, as a quoted
                                       alphanumeric string */
} at_result_cops_t;

/* AT Result Information */
/* A struct of this type is passed to the application within the @at_result
   parameter of the driver's input notification callback (e.g.,
   bt_hsp_hs_input_t for the HSP HS role driver - see bt_hsp_hs.h). */
typedef struct {
    at_result_code_t code;  /* AT Result Code */
    /* AT result data; the comments indicate the related result code. */
    union {
        at_result_vgm_t         vgm;            /* AT_RESULT_VGM */
        at_result_vgs_t         vgs;            /* AT_RESULT_VGS */
        at_result_bcs_t         bcs; 
#ifdef CONFIG_PRODUCT_TEST_INF
        at_result_code_t       bkcmd;
#endif
        /* HFP-Specific AT Result Data */
        at_result_brsf_t        brsf;           /* AT_RESULT_BRSF */
        at_result_bsir_t        bsir;           /* AT_RESULT_BSIR */
        at_result_btrh_t        btrh;           /* AT_RESULT_BTRH */
        at_result_bvra_t        bvra;           /* AT_RESULT_BVRA */
        at_result_ccwa_t        ccwa;           /* AT_RESULT_CCWA */
        at_result_chld_t        chld;           /* AT_RESULT_CHLD_TEST */
        at_result_ciev_t        ciev;           /* AT_RESULT_CIEV */
        at_result_cind_t        cind;           /* AT_RESULT_CIND_READ */
        at_result_cind_map_t    cind_map;       /* AT_RESULT_CIND_TEST */
        at_result_clcc_t        clcc;           /* AT_RESULT_CLCC */
        at_result_clip_t        clip;           /* AT_RESULT_CLIP */
        at_result_cme_error_t   cme_error;      /* AT_RESULT_CME_ERROR */
        at_result_cnum_t        cnum;           /* AT_RESULT_CNUM */
        at_result_cops_t        cops;           /* AT_RESULT_COPS */
        at_result_xapl_t        xapl;           /* AT_RESULT_XAPL */
    } u;
} at_result_t;

/* Hands-Free (HF) Information */
typedef struct {
    /* Audio Gateway (AG) Information */
    at_result_brsf_t    ag_supported_features;
    at_result_chld_t    ag_3way_services;

    /* Hands-Free (HF) Information */
    at_result_brsf_t    hf_supported_features;
} handsfree_info_t;

uint16_t hfp_check_features(uint16_t flag);

#endif
