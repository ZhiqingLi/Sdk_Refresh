#include "stdio.h"
#include "cmsis_os.h"
#include "list.h"
#include "string.h"

#include "hal_timer.h"
#include "hal_trace.h"
#include "hal_bootmode.h"
#include "hal_location.h"

#include "export_fn_rom_cust.h"

static unsigned char sha256_result[32] =  {0,};
static char str[] = "this is test string for sha256!";

void test_sha256_rom_fn(void)
{
    const unsigned char *ret = NULL;
    TRACE("%s:line %d, in", __func__, __LINE__);


    TRACE("function address 0x%x", __export_fn_rom_cust.SHA256_hash);
    memset(sha256_result, 0, sizeof(sha256_result));

    TRACE("hash array address %x", str);
    TRACE("result array address %x", sha256_result);
    ret = __export_fn_rom_cust.SHA256_hash(str, sizeof(str), sha256_result);
    TRACE("sha256 return %x", ret);
    TRACE("result array:");
    for (int i = 0;i < 32; ++i) {
        TRACE("0x%x", sha256_result[i]);
    }


    TRACE("%s:line %d, out", __func__, __LINE__);
}
