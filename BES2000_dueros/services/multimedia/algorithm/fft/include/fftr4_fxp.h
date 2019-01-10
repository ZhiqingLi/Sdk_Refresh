/*---------------------------------------------------------------------------
 * FILE NAME      : $RCSfile: fftr4_fxp.h,v $
 * FILE PATH      : $Source: /design/CODCVS/argon/SystemDesign/PHYSimulator/ofdm/common/libraries/fftlib/fftr4v2/fftr4_fxp.h,v $
 * COMMITTED BY   : $Author: robertsr $
 * ISSUE          : $Revision: 1.3 $
 * CURRENT STATUS : $Id: fftr4_fxp.h,v 1.3 2008-03-06 12:13:54 robertsr Exp $
 * FILE STATUS    : $State: Exp $
 * LAST UPDATED   : $Date: 2008-03-06 12:13:54 $
 * 
 * --------------------------------------------------------------------------
 * Copyright (c) 2006-2008 by STMicroelectronics.  All Rights Reserved.
 * This model/code/algorithm is the confidential and proprietary 
 * property of STMicroelectronics and the possession or use of this file 
 * requires a written license from STMicroelectronics.
 *---------------------------------------------------------------------------
*/

#ifndef FFTR4_FXP_H
#define FFTR4_FXP_H

#define FFTR4_TWIDDLE_WIDTH 16
#define FFTR4_DATA_WIDTH    16
#define FFTR4_SCALE          6

/* Q1.15 */
#define FFTR4_INPUT_FORMAT_X   1
#define FFTR4_INPUT_FORMAT_Y   15
#define FFTR4_INPUT_FORMAT     (FFTR4_INPUT_FORMAT_X+FFTR4_INPUT_FORMAT_Y)

/* Q1.15 */
#define FFTR4_OUTPUT_FORMAT_X   1
#define FFTR4_OUTPUT_FORMAT_Y   15
#define FFTR4_OUTPUT_FORMAT     (FFTR4_OUTPUT_FORMAT_X+FFTR4_OUTPUT_FORMAT_Y)

typedef struct {
	int re;
	int im;
} FftData_t;

typedef struct {
	int re;
	int im;
} FftTwiddle_t;

typedef enum 
{
    FFT_MODE = 0,
    IFFT_MODE
}FftMode_t;

void make_symmetric_twiddles(FftTwiddle_t w[], int N, int width);
void fftr4(int N, FftData_t x[], FftTwiddle_t w[], int twiddleWidth, int dataWidth, FftMode_t ifft);
void dibit_reverse_array(FftData_t *vector);
//void dibit_reverse_array(FftData_t *vector);
unsigned int dibit_reverse_int(unsigned int x, unsigned int N);

#endif