#include <assert.h>
#include "nvrecord.h"
#include "nvrecord_env.h"
#include "nvrec_mempool.h"
#include "hal_trace.h"
#include "customparam_section.h"

#define NV_RECORD_ENV_KEY  "env_key"

extern nv_record_struct nv_record_config;
static struct nvrecord_env_t *nvrecord_env_p = NULL;

static int nv_record_env_new(void)
{

	nvrecord_env_p = (struct nvrecord_env_t *)pool_malloc(sizeof(struct nvrecord_env_t));

	if (!nvrecord_env_p){
		TRACE("%s pool_malloc failed ");
		return -1;
	}
	nvrec_config_set_int(nv_record_config.config,section_name_other,(const char *)NV_RECORD_ENV_KEY,(int)nvrecord_env_p);

	nvrecord_env_p->media_language.language = NVRAM_ENV_MEDIA_LANGUAGE_DEFAULT;
#ifdef __TWS__		
	nvrecord_env_p->tws_mode.mode = NVRAM_ENV_TWS_MODE_DEFAULT;
#endif
	nvrecord_env_p->factory_tester_status.status = NVRAM_ENV_FACTORY_TESTER_STATUS_DEFAULT;
	nv_record_config.is_update = true;

    TRACE("%s nvrecord_env_p:%x",__func__, nvrecord_env_p);

	return 0;
}

int nv_record_env_init(void)
{
	int nRet = 0; 
	nv_record_open(section_usrdata_ddbrecord);

	nv_custom_parameter_section_init();

	nvrecord_env_p = (struct nvrecord_env_t *)nvrec_config_get_int(nv_record_config.config, (const char *)section_name_other,(const char *)NV_RECORD_ENV_KEY, NVRAM_ENV_INVALID);

	if (nvrecord_env_p == (struct nvrecord_env_t *)NVRAM_ENV_INVALID){
		if (nv_record_env_new())
			nRet = -1;
	}
    TRACE("%s nvrecord_env_p:%x",__func__, nvrecord_env_p);
#ifdef __TWS__			
	TRACE("tws_mode.mode:%d",nvrecord_env_p->tws_mode.mode);
	DUMP8("0x%02x ", nvrecord_env_p->tws_mode.record.bdAddr.addr, 6);
#endif
	return nRet;
}

int nv_record_env_get(struct nvrecord_env_t **nvrecord_env)
{
	if (!nvrecord_env)
		return -1;

	if (!nvrecord_env_p)
		return -1;

	*nvrecord_env = nvrecord_env_p;

	return 0;
}

int nv_record_env_set(struct nvrecord_env_t *nvrecord_env)
{
	if (!nvrecord_env)
		return -1;

	if (!nvrecord_env_p)
		return -1;

	nv_record_config.is_update = true;

	return 0;
}

