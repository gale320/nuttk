
#ifndef __NUTCOMMON_H__
#define __NUTCOMMON_H__

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "nuterror.h"

#ifdef ARCH_64
#define MAX_POW_TWO (((size_t) 1) << 63)
#else
#define MAX_POW_TWO (((size_t) 1) << 31)
#endif /* ARCH_64 */




#define NUT_MAX_ELEMENTS ((size_t) - 2)


#define       INLINE inline
#define FORCE_INLINE inline __attribute__((always_inline))



int nut_common_cmp_str(const void *key1, const void *key2);
int nut_common_cmp_ptr(const void *key1, const void *key2);


#define NUT_CMP_STRING  nut_common_cmp_str
#define NUT_CMP_POINTER nut_common_cmp_ptr


#endif
