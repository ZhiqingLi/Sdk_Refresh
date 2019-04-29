/* ***** BEGIN LICENSE BLOCK ***** 
 * Version: RCSL 1.0/RPSL 1.0 
 *  
 * Portions Copyright (c) 1995-2002 RealNetworks, Inc. All Rights Reserved. 
 *      
 * The contents of this file, and the files included with this file, are 
 * subject to the current version of the RealNetworks Public Source License 
 * Version 1.0 (the "RPSL") available at 
 * http://www.helixcommunity.org/content/rpsl unless you have licensed 
 * the file under the RealNetworks Community Source License Version 1.0 
 * (the "RCSL") available at http://www.helixcommunity.org/content/rcsl, 
 * in which case the RCSL will apply. You may also obtain the license terms 
 * directly from RealNetworks.  You may not use this file except in 
 * compliance with the RPSL or, if you have a valid RCSL with RealNetworks 
 * applicable to this file, the RCSL.  Please see the applicable RPSL or 
 * RCSL for the rights, obligations and limitations governing use of the 
 * contents of the file.  
 *  
 * This file is part of the Helix DNA Technology. RealNetworks is the 
 * developer of the Original Code and owns the copyrights in the portions 
 * it created. 
 *  
 * This file, and the files included with this file, is distributed and made 
 * available on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER 
 * EXPRESS OR IMPLIED, AND REALNETWORKS HEREBY DISCLAIMS ALL SUCH WARRANTIES, 
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS 
 * FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT. 
 * 
 * Technology Compatibility Kit Test Suite(s) Location: 
 *    http://www.helixcommunity.org/content/tck 
 * 
 * Contributor(s): 
 *  
 * ***** END LICENSE BLOCK ***** */ 

/**************************************************************************************
 * Fixed-point MP3 decoder
 * Jon Recker (jrecker@real.com), Ken Cooke (kenc@real.com)
 * June 2003
 *
 * buffers.c - allocation and freeing of internal MP3 decoder buffers
 * 所有的编解码器的内存分配是在这个文件中
 * All memory allocation for the codec is done in this file, so if you don't want 
 *  to use other the default system malloc() and free() for heap management this is 
 *  the only file you'll need to change.
 **************************************************************************************/

#include "coder.h"
#include "layer21.h"
#include <string.h>
#include <stdlib.h>
#include "bkmp3_resample.h"
#include "port_mem.h"
#include "mp3common.h"
#if (CONFIG_APP_MP3PLAYER == 1)
#include <include/os_common.h>
#include "app_beken_includes.h"

/**************************************************************************************
 * Function:    ClearBuffer
 *
 * Description: fill buffer with 0's
 *
 * Inputs:      pointer to buffer
 *              number of bytes to fill with 0
 *
 * Outputs:     cleared buffer
 *
 * Return:      none
 *
 * Notes:       slow, platform-independent equivalent to memset(buf, 0, nBytes)
 **************************************************************************************/
void ClearBuffer(void *buf, int nBytes)
{
	int i;
	unsigned char *cbuf = (unsigned char *)buf;

	for (i = 0; i < nBytes; i++)
		cbuf[i] = 0;

}

void ClearMP3(MP3DecInfo *mp3DecInfo)
{
    ClearBuffer(mp3DecInfo->FrameHeaderPS,  sizeof(FrameHeader));
	ClearBuffer(mp3DecInfo->SideInfoPS,  sizeof(SideInfo));
	ClearBuffer(mp3DecInfo->ScaleFactorInfoPS, sizeof(ScaleFactorInfo));
	ClearBuffer(mp3DecInfo->HuffmanInfoPS,  sizeof(HuffmanInfo));
	ClearBuffer(mp3DecInfo->DequantInfoPS,  sizeof(DequantInfo));
	ClearBuffer(mp3DecInfo->IMDCTInfoPS,  sizeof(IMDCTInfo));
	ClearBuffer(mp3DecInfo->SubbandInfoPS, sizeof(SubbandInfo));
    ClearBuffer(mp3DecInfo->L2DecInfo, sizeof(L2DecodeContext));
    
    mp3DecInfo->freeBitrateFlag = 0;
	mp3DecInfo->freeBitrateSlots = 0;

	/* user-accessible info */
	mp3DecInfo->bitrate = 0;
	mp3DecInfo->nChans = 0;
	mp3DecInfo->samprate = 0;
	mp3DecInfo->nGrans = 0;				/* granules per frame */
	mp3DecInfo->nGranSamps = 0;			/* samples per granule */
	mp3DecInfo->nSlots = 0;
	mp3DecInfo->layer = 0;
	memset(&mp3DecInfo->version, 0, sizeof(MPEGVersion));

	mp3DecInfo->mainDataBegin = 0;
	mp3DecInfo->mainDataBytes = 0;
}
/**************************************************************************************
 * Function:    AllocateBuffers
 *
 * Description: allocate all the memory needed for the MP3 decoder
 *
 * Inputs:      none		   分配内存
 *
 * Outputs:     none
 *
 * Return:      pointer to MP3DecInfo structure (initialized with pointers to all 
 *                the internal buffers needed for decoding, all other members of 
 *                MP3DecInfo structure set to 0)
 *
 * Notes:       if one or more mallocs fail, function frees any buffers already
 *                allocated before returning
 *
 *              Changed by Kasper Jepsen to support static buffers as well.
 *
 **************************************************************************************/
MP3DecInfo *AllocateBuffers(void)
{
//#define MALLOC_DEBUG

    MP3DecInfo *mp3DecInfo_pointer;
    FrameHeader *fh;
    SideInfo *si;
    ScaleFactorInfo *sfi;
    HuffmanInfo *hi;
    DequantInfo *di;
    IMDCTInfo *mi;
    SubbandInfo *sbi;
    L2DecodeContext *l2i;

    #ifdef MALLOC_DEBUG
    {
        int left_jmalloc_size = memory_usage_show();
        int mp3_need_space_size;

        mp3_need_space_size = sizeof(MP3DecInfo) +
                              sizeof(FrameHeader)+
                              sizeof(SideInfo)+
                              sizeof(ScaleFactorInfo)+
                              sizeof(HuffmanInfo)+
                              sizeof(DequantInfo)+
                              sizeof(IMDCTInfo)+
                              sizeof(SubbandInfo)+
                              sizeof(L2DecodeContext);
        if(mp3_need_space_size >= left_jmalloc_size)
        {
            os_printf("AllocateBuffers_dange:%p:%p\r\n", mp3_need_space_size, left_jmalloc_size);
        }
        else
        {
            os_printf("AllocateBuffers_goForIt:%p:%p\r\n", mp3_need_space_size, left_jmalloc_size);
        }
    }
    #endif    

	memory_usage_show();
	
    mp3DecInfo_pointer = (MP3DecInfo *)jmalloc(sizeof(MP3DecInfo),M_ZERO);
	mp3DecInfo_pointer->decode_state = MP3_DECODE_NONE;

    sbi = (SubbandInfo *)     jmalloc(sizeof(SubbandInfo), M_ZERO);
    mi =  (IMDCTInfo *)       jmalloc(sizeof(IMDCTInfo), M_ZERO);
    hi =  (HuffmanInfo *)     jmalloc(sizeof(HuffmanInfo), M_ZERO);
    si =  (SideInfo *)        jmalloc(sizeof(SideInfo), M_ZERO);
    fh =  (FrameHeader *)     jmalloc(sizeof(FrameHeader), M_ZERO);
    sfi = (ScaleFactorInfo *) jmalloc(sizeof(ScaleFactorInfo), M_ZERO);
    di =  (DequantInfo *)     jmalloc(sizeof(DequantInfo), M_ZERO);
    l2i = (L2DecodeContext *) jmalloc(sizeof(L2DecodeContext), M_ZERO);

    mp3DecInfo_pointer->FrameHeaderPS =     (void *)fh;
    mp3DecInfo_pointer->SideInfoPS =        (void *)si;
    mp3DecInfo_pointer->ScaleFactorInfoPS = (void *)sfi;
    mp3DecInfo_pointer->HuffmanInfoPS =     (void *)hi;
    mp3DecInfo_pointer->DequantInfoPS =     (void *)di;
    mp3DecInfo_pointer->IMDCTInfoPS =       (void *)mi;
    mp3DecInfo_pointer->SubbandInfoPS =     (void *)sbi;
    mp3DecInfo_pointer->L2DecInfo =         (void *)l2i;

	mp3DecInfo_pointer->err_cnt = 0;
#if 0
    if (!fh || !si || !sfi || !hi || !di || !mi || !sbi || !l2i) {
        os_printf("sbi=%x\r\n",sbi);
        os_printf("sizeof(SubbandInfo)=%d\r\n",sizeof(SubbandInfo));
        
        os_printf("mi=%x\r\n",mi);
        os_printf("sizeof(IMDCTInfo)=%d\r\n",sizeof(IMDCTInfo));
        
        os_printf("hi=%x\r\n",hi);
        os_printf("sizeof(HuffmanInfo)=%d\r\n",sizeof(HuffmanInfo));
        
        os_printf("si=%x\r\n",si);
        os_printf("sizeof(SideInfo)=%d\r\n",sizeof(SideInfo));
        
        os_printf("fh=%x\r\n",fh);
        os_printf("sizeof(FrameHeader)=%d\r\n",sizeof(FrameHeader));
        
        os_printf("sfi=%x\r\n",sfi);
        os_printf("sizeof(ScaleFactorInfo)=%d\r\n",sizeof(ScaleFactorInfo));
        
        os_printf("di=%x\r\n",di);
        os_printf("sizeof(DequantInfo)=%d\r\n",sizeof(DequantInfo));
        
        os_printf("l2i=%x\r\n",l2i);
        os_printf("sizeof(L2DecodeContext)=%d\r\n",sizeof(L2DecodeContext));
        
        os_printf("sizeof=%d\r\n",sizeof(FrameHeader)+sizeof(SideInfo)+sizeof(ScaleFactorInfo)+sizeof(HuffmanInfo)+sizeof(DequantInfo)+sizeof(IMDCTInfo)+sizeof(SubbandInfo)+sizeof(L2DecodeContext));

        FreeBuffers(mp3DecInfo_pointer);	/* 安全地调用 - 只释放内存，已成功分配 */
        return 0;
    }
#endif
    return mp3DecInfo_pointer;
}

/**************************************************************************************
 * Function:    FreeBuffers
 *
 * Description: frees all the memory used by the MP3 decoder
 *
 * Inputs:      pointer to initialized MP3DecInfo structure
 *
 * Outputs:     none
 *
 * Return:      none
 *
 * Notes:       safe to call even if some buffers were not allocated
 **************************************************************************************/
void FreeBuffers(MP3DecInfo *mp3DecInfo)
{
    L2DecodeContext *l2i;
    if (!mp3DecInfo)
		return;
	l2i = (L2DecodeContext *)(mp3DecInfo->L2DecInfo);
	
	jfree(mp3DecInfo->FrameHeaderPS);
	jfree(mp3DecInfo->SideInfoPS);
	jfree(mp3DecInfo->ScaleFactorInfoPS);
	jfree(mp3DecInfo->HuffmanInfoPS);
	jfree(mp3DecInfo->DequantInfoPS);
	
    l2i->inbuf = NULL;
	l2i->inbuf_ptr = NULL;
	l2i->sb_samples = NULL;
	jfree(mp3DecInfo->L2DecInfo);
	jfree(mp3DecInfo->IMDCTInfoPS);
	jfree(mp3DecInfo->SubbandInfoPS);
	jfree(mp3DecInfo);
}
#endif /* CONFIG_APP_MP3PLAYER */
//EOF
