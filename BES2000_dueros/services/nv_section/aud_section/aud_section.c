#include <stdio.h>
#include "hal_trace.h"
#include "aud_section.h"
#include "crc32.h"
#include "tgt_hardware.h"

extern uint32_t __aud_start[];

#ifndef ANC_COEF_LIST_NUM
#define ANC_COEF_LIST_NUM                   0
#endif

#ifndef CHIP_BEST1000
// static uint32_t active_app_mode_index = 0;
#endif

int anccfg_loadfrom_audsec(const struct_anc_cfg *list[], const struct_anc_cfg *list_44p1k[], uint32_t count)
{
#ifdef CHIP_BEST1000

    uint32_t crc;
    const struct_audsec *audsec_ptr;
    int i;

    audsec_ptr = (struct_audsec *)__aud_start;
    TRACE("0x%x,0x%x,0x%x",audsec_ptr->sec_head.magic,audsec_ptr->sec_head.version,audsec_ptr->sec_head.crc);

    if (audsec_ptr->sec_head.magic != aud_section_magic) {
        TRACE("Invalid aud section - magic");
        return 1;
    }

    crc = crc32(0, (unsigned char *)&(audsec_ptr->sec_data), sizeof(aud_section));
    TRACE("ln %d,crc = 0x%x",__LINE__,crc);

    if (crc != audsec_ptr->sec_head.crc){
        TRACE("Invalid aud section - crc");
        return 2;
    }
    TRACE("valid aud section.");

    for (i = 0; i < count && i < ANC_INDEX_TOTAL; i++) {
        list[i] = (const struct_anc_cfg *)&audsec_ptr->sec_data.anc_config.anc_config_arr[i];
    }
    for (i = count; i < (count * 2) && i < ANC_INDEX_TOTAL; i++) {
        list_44p1k[i] = (const struct_anc_cfg *)&audsec_ptr->sec_data.anc_config.anc_config_arr[i];
    }

#else

    unsigned int re_calc_crc,i;
    const best2000_aud_section *audsec_ptr;

    audsec_ptr = (best2000_aud_section *)__aud_start;
    TRACE("0x%x,0x%x,0x%x",audsec_ptr->sec_head.magic,audsec_ptr->sec_head.version,audsec_ptr->sec_head.crc);
    if (audsec_ptr->sec_head.magic != aud_section_magic) {
        TRACE("Invalid aud section - magic");
        return 1;
    }
    re_calc_crc = crc32(0,(unsigned char *)&(audsec_ptr->sec_body),sizeof(audsec_body)-4);
    if (re_calc_crc != audsec_ptr->sec_head.crc){
        TRACE("crc verify failure, invalid aud section.");
        return 1;
    }
    TRACE("Valid aud section.");
    for(i=0;i<ANC_COEF_LIST_NUM;i++)
        list[i] = (struct_anc_cfg *)&(audsec_ptr->sec_body.anc_config.anc_config_arr[i].anc_cfg[PCTOOL_SAMPLERATE_48X8K]);
    for(i=0;i<ANC_COEF_LIST_NUM;i++)
        list_44p1k[i] = (struct_anc_cfg *)&(audsec_ptr->sec_body.anc_config.anc_config_arr[i].anc_cfg[PCTOOL_SAMPLERATE_44_1X8K]);

#endif

    return 0;
}

