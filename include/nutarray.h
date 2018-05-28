#ifndef __NUTARRAY_H__
#define __NUTARRAY_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "nutcommon.h"

/**
 * A dynamic array that expands automatically as elements are
 * added. The array supports amortized constant time insertion
 * and removal of elements at the end of the array, as well as
 * constant time access.
 */
typedef struct nut_array_s Array;

/**
 * Array configuration structure. Used to initialize a new Array
 * with specific values.
 */
typedef struct nut_array_conf_s {
    /**
     * The initial capacity of the array */
    size_t capacity;

    /**
     * The rate at which the buffer expands (capacity * exp_factor). */
    float  exp_factor;

    /**
     * Memory allocators used to allocate the Array structure and the
     * underlying data buffers. */
    void *(*mem_alloc)  (size_t size);
    void *(*mem_calloc) (size_t blocks, size_t size);
    void  (*mem_free)   (void *block);
} ArrayConf;

/**
 * Array iterator structure. Used to iterate over the elements of
 * the array in an ascending order. The iterator also supports
 * operations for safely adding and removing elements during
 * iteration.
 */

typedef struct nut_array_iter_s {
    /**
     * The array associated with this iterator */
    Array  *ar;

    /**
     * The current position of the iterator.*/
    size_t  index;

    /**
     * Set to true if the last returned element was removed. */
    bool last_removed;
} ArrayIter;

/**
 * Array zip iterator structure. Used to iterate over the elements of two
 * arrays in lockstep in an ascending order until one of the Arrays is
 * exhausted. The iterator also supports operations for safely adding
 * and removing elements during iteration.
 */
typedef struct nut_array_zip_iter_s {
    Array *ar1;
    Array *ar2;
    size_t index;
    bool last_removed;
} ArrayZipIter;


NutState  nut_array_new             (Array **out);
NutState  nut_array_new_conf        (ArrayConf const * const conf, Array **out);
void      nut_array_conf_init       (ArrayConf *conf);

void      nut_array_destroy         (Array *ar);
void      nut_array_destroy_cb      (Array *ar, void (*cb) (void*));

NutState  nut_array_add             (Array *ar, void *element);
NutState  nut_array_add_at          (Array *ar, void *element, size_t index);
NutState  nut_array_replace_at      (Array *ar, void *element, size_t index, void **out);
NutState  nut_array_swap_at         (Array *ar, size_t index1, size_t index2);

NutState  nut_array_remove          (Array *ar, void *element, void **out);
NutState  nut_array_remove_at       (Array *ar, size_t index, void **out);
NutState  nut_array_remove_last     (Array *ar, void **out);
void      nut_array_remove_all      (Array *ar);
void      nut_array_remove_all_free (Array *ar);

NutState  nut_array_get_at          (Array *ar, size_t index, void **out);
NutState  nut_array_get_last        (Array *ar, void **out);

NutState  nut_array_subarray        (Array *ar, size_t from, size_t to, Array **out);
NutState  nut_array_copy_shallow    (Array *ar, Array **out);
NutState  nut_array_copy_deep       (Array *ar, void *(*cp) (void*), Array **out);

void      nut_array_reverse         (Array *ar);
NutState  nut_array_trim_capacity   (Array *ar);

size_t    nut_array_contains        (Array *ar, void *element);
size_t    nut_array_contains_value  (Array *ar, void *element, int (*cmp) (const void*, const void*));
size_t    nut_array_size            (Array *ar);
size_t    nut_array_capacity        (Array *ar);

NutState  nut_array_index_of        (Array *ar, void *element, size_t *index);
void      nut_array_sort            (Array *ar, int (*cmp) (const void*, const void*));

void      nut_array_map             (Array *ar, void (*fn) (void*));
void      nut_array_reduce          (Array *ar, void (*fn) (void*, void*, void*), void *result);

NutState  nut_array_filter_mut      (Array *ar, bool (*predicate) (const void*));
NutState  nut_array_filter          (Array *ar, bool (*predicate) (const void*), Array **out);

void      nut_array_iter_init       (ArrayIter *iter, Array *ar);
NutState  nut_array_iter_next       (ArrayIter *iter, void **out);
NutState  nut_array_iter_remove     (ArrayIter *iter, void **out);
NutState  nut_array_iter_add        (ArrayIter *iter, void *element);
NutState  nut_array_iter_replace    (ArrayIter *iter, void *element, void **out);
size_t    nut_array_iter_index      (ArrayIter *iter);


void      nut_array_zip_iter_init   (ArrayZipIter *iter, Array *a1, Array *a2);
NutState  nut_array_zip_iter_next   (ArrayZipIter *iter, void **out1, void **out2);
NutState  nut_array_zip_iter_add    (ArrayZipIter *iter, void *e1, void *e2);
NutState  nut_array_zip_iter_remove (ArrayZipIter *iter, void **out1, void **out2);
NutState  nut_array_zip_iter_replace(ArrayZipIter *iter, void *e1, void *e2, void **out1, void **out2);
size_t        nut_array_zip_iter_index  (ArrayZipIter *iter);

const void* const* nut_array_get_buffer(Array *ar);


#define ARRAY_FOREACH(val, array, body)         \
    {                                           \
        ArrayIter nut_array_iter_53d46d2a04458e7b;  \
        nut_array_iter_init(&nut_array_iter_53d46d2a04458e7b, array);   \
        void *val;                                              \
        while (nut_array_iter_next(&nut_array_iter_53d46d2a04458e7b, &val) != CC_ITER_END) \
            body                                                        \
                }


#define ARRAY_FOREACH_ZIP(val1, val2, array1, array2, body)     \
    {                                                           \
        ArrayZipIter nut_array_zip_iter_ea08d3e52f25883b3;                 \
        nut_array_zip_iter_init(&nut_array_zip_iter_ea08d3e52f25883b, array1, array2); \
        void *val1;                                                     \
        void *val2;                                                     \
        while (nut_array_zip_iter_next(&nut_array_zip_iter_ea08d3e52f25883b3, &val1, &val2) != CC_ITER_END) \
            body                                                        \
                }

#ifdef __cplusplus
}
#endif

#endif
