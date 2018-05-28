
#ifndef __NUTTREESET_H__
#define __NUTTREESET_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nutcommon.h"
#include "nuttreetable.h"

/**
 * An ordered set. The lookup, deletion, and insertion are
 * performed in logarithmic time.
 */
typedef struct nut_treeset_s TreeSet;

/**
 * TreeSet configuration structure.
 */
typedef TreeTableConf TreeSetConf;

/**
 * TreeSet iterator structure. Used to iterate over the elements of the set.
 * The iterator also supports operations for safely removing elements
 * during iteration.
 */
typedef struct nut_treeset_iter_s {
    TreeTableIter i;
} TreeSetIter;


void          nut_treeset_conf_init        (TreeSetConf *conf);
NutState  nut_treeset_new              (int (*cmp) (const void*, const void*), TreeSet **set);
NutState  nut_treeset_new_conf         (TreeSetConf const * const conf, TreeSet **set);

void          nut_treeset_destroy          (TreeSet *set);

NutState  nut_treeset_add              (TreeSet *set, void *element);
NutState  nut_treeset_remove           (TreeSet *set, void *element, void **out);
void          nut_treeset_remove_all       (TreeSet *set);

NutState  nut_treeset_get_first        (TreeSet *set, void **out);
NutState  nut_treeset_get_last         (TreeSet *set, void **out);
NutState  nut_treeset_get_greater_than (TreeSet *set, void *element, void **out);
NutState  nut_treeset_get_lesser_than  (TreeSet *set, void *element, void **out);

bool          nut_treeset_contains         (TreeSet *set, void *element);
size_t        nut_treeset_size             (TreeSet *set);

void          nut_treeset_foreach          (TreeSet *set, void (*op) (const void*));

void          nut_treeset_iter_init        (TreeSetIter *iter, TreeSet *set);
NutState  nut_treeset_iter_next        (TreeSetIter *iter, void **element);
NutState  nut_treeset_iter_remove      (TreeSetIter *iter, void **out);


#define TREESET_FOREACH(val, treeset, body)                             \
    {                                                                   \
        TreesetIter nut_treeset_iter_53d46d2a04458e7b;                      \
        nut_treeset_iter_init(&nut_treeset_iter_53d46d2a04458e7b, treeset);     \
        void *val;                                                      \
        while (nut_treeset_iter_next(&nut_treeset_iter_53d46d2a04458e7b, &val) != CC_ITER_END) \
            body                                                        \
                }

#ifdef __cplusplus
}
#endif
#endif
