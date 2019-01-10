#ifndef __APP_AI_VOICE_H
#define __APP_AI_VOICE_H


#ifdef __cplusplus
extern "C" {
#endif

#define AUDIO_OUTPUT_STREAMING			0
#define VOICEPATH_STREAMING				1


typedef struct{
	volatile uint8_t					 isSvStreamRunning	 : 1;
	volatile uint8_t                     isAudioAlgorithmEngineReseted   : 1;
	volatile uint8_t                     reserve             : 6;

}APP_AI_STREAM_ENV;



int app_ai_voice_start_mic_stream(void);
int app_ai_voice_stop_mic_stream(void);


void app_voicepath_set_pending_started_stream(uint8_t pendingStream, bool isEnabled);
bool app_voicepath_get_stream_pending_state(uint8_t pendingStream);
bool app_voicepath_get_stream_state(uint8_t stream);
void app_voicepath_set_stream_state(uint8_t stream, bool isEnabled);

int app_ai_voice_uart_audio_init();
int app_ai_voice_start_stream_by_uart();
int app_ai_voice_stop_stream_by_uart();

int app_ai_voice_uart_audio_init();

#ifdef __cplusplus
}
#endif

#endif

