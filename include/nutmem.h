#ifndef __NUTMEM_H__
#define __NUTMEM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nutconf.h"
#include "nutinc.h"
#include "nutport.h"


void  *nut_mem_malloc (size_t size);
void  *nut_mem_calloc(size_t blocks, size_t size);
void   nut_mem_free(void *block);



#ifdef __cplusplus
}
#endif

#endif
