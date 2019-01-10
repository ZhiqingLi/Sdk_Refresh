/*************************************************************
SBC Example PLC ANSI-C Source Code
File: sbcplc.c
*************************************************************/
#include <math.h>
//#include "sbc.h"
#include "sbcplc.h"
/* Local Function Prototypes */
int CrossCorrelation(short *x, short *y);
int PatternMatch(short *y);
short AmplitudeMatch(short *y, short bestmatch);
/* Raised COSine table for OLA */
//float rcos[OLAL] = { 0.99148655f, 0.96623611f, 0.92510857f, 0.86950446f,
//0.80131732f, 0.72286918f, 0.63683150f, 0.54613418f,
//0.45386582f, 0.36316850f, 0.27713082f, 0.19868268f,
//0.13049554f, 0.07489143f, 0.03376389f, 0.00851345f };


short rcos[] = {
	32504, 32177, 31723, 31145, 30448, 29638, 28722, 27706,
	26599, 25409, 24147, 22823, 21446, 20029, 18583, 17119,
	15648, 14184, 12738, 11321, 9944, 8620, 7358, 6168,
	5061, 4045, 3129, 2319, 1622, 1044, 590, 263

};



unsigned char indices0[] = { 0xad, 0x0, 0x0, 0xc5, 0x0, 0x0, 0x0, 0x0,
0x77, 0x6d, 0xb6, 0xdd, 0xdb, 0x6d, 0xb7,
0x76, 0xdb, 0x6d, 0xdd, 0xb6, 0xdb, 0x77,
0x6d, 0xb6, 0xdd, 0xdb, 0x6d, 0xb7, 0x76,
0xdb, 0x6d, 0xdd, 0xb6, 0xdb, 0x77, 0x6d,
0xb6, 0xdd, 0xdb, 0x6d, 0xb7, 0x76, 0xdb,
0x6d, 0xdd, 0xb6, 0xdb, 0x77, 0x6d, 0xb6,
0xdd, 0xdb, 0x6d, 0xb7, 0x76, 0xdb, 0x6c
};



#define ATTEFACTOR 6553 //0.2
#define ATTEINCR (ATTEFACTOR / FS)

/*****************************************************************************
* Function: InitPLC() *
* Purpose: Perform PLC initialization of memory vectors. *
* Inputs: *plc_state - pointer to PLC state memory *
* Outputs: *plc_state - initialized memory. *
* Date: 03-18-2009
*****************************************************************************/
void InitPLC(PLC_State *plc_state)
{
	int i;
	plc_state->nbf = 0;
	plc_state->bestlag = 0;
	for (i = 0; i<LHIST + SBCRT; i++)
		plc_state->hist[i] = 0;

}




void scalspeech(short *out,short num)
{
	int i;
	int g = 32768 - (num - 1) * ATTEFACTOR;
	for (i = 0; i < FS; i++) {
		out[i] = (short)((out[i] * g)>>15);
		g -= ATTEINCR;
	}
}

/***********************************************************
* Function: PLC_bad_frame()
*
* Purpose: Perform bad frame processing.
*
* Inputs: *plc_state - pointer to PLC state memory
* *ZIRbuf - pointer to the ZIR response of the SBC decoder
*
* Outputs: *out - pointer to the output samples
*
* Date: 03-18-2009
************************************************************/
void PLC_bad_frame(PLC_State *plc_state, short *ZIRbuf, short *out)
{
	int i;
	int val;
	short sf;
	

	sf = 16384; i = 0;
	if (plc_state->nbf == 0)
	{
		/* Perform pattern matching to find where to replicate */
		plc_state->bestlag = PatternMatch(plc_state->hist);
		plc_state->bestlag += M; /* the replication begins after the template match */
		/* Compute Scale Factor to Match Amplitude of Substitution Packet to that of Preceding Packet */
		sf = AmplitudeMatch(plc_state->hist, plc_state->bestlag);
		
		
		for (i = 0; i<OLAL; i++)
		{
	
			val = (((int)ZIRbuf[i] * rcos[i]) >> 15) + (((int)sf *(((int)plc_state->hist[plc_state->bestlag + i] * rcos[OLAL - i - 1]) >> 15)) >> 14);
			if (val > 32767) val = 32767;
			if (val < -32768) val = -32768;
			plc_state->hist[LHIST + i] = (short)val;
		}
		for (; i<FS; i++)
		{
			val = (((int)sf)*plc_state->hist[plc_state->bestlag + i])>>14;
			if (val > 32767) val = 32767;
			if (val < -32768) val = -32768;
			plc_state->hist[LHIST + i] = (short)val;
		}
		for (; i<FS + OLAL; i++)
		{
			val = (((int)sf * (((int)plc_state->hist[plc_state->bestlag + i] * rcos[i - FS])>>15)) >> 14) + (((int)plc_state->hist[plc_state->bestlag + i] * rcos[OLAL - 1 - i + FS])>>15);
			if (val > 32767) val = 32767;
			if (val < -32768) val = -32768;
			plc_state->hist[LHIST + i] = (short)val;
		}
		for (; i<FS + SBCRT + OLAL; i++)
			plc_state->hist[LHIST + i] = plc_state->hist[plc_state->bestlag + i];
	}
	else if(plc_state->nbf >= 1 && plc_state->nbf<6)
	{
		for (; i<FS; i++)
			plc_state->hist[LHIST + i] = plc_state->hist[plc_state->bestlag + i];
		for (; i<FS + SBCRT + OLAL; i++)
			plc_state->hist[LHIST + i] = plc_state->hist[plc_state->bestlag + i];
	}
	else
	{

		for (; i<FS + SBCRT + OLAL; i++)
			plc_state->hist[LHIST + i] = 0;

	}
	for (i = 0; i<FS; i++)
		out[i] = plc_state->hist[LHIST + i];
	/* shift the history buffer */
	for (i = 0; i<LHIST + SBCRT + OLAL; i++)
		plc_state->hist[i] = plc_state->hist[i + FS];

	if (plc_state->nbf >= 1 && plc_state->nbf<6)
		scalspeech(out, plc_state->nbf);
	plc_state->nbf++;

}

/*****************************************************************************
* Function: PLC_good_frame()
*
* Purpose: Perform good frame processing. Most of the time, this function
* just updates history buffers and passes the input to the output,
* but in the first good frame after frame loss, it must conceal the
* received signal as it reconverges with the true output.
*
* Inputs: *plc_state - pointer to PLC state memory
* *in - pointer to the input vector
*
* Outputs: *out - pointer to the output samples
* Date: 03-18-2009*****************************************************************************/



void PLC_good_frame(PLC_State *plc_state, short *in, short *out)
{

	int gain = 32768 - (plc_state->nbf - 1) * ATTEFACTOR;
	if (gain < 0)
		gain = 0;

	int i;
	i = 0;
	if (plc_state->nbf>0)
	{
		int olen = 24+(plc_state->nbf - 1) * 32;
		if (olen > FS)
			olen = FS;

		int incr = 32768 / olen;

		int incrg = (incr * gain)>>15;
		int lw = ((32768 - incr) * gain)>>15;
		int rw = incr;
		

		for (; i < olen; i++)
		{
			out[i] = (short)((plc_state->hist[LHIST + i] * lw + in[i] * rw)>>15);
			lw -= incrg;
			rw += incr;
		}
		
	}
	for (; i<FS; i++)
		out[i] = in[i];
	/*Copy the output to the history buffer */
	for (i = 0; i<FS; i++)
		plc_state->hist[LHIST + i] = out[i];
	/* shift the history buffer */
	for (i = 0; i<LHIST; i++)
		plc_state->hist[i] = plc_state->hist[i + FS];
	plc_state->nbf = 0;

}

#define FIXED_SQRT_STEP 16
#if 0
int fix_sqrt(long long x, int Fixstep)
{
	int bit = 7;
	int i, z, Sn;
	long long Xn, Yn, Xn1, Yn1;
	long long ComHalf = 1;
	x = x * 64;
	while (x >= (ComHalf << bit))
		bit = bit + 1;
	bit = bit - 1;
	if (bit & 0x1)
		bit = bit + 1;
	if (x == (ComHalf << bit))
		z = ComHalf << (bit / 2 - 3);
	else
	{
		Xn = x + (ComHalf << bit);
		Yn = x - (ComHalf << bit);
		if (Yn == 0)
			Sn = 0;
		else if (Yn>0)
			Sn = -1;
		else
			Sn = 1;
		for (i = 1; i <= Fixstep; i++)
		{
			Xn1 = Xn + ((Sn*Yn) >> i);
			Yn1 = Yn + ((Sn*Xn) >> i);
			Xn = Xn1;
			Yn = Yn1;
			if (Yn == 0)
				Sn = 0;
			else if (Yn>0)
				Sn = -1;
			else
				Sn = 1;
		}
		Xn1 = Xn1 * 1234;
		z = Xn1 >> (11 + bit / 2 + 2);
		if (z & 0x1)
			z = (z >> 1) + 1;
		else
			z = z >> 1;
	}
	return z;
}
#else
static inline int fix_sqrt(int op1,int Fixstep)
{
    int result;
    asm volatile("vmov s0, %0" : :"r" (op1));
    asm volatile("vcvt.f32.s32 s0,s0");
    asm volatile("vsqrt.f32 s0,s0");
    asm volatile("vcvt.s32.f32 s0,s0");
    asm volatile("vmov %0, s0" : "=r" (result));
    return(result);
}

#endif


/*****************************************************************************
* Function: CrossCorrelation()
*
* Purpose: Compute the cross correlation according to Eq. (4) of Goodman
* paper, except that the true correlation is used. His formula* seems to be incorrect.
*
* Inputs: *x - pointer to x input vector
* *y - pointer to y input vector
*
* Outputs: Cn - return value containing the cross-correlation of x and y
*
* Date: 03-18-2009
*****************************************************************************/
int CrossCorrelation(short *x, short *y)
{
	int m;
	int num;
	int den;
	int Cn=0;
	int x2, y2;
	num = 0;
	den = 0;
	x2 = 0;
	y2 = 0;
	for (m = 0; m<M; m++)
	{
		num += (x[m]>>3)*(y[m]>>3);
		x2 += (x[m]>>3)*(x[m]>>3);
		y2 += (y[m]>>3)*(y[m]>>3);
	}
	//den = fix_sqrt(((long long)x2)*y2, FIXED_SQRT_STEP);
	den = fix_sqrt(x2,FIXED_SQRT_STEP);
	den = den*fix_sqrt(y2,FIXED_SQRT_STEP);
	if (den != 0)
		Cn = (((long long)num)<<15) / den;
	return(Cn);
}
/*****************************************************************************
* Function: PatternMatch()
*
* Purpose: Perform pattern matching to find the match of template with the
* history buffer according to Section B of Goodman paper.
*
* Inputs: *y : pointer to history buffer
*
* Outputs: return(int): the lag corresponding to the best match. The lag is
* with respect to the beginning of the history buffer.
*
* Date: 03-18-2009
*****************************************************************************/
int PatternMatch(short *y)
{
	int n;
	int maxCn;
	int Cn;
	int bestmatch;
	maxCn = -9999999; /* large negative number */
	bestmatch = 0;
	for (n = 0; n<N; n+=2)
	{
		Cn = CrossCorrelation(&y[LHIST - M] /* x */, &y[n]);
		if (Cn>maxCn)
		{
			bestmatch = n; 
			maxCn = Cn;
		}
	}

	Cn = CrossCorrelation(&y[LHIST - M] /* x */, y + bestmatch-1);
	if (Cn>maxCn)
	{
		bestmatch = bestmatch-1;
		maxCn = Cn;
	}
	else
	{
		Cn = CrossCorrelation(&y[LHIST - M] /* x */, y + bestmatch + 1);
		if (Cn>maxCn)
		{
			bestmatch = bestmatch + 1;
			maxCn = Cn;
		}
	}


	return(bestmatch);
}
/*****************************************************************************
* Function: AmplitudeMatch()
*
* Purpose: Perform amplitude matching using mean-absolute-value according
* to Goodman paper.
*
* Inputs: *y : pointer to history buffer
* bestmatch : value of the lag to the best match
*
* Outputs: return(float): scale factor
*
* Date: 03-19-2009
*****************************************************************************/
short AmplitudeMatch(short *y, short bestmatch)
{
	int i;
	int sumx;
	int sumy;
	short sf=0;
	sumx = 0;
	sumy = 0;
	for (i = 0; i<FS; i++)
	{
		sumx += ABS(y[LHIST - FS + i]);
		sumy += ABS(y[bestmatch + i]);
	}
	if (sumy != 0)
		sf = (((long long)sumx)<<14) / sumy;
	/* This is not in the paper, but limit the scaling factor to something
	reasonable to avoid creating artifacts */
	if (sf<12288) sf = 12288;
	if (sf>19661) sf = 19661;
	return(sf);
}


#if 0
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

typedef  char int8_t;
typedef  short int16_t;
typedef  int int32_t;


/*---------------------------------------------------------------------------
*
* Loudness offset table for bit allocation (4 subbands).
*/
const int8_t loudness_4[4][4] = {
	{ (int8_t)-1, 0, 0, 0 },
	{ (int8_t)-2, 0, 0, 1 },
	{ (int8_t)-2, 0, 0, 1 },
	{ (int8_t)-2, 0, 0, 1 }
};

/*---------------------------------------------------------------------------
*
* Loudness offset table for bit allocation (8 subbands).
*/
const int8_t loudness_8[4][8] = {
	{ (int8_t)-2, 0, 0, 0, 0, 0, 0, 1 },
	{ (int8_t)-3, 0, 0, 0, 0, 0, 1, 2 },
	{ (int8_t)-4, 0, 0, 0, 0, 0, 1, 2 },
	{ (int8_t)-4, 0, 0, 0, 0, 0, 1, 2 }
};

#define SBC_GROUP_SIZE 240



void SBC_Decode(char * DataIn, FILE * fOutput)
{
#define SBC_SAMPLING_FREQ 16
#define SBC_CHANNEL_MODE 0
#define SBC_NUM_OF_SUBBANDS 8
#define SBC_NUM_OF_CHANNELS 1
#define SBC_NUM_OF_BLOCKS   15
#define SBC_ALLOC_METHOD    0
#define SBC_BITPOOL 26          
#define SBC_DECODED_BUFFER_SIZE     (16*8)

	uint8_t blocks_per_packet = SBC_NUM_OF_BLOCKS;
	uint8_t num_bits = SBC_BITPOOL;
	const uint8_t * buf = (DataIn + 1);//ignore CRC byte
	uint16_t len = SBC_GROUP_SIZE;
	uint16_t usDecodedBuffer[SBC_DECODED_BUFFER_SIZE];

	/* convenience  */
	const uint8_t * end = buf + len;
#define left (end - buf)
	uint16_t * outBufPtr = usDecodedBuffer;

	/* workspace */
	static int samples[16][8]; /*  We blow the stack if this is not static. */
	int i, j, k;
	uint32_t scaleFactors[8]; //= {0x0f, 0x0c, 0x0b, 0x0b, 0x0a, 0x0a, 0x09, 0x09};
	int32_t bitneed[8];
	uint32_t bits[8];
	int32_t bitcount, slicecount, bitslice;
	uint8_t samplingRate, blocks, snr, numSubbands, bitpoolSz, bitpos = 0x80;
	int8_t max_bitneed = 0;
#ifndef SPEED_OVER_ACCURACY
	int32_t levels[8];
#endif

#if (DEBUG_DECODING == 1)
	const uint8_t *start_buf = buf;
	pr_info("%s: blocks_per_packet = %d, num_bits = %d, buf = %p, len = %d\n",
		__func__, blocks_per_packet, num_bits, buf, len);
	for (i = 0; i < len; i++) {
		pr_info("buf[%d] = 0x%02x\n", i, buf[i]);
	}
#endif

	/* look into the frame header */
	if (left < SBC_GROUP_SIZE) goto out;/* too short a frame header  */

	/*  use Bemote specific constants  */
	samplingRate = 0; /*  always 16000 Hz */
	blocks = blocks_per_packet;
	snr = 0;
	numSubbands = SBC_NUM_OF_SUBBANDS;
	bitpoolSz = num_bits;

	/* read scale factors */
	/* pr_info("sbc_decode: read scale factors, numSubbands = %d\n", numSubbands); */
	/**/
	for (i = 0; i < numSubbands; i++)
	{

		if (bitpos == 0x80)
		{

			scaleFactors[i] = (*buf) >> 4;
			bitpos = 0x08;
		}
		else{

			scaleFactors[i] = (*buf++) & 0x0F;
			bitpos = 0x80;
		}
	}

	/* calculate bitneed table and max_bitneed value (A2DP 12.6.3.1)  */
	if (snr)
	{

		for (i = 0; i < numSubbands; i++)
		{

			bitneed[i] = scaleFactors[i];
			if (bitneed[i] > max_bitneed) max_bitneed = bitneed[i];
		}
	}
	else
	{

		const signed char* tbl;

		if (numSubbands == 4) tbl = (const signed char*)loudness_4[samplingRate];
		else tbl = (const signed char*)loudness_8[samplingRate];

		for (i = 0; i < numSubbands; i++)
		{

			if (scaleFactors[i])
			{

				int loudness = scaleFactors[i] - tbl[i];

				if (loudness > 0) loudness /= 2;
				bitneed[i] = loudness;
			}
			else bitneed[i] = -5;
			if (bitneed[i] > max_bitneed) max_bitneed = bitneed[i];
		}
	}

	/* fit bitslices into the bitpool */
	bitcount = 0;
	slicecount = 0;
	bitslice = max_bitneed + 1;
	/* pr_info("sbc_decode: fit bitslices into the bitpool, bitslice = %d\n", bitslice ); */
	do{
		bitslice--;
		bitcount += slicecount;
		slicecount = 0;
		for (i = 0; i < numSubbands; i++){

			if (bitneed[i] > bitslice + 1 && bitneed[i] < bitslice + 16) slicecount++;
			else if (bitneed[i] == bitslice + 1) slicecount += 2;
		}

	} while (bitcount + slicecount < bitpoolSz);

	/* distribute bits */
	for (i = 0; i < numSubbands; i++){

		if (bitneed[i] < bitslice + 2) bits[i] = 0;
		else{

			int8_t v = bitneed[i] - bitslice;
			if (v > 16) v = 16;
			bits[i] = v;
		}
	}

	/* allocate remaining bits */
	for (i = 0; i < numSubbands && bitcount < bitpoolSz; i++)
	{

		if (bits[i] >= 2 && bits[i] < 16)
		{

			bits[i]++;
			bitcount++;
		}
		else if (bitneed[i] == bitslice + 1 && bitpoolSz > bitcount + 1)
		{

			bits[i] = 2;
			bitcount += 2;
		}
	}
	for (i = 0; i < numSubbands && bitcount < bitpoolSz; i++)
	{

		if (bits[i] < 16){

			bits[i]++;
			bitcount++;
		}
	}

	/* reconstruct subband samples (A2DP 12.6.4) */
#ifndef SPEED_OVER_ACCURACY
	for (i = 0; i < numSubbands; i++) levels[i] = (1 << bits[i]) - 1;
#endif

	/* pr_info("sbc_decode: reconstruct subband samples, blocks = %d\n", blocks );  */
	for (j = 0; j < blocks; j++)
	{

		for (i = 0; i < numSubbands; i++)
		{

			if (bits[i])
			{

				uint32_t val = 0;
				k = bits[i];
				do
				{

					val <<= 1;
#if (DEBUG_DECODING == 1)
					pr_info("%s: buf = %p, offset %d\n",
						__func__, buf, buf - start_buf);
#endif
					if (*buf & bitpos) val++;
					if (!(bitpos >>= 1))
					{
						bitpos = 0x80;
						buf++;
					}
				} while (--k);

				val = (val << 1) | 1;
				val <<= scaleFactors[i];

#ifdef SPEED_OVER_ACCURACY
				val = mulshift(val, bits[i]);
#else
				val /= levels[i];
#endif

				val -= (1 << scaleFactors[i]);

				samples[j][i] = SAMPLE_CVT(val);
			}
			else 
				samples[j][i] = SAMPLE_CVT(0);
		}
	}

	//sbc_decoder_reset();

	for (j = 0; j < blocks; j++)
	{
		synth(outBufPtr, samples[j], numSubbands, gV);
		outBufPtr += numSubbands;
	}

	/* if we used a byte partially, skip the rest of it, it is "padding"  */
	if (bitpos != 0x80) buf++;

out:
#if (DEBUG_DECODING == 1)
	if (left < 0)
		pr_err("SBC: buffer over-read by %d bytes.\n", -left);
	if (left > 0)
		pr_err("SBC: buffer under-read by %d bytes.\n", left);
#endif              

//	fwrite(usDecodedBuffer, sizeof(uint16_t), 120, fOutput);
//	fflush(fOutput);
	memset(usDecodedBuffer, 0, sizeof(usDecodedBuffer));

}

#endif
