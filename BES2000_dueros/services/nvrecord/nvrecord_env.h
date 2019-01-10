#ifndef NVRECORD_ENV_H
#define NVRECORD_ENV_H

#ifdef __cplusplus
extern "C" {
#endif

#define NVRAM_ENV_INVALID (0xdead0000)
#define NVRAM_ENV_MEDIA_LANGUAGE_DEFAULT (0)
#define NVRAM_ENV_STREAM_VOLUME_A2DP_VOL_DEFAULT (AUDIO_OUTPUT_VOLUME_DEFAULT)
#define NVRAM_ENV_STREAM_VOLUME_HFP_VOL_DEFAULT (AUDIO_OUTPUT_VOLUME_DEFAULT)
#define NVRAM_ENV_TWS_MODE_DEFAULT (0)
#define NVRAM_ENV_FACTORY_TESTER_STATUS_DEFAULT (0xaabbccdd)

struct media_language_t{
	int8_t language;
};

#if defined(APP_LINEIN_A2DP_SOURCE)||defined(APP_I2S_A2DP_SOURCE)
struct src_snk_t{
	int8_t src_snk_mode;
};
#endif

#ifdef __TWS__
struct tws_mode_t{
    uint32_t mode;
    BtDeviceRecord record;
};
#endif

struct factory_tester_status_t{
    uint32_t status;
};

struct nvrecord_env_t{
	struct media_language_t media_language;
#if defined(APP_LINEIN_A2DP_SOURCE)||defined(APP_I2S_A2DP_SOURCE)
	struct src_snk_t src_snk_flag;
#endif
#ifdef __TWS__	
	struct tws_mode_t tws_mode;
#endif
	struct factory_tester_status_t factory_tester_status;
};

int nv_record_env_init(void);

int nv_record_env_get(struct nvrecord_env_t **nvrecord_env);

int nv_record_env_set(struct nvrecord_env_t *nvrecord_env);

#ifdef __cplusplus
}
#endif
#endif
