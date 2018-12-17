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

#include "common.h"
#define SBC_ALIGNED

#ifdef __cplusplus
extern "C" {
#endif

#define SBC_MAX_SYNC_SIZE 1024

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
	void *priv_alloc_base;
};


#define ENA_SBC_JOINT_STEREO
#define ENA_SBC_SB_8
#define ENA_SBC_LE

//#define SBC_USE_DYN_MEM
#ifndef SBC_USE_DYN_MEM
//#define SBC_ENDCODE_BUF_ADDR 0x9fc0f000//0x9fc24c00//0x9fc0f000
#define SBC_ENDCODE_BUF_ADDR 0x9fc15a00
#endif


#define SBC_SYNCWORD	0x9C

#define SCALE_OUT_BITS 15
#define SBC_X_BUFFER_SIZE 328
/*
#ifdef __GNUC__
#define SBC_ALWAYS_INLINE inline __attribute__((always_inline))
#else
#define SBC_ALWAYS_INLINE inline
#endif
*/

struct sbc_encoder_state {
	int position;
	int16_t SBC_ALIGNED X[2][SBC_X_BUFFER_SIZE];
	/* Polyphase analysis filter for 4 subbands configuration,
	 * it handles 4 blocks at once */
	void (*sbc_analyze_4b_4s)(int16_t *x, int32_t *out, int out_stride);
	/* Polyphase analysis filter for 8 subbands configuration,
	 * it handles 4 blocks at once */
	void (*sbc_analyze_4b_8s)(int16_t *x, int32_t *out, int out_stride);
	/* Process input data (deinterleave, endian conversion, reordering),
	 * depending on the number of subbands and input data byte order */
	int (*sbc_enc_process_input_4s_le)(int position,
			const uint8_t *pcm, int16_t X[2][SBC_X_BUFFER_SIZE],
			int nsamples, int nchannels);
	int (*sbc_enc_process_input_4s_be)(int position,
			const uint8_t *pcm, int16_t X[2][SBC_X_BUFFER_SIZE],
			int nsamples, int nchannels);
	int (*sbc_enc_process_input_8s_le)(int position,
			const uint8_t *pcm, int16_t X[2][SBC_X_BUFFER_SIZE],
			int nsamples, int nchannels);
	int (*sbc_enc_process_input_8s_be)(int position,
			const uint8_t *pcm, int16_t X[2][SBC_X_BUFFER_SIZE],
			int nsamples, int nchannels);
	/* Scale factors calculation */
	void (*sbc_calc_scalefactors)(int32_t sb_sample_f[16][2][8],
			uint32_t scale_factor[2][8],
			int blocks, int channels, int subbands);
	/* Scale factors calculation with joint stereo support */
	int (*sbc_calc_scalefactors_j)(int32_t sb_sample_f[16][2][8],
			uint32_t scale_factor[2][8],
			int blocks, int subbands);
	const char *implementation_info;
};


/* This structure contains an unpacked SBC frame.
   Yes, there is probably quite some unused space herein */
struct sbc_frame {
	uint8_t frequency;
	uint8_t block_mode;
	uint8_t blocks;
	enum {
		MONO		= SBC_MODE_MONO,
		DUAL_CHANNEL	= SBC_MODE_DUAL_CHANNEL,
		STEREO		= SBC_MODE_STEREO,
		JOINT_STEREO	= SBC_MODE_JOINT_STEREO
	} mode;
	uint8_t channels;
	enum {
		LOUDNESS	= SBC_AM_LOUDNESS,
		SNR		= SBC_AM_SNR
	} allocation;
	uint8_t subband_mode;
	uint8_t subbands;
	uint8_t bitpool;
	uint16_t codesize;
	uint8_t length;

	/* bit number x set means joint stereo has been used in subband x */
	uint8_t joint;

	/* only the lower 4 bits of every element are to be used */
	uint32_t SBC_ALIGNED scale_factor[2][8];

	/* raw integer subband samples in the frame */
	int32_t SBC_ALIGNED sb_sample_f[16][2][8];

	/* modified subband samples */
//	int32_t SBC_ALIGNED sb_sample[16][2][8];

	/* original pcm audio samples */
//	int16_t SBC_ALIGNED pcm_sample[2][16*8];
};


struct sbc_priv {
	int init;
	struct SBC_ALIGNED sbc_frame frame;
	struct SBC_ALIGNED sbc_encoder_state enc_state;
};


typedef struct sbc_struct sbc_t;

/*****************************************************
description Init encoder parametres and malloc internal memory 
param[in]   sbc  数据结构指针
param[in]   flags  编码参数标志，未使用，请填零。
retval       0：正常，1: 异常
******************************************************/
int sbc_init(sbc_t *sbc, unsigned long flags);

/******************************************************
description Encodes ONE input block into ONE output block
param[in]   sbc  数据结构指针
param[in]   input  待编码pcm首地址
param[in]   input_len  待编码数据量
param[in]   bit_depth  每个样本点的位数，16或32
param[in]   gain  增益系数，8.24格式，1<<24意味着不放大也不缩小。	
param[in]   output  编码数据区首地址
param[in]   output_len  编码数据区大小
param[out]  written 编码后数据量	
retval       已编码的pcm数据量，0：异常；非0：对于每次一个block编码，返回值应该跟input_len一致。
******************************************************/
ssize_t sbc_encode(sbc_t *sbc, const void *input, size_t input_len, size_t bit_depth, size_t gain,
			void *output, size_t output_len, ssize_t *written);

/******************************************************
description Encodes ONE input block into ONE output block
param[in]   sbc  数据结构指针
param[in]   bit_depth  每个样本点的位数，16或32	
retval      一帧pcm的数据量，单位：字节
******************************************************/
size_t sbc_get_codesize(sbc_t *sbc, size_t bit_depth);

/******************************************************
description Release internal malloc memory
param[in]   sbc  编码数据结构指针
retval      无
******************************************************/
void sbc_finish(sbc_t *sbc);

size_t sbc_get_frame_length(sbc_t *sbc);
void sbc_calculate_bits_internal(
		const struct sbc_frame *frame, int (*bits)[8], int subbands);
ssize_t sbc_pack_frame_internal(uint8_t *data,
					struct sbc_frame *frame, size_t len,
					int frame_subbands, int frame_channels,
					int joint);		

/*
 * Initialize pointers to the functions which are the basic "building bricks"
 * of SBC codec. Best implementation is selected based on target CPU
 * capabilities.
 */
void sbc_init_primitives(struct sbc_encoder_state *encoder_state);

int sbc_clz(uint32_t x);

int sbc_encode_entry(sbc_t *sbc,void *param,ssize_t *written);


#ifdef __cplusplus
}
#endif

#endif /* __SBC_H */
