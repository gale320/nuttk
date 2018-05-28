
#ifndef __NUTSLIST_H__
#define __NUTSLIST_H__


#ifdef __cplusplus
extern "C" {
#endif


#include "nutcommon.h"

/**
 * A singly linked list. List is a sequential structure that
 * supports constant time insertion, deletion  and lookup at
 * the beginning of the list, while the worst case for these
 * operations is linear time.
 */
typedef struct nut_slist_s SList;

/**
 * SList node.
 *
 * @note Modifying the links may invalidate the list structure.
 */
typedef struct snode_s {
    void           *data;
    struct snode_s *next;
} SNode;

/**
 * SList iterator structure. Used to iterate over the elements
 * of the list in an ascending order. The iterator also supports
 * operations for safely adding and removing elements during iteration.
 */
typedef struct nut_slist_iter_s {
    size_t  index;
    SList  *list;
    SNode   *next;
    SNode   *current;
    SNode   *prev;
} SListIter;

/**
 * SList zip iterator structure. Used to iterate over two SLists in
 * lockstep in an ascending order until one of the lists is exhausted.
 * The iterator also supports operations for safely adding and
 * removing elements during iteration.
 */
typedef struct nut_slist_zip_iter_s {
    size_t index;
    SList *l1;
    SList *l2;
    SNode *l1_next;
    SNode *l2_next;
    SNode *l1_current;
    SNode *l2_current;
    SNode *l1_prev;
    SNode *l2_prev;
} SListZipIter;


/**
 * SList configuration structure. Used to initialize a new SList with
 * specific values.
 */
typedef struct nut_slist_conf_s {
    void  *(*mem_alloc)  (size_t size);
    void  *(*mem_calloc) (size_t blocks, size_t size);
    void   (*mem_free)   (void *block);
} SListConf;


void          nut_slist_conf_init       (SListConf *conf);
NutState  nut_slist_new             (SList **list);
NutState  nut_slist_new_conf        (SListConf const * const conf, SList **list);
void          nut_slist_destroy         (SList *list);
void          nut_slist_destroy_cb      (SList *list, void (*cb) (void*));

NutState  nut_slist_splice          (SList *list1, SList *list2);
NutState  nut_slist_splice_at       (SList *list1, SList *list2, size_t index);

NutState  nut_slist_add             (SList *list, void *element);
NutState  nut_slist_add_at          (SList *list, void *element, size_t index);
NutState  nut_slist_add_all         (SList *list1, SList *list2);
NutState  nut_slist_add_all_at      (SList *list1, SList *list2, size_t index);
NutState  nut_slist_add_first       (SList *list, void *element);
NutState  nut_slist_add_last        (SList *list, void *element);

NutState  nut_slist_remove          (SList *list, void *element, void **out);
NutState  nut_slist_remove_first    (SList *list, void **out);
NutState  nut_slist_remove_last     (SList *list, void **out);
NutState  nut_slist_remove_at       (SList *list, size_t index, void **out);

NutState  nut_slist_remove_all      (SList *list);
NutState  nut_slist_remove_all_cb   (SList *list, void (*cb) (void*));

NutState  nut_slist_get_at          (SList *list, size_t index, void **out);
NutState  nut_slist_get_first       (SList *list, void **out);
NutState  nut_slist_get_last        (SList *list, void **out);

NutState  nut_slist_sublist         (SList *list, size_t from, size_t to, SList **out);
NutState  nut_slist_copy_shallow    (SList *list, SList **out);
NutState  nut_slist_copy_deep       (SList *list, void *(*cp) (void*), SList **out);

NutState  nut_slist_replace_at      (SList *list, void *element, size_t index, void **out);

size_t        nut_slist_contains        (SList *list, void *element);
size_t        nut_slist_contains_value  (SList *list, void *element, int (*cmp) (const void*, const void*));
NutState  nut_slist_index_of        (SList *list, void *element, size_t *index);
NutState  nut_slist_to_array        (SList *list, void ***out);

void          nut_slist_reverse         (SList *list);
NutState  nut_slist_sort            (SList *list, int (*cmp) (void const*, void const*));
size_t        nut_slist_size            (SList *list);

void          nut_slist_foreach         (SList *list, void (*op) (void *));

NutState  nut_slist_filter          (SList *list, bool (*predicate) (const void*), SList **out);
NutState  nut_slist_filter_mut      (SList *list, bool (*predicate) (const void*));

void          nut_slist_iter_init       (SListIter *iter, SList *list);
NutState  nut_slist_iter_remove     (SListIter *iter, void **out);
NutState  nut_slist_iter_add        (SListIter *iter, void *element);
NutState  nut_slist_iter_replace    (SListIter *iter, void *element, void **out);
NutState  nut_slist_iter_next       (SListIter *iter, void **out);
size_t        nut_slist_iter_index      (SListIter *iter);

void          nut_slist_zip_iter_init   (SListZipIter *iter, SList *l1, SList *l2);
NutState  nut_slist_zip_iter_next   (SListZipIter *iter, void **out1, void **out2);
NutState  nut_slist_zip_iter_add    (SListZipIter *iter, void *e1, void *e2);
NutState  nut_slist_zip_iter_remove (SListZipIter *iter, void **out1, void **out2);
NutState  nut_slist_zip_iter_replace(SListZipIter *iter, void *e1, void *e2, void **out1, void **out2);
size_t        nut_slist_zip_iter_index  (SListZipIter *iter);


#define nut_slist_FOREACH(val, slist, body)                                 \
    {                                                                   \
        SlistIter nut_slist_iter_53d46d2a04458e7b;                          \
        nut_slist_iter_init(&nut_slist_iter_53d46d2a04458e7b, slist);           \
        void *val;                                                      \
        while (nut_slist_iter_next(&nut_slist_iter_53d46d2a04458e7b, &val) != CC_ITER_END) \
            body                                                        \
                }


#define nut_slist_FOREACH_ZIP(val1, val2, slist1, slist2, body)             \
    {                                                                   \
        SlistZipIter nut_slist_zip_iter_ea08d3e52f25883b3;                  \
        nut_slist_zip_iter_init(&nut_slist_zip_iter_ea08d3e52f25883b, slist1, slist2); \
        void *val1;                                                     \
        void *val2;                                                     \
        while (nut_slist_zip_iter_next(&nut_slist_zip_iter_ea08d3e52f25883b3, &val1, &val2) != CC_ITER_END) \
            body                                                        \
                }


#ifdef __cplusplus
}
#endif
#endif
