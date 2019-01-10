#ifndef __CUSTOMPARAM_SECTION_H__
#define __CUSTOMPARAM_SECTION_H__

#ifdef __cplusplus
extern "C" {
#endif

// Could be customized
#define CUSTOMPARAM_MAGIC_CODE	0x54534542
#define CUSTOMPARAM_VERSION		1

#define CUSTOMPARAM_SECTION_SIZE	4096	// one flash page

typedef struct 
{
	uint32_t 	magic_code;	// fixed value as CUSTOMPARAM_MAGIC_CODE
	uint16_t	version;
	uint16_t	length;		// length in bytes of the following data in the section
	uint16_t	entryCount;
	// following are parameter entries
	
} __attribute__((packed)) CUSTOM_PARAM_SECTION_HEADER_T;

typedef struct
{
	uint16_t	paramIndex;
	uint16_t	paramLen;
	// following are the parameter content with length paramLen
} __attribute__((packed)) CUSTOM_PARAM_ENTRY_HEADER_T;

#define CUSTOM_PARAM_SERIAL_NUM_INDEX	0
#define CUSTOM_PARAM_SERIAL_NUM_LEN		16
typedef struct
{
	uint8_t sn[CUSTOM_PARAM_SERIAL_NUM_LEN];
} CUSTOM_PARAM_SERIAL_NUM_T;

// TODO:
// Add your own custom parameters here


void nv_custom_parameter_section_init(void);
bool nv_custom_parameter_section_get_entry(
	uint16_t paramIndex, uint8_t* pParamVal, uint32_t* pParamLen);

#ifdef __cplusplus
}
#endif
#endif

