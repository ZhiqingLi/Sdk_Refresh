
#ifndef __CODEC_COM_CODEC_H__
#define __CODEC_COM_CODEC_H__

/*****************************************************************************
  1 ÆäËûÍ·ÎÄ¼þ°üº¬
*****************************************************************************/
//#include "codec_op_lib.h"
//#include "CodecInterface.h"
//#include "ImsCodecInterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  2 ºê¶¨Òå
*****************************************************************************/
extern VOS_INT16                        g_shwCodecPcmFrameLength;              /* PCMÊý¾ÝÖ¡³¤¶È */

#define CODEC_WB_PCM_MASK           ((VOS_INT16)0xfffC)                     /*¶ÔÓÚ¿í´ø£¬16±ÈÌØPCM±à½âÂëÆ÷½öÈ¡¸ß14±ÈÌØÁ¿»¯ÓÐÐ§ */
#define CODEC_PCM_MASK              (0xfff8)                                /*16±ÈÌØPCM±à½âÂëÆ÷½öÈ¡¸ß13±ÈÌØÁ¿»¯ÓÐÐ§ */
#define CODEC_EHF_MASK              (0x0008)                                /*HomingÖ¡±àÂëÇ°/½âÂëºó¹Ì¶¨Öµ*/
#ifdef SPEECH_STREAM_UNIT_128
#define CODEC_FRAME_LENGTH_NB       (128)                                    /*8K²ÉÑùÂÊÊÇÃ¿Ö¡PCMÂëÁ÷µÄÑùµãÊý*/
#define CODEC_FRAME_LENGTH_WB       (256)                                   /*16K²ÉÑùÂÊÊÇÃ¿Ö¡PCMÂëÁ÷µÄÑùµãÊý*/
#define CODEC_FRAME_LENGTH_MU       (768)                                   /*48K²ÉÑùÂÊÊÇÃ¿Ö¡PCMÂëÁ÷µÄÑùµãÊý*/
#else
#define CODEC_FRAME_LENGTH_NB       (120)                                    /*8K²ÉÑùÂÊÊÇÃ¿Ö¡PCMÂëÁ÷µÄÑùµãÊý*/
#define CODEC_FRAME_LENGTH_WB       (240)                                   /*16K²ÉÑùÂÊÊÇÃ¿Ö¡PCMÂëÁ÷µÄÑùµãÊý*/
#define CODEC_FRAME_LENGTH_MU       (720)                                   /*48K²ÉÑùÂÊÊÇÃ¿Ö¡PCMÂëÁ÷µÄÑùµãÊý*/
#endif

#define CODEC_DWORD_BITS_NUM              (32)                                    /* 4×Ö½Ú¶ÔÓ¦±ÈÌØÊý */
#define CODEC_WORD_BITS_NUM               (16)                                    /* 2×Ö½Ú¶ÔÓ¦±ÈÌØÊý */
#define CODEC_BYTE_BITS_NUM               (8)                                     /* 1×Ö½Ú¶ÔÓ¦±ÈÌØÊý */
#define BIT0_MASK                       (0x0001)
#define BIT15_MASK                      (0x8000)
#define BIT31_MASK                      (0x80000000)

#ifndef MIN
#define MIN(var1,var2)                  ((var1)>(var2)?(var2):(var1))
#endif

#ifdef SPEECH_STREAM_UNIT_128
#define CODEC_PCM_WB_FRAME_LENGTH             ((VOS_UINT16)256)              /* ¿í´øÓïÒôÒ»Ö¡PCMÊý¾ÝµÄÖ¡³¤,µ¥Î»²ÉÑùµã */
#else
#define CODEC_PCM_WB_FRAME_LENGTH             ((VOS_UINT16)240)              /* ¿í´øÓïÒôÒ»Ö¡PCMÊý¾ÝµÄÖ¡³¤,µ¥Î»²ÉÑùµã */
#endif
#define CODEC_PCM_MAX_FRAME_LENGTH            (CODEC_FRAME_LENGTH_WB)    /* Ò»Ö¡PCMÊý¾ÝµÄ×î´óÖ¡³¤,ÓÃÓÚ¶¨ÒåPCM»º³åÇø£¬µ¥Î»²ÉÑùµã */

#define CODEC_PcmGetPcmFrameLength()          (g_shwCodecPcmFrameLength)
#define CODEC_PcmSetPcmFrameLength(shwPcmFrameLen) (g_shwCodecPcmFrameLength = shwPcmFrameLen)
#define CODEC_PCM_FRAME_LENGTH                 CODEC_PcmGetPcmFrameLength()
#define CODEC_PCM_FRAME_LENGTH_BYTES          (CODEC_PCM_FRAME_LENGTH * (VOS_UINT16)sizeof(VOS_INT16))

/*****************************************************************************
  3 Ã¶¾Ù¶¨Òå
*****************************************************************************/
/*****************************************************************************
 ÊµÌåÃû³Æ  : CODEC_SAMPLE_RATE_MODE_ENUM
 ¹¦ÄÜÃèÊö  : ²ÉÑùÂÊ¶¨Òå
*****************************************************************************/
enum CODEC_SAMPLE_RATE_MODE_ENUM
{
    CODEC_SAMPLE_RATE_MODE_8000             = 8000,
    CODEC_SAMPLE_RATE_MODE_16000            = 16000,
    CODEC_SAMPLE_RATE_MODE_BUTT
};
typedef VOS_INT32 CODEC_SAMPLE_RATE_MODE_ENUM_INT32;

/*****************************************************************************
 ÊµÌåÃû³Æ  : CODEC_VAD_FLAG_ENUM
 ¹¦ÄÜÃèÊö  : VAD¶¨Òå
*****************************************************************************/
enum CODEC_VAD_FLAG_ENUM
{
    CODEC_VAD_FLAG_NOISE              = 0,
    CODEC_VAD_FLAG_SPEECH             = 1,
    CODEC_USED_BUTT
};
typedef VOS_INT16 CODEC_VAD_FLAG_ENUM_INT16;

/* Ä£¿éÊÇ·ñÔÚÊ¹ÓÃÃ¶¾Ù¶¨Òå */
/*****************************************************************************
 ÊµÌåÃû³Æ  : CODEC_OBJ_USED_STATUS_ENUM
 ¹¦ÄÜÃèÊö  : Ä£¿éÊÇ·ñÒÑÊ¹ÓÃ¶¨Òå
*****************************************************************************/
enum CODEC_OBJ_USED_STATUS_ENUM
{
    CODEC_OBJ_UNUSED              = 0,
    CODEC_OBJ_USED,
    CODEC_OBJ_BUTT
};
typedef VOS_UINT16  CODEC_OBJ_USED_STATUS_ENUM_UINT16;

/*****************************************************************************
 ÊµÌåÃû³Æ  : CODEC_RET_ENUM
 ¹¦ÄÜÃèÊö  : ½Ó¿Ú·µ»ØÖµ¶¨Òå
*****************************************************************************/
enum CODEC_RET_ENUM
{
    CODEC_RET_OK                    = 0,
    CODEC_RET_ERR                   = 1,
    CODEC_RET_BUTT
};
typedef VOS_UINT32 CODEC_RET_ENUM_UINT32;

/*****************************************************************************
 ÊµÌåÃû³Æ  : CODEC_AMR_MODE_ENUM
 ¹¦ÄÜÃèÊö  : AMRËÙÂÊÄ£Ê½¶¨Òå
*****************************************************************************/
enum CODEC_AMR_MODE_ENUM
{
    CODEC_AMR_MODE_475              = 0,
    CODEC_AMR_MODE_515,
    CODEC_AMR_MODE_59,
    CODEC_AMR_MODE_67,
    CODEC_AMR_MODE_74,
    CODEC_AMR_MODE_795,
    CODEC_AMR_MODE_102,
    CODEC_AMR_MODE_122,
    CODEC_AMR_MODE_MRDTX,
    CODEC_AMR_MODE_BUTT
};
typedef VOS_UINT16 CODEC_AMR_MODE_ENUM_UINT16;

/*****************************************************************************
 ÊµÌåÃû³Æ  : CODEC_AMRWB_MODE_ENUM
 ¹¦ÄÜÃèÊö  : AMRËÙÂÊÄ£Ê½¶¨Òå
*****************************************************************************/
enum CODEC_AMRWB_MODE_ENUM
{
    CODEC_AMRWB_MODE_660            = 0,
    CODEC_AMRWB_MODE_885,
    CODEC_AMRWB_MODE_1265,
    CODEC_AMRWB_MODE_1425,
    CODEC_AMRWB_MODE_1585,
    CODEC_AMRWB_MODE_1825,
    CODEC_AMRWB_MODE_1985,
    CODEC_AMRWB_MODE_2305,
    CODEC_AMRWB_MODE_2385,
    CODEC_AMRWB_MODE_MRDTX,
    CODEC_AMRWB_MODE_BUTT
};
typedef VOS_UINT16 CODEC_AMRWB_MODE_ENUM_UINT16;

/*****************************************************************************
 ÊµÌåÃû³Æ  : CODEC_OBJ_ENUM
 ¹¦ÄÜÃèÊö  : ÉùÂëÆ÷±àÂëÆ÷/½âÂëÆ÷ÀàÐÍ¶¨Òå
*****************************************************************************/
enum CODEC_OBJ_ENUM
{
    CODEC_OBJ_ENCODER              = 0,
    CODEC_OBJ_DECODER,
    CODEC_OBJ_BUT
};
typedef VOS_UINT16  CODEC_OBJ_ENUM_UINT16;

/*****************************************************************************
 ÊµÌåÃû³Æ  : CODEC_PREFRAME_TYPE_ENUM
 ¹¦ÄÜÃèÊö  : Ö®Ç°Ö¡×´Ì¬
*****************************************************************************/
enum CODEC_PREFRAME_TYPE_ENUM
{
    CODEC_PREFRAME_NONE            = 0,                                         /* ÎÞÉÏÒ»Ö¡ */
    CODEC_PREFRAME_SPEECH,                                                      /* ÓïÒôÖ¡ */
    CODEC_PREFRAME_SID,                                                         /* SIDÖ¡ */
    CODEC_PREFRAME_BUTT
};
typedef VOS_UINT16  CODEC_PREFRAME_TYPE_ENUM_UINT16;

/*****************************************************************************
  4 ÏûÏ¢Í·¶¨Òå
*****************************************************************************/


/*****************************************************************************
  5 ÏûÏ¢¶¨Òå
*****************************************************************************/


/*****************************************************************************
  6 STRUCT¶¨Òå
*****************************************************************************/

/*****************************************************************************
 ÊµÌåÃû³Æ  : CODEC_ENC_IN_PARA_STRU
 ¹¦ÄÜÃèÊö  : ÉùÂëÆ÷±àÂëÊäÈë²ÎÊý½á¹¹Ìå
*****************************************************************************/
#if 0
typedef struct
{
    CODEC_DTX_ENUM_UINT16               enDtxMode;                              /* Ê¹ÄÜDTX¹¦ÄÜÄ£Ê½ */
    CODEC_AMR_FORMAT_ENUM_UINT16        enAmrFormat;                            /* ¶ÔÓÚAMR£¬Ö¸Ê¾µ±Ç°ÊÇUMTS»òGSMÂëÁ÷ */
    VOS_UINT16                          enAmrMode;                              /* AMR±àÂëÊ±Ê¹ÓÃµÄËÙÂÊÄ£Ê½ */
    CODEC_AMR_NSYNC_ENUM_UINT16         enAmrNsync;                             /* ÊäÈë£¬AMR DTXÊ¹ÄÜÊ±Ö¸Ê¾ÇÐ»», ¹¦ÄÜÎ´ÊµÏÖ, ÔÝÎ´Ê¹ÓÃ */
    VOS_INT16                          *pshwEncSpeech;                          /* Ö¸ÏòÊäÈë±àÂëÆ÷µÄPCMÂëÁ÷Ö¸Õë */
}CODEC_ENC_IN_PARA_STRU;

/*****************************************************************************
 ÊµÌåÃû³Æ  : CODEC_ENC_OUT_PARA_STRU
 ¹¦ÄÜÃèÊö  : ÉùÂëÆ÷±àÂëÊä³ö²ÎÊý½á¹¹Ìå
*****************************************************************************/
typedef struct
{
    CODEC_AMR_TYPE_TX_ENUM_UINT16       enAmrFrameType;                         /* GSMÏÂAMR±àÂëºóÊä³öµÄÖ¡ÀàÐÍ */
    CODEC_VAD_ENUM_UINT16               enVadFlag;                              /* HR/FR/EFRÊ¹ÓÃ, Voice Active Detection flag */
    CODEC_SP_ENUM_UINT16                enSpFlag;                               /* HR/FR/EFRÊ¹ÓÃ, Speech flag */
    VOS_UINT16                          enAmrMode;                              /* AMR/AMR WB±àÂëÊ±Ê¹ÓÃµÄËÙÂÊÄ£Ê½ */
    VOS_INT16                          *pshwEncSerial;                          /* Ö¸Ïò±àÂëÆ÷Êä³öµÄÂëÁ÷Ö¸Õë */
}CODEC_ENC_OUT_PARA_STRU;

/*****************************************************************************
 ÊµÌåÃû³Æ  : CODEC_DEC_IN_PARA_STRU
 ¹¦ÄÜÃèÊö  : ÉùÂëÆ÷½âÂëÊäÈë²ÎÊý½á¹¹Ìå
*****************************************************************************/
typedef struct
{
    CODEC_AMR_FORMAT_ENUM_UINT16        enAmrFormat;                            /* Ö¸Ê¾µ±Ç°ÊÇUMTS»òGSMÂëÁ÷ */
    VOS_UINT16                          enAmrMode;                              /* AMR½âÂëÊ±Ê¹ÓÃµÄËÙÂÊÄ£Ê½ */
    CODEC_AMR_TYPE_RX_ENUM_UINT16       enAmrFrameType;                         /* GSMÏÂAMR½ÓÊÕÖ¡ÀàÐÍ */
    CODEC_BFI_ENUM_UINT16               enBfiFlag;                              /* HR/FR/EFRÊ¹ÓÃ, Bad Frame Indication */
    CODEC_SID_ENUM_UINT16               enSidFlag;                              /* HR/FR/EFRÊ¹ÓÃ, Silence Descriptor flag */
    CODEC_TAF_ENUM_UINT16               enTafFlag;                              /* HR/FR/EFRÊ¹ÓÃ, Time Alignment flag  */
    CODEC_BFI_ENUM_UINT16               enHrUfiFlag;                            /* HRÊ¹ÓÃ, Unreliable Frame Indication */
    VOS_UINT16                          uhwFrameFacchCnt;                       /* Á¬ÐøÊÕµ½µÄFAÖ¡¼ÆÊý */
    VOS_INT16                           shwFrameRangeMax;                       /* Ê±Óò·ù¶È×î´óÖµ */
    VOS_INT16                           shwReserved;
    VOS_INT16                          *pshwDecSerial;                          /* Ö¸ÏòÐÅµÀÒëÂë²úÉúÓÃÓÚÉùÂëÆ÷½âÂëÊý¾ÝÁ÷µÄÖ¸Õë*/
    CODEC_PREFRAME_TYPE_ENUM_UINT16     enGoodFrameType;                        /* ºÃÖ¡ÀàÐÍ:0-ÎÞÉÏÒ»Ö¡ 1-ÓïÒôÖ¡ 2-SIDÖ¡ */
    VOS_UINT16                          uhwFrameBfiCnt;                         /* Á¬ÐøÊÕµ½µÄBFIÖ¡¼ÆÊý */
    VOS_UINT16                          uhwRxTypeForAjb;                        /* Ö¡ÀàÐÍ£¬ÓÃÓÚAJB£¬·ÇAJBÊ±²»ÓÃ¹ØÐÄ */
    VOS_UINT16                          uhwReserve;
}CODEC_DEC_IN_PARA_STRU;

/*****************************************************************************
 ÊµÌåÃû³Æ  : CODEC_DEC_PREIN_PARA_STRU
 ¹¦ÄÜÃèÊö  : ÉùÂëÆ÷½âÂëÇ°Ò»Ö¡ÊäÈë²ÎÊý½á¹¹Ìå
*****************************************************************************/
typedef struct
{
    CODEC_AMR_FORMAT_ENUM_UINT16        enAmrFormat;                            /* Ö¸Ê¾µ±Ç°ÊÇUMTS»òGSMÂëÁ÷ */
    VOS_UINT16                          enAmrMode;                              /* AMR½âÂëÊ±Ê¹ÓÃµÄËÙÂÊÄ£Ê½ */
    CODEC_AMR_TYPE_RX_ENUM_UINT16       enAmrFrameType;                         /* GSMÏÂAMR½ÓÊÕÖ¡ÀàÐÍ */
    CODEC_BFI_ENUM_UINT16               enBfiFlag;                              /* HR/FR/EFRÊ¹ÓÃ, Bad Frame Indication */
    CODEC_SID_ENUM_UINT16               enSidFlag;                              /* HR/FR/EFRÊ¹ÓÃ, Silence Descriptor flag */
    CODEC_TAF_ENUM_UINT16               enTafFlag;                              /* HR/FR/EFRÊ¹ÓÃ, Time Alignment flag  */
    CODEC_BFI_ENUM_UINT16               enHrUfiFlag;                            /* HRÊ¹ÓÃ, Unreliable Frame Indication */
    VOS_UINT16                          uhwFrameFacchCnt;                       /* Á¬ÐøÊÕµ½µÄFAÖ¡¼ÆÊý */
}CODEC_DEC_PREIN_PARA_STRU;
/*****************************************************************************
 ÊµÌåÃû³Æ  : IMS_DATA_OBJ_STRU
 ¹¦ÄÜÃèÊö  : Decode Buffer´æ·ÅµÄIMSÏÂÐÐÊý¾Ý¸ñÊ½
*****************************************************************************/
typedef struct
{
    unsigned short                      usRateMode;                             /* ÉÏÐÐ±àÂëµÄRateMode */
    unsigned short                      usErrorFlag;                            /* BufferÃ»Êý¾ÝÊ±µÄErrorFlag */
    unsigned short                      usFrameType;                            /* ÏÂÐÐ±¾Ö¡µÄFrameType */
    unsigned short                      usQualityIdx;                           /* Quality indification */
    unsigned short                      ausData[IMSA_CODEC_MAX_DATA_LEN];       /* IF1Ð­ÒéÖÐµÄA¡¢B¡¢C×ÓÁ÷£¬ABC½ôÃÜÅÅÁÐ£¬Byte¶ÔÆë */
}IMS_DL_DATA_OBJ_STRU;

/*****************************************************************************
 ÊµÌåÃû³Æ  : IMS_DATA_OBJ_STRU
 ¹¦ÄÜÃèÊö  : Encode Buffer´æ·ÅµÄIMSÉÏÐÐÊý¾Ý¸ñÊ½,ÓëImsCodecInterface¶ÔÓ¦
*****************************************************************************/
typedef struct
{
    CODEC_AMR_TYPE_TX_ENUM_UINT16       usStatus;
    VOS_UINT16                          usFrameType;                            /* ÉÏÐÐ±¾Ö¡µÄFrameType */
    VOS_UINT16                          ausData[IMSA_CODEC_MAX_DATA_LEN];       /* IF1Ð­ÒéÖÐµÄA¡¢B¡¢C×ÓÁ÷£¬ABC½ôÃÜÅÅÁÐ£¬Byte¶ÔÆë */
}IMS_UL_DATA_OBJ_STRU;
#endif
/*****************************************************************************
  7 UNION¶¨Òå
*****************************************************************************/


/*****************************************************************************
  8 OTHERS¶¨Òå
*****************************************************************************/
/* Ö¡ÀàÐÍ»Øµ÷º¯Êý */
typedef VOS_INT16 (*CODEC_FRAMETYPE_CALLBACK)(VOS_UINT16);

/*****************************************************************************
  9 È«¾Ö±äÁ¿ÉùÃ÷
*****************************************************************************/


/*****************************************************************************
  10 º¯ÊýÉùÃ÷
*****************************************************************************/
#if 0
/*****************************************************************************
 º¯ Êý Ãû  : AMR_Dec_Init
 ¹¦ÄÜÃèÊö  : AMR³õÊ¼»¯º¯Êý
 ÊäÈë²ÎÊý  : ÎÞ
 Êä³ö²ÎÊý  : ÎÞ
 ·µ »Ø Öµ  : ³É¹¦·µ»ØCODEC_RET_OK
             Ê§°Ü·µ»ØCODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 AMR_Dec_Init(VOS_VOID);

/*****************************************************************************
 º¯ Êý Ãû  : AMR_Enc_Init
 ¹¦ÄÜÃèÊö  : AMR±àÂë³õÊ¼»¯º¯Êý
 ÊäÈë²ÎÊý  : enDtxMode         --Ê¹ÄÜDTX¹¦ÄÜÄ£Ê½, 1ÎªÊ¹ÄÜ, 0Îª½ûÓÃ
             pfCodecCallBack   --Ö¡ÀàÐÍ»Øµ÷º¯Êý
 Êä³ö²ÎÊý  : ÎÞ
 ·µ »Ø Öµ  : ³É¹¦·µ»ØCODEC_RET_OK
             Ê§°Ü·µ»ØCODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 AMR_Enc_Init(
                       CODEC_DTX_ENUM_UINT16 enDtxMode,
                       VOS_VOID                 *pfCodecCallBack);

/*****************************************************************************
 º¯ Êý Ãû  : AMR_Encode
 ¹¦ÄÜÃèÊö  : AMR±àÂë
 ÊäÈë²ÎÊý  : pstEncInPara      --  Ö¸ÏòÉùÂëÆ÷±àÂëÊäÈë²ÎÊý½á¹¹ÌåµÄÖ¸Õë
 Êä³ö²ÎÊý  : pstEncOutPara     --  Ö¸Ïò±àÂëºóÊä³ö²ÎÊý½á¹¹ÌåµÄÖ¸Õë
 ·µ »Ø Öµ  : ³É¹¦·µ»ØCODEC_RET_OK
             Ê§°Ü·µ»ØCODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 AMR_Encode(
                CODEC_ENC_IN_PARA_STRU  *pstEncInPara,
                CODEC_ENC_OUT_PARA_STRU *pstEncOutPara);

/*****************************************************************************
 º¯ Êý Ãû  : AMR_Decode
 ¹¦ÄÜÃèÊö  : AMR½âÂë
 ÊäÈë²ÎÊý  : pstDecPara     --  Ö¸ÏòÉùÂëÆ÷½âÂë½á¹¹ÌåµÄÖ¸Õë
 Êä³ö²ÎÊý  : pshwDecSpeech  --  ½âÂëÊý¾Ý,³¤¶ÈÎª160¸ö×Ö½ÚµÄPCMÊý¾Ý
 ·µ »Ø Öµ  : ³É¹¦·µ»ØCODEC_RET_OK
             Ê§°Ü·µ»ØCODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 AMR_Decode(
                CODEC_DEC_IN_PARA_STRU *pstDecPara,
                VOS_INT16                  *pshwDecSpeech);

/*****************************************************************************
 º¯ Êý Ãû  : AMRWB_Dec_Init
 ¹¦ÄÜÃèÊö  : AMRWB½âÂë³õÊ¼»¯º¯Êý
 ÊäÈë²ÎÊý  : ÎÞ
 Êä³ö²ÎÊý  : ÎÞ
 ·µ »Ø Öµ  : ³É¹¦·µ»ØCODEC_RET_OK
             Ê§°Ü·µ»ØCODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 AMRWB_Dec_Init(VOS_VOID);

/*****************************************************************************
 º¯ Êý Ãû  : AMRWB_Enc_Init
 ¹¦ÄÜÃèÊö  : AMRWB±àÂë³õÊ¼»¯º¯Êý
 ÊäÈë²ÎÊý  : enDtxMode         --Ê¹ÄÜDTX¹¦ÄÜÄ£Ê½, 1ÎªÊ¹ÄÜ, 0Îª½ûÓÃ
             pfCodecCallBack   --Ö¡ÀàÐÍ»Øµ÷º¯Êý
 Êä³ö²ÎÊý  : ÎÞ
 ·µ »Ø Öµ  : ³É¹¦·µ»ØCODEC_RET_OK
             Ê§°Ü·µ»ØCODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 AMRWB_Enc_Init(
                CODEC_DTX_ENUM_UINT16  enDtxMode,
                VOS_VOID                  *pfCodecCallBack);

/*****************************************************************************
 º¯ Êý Ãû  : AMRWB_Encode
 ¹¦ÄÜÃèÊö  : AMRWB±àÂë
 ÊäÈë²ÎÊý  : pstEncInPara      --  Ö¸ÏòÉùÂëÆ÷±àÂëÊäÈë²ÎÊý½á¹¹ÌåµÄÖ¸Õë
 Êä³ö²ÎÊý  : pstEncOutPara     --  Ö¸Ïò±àÂëºóÊä³ö²ÎÊý½á¹¹ÌåµÄÖ¸Õë
 ·µ »Ø Öµ  : ³É¹¦·µ»ØCODEC_RET_OK
             Ê§°Ü·µ»ØCODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 AMRWB_Encode(
                CODEC_ENC_IN_PARA_STRU  *pstEncInPara,
                CODEC_ENC_OUT_PARA_STRU *pstEncOutPara);

/*****************************************************************************
 º¯ Êý Ãû  : AMRv_Decode
 ¹¦ÄÜÃèÊö  : AMRWB½âÂë
 ÊäÈë²ÎÊý  : pstDecPara     --  Ö¸ÏòÉùÂëÆ÷½âÂë½á¹¹ÌåµÄÖ¸Õë
 Êä³ö²ÎÊý  : pshwDecSpeech  --  ½âÂëÊý¾Ý,³¤¶ÈÎª160¸ö×Ö½ÚµÄPCMÊý¾Ý
 ·µ »Ø Öµ  : ³É¹¦·µ»ØCODEC_RET_OK
             Ê§°Ü·µ»ØCODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 AMRWB_Decode(
                CODEC_DEC_IN_PARA_STRU *pstDecPara,
                VOS_INT16                  *pshwDecSpeech);

/*****************************************************************************
 º¯ Êý Ãû  : EFR_Dec_Init
 ¹¦ÄÜÃèÊö  : EFR½âÂë³õÊ¼»¯º¯Êý
 ÊäÈë²ÎÊý  : ÎÞ
 Êä³ö²ÎÊý  : ÎÞ
 ·µ »Ø Öµ  : ³É¹¦·µ»ØCODEC_RET_OK
             Ê§°Ü·µ»ØCODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 EFR_Dec_Init(VOS_VOID);

/*****************************************************************************
 º¯ Êý Ãû  : EFR_Enc_Init
 ¹¦ÄÜÃèÊö  : EFR³õÊ¼»¯º¯Êý
 ÊäÈë²ÎÊý  : enDtxMode  --±íÊ¾Ê¹ÄÜDTX¹¦ÄÜÄ£Ê½£¬1ÎªÊ¹ÄÜ£¬0Îª½ûÓÃ
 Êä³ö²ÎÊý  : ÎÞ
 ·µ »Ø Öµ  : ³É¹¦·µ»ØCODEC_RET_OK
             Ê§°Ü·µ»ØCODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 EFR_Enc_Init(CODEC_DTX_ENUM_UINT16 enDtxMode);

/*****************************************************************************
 º¯ Êý Ãû  : EFR_Encode
 ¹¦ÄÜÃèÊö  : EFR±àÂëº¯Êý
 ÊäÈë²ÎÊý  : pstEncInPara      --Ö¸ÏòÉùÂëÆ÷±àÂëÊäÈë²ÎÊý½á¹¹ÌåµÄÖ¸Õë
 Êä³ö²ÎÊý  : pstEncOutPara     --Ö¸Ïò±àÂëºóÊä³ö²ÎÊý½á¹¹ÌåµÄÖ¸Õë
 ·µ »Ø Öµ  : ³É¹¦·µ»ØCODEC_RET_OK
             Ê§°Ü·µ»ØCODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 EFR_Encode(
                CODEC_ENC_IN_PARA_STRU  *pstEncInPara,
                CODEC_ENC_OUT_PARA_STRU *pstEncOutPara);

/*****************************************************************************
 º¯ Êý Ãû  : EFR_Decode
 ¹¦ÄÜÃèÊö  : EFR½âÂëº¯Êý
 ÊäÈë²ÎÊý  : pstDecPara     --Ö¸ÏòÉùÂëÆ÷±àÂë½á¹¹ÌåµÄÖ¸Õë
 Êä³ö²ÎÊý  : pshwDecSpeech  --±àÂëÊý¾Ý£¬³¤¶ÈÎª160¸ö×Ö½ÚµÄPCMÊý¾Ý
 ·µ »Ø Öµ  : ³É¹¦·µ»ØCODEC_RET_OK
             Ê§°Ü·µ»ØCODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 EFR_Decode(
                CODEC_DEC_IN_PARA_STRU *pstDecPara,
                VOS_INT16                  *pshwDecSpeech);

/*****************************************************************************
 º¯ Êý Ãû  : FR_Dec_Init
 ¹¦ÄÜÃèÊö  : FR½âÂë³õÊ¼»¯º¯Êý
 ÊäÈë²ÎÊý  : ÎÞ
 Êä³ö²ÎÊý  : ÎÞ
 ·µ »Ø Öµ  : ³É¹¦·µ»ØCODEC_RET_OK
             Ê§°Ü·µ»ØCODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 FR_Dec_Init(VOS_VOID);

/*****************************************************************************
 º¯ Êý Ãû  : FR_Enc_Init
 ¹¦ÄÜÃèÊö  : FR±àÂë³õÊ¼»¯º¯Êý
 ÊäÈë²ÎÊý  : enDtxMode ±íÊ¾Ê¹ÄÜDTX¹¦ÄÜÄ£Ê½£¬1ÎªÊ¹ÄÜ£¬0Îª½ûÓÃ
 Êä³ö²ÎÊý  : ÎÞ
 ·µ »Ø Öµ  : ³É¹¦·µ»ØCODEC_RET_OK
             Ê§°Ü·µ»ØCODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 FR_Enc_Init(CODEC_DTX_ENUM_UINT16 enDtxMode);

/*****************************************************************************
 º¯ Êý Ãû  : FR_Encode
 ¹¦ÄÜÃèÊö  : Íê³ÉÒ»Ö¡160¸öÓïÒôÊý¾ÝµÄFR±àÂë²Ù×÷
 ÊäÈë²ÎÊý  : pstEncInPara      --  Ö¸ÏòÉùÂëÆ÷±àÂëÊäÈë²ÎÊý½á¹¹ÌåµÄÖ¸Õë
 Êä³ö²ÎÊý  : pstEncOutPara     --  Ö¸Ïò±àÂëºóÊä³ö²ÎÊý½á¹¹ÌåµÄÖ¸Õë
 ·µ »Ø Öµ  : ³É¹¦·µ»ØCODEC_RET_OK
             Ê§°Ü·µ»ØCODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 FR_Encode(
                CODEC_ENC_IN_PARA_STRU  *pstEncInPara,
                CODEC_ENC_OUT_PARA_STRU *pstEncOutPara);

/*****************************************************************************
 º¯ Êý Ãû  : FR_Decode
 ¹¦ÄÜÃèÊö  : FR½âÂëº¯Êý
 ÊäÈë²ÎÊý  : pstVocodecDecPara --Ö¸ÏòÉùÂëÆ÷½âÂë½á¹¹ÌåµÄÖ¸Õë
 Êä³ö²ÎÊý  : pshwDecSpeech     --½âÂëºóÊý¾Ý£¬³¤¶ÈÎª160¸ö×Ö½ÚµÄPCMÊý¾Ý
 ·µ »Ø Öµ  : ³É¹¦·µ»ØCODEC_RET_OK
             Ê§°Ü·µ»ØCODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 FR_Decode(
                CODEC_DEC_IN_PARA_STRU  *pstVocodecDecPara,
                VOS_INT16 *pshwDecSpeech);

/*****************************************************************************
 º¯ Êý Ãû  : HR_Dec_Init
 ¹¦ÄÜÃèÊö  : HR½âÂë³õÊ¼»¯º¯Êý
 ÊäÈë²ÎÊý  : ÎÞ
 Êä³ö²ÎÊý  : ÎÞ
 ·µ »Ø Öµ  : ³É¹¦·µ»ØCODEC_RET_OK
             Ê§°Ü·µ»ØCODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 HR_Dec_Init(VOS_VOID);

/*****************************************************************************
 º¯ Êý Ãû  : HR_Enc_Init
 ¹¦ÄÜÃèÊö  : HR±àÂë³õÊ¼»¯º¯Êý
 ÊäÈë²ÎÊý  : enDtxMode ±íÊ¾Ê¹ÄÜDTX¹¦ÄÜÄ£Ê½£¬1ÎªÊ¹ÄÜ£¬0Îª½ûÓÃ
 Êä³ö²ÎÊý  : ÎÞ
 ·µ »Ø Öµ  : ³É¹¦·µ»ØCODEC_RET_OK
             Ê§°Ü·µ»ØCODEC_RET_ERR
*****************************************************************************/

extern VOS_UINT32 HR_Enc_Init(CODEC_DTX_ENUM_UINT16 enDtxMode);

/*****************************************************************************
 º¯ Êý Ãû  : HR_Encode
 ¹¦ÄÜÃèÊö  : Íê³ÉÒ»Ö¡160¸öÓïÒôÊý¾ÝµÄHR±àÂë²Ù×÷
 ÊäÈë²ÎÊý  : pstEncInPara      --  Ö¸ÏòÉùÂëÆ÷±àÂëÊäÈë²ÎÊý½á¹¹ÌåµÄÖ¸Õë
 Êä³ö²ÎÊý  : pstEncOutPara     --  Ö¸Ïò±àÂëºóÊä³ö²ÎÊý½á¹¹ÌåµÄÖ¸Õë
 ·µ »Ø Öµ  : ³É¹¦·µ»ØCODEC_RET_OK
             Ê§°Ü·µ»ØCODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 HR_Encode(
                CODEC_ENC_IN_PARA_STRU  *pstEncInPara,
                CODEC_ENC_OUT_PARA_STRU  *pstEncOutPara);

/*****************************************************************************
 º¯ Êý Ãû  : HR_Decode
 ¹¦ÄÜÃèÊö  : HR½âÂëº¯Êý
 ÊäÈë²ÎÊý  : pstVocodecDecPara -- Ö¸ÏòÉùÂëÆ÷½âÂë½á¹¹ÌåµÄÖ¸Õë
 Êä³ö²ÎÊý  : pshwDecSpeech     -- ½âÂëºóÊý¾Ý£¬³¤¶ÈÎª160¸ö×Ö½ÚµÄPCMÊý¾Ý
 ·µ »Ø Öµ  : ³É¹¦·µ»ØCODEC_RET_OK
             Ê§°Ü·µ»ØCODEC_RET_ERR
*****************************************************************************/
extern VOS_UINT32 HR_Decode(
                CODEC_DEC_IN_PARA_STRU  *pstVocodecDecPara,
                VOS_INT16                   *pshwDecSpeech);

/*****************************************************************************
 º¯ Êý Ãû  : CODEC_ComCodecBits2Prm
 ¹¦ÄÜÃèÊö  : ½«¶ÔÓ¦±ÈÌØÊý×ª»»Îª²ÎÊý
 ÊäÈë²ÎÊý  : VOS_INT16 *pshwBitStream      --±ÈÌØÁ÷Ö¸Õë
             VOS_INT16 shwNumOfBits        --´ý×ª»»±ÈÌØ¸öÊý
 Êä³ö²ÎÊý  : VOS_INT16 *pshwPrmValue       --×ª»»»ñµÃµÄ²ÎÊýÖ¸Õë
 ·µ »Ø Öµ  : VOS_VOID
*****************************************************************************/
extern VOS_VOID CODEC_ComCodecBits2Prm(
                VOS_INT16 *pshwBitStream,
                VOS_INT16 shwNumOfBits,
                VOS_INT16 *pshwPrmValue);

/*****************************************************************************
 º¯ Êý Ãû  : CODEC_ComCodecPrm2Bits
 ¹¦ÄÜÃèÊö  : ½«²ÎÊý×ª»¯Îª¶ÔÓ¦±ÈÌØ£¬Ã¿¸ö±ÈÌØÕ¼Ò»¸öword
 ÊäÈë²ÎÊý  : VOS_INT16 shwValue         --ÐèÒª×ª»»µÄ²ÎÊýÖµ
             VOS_INT16 shwNumOfBits     --²ÎÊý°üº¬µÄ±ÈÌØÊý
 Êä³ö²ÎÊý  : VOS_INT16 *pshwBitStream   --×ª»»ºóµÄ±ÈÌØÁ÷
 ·µ »Ø Öµ  : VOS_VOID
*****************************************************************************/
extern VOS_VOID CODEC_ComCodecPrm2Bits(
                VOS_INT16 shwValue,
                VOS_INT16 shwNumOfBits,
                VOS_INT16 *pshwBitStream);

/*****************************************************************************
 º¯ Êý Ãû  : CODEC_ComCodecBits2Bytes
 ¹¦ÄÜÃèÊö  : ½«·Ç½ô´ÕÖð±ÈÌØÅÅÁÐµÄ±ÈÌØÁ÷¸ñÊ½×ª»»Îª½ô´ÕÅÅÁÐµÄÂëÁ÷¸ñÊ½
 ÊäÈë²ÎÊý  : VOS_INT16 *pshwBits       --ÊäÈë·Ç½ô´ÕÖð±ÈÌØÅÅÁÐµÄ±ÈÌØÁ÷Ö¸Õë
             VOS_INT16  shwBitsNum     --ÐèÒª×ª»»µÄ±ÈÌØÊý
 Êä³ö²ÎÊý  : VOS_INT8  *pscBytes       --Êä³ö½ô´ÕÅÅÁÐµÄÂëÁ÷Ö¸Õë
 ·µ »Ø Öµ  : VOS_VOID
*****************************************************************************/
VOS_VOID CODEC_ComCodecBits2Bytes(
                VOS_INT16 *pshwBits,
                VOS_INT16  shwBitsNum,
                VOS_INT8  *pscBytes);


/*****************************************************************************
 º¯ Êý Ãû  : CODEC_ComCodecBytes2Bits
 ¹¦ÄÜÃèÊö  : ½«½ô´ÕÅÅÁÐµÄÂëÁ÷¸ñÊ½×ª»»Îª·Ç½ô´ÕÖð±ÈÌØÅÅÁÐµÄ±ÈÌØÁ÷¸ñÊ½
 ÊäÈë²ÎÊý  : VOS_INT8  *pscBytes      --ÊäÈë½ô´ÕÅÅÁÐµÄÂëÁ÷Ö¸Õë
             VOS_INT16  shwBitsNum    --ÐèÒª×ª»»µÄ±ÈÌØÊý
 Êä³ö²ÎÊý  : VOS_INT16 *pshwBits      --×ª»»ºó·Ç½ô´Õ±ÈÌØÅÅÁÐµÄ±ÈÌØÁ÷Ö¸Õë
 ·µ »Ø Öµ  : VOS_VOID
*****************************************************************************/
VOS_VOID CODEC_ComCodecBytes2Bits(
                VOS_INT8  *pscBytes,
                VOS_INT16  shwBitsNum,
                VOS_INT16 *pshwBits);
#endif



#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

#endif /* end of codec_com_codec.h */

