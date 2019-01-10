/*******************************************************************************
** namer£ºCVSD CODEC header
** description£ºCVSD encoder and CVSD decoder header
** version£ºV1.0
** author: xuml
** modify£º2015.8.21.
*******************************************************************************/

#ifndef _CVSD_CODEC_H_
#define _CVSD_CODEC_H_


//Q fixed
#define CVSD_Q 10

//encoder
struct CvsdEncSt
{
	unsigned short CvsdEncJudgeAlpha;
	int   CvsdEncLastData;
	int   CvsdEncDelta;
	int CvsdEncFirstFrameFlag;
};

//decoder
struct CvsdDecSt
{
	unsigned short CvsdDecJudgeAlpha;
	int   CvsdDecLastData;
	int   CvsdDecDelta;
};

//encoder
void CvsdEncInit(struct CvsdEncSt *CvsdEnc);
void CvsdEncode(struct CvsdEncSt *CvsdEnc, short *pInPutData, unsigned char *pOutPutdata, int iEncSize);

//decoder
void CvsdDecInit(struct CvsdDecSt *CvsdDec);
void CvsdDecode(struct CvsdDecSt *CvsdDec, unsigned char *pInPutData, short *pOutPutdata, int iDecSize);

#endif












