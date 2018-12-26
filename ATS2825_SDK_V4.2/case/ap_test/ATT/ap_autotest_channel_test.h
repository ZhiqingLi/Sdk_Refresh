#ifndef __AP_AUTOTEST_CHANNEL_TEST_H
#define __AP_AUTOTEST_CHANNEL_TEST_H

#define ADC_SAMPLE_RATE             (16)

#define POWER_LOW_THRESHOLD         (0x1000)

#define MIN_POWER_SAMPLE_COUNT      (4)

#define MAX_POWER_SAMPLE_COUNT      (10)

#define INVALID_POWER_VAL           0xffff

#define MAX_POWER_DIFF_VAL          (0x300)

#define MAX_SAVE_DATA_COUNT         (4)
//#define PRINT_CHANNEL_DATA

#ifdef DEBUG_WRITE_CHANNEL_DATA
extern void write_temp_file(uint8 file_index, uint8 *write_buffer, uint32 write_len);
#endif

extern void adc_data_deal(void);

#endif
