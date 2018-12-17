

/**
* @addtogroup Bluetooth
* @{
* @defgroup bt_stack_types bt_stack_types.h
* @{
*/

#ifndef __BT_STACK_TYPES_H__
#define __BT_STACK_TYPES_H__

#include "type.h"

typedef struct _BT_DB_RECORD
{
	uint8_t			bdAddr[6];
	bool			trusted;
	uint8_t			linkKey[16];
	uint8_t			keyType;
	uint8_t			pinLen;
}BT_DB_RECORD;

#endif // __BT_STACK_TYPES_H__

