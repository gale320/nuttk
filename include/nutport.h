#ifndef __NUTPORT_H__
#define __NUTPORT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nutconf.h"
#include "nutinc.h"


/*
struct _NutAlloc{
	void		*(*alloc)(void *data, size_t size);
	void		(*free)(void *data, void *pointer);
	void		*allocator_data;
};

typedef struct _NutAlloc NutAlloc;
*/

#ifdef OS_FREERTOS
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "cmsis_os.h"
#endif



#ifdef __cplusplus
}
#endif

#endif
