#include <audio_out_interface.h>
#include "driver_beken_includes.h"
#include "app_beken_includes.h"

result_t audio_out_open(int_t rate,
                            int_t channels,
                            int_t bits_per_sample,
                            audio_out_ctx_h *audio_out_ctx)
{
    *audio_out_ctx = (void *)((uint8_t *)NULL + 1);

    return UWE_OK;
}

void audio_out_close(audio_out_ctx_h *audio_out_ctx)
{
    *audio_out_ctx = NULL;

#if 0
    #ifdef BT_ONE_TO_MULTIPLE
    if((!a2dp_all_apps_is_unused())
        || (avrcp_is_connection()))
    {
        return;
    }
    #endif

    if(!app_wave_playing())
    {
        os_printf("audio_out_close\r\n");
        extPA_close(1);
	    aud_PAmute_oper(1);
        aud_close();
    }
#endif
}

result_t audio_out_write(audio_out_ctx_h audio_out_ctx, uint8_t *buffer,
    j_size_t size)
{
    aud_fill_buffer(buffer, size);
    aud_open();
    aud_PAmute_oper(0);
    return UWE_OK;
}
// EOF
