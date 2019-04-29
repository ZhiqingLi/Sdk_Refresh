/* (c) 2012 Jungo Ltd. All Rights Reserved. Jungo Confidential */
#ifndef _COMPILER_H_
#define _COMPILER_H_

#include <config/config.h>

#include "sys_config.h"

#if defined CONFIG_GCC_COMPILER
    #include "compiler_gcc.h"
#elif defined CONFIG_ARM_COMPILER
    #include "compiler_arm.h"
#elif defined CONFIG_IAR_COMPILER
    #include "compiler_iar.h"
#elif defined CONFIG_CL_COMPILER
    #include "compiler_cl.h"
#elif defined CONFIG_ST20_COMPILER
    #include "compiler_st20.h"
#elif defined CONFIG_CW_COMPILER
    #include "compiler_cw.h"
#elif defined CONFIG_DIAB_COMPILER
    #include "compiler_diab.h"
#else
    #error "Error - Compiler type is not defined!!!"
#endif

#ifndef __RESTRICT__
#error "Variable __RESTRICT__ was not defined"
#endif

#ifndef __VOLATILE__
#error "Variable __VOLATILE__ was not defined"
#endif

#ifndef __INLINE__
#error "Variable __INLINE__ was not defined"
#endif

#if defined(__cplusplus)
#ifndef __BEGIN_DECLS
    #define __BEGIN_DECLS   extern "C" {
#endif
#ifndef __END_DECLS
    #define __END_DECLS     }
#endif
#else
#ifndef __BEGIN_DECLS
    #define __BEGIN_DECLS
#endif
#ifndef __END_DECLS
    #define __END_DECLS
#endif
#endif

#ifndef __PACKED_POST__
#error "Variable __PACKED_POST__ was not defined"
#endif

#ifndef DECLARE_VAR_ZERO_INIT_FUNC
#define DECLARE_VAR_ZERO_INIT_FUNC(var)
#endif

#ifndef VAR_ZERO_INIT
#define VAR_ZERO_INIT(var)              do {} while (0)
#endif

#ifndef EXTERN_VAR_ZERO_INIT
#define EXTERN_VAR_ZERO_INIT(var)       do {} while (0)
#endif

#define UNUSED_VAR(x) (x)=(x)

#ifndef __BUFFER_ATTRIBUTE__
#define __BUFFER_ATTRIBUTE__
#endif

#endif
// EOF
