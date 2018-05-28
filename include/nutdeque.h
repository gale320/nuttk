#ifndef __NUTDEQUE_H__
#define __NUTDEQUE_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "nutcommon.h"

/**
 * A dynamic array that supports amortized constant time insertion
 * and removal at both ends and constant time access.
 */
typedef struct nut_deque_s Deque;

/**
 * Deque configuration structure. Used to initialize a new Deque
 * with specific values.
 */
typedef struct nut_deque_conf_s {
    /**
     * The initial capacity of the deque. Must be a power of two.
     * if a non power of two is passed, it will be rounded to the
     * closest upper power of two */
    size_t capacity;

    /**
     * Memory allocators used to allocate the Vector structure and the
     * underlying data buffers. */
    void *(*mem_alloc)  (size_t size);
    void *(*mem_calloc) (size_t blocks, size_t size);
    void  (*mem_free)   (void *block);
} DequeConf;

/**
 * Deque iterator object. Used to iterate over the elements of
 * a deque in an ascending order. The iterator also supports
 * operations for safely  removing and adding elements during
 * iteration.
 */
typedef struct nut_deque_iter_s {
    /**
     * The deque associated with this iterator.*/
    Deque *deque;

    /**
     * The current logical position of the iterator. */
    size_t index;

    bool last_removed;
} DequeIter;

/**
 * Deque zip iterator structure. Used to iterate over two Deques in
 * lockstep in an ascending order until one of the deques is exhausted.
 * The iterator also supports operations for safely adding and removing
 * elements during iteration.
 */
typedef struct nut_deque_zip_iter_s {
    Deque *d1;
    Deque *d2;
    size_t index;
    bool last_removed;
} DequeZipIter;


NutState  nut_deque_new             (Deque **deque);
NutState  nut_deque_new_conf        (DequeConf const * const conf, Deque **deque);
void          nut_deque_conf_init       (DequeConf *conf);

void          nut_deque_destroy         (Deque *deque);
void          nut_deque_destroy_cb      (Deque *deque, void (*cb) (void*));

NutState  nut_deque_add             (Deque *deque, void *element);
NutState  nut_deque_add_first       (Deque *deque, void *element);
NutState  nut_deque_add_last        (Deque *deque, void *element);
NutState  nut_deque_add_at          (Deque *deque, void *element, size_t index);
NutState  nut_deque_replace_at      (Deque *deque, void *element, size_t index, void **out);

NutState  nut_deque_remove          (Deque *deque, void *element, void **out);
NutState  nut_deque_remove_at       (Deque *deque, size_t index, void **out);
NutState  nut_deque_remove_first    (Deque *deque, void **out);
NutState  nut_deque_remove_last     (Deque *deque, void **out);
void          nut_deque_remove_all      (Deque *deque);
void          nut_deque_remove_all_cb   (Deque *deque, void (*cb) (void*));

NutState  nut_deque_get_at          (Deque const * const deque, size_t index, void **out);
NutState  nut_deque_get_first       (Deque const * const deque, void **out);
NutState  nut_deque_get_last        (Deque const * const deque, void **out);

NutState  nut_deque_copy_shallow    (Deque const * const deque, Deque **out);
NutState  nut_deque_copy_deep       (Deque const * const deque, void *(*cp) (void*), Deque **out);

void          nut_deque_reverse         (Deque *deque);
NutState  nut_deque_trim_capacity   (Deque *deque);

size_t        nut_deque_contains        (Deque const * const deque, const void *element);
size_t        nut_deque_contains_value  (Deque const * const deque, const void *element, int (*cmp)(const void*, const void*));
size_t        nut_deque_size            (Deque const * const deque);
size_t        nut_deque_capacity        (Deque const * const deque);

NutState  nut_deque_index_of        (Deque const * const deque, const void *element, size_t *i);

void          nut_deque_foreach         (Deque *deque, void (*fn) (void *));

NutState  nut_deque_filter_mut      (Deque *deque, bool (*predicate) (const void*));
NutState  nut_deque_filter          (Deque *deque, bool (*predicate) (const void*), Deque **out);

void          nut_deque_iter_init       (DequeIter *iter, Deque *deque);
NutState  nut_deque_iter_next       (DequeIter *iter, void **out);
NutState  nut_deque_iter_remove     (DequeIter *iter, void **out);
NutState  nut_deque_iter_add        (DequeIter *iter, void *element);
NutState  nut_deque_iter_replace    (DequeIter *iter, void *replacement, void **out);
size_t        nut_deque_iter_index      (DequeIter *iter);

void          nut_deque_zip_iter_init   (DequeZipIter *iter, Deque *d1, Deque *d2);
NutState  nut_deque_zip_iter_next   (DequeZipIter *iter, void **out1, void **out2);
NutState  nut_deque_zip_iter_add    (DequeZipIter *iter, void *e1, void *e2);
NutState  nut_deque_zip_iter_remove (DequeZipIter *iter, void **out1, void **out2);
NutState  nut_deque_zip_iter_replace(DequeZipIter *iter, void *e1, void *e2, void **out1, void **out2);
size_t        nut_deque_zip_iter_index  (DequeZipIter *iter);

const void* const* nut_deque_get_buffer (Deque const * const deque);


#define DEQUE_FOREACH(val, deque, body)                                 \
    {                                                                   \
        DequeIter nut_deque_iter_53d46d2a04458e7b;                          \
        nut_deque_iter_init(&nut_deque_iter_53d46d2a04458e7b, deque);           \
        void *val;                                                      \
        while (nut_deque_iter_next(&nut_deque_iter_53d46d2a04458e7b, &val) != CC_ITER_END) \
            body                                                        \
                }


#define DEQUE_FOREACH_ZIP(val1, val2, deque1, deque2, body)             \
    {                                                                   \
        DequeZipIter nut_deque_zip_iter_ea08d3e52f25883b3;                  \
        nut_deque_zip_iter_init(&nut_deque_zip_iter_ea08d3e52f25883b, deque1, deque2); \
        void *val1;                                                     \
        void *val2;                                                     \
        while (nut_deque_zip_iter_next(&nut_deque_zip_iter_ea08d3e52f25883b3, &val1, &val2) != CC_ITER_END) \
            body                                                        \
                }


#ifdef __cplusplus
}
#endif
#endif
