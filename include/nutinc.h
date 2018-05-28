#ifndef __NUTINC_H__
#define __NUTINC_H__

#ifdef __cplusplus
extern "C" {
#endif


#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <assert.h>

#ifdef C11
#include <stdatomic.h>
#include <stdnoreturn.h>
#include <stdalign.h>
#include <threads.h>

#include <uchar.h>

#endif




#define and	&&
#define and_eq	&=
#define bitand	&
#define bitor	|
#define compl	~
#define not	!
#define not_eq	!=
#define or	||
#define or_eq	|=
#define xor	^
#define xor_eq	^=







#ifdef __cplusplus
}
#endif

#endif
