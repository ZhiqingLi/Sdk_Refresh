#include "common.h"

#include "sbc_math.h"
#include "sbc_tables.h"

#include "sbc_encode.h"

int sbc_clz(uint32_t x)
{
//#ifdef __GNUC__
	//return __builtin_clz(x);
//#else
	/* TODO: this should be replaced with something better if good
	 * performance is wanted when using compilers other than gcc */
	int cnt = 0;
	while (x) {
		cnt++;
		x >>= 1;
	}
	return 32 - cnt;
//#endif
}
