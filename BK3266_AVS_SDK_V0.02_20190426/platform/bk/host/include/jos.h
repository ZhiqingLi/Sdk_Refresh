/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef __JOS_H__
#define __JOS_H__

#include <config/config.h>
#include "include/uw_errno.h"

#include "include/uw_types.h"
#include "uw_types_util.h"
#include "uw_compiler.h"
#include "uw_debug.h"
#include "uw_lists.h"
#include "os_port.h"
#include "uw_controllers.h"
#include <include/uw_event.h>

#ifdef CONFIG_TEST_DICT
# include <test/test_dict.h>
#endif

#if defined(CONFIG_JHOST) || defined(CONFIG_JSLAVE)
# include <jos/include/jos_uw.h>
#endif
#if defined(CONFIG_JOS_BUS) || defined(CONFIG_JOS_THREADS)
# include <jos/include/jos_bus.h>
#endif

#include <jos/include/jos_device.h>
#include <jos/include/jos_mem.h>
#include <jos/include/jos_stdlib.h>
#include <jos/include/jos_sync.h>
#include <jos/include/jos_strings.h>
#ifdef CONFIG_JOS_SECURE_PTR
#include <jos/include/jos_secure_ptr.h>
#endif
#ifdef CONFIG_JOS_DISK
#include <jos/include/jos_disk.h>
#endif
#if defined(CONFIG_KERNEL_SPACE) || defined(CONFIG_USER_SPACE)
# include <jos/include/jos_kernel_user_common.h>
#endif
#ifdef CONFIG_KERNEL_SPACE
# include <jos/include/jos_kernel.h>
#endif

#ifdef CONFIG_EVALUATION_TIME_MINUTES
bool_t jevaluation_expired(void);
#endif

#ifdef CONFIG_BUS_PCI
# include "os_pci.h"
# include <jos/include/jos_pci.h>
#endif

#endif

