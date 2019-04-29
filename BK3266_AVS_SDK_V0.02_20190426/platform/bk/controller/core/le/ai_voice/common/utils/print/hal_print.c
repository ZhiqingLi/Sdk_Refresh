#include "hal_print.h"

/**
 ****************************************************************************************
 * @brief       function used to Print data in a fixed format
 *
 * @param[in]   fmt      (Print format)
 *              size     (The size of the data)
 *              count    (Length of data)
 *              buffer   (The address where data is stored)
 *
 * @return      0(successful) or -1(fail)
 ****************************************************************************************
 */
int hal_print_dump(const char *fmt, unsigned int size,  unsigned int count, const void *buffer)
{
	char buf[255];
	int len=0,n=0,i=0;

	switch( size )
	{
		case sizeof(uint32_t):
			while(i<count && len<sizeof(buf))
			{
				len += j_snprintf(&buf[len], sizeof(buf) - len, fmt, *(uint32_t *)((uint32_t *)buffer+i));
				i++;
			}
			break;
		case sizeof(uint16_t):
				while(i<count && len<sizeof(buf))
				{
					len += j_snprintf(&buf[len], sizeof(buf) - len, fmt, *(uint16_t *)((uint16_t *)buffer+i));
					i++;
				}
				break;
		case sizeof(uint8_t):
		default:
			while(i<count && len<sizeof(buf))
			{
				len += j_snprintf(&buf[len], sizeof(buf) - len, fmt, *(uint8_t *)((uint8_t *)buffer+i));
				i++;
			}
			break;
	}

    if (len + 1 < sizeof(buf)) {
        buf[len++] = '\r';
    }
    if (len + 1 < sizeof(buf)) {
        buf[len++] = '\n';
    }
    if (len + 1 < sizeof(buf)) {
        buf[len++] = '\0';
    }


    n = os_printf("%s",buf);

	return n;
}


