/*
 *
 *  Bluetooth low-complexity, subband codec (SBC) library
 *
 *  Copyright (C) 2008-2010  Nokia Corporation
 *  Copyright (C) 2004-2010  Marcel Holtmann <marcel@holtmann.org>
 *  Copyright (C) 2004-2005  Henryk Ploetz <henryk@ploetzli.ch>
 *  Copyright (C) 2005-2006  Brad Midgley <bmidgley@xmission.com>
 *
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef __SBC_H
#define __SBC_H
#include "amr_vad.h"

#define SBC_MODE			0
#define MSBC_MODE			1

//#include "jheaders.h"

/* sampling frequency */
#define SBC_FREQ_16000		0x00
#define SBC_FREQ_32000		0x01
#define SBC_FREQ_44100		0x02
#define SBC_FREQ_48000		0x03

/* blocks */
#define SBC_BLK_4		0x00
#define SBC_BLK_8		0x01
#define SBC_BLK_12		0x02
#define SBC_BLK_16		0x03

/* channel mode */
#define SBC_MODE_MONO		0x00
#define SBC_MODE_DUAL_CHANNEL	0x01
#define SBC_MODE_STEREO		0x02
#define SBC_MODE_JOINT_STEREO	0x03

/* allocation method */
#define SBC_AM_LOUDNESS		0x00
#define SBC_AM_SNR		0x01

/* subbands */
#define SBC_SB_4		0x00
#define SBC_SB_8		0x01

/* Data endianess */
#define SBC_LE			0x00
#define SBC_BE			0x01

struct sbc_struct {
	unsigned long flags;

	uint8_t frequency;
	uint8_t blocks;
	uint8_t subbands;
	uint8_t mode;
	uint8_t allocation;
	uint8_t bitpool;
	uint8_t endian;

	void *priv;
};

typedef struct sbc_struct sbc_t;
void sbc_set_defaults(sbc_t *sbc, unsigned long flags);
void msbc_set_defaults(sbc_t *sbc, unsigned long flags);
int sbc_init(sbc_t *sbc, unsigned long flags,uint8 sbc_mode);
int sbc_re_init(sbc_t *sbc);

#ifdef CONFIG_SBC_PROMPT
int wave_sbc_init(sbc_t *sbc, unsigned long flags,uint8_t sbc_mode);
#endif

uint16_t sbc_parse(sbc_t *sbc, const void *input, uint16_t input_len);

/* Decodes ONE input block into ONE output block */
RAM_CODE int16_t sbc_decode(sbc_t *sbc, const void *input, uint16_t input_len,
			void *output, uint16_t output_len, uint16_t *written,uint8_t sbc_mode);

/* Returns the input block size in bytes */
size_t sbc_get_codesize(sbc_t *sbc,uint8_t sbc_mode);

void sbc_finish(sbc_t *sbc);
int16_t sbc_enc_init(sbc_t *sbc, unsigned long flags);
#if((CONFIG_APP_ADC_VAD == 1) || (CONFIG_APP_DAC_VAD == 1))
int16_t sbc_vad(vadState1 *p_vad_st,int16_t *pRawData);
#endif
int8_t sbc_frame_crc(const uint8_t *data);
int16_t sbc_encode(void * hEnc, uint16_t* pnFrameSize, uint8_t * pFrame, uint16_t nRawLen, int16_t *pRawData,uint8_t sbc_mode);
int16_t sbc_get_frame_length(sbc_t *sbc,uint8_t sbc_mode);


#endif /* __SBC_H */
