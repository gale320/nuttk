
#ifndef __NUTHASHSET_H__
#define __NUTHASHSET_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "nutcommon.h"
#include "nuthashtable.h"

/**
 * An unordered set. The lookup, deletion, and insertion are
 * performed in amortized constant time and in the worst case
 * in amortized linear time.
 */
typedef struct nut_hashset_s HashSet;

/**
 * HashSet configuration object.
 */
typedef HashTableConf HashSetConf;

/**
 * HashSet iterator structure. Used to iterate over the elements
 * of the HashSet. The iterator also supports operations for safely
 * removing elements during iteration.
 */
typedef struct nut_hashset_iter_s {
    HashTableIter iter;
} HashSetIter;

void          nut_hashset_conf_init     (HashSetConf *conf);

NutState  nut_hashset_new           (HashSet **hs);
NutState  nut_hashset_new_conf      (HashSetConf const * const conf, HashSet **hs);
void          nut_hashset_destroy       (HashSet *set);

NutState  nut_hashset_add           (HashSet *set, void *element);
NutState  nut_hashset_remove        (HashSet *set, void *element, void **out);
void          nut_hashset_remove_all    (HashSet *set);

bool          nut_hashset_contains      (HashSet *set, void *element);
size_t        nut_hashset_size          (HashSet *set);
size_t        nut_hashset_capacity      (HashSet *set);

void          nut_hashset_foreach       (HashSet *set, void (*op) (const void*));

void          nut_hashset_iter_init     (HashSetIter *iter, HashSet *set);
NutState  nut_hashset_iter_next     (HashSetIter *iter, void **out);
NutState  nut_hashset_iter_remove   (HashSetIter *iter, void **out);


#define HASHSET_FOREACH(val, hashset, body)                             \
    {                                                                   \
        HashsetIter nut_hashset_iter_53d46d2a04458e7b;                      \
        nut_hashset_iter_init(&nut_hashset_iter_53d46d2a04458e7b, hashset);     \
        void *val;                                                      \
        while (nut_hashset_iter_next(&nut_hashset_iter_53d46d2a04458e7b, &val) != CC_ITER_END) \
            body                                                        \
                }

#ifdef __cplusplus
}
#endif

#endif
