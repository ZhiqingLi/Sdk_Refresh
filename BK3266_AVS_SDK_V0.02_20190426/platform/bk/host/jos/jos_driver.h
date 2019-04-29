/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef _JOS_DRIVERS_H_
#define _JOS_DRIVERS_H_

#include <jos.h>
#include "jos_internal.h"

result_t j_drivers_init(uw_args_t *args);
void j_drivers_uninit(void);

/* Drivers manipulation */
result_t j_driver_attach(j_device_h device);
void j_driver_detach(j_device_h device);
void j_device_add_to_unmatched_list(j_device_h device);
result_t j_device_remove_from_unmatched_list(j_device_h device);

#endif

