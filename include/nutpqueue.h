
#ifndef __NUTPQUEUE_H__
#define __NUTPQUEUE_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "nutcommon.h"
#include "nutarray.h"

/**
 * A Priority Queue data structure. Stores the elements and according to
 * some property associated with the element stored in the pqueue, the elements
 * can be retrieved in a specific order
 **/

typedef struct nut_pqueue_s PQueue;

/**
 * The pqueue initialization configuration structure. Used to initialize the
 * PQueue with the specified attributes
 **/

typedef struct nut_pqueue_conf_s {
    /**
     * The initial capacity of the array */
    size_t capacity;

    /**
     * The rate at which the buffer expands (capacity * exp_factor). */
    float  exp_factor;

    /**
     * comparator, used to hold the address of the function which will
     * be used to compare the elements of the PQueue
     */
    int (*cmp) (const void *a, const void *b);

    /**
     * Memory allocators used to allocate the Array structure and the
     * underlying data buffers. */
    void *(*mem_alloc)  (size_t size);
    void *(*mem_calloc) (size_t blocks, size_t size);
    void  (*mem_free)   (void *block);
} PQueueConf;

void          nut_pqueue_conf_init       (PQueueConf *conf, int (*)(const void *, const void *));
NutState  nut_pqueue_new             (PQueue **out, int (*)(const void *, const void *));
NutState  nut_pqueue_new_conf        (PQueueConf const * const conf, PQueue **out);
void          nut_pqueue_destroy         (PQueue *pqueue);
void          nut_pqueue_destroy_cb      (PQueue *pqueue, void (*cb) (void*));

NutState  nut_pqueue_push            (PQueue *pqueue, void *element);
NutState  nut_pqueue_top             (PQueue *pqueue, void **out);
NutState  nut_pqueue_pop             (PQueue *pqueue, void **out);

#ifdef __cplusplus
}
#endif
#endif
