
#ifndef __NUTLIST_H__
#define __NUTLIST_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "nutcommon.h"

/**
 * A doubly linked list. List is a sequential structure that
 * supports insertion, deletion and lookup from both ends in
 * constant time, while the worst case is O(n/2) at the middle
 * of the list.
 */
typedef struct nut_list_s List;

/**
 * List node.
 *
 * @note Modifying the links may invalidate the list structure.
 */
typedef struct node_s {
    void          *data;
    struct node_s *next;
    struct node_s *prev;
} Node;

/**
 * List iterator structure. Used to iterate over the elements of the
 * list in an ascending or descending order. The iterator also supports
 * operations for safely adding and removing elements during iteration.
 */
typedef struct nut_list_iter_s {
    /**
     * The current position of the iterator.*/
    size_t  index;

    /**
     * The list associated with this iterator */
    List   *list;

    /**
     * Last returned node */
    Node   *last;

    /**
     * Next node in the sequence. */
    Node   *next;
} ListIter;

/**
 * List zip iterator structure. Used to iterate over two Lists in
 * lockstep in an ascending order until one of the lists is exhausted.
 * The iterator also supports operations for safely adding and
 * removing elements during iteration.
 */
typedef struct nut_list_zip_iter_s {
    List *l1;
    List *l2;
    Node *l1_last;
    Node *l2_last;
    Node *l1_next;
    Node *l2_next;
    size_t index;
} ListZipIter;


/**
 * List configuration structure. Used to initialize a new List with specific
 * values.
 */
typedef struct nut_list_conf_s {
    void  *(*mem_alloc)  (size_t size);
    void  *(*mem_calloc) (size_t blocks, size_t size);
    void   (*mem_free)   (void *block);
} ListConf;


void          nut_list_conf_init       (ListConf *conf);
NutState  nut_list_new             (List **list);
NutState  nut_list_new_conf        (ListConf const * const conf, List **list);
void          nut_list_destroy         (List *list);
void          nut_list_destroy_cb      (List *list, void (*cb) (void*));

NutState  nut_list_splice          (List *list1, List *list2);
NutState  nut_list_splice_at       (List *list, List *list2, size_t index);

NutState  nut_list_add             (List *list, void *element);
NutState  nut_list_add_at          (List *list, void *element, size_t index);
NutState  nut_list_add_all         (List *list1, List *list2);
NutState  nut_list_add_all_at      (List *list, List *list2, size_t index);
NutState  nut_list_add_first       (List *list, void *element);
NutState  nut_list_add_last        (List *list, void *element);

NutState  nut_list_remove          (List *list, void *element, void **out);
NutState  nut_list_remove_first    (List *list, void **out);
NutState  nut_list_remove_last     (List *list, void **out);
NutState  nut_list_remove_at       (List *list, size_t index, void **out);

NutState  nut_list_remove_all      (List *list);
NutState  nut_list_remove_all_cb   (List *list, void (*cb) (void*));

NutState  nut_list_get_at          (List *list, size_t index, void **out);
NutState  nut_list_get_first       (List *list, void **out);
NutState  nut_list_get_last        (List *list, void **out);

NutState  nut_list_sublist         (List *list, size_t from, size_t to, List **out);
NutState  nut_list_copy_shallow    (List *list, List **out);
NutState  nut_list_copy_deep       (List *list, void *(*cp) (void*), List **out);

NutState  nut_list_replace_at      (List *list, void *element, size_t index, void **out);

size_t        nut_list_contains        (List *list, void *element);
size_t        nut_list_contains_value  (List *list, void *element, int (*cmp) (const void*, const void*));
NutState  nut_list_index_of        (List *list, void *element, int (*cmp) (const void*, const void*), size_t *index);
NutState  nut_list_to_array        (List *list, void ***out);

void          nut_list_reverse         (List *list);
NutState  nut_list_sort            (List *list, int (*cmp) (void const*, void const*));
void          nut_list_sort_in_place   (List *list, int (*cmp) (void const*, void const*));
size_t        nut_list_size            (List *list);

void          nut_list_foreach         (List *list, void (*op) (void *));

NutState  nut_list_filter_mut      (List *list, bool (*predicate) (const void*));
NutState  nut_list_filter          (List *list, bool (*predicate) (const void*), List **out);

void          nut_list_iter_init       (ListIter *iter, List *list);
NutState  nut_list_iter_remove     (ListIter *iter, void **out);
NutState  nut_list_iter_add        (ListIter *iter,  void *element);
NutState  nut_list_iter_replace    (ListIter *iter, void *element, void **out);
size_t        nut_list_iter_index      (ListIter *iter);
NutState  nut_list_iter_next       (ListIter *iter, void **out);

void          nut_list_diter_init      (ListIter *iter, List *list);
NutState  nut_list_diter_remove    (ListIter *iter, void **out);
NutState  nut_list_diter_add       (ListIter *iter, void *element);
NutState  nut_list_diter_replace   (ListIter *iter, void *element, void **out);
size_t        nut_list_diter_index     (ListIter *iter);
NutState  nut_list_diter_next      (ListIter *iter, void **out);

void          nut_list_zip_iter_init   (ListZipIter *iter, List *l1, List *l2);
NutState  nut_list_zip_iter_next   (ListZipIter *iter, void **out1, void **out2);
NutState  nut_list_zip_iter_add    (ListZipIter *iter, void *e1, void *e2);
NutState  nut_list_zip_iter_remove (ListZipIter *iter, void **out1, void **out2);
NutState  nut_list_zip_iter_replace(ListZipIter *iter, void *e1, void *e2, void **out1, void **out2);
size_t        nut_list_zip_iter_index  (ListZipIter *iter);


#define nut_list_FOREACH(val, list, body)                                   \
    {                                                                   \
        ListIter nut_list_iter_53d46d2a04458e7b;                            \
        nut_list_iter_init(&nut_list_iter_53d46d2a04458e7b, list);              \
        void *val;                                                      \
        while (nut_list_iter_next(&nut_list_iter_53d46d2a04458e7b, &val) != CC_ITER_END) \
            body                                                        \
                }


#define nut_list_FOREACH_ZIP(val1, val2, list1, list2, body)                \
    {                                                                   \
        ListZipIter nut_list_zip_iter_ea08d3e52f25883b3;                    \
        nut_list_zip_iter_init(&nut_list_zip_iter_ea08d3e52f25883b, list1, list2); \
        void *val1;                                                     \
        void *val2;                                                     \
        while (nut_list_zip_iter_next(&nut_list_zip_iter_ea08d3e52f25883b3, &val1, &val2) != CC_ITER_END) \
            body                                                        \
                }


#ifdef __cplusplus
}
#endif

#endif
