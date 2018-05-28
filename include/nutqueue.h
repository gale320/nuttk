#ifndef __NUTQUEUE_H__
#define __NUTQUEUE_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "nutcommon.h"
#include "nutdeque.h"

/**
 * A FIFO (first in first out) structure. Supports constant time
 * insertion, removal and lookup.
 */
typedef struct nut_queue_s Queue;

/**
 * Queue configuration object.
 */
typedef DequeConf QueueConf;

/**
 * Queue iterator object. Used to iterate over the elements of a
 * queue in an ascending order.
 */
typedef struct nut_queue_iter_s {
    DequeIter i;
} QueueIter;

/**
 * Queue zip iterator structure. Used to iterate over the elements of two
 * queues in lockstep in an ascending order until one of the queues is
 * exhausted. The iterator also supports operations for safely adding
 * and removing elements during iteration.
 */
typedef struct nut_queue_zip_iter_s {
    DequeZipIter i;
} QueueZipIter;


void         nut_queue_conf_init       (QueueConf *conf);
NutState nut_queue_new             (Queue **q);
NutState nut_queue_new_conf        (QueueConf const * const conf, Queue **q);
void         nut_queue_destroy         (Queue *queue);
void         nut_queue_destroy_cb      (Queue *queue, void (*cb) (void*));

NutState nut_queue_peek            (Queue const * const queue, void **out);
NutState nut_queue_poll            (Queue *queue, void **out);
NutState nut_queue_enqueue         (Queue *queue, void *element);

size_t       nut_queue_size            (Queue const * const queue);
void         nut_queue_foreach         (Queue *queue, void (*op) (void*));

void         nut_queue_iter_init       (QueueIter *iter, Queue *queue);
NutState nut_queue_iter_next       (QueueIter *iter, void **out);
NutState nut_queue_iter_replace    (QueueIter *iter, void *replacement, void **out);

void         nut_queue_zip_iter_init   (QueueZipIter *iter, Queue *q1, Queue *q2);
NutState nut_queue_zip_iter_next   (QueueZipIter *iter, void **out1, void **out2);
NutState nut_queue_zip_iter_replace(QueueZipIter *iter, void *e1, void *e2, void **out1, void **out2);


#define QUEUE_FOREACH(val, queue, body)                                 \
    {                                                                   \
        QueueIter nut_queue_iter_53d46d2a04458e7b;                          \
        nut_queue_iter_init(&nut_queue_iter_53d46d2a04458e7b, queue);           \
        void *val;                                                      \
        while (nut_queue_iter_next(&nut_queue_iter_53d46d2a04458e7b, &val) != CC_ITER_END) \
            body                                                        \
                }


#define QUEUE_FOREACH_ZIP(val1, val2, queue1, queue2, body)             \
    {                                                                   \
        QueueZipIter nut_queue_zip_iter_ea08d3e52f25883b3;                  \
        nut_queue_zip_iter_init(&nut_queue_zip_iter_ea08d3e52f25883b, queue1, queue2); \
        void *val1;                                                     \
        void *val2;                                                     \
        while (nut_queue_zip_iter_next(&nut_queue_zip_iter_ea08d3e52f25883b3, &val1, &val2) != CC_ITER_END) \
            body                                                        \
                }

#ifdef __cplusplus
}
#endif
#endif
