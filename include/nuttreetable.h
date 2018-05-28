#ifndef __NUTTREETABLE_H__
#define __NUTTREETABLE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "nutcommon.h"

/**
 * An ordered key-value map. TreeTable supports logarithmic time
 * insertion, removal and lookup of values.
 */
typedef struct nut_treetable_s TreeTable;

/**
 * Red-Black tree node.
 *
 * @note Modifying this structure may invalidate the table.
 */
typedef struct rbnode_s {
    /**
     * Key in the table. */
    void *key;

    /**
     * Value associated with the key */
    void *value;

    /**
     * The color of this node */
    char  color;

    /**
     * Parent of this node */
    struct rbnode_s *parent;

    /**
     * Left child node */
    struct rbnode_s *left;

    /**
     * Right child node */
    struct rbnode_s *right;
} RBNode;

/**
 * TreeTable table entry.
 */
typedef struct tree_table_entry_s {
    void *key;
    void *value;
} TreeTableEntry;

/**
 * TreeTable iterator structure. Used to iterate over the entries
 * of the table. The iterator also supports operations for safely
 * removing elements during iteration.
 *
 * @note This structure should only be modified through the
 * iterator functions.
 */
typedef struct tree_table_iter_s {
    TreeTable *table;
    RBNode    *current;
    RBNode    *next;
} TreeTableIter;

/**
 * TreeTable configuration structure. Used to initialize a new
 * TreeTable with specific attributes.
 */
typedef struct nut_treetable_conf_s {
    int    (*cmp)         (const void *k1, const void *k2);
    void  *(*mem_alloc)   (size_t size);
    void  *(*mem_calloc)  (size_t blocks, size_t size);
    void   (*mem_free)    (void *block);
} TreeTableConf;


void          nut_treetable_conf_init        (TreeTableConf *conf);
NutState  nut_treetable_new              (int (*cmp) (const void*, const void*), TreeTable **tt);
NutState  nut_treetable_new_conf         (TreeTableConf const * const conf, TreeTable **tt);

void          nut_treetable_destroy          (TreeTable *table);
NutState  nut_treetable_add              (TreeTable *table, void *key, void *val);

NutState  nut_treetable_remove           (TreeTable *table, void *key, void **out);
void          nut_treetable_remove_all       (TreeTable *table);
NutState  nut_treetable_remove_first     (TreeTable *table, void **out);
NutState  nut_treetable_remove_last      (TreeTable *table, void **out);

NutState  nut_treetable_get              (TreeTable const * const table, const void *key, void **out);
NutState  nut_treetable_get_first_value  (TreeTable const * const table, void **out);
NutState  nut_treetable_get_first_key    (TreeTable const * const table, void **out);
NutState  nut_treetable_get_last_value   (TreeTable const * const table, void **out);
NutState  nut_treetable_get_last_key     (TreeTable const * const table, void **out);
NutState  nut_treetable_get_greater_than (TreeTable const * const table, const void *key, void **out);
NutState  nut_treetable_get_lesser_than  (TreeTable const * const table, const void *key, void **out);

size_t        nut_treetable_size             (TreeTable const * const table);
bool          nut_treetable_contains_key     (TreeTable const * const table, const void *key);
size_t        nut_treetable_contains_value   (TreeTable const * const table, const void *value);

void          nut_treetable_foreach_key      (TreeTable *table, void (*op) (const void*));
void          nut_treetable_foreach_value    (TreeTable *table, void (*op) (void*));

void          nut_treetable_iter_init        (TreeTableIter *iter, TreeTable *table);
NutState  nut_treetable_iter_next        (TreeTableIter *iter, TreeTableEntry *entry);
NutState  nut_treetable_iter_remove      (TreeTableIter *iter, void **out);


#define TREETABLE_FOREACH(entry, treetable, body)                       \
    {                                                                   \
        TreetableIter nut_treetable_iter_53d46d2a04458e7b;                  \
        nut_treetable_iter_init(&nut_treetable_iter_53d46d2a04458e7b, treetable); \
        TreeTableEntry *val;                                            \
        while (nut_treetable_iter_next(&nut_treetable_iter_53d46d2a04458e7b, &entry) != CC_ITER_END) \
            body                                                        \
                }


#ifdef DEBUG
#define RB_ERROR_CONSECUTIVE_RED 0
#define RB_ERROR_BLACK_HEIGHT    1
#define RB_ERROR_TREE_STRUCTURE  2
#define RB_ERROR_OK              4

int nut_treetable_assert_rb_rules(TreeTable *table);
#endif /* DEBUG */

#ifdef __cplusplus
}
#endif

#endif /* COLLECTIONS_C_TREETABLE_H */
