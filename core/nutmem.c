#include "nutmem.h"

void  *nut_mem_malloc (size_t size)
{
	return pvPortMalloc(size);
}

void  *nut_mem_calloc(size_t blocks, size_t size)
{
	return pvPortMalloc(blocks * size);

}

void  nut_mem_free(void *block)
{
	vPortFree(block);
}
