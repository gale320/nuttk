
#ifndef __NUTSTACK_H__
#define __NUTSTACK_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nutcommon.h"
#include "nutarray.h"

/**
 * A LIFO (last in first out) structure. Supports constant time
 * insertion, removal and lookup.
 */
typedef struct nut_stack_s Stack;

/**
 * Stack configuration structure. Used to initialize a new Stack
 * with specific attributes.
 */
typedef ArrayConf StackConf;

/**
 * Stack iterator structure. Used to iterate over the elements of
 * the Stack in an ascending order. The iterator also supports
 * operations for safely adding and removing elements during
 * iteration.
 */
typedef struct nut_stack_iter_s {
    ArrayIter i;
} StackIter;

/**
 * Stack zip iterator structure. Used to iterate over the elements
 * of two Stacks in lockstep in an ascending order until one of the
 * Stacks is exhausted. The iterator also supports operations for
 * safely adding and removing elements during iteration.
 */
typedef struct nut_stack_zip_iter_s {
    ArrayZipIter i;
} StackZipIter;


void          nut_stack_conf_init       (StackConf *conf);
NutState  nut_stack_new             (Stack **out);
NutState  nut_stack_new_conf        (StackConf const * const conf, Stack **out);
void          nut_stack_destroy         (Stack *stack);
void          nut_stack_destroy_cb      (Stack *stack, void (*cb) (void*));

NutState  nut_stack_push            (Stack *stack, void *element);
NutState  nut_stack_peek            (Stack *stack, void **out);
NutState  nut_stack_pop             (Stack *stack, void **out);

size_t        nut_stack_size            (Stack *stack);
void          nut_stack_map             (Stack *stack, void (*fn) (void *));

void          nut_stack_iter_init       (StackIter *iter, Stack *s);
NutState  nut_stack_iter_next       (StackIter *iter, void **out);
NutState  nut_stack_iter_replace    (StackIter *iter, void *element, void **out);

void          nut_stack_zip_iter_init   (StackZipIter *iter, Stack *a1, Stack *a2);
NutState  nut_stack_zip_iter_next   (StackZipIter *iter, void **out1, void **out2);
NutState  nut_stack_zip_iter_replace(StackZipIter *iter, void *e1, void *e2, void **out1, void **out2);


#define STACK_FOREACH(val, stack, body)                                 \
    {                                                                   \
        StackIter nut_stack_iter_53d46d2a04458e7b;                          \
        nut_stack_iter_init(&nut_stack_iter_53d46d2a04458e7b, stack);           \
        void *val;                                                      \
        while (nut_stack_iter_next(&nut_stack_iter_53d46d2a04458e7b, &val) != CC_ITER_END) \
            body                                                        \
                }


#define STACK_FOREACH_ZIP(val1, val2, stack1, stack2, body)             \
    {                                                                   \
        StackZipIter nut_stack_zip_iter_ea08d3e52f25883b3;                  \
        nut_stack_zip_iter_init(&nut_stack_zip_iter_ea08d3e52f25883b, stack1, stack2); \
        void *val1;                                                     \
        void *val2;                                                     \
        while (nut_stack_zip_iter_next(&nut_stack_zip_iter_ea08d3e52f25883b3, &val1, &val2) != CC_ITER_END) \
            body                                                        \
                }


#ifdef __cplusplus
}
#endif
#endif
