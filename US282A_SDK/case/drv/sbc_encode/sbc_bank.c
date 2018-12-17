/*
 *
 *  Bluetooth low-complexity, subband codec (SBC) library
 *
 *  Copyright (C) 2008-2010  Nokia Corporation
 *  Copyright (C) 2004-2010  Marcel Holtmann <marcel@holtmann.org>
 *  Copyright (C) 2004-2005  Henryk Ploetz <henryk@ploetzli.ch>
 *  Copyright (C) 2005-2008  Brad Midgley <bmidgley@xmission.com>
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

/* todo items:

  use a log2 table for byte integer scale factors calculation (sum log2 results
  for high and low bytes) fill bitpool by 16 bits instead of one at a time in
  bits allocation/bitpool generation port to the dsp

*/

#include "common.h"

#include "sbc_math.h"
#include "sbc_tables.h"

#include "sbc_encode.h"
//#include "mem_op.h"


static void sbc_set_defaults(sbc_t *sbc, unsigned long flags)
{
	sbc->frequency = SBC_FREQ_44100;
	sbc->mode = SBC_MODE_JOINT_STEREO;		//SBC_MODE_STEREO;
	sbc->subbands = SBC_SB_8;
	sbc->blocks = SBC_BLK_16;
	sbc->bitpool = 53;
    sbc->allocation = SBC_AM_LOUDNESS;
	sbc->endian = SBC_LE;
}

int sbc_init(sbc_t *sbc, unsigned long flags)
{
	if (!sbc)
		return -1;//no sbc_t parameter
libc_print("@@@sbc_init",0,0);
	memset(sbc, 0, sizeof(sbc_t));
#ifdef SBC_USE_DYN_MEM
	sbc->priv_alloc_base = pMalloc(sizeof(struct sbc_priv) + SBC_ALIGN_MASK);
#else
	//sbc->priv_alloc_base = SBC_ENDCODE_BUF_ADDR;//HEAP_END_BADDR;
	sbc->priv_alloc_base = sys_malloc_large_data(0xa00);

	//libc_print("malloc large data:", sbc->priv_alloc_base, 2);
#endif
	if (!sbc->priv_alloc_base)
		return -2;//no mem

	sbc->priv = (void *) (((uintptr_t) sbc->priv_alloc_base +
			SBC_ALIGN_MASK) & ~((uintptr_t) SBC_ALIGN_MASK)); /*0X9AB*  priv_alloc_base*/

	memset(sbc->priv, 0, sizeof(struct sbc_priv));

	sbc_set_defaults(sbc, flags);

	return 0;
}


void sbc_finish(sbc_t *sbc)
{
	if (!sbc)
		return;
#ifdef SBC_USE_DYN_MEM
	pFree(sbc->priv_alloc_base);
#else
    sys_free_large_data(sbc->priv_alloc_base);
#endif
	memset(sbc, 0, sizeof(sbc_t));
}



