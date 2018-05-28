#include "nutconf.h"
#include "nutport.h"
#include "nutmem.h"


#include "nuthashset.h"

struct nut_hashset_s {
    HashTable *table;
    int       *dummy;

    void *(*mem_alloc)  (size_t size);
    void *(*mem_calloc) (size_t blocks, size_t size);
    void  (*mem_free)   (void *block);
};

/**
 * Initializes the fields of the HashSetConf struct to default values.
 *
 * @param[in, out] conf the configuration struct that is being initialized
 */
void nut_hashset_conf_init(HashSetConf *conf)
{
    nut_hashtable_conf_init(conf);
}

/**
 * Creates a new HashSet and returns a status code.
 *
 * @note The newly created HashSet will be a set of strings.
 *
 * @return NUT_OK if the creation was successful, or CC_ERR_ALLOC if the memory
 * allocation for the new HashSet failed.
 */
NutState nut_hashset_new(HashSet **hs)
{
    HashSetConf hsc;
    nut_hashset_conf_init(&hsc);
    return nut_hashset_new_conf(&hsc, hs);
}

/**
 * Creates a new empty HashSet based on the specified HashSetConf struct and
 * returns a status code.
 *
 * The HashSet is allocated using the allocators specified in the HashSetConf
 * object. The allocation may fail if the underlying allocator fails.
 *
 * @param[in] conf The hashset configuration object. All fields must be initialized.
 * @param[out] out Pointer to where the newly created HashSet is stored
 *
 * @return NUT_OK if the creation was successful, or CC_ERR_ALLOC if the memory
 * allocation for the new HashSet structure failed.
 */
NutState nut_hashset_new_conf(HashSetConf const * const conf, HashSet **hs)
{
    HashSet *set = conf->mem_calloc(1, sizeof(HashSet));

    if (!set)
        return NUT_ERR_MALLOC;

    HashTable *table;
    NutState stat = nut_hashtable_new_conf(conf, &table);

    if (stat != NUT_OK) {
        conf->mem_free(set);
        return stat;
    }

    set->table      = table;
    set->mem_alloc  = conf->mem_alloc;
    set->mem_calloc = conf->mem_calloc;
    set->mem_free   = conf->mem_free;

    /* A dummy pointer that is never actually dereferenced
    *  that must not be null.*/
    set->dummy = (int*) 1;
    *hs = set;
    return NUT_OK;
}

/**
 * Destroys the specified HashSet structure without destroying the data
 * it holds.
 *
 * @param[in] table HashSet to be destroyed.
 */
void nut_hashset_destroy(HashSet *set)
{
    nut_hashtable_destroy(set->table);
    set->mem_free(set);
}

/**
 * Adds a new element to the HashSet.
 *
 * @param[in] set the set to which the element is being added
 * @param[in] element the element being added
 *
 * @return NUT_OK if the element was successfully added, or CC_ERR_ALLOC
 * if the memory allocation failed.
 */
NutState nut_hashset_add(HashSet *set, void *element)
{
    return nut_hashtable_add(set->table, element, set->dummy);
}

/**
 * Removes the specified element from the HashSet and sets the out
 * parameter to its value.
 *
 * @param[in] set the set from which the element is being removed
 * @param[in] element the element being removed
 * @param[out] out Pointer to where the removed value is stored, or NULL
 *                 if it is to be ignored
 *
 * @return NUT_OK if the element was successfully removed, or CC_ERR_VALUE_NOT_FOUND
 * if the value was not found.
 */
NutState nut_hashset_remove(HashSet *set, void *element, void **out)
{
    return nut_hashtable_remove(set->table, element, out);
}

/**
 * Removes all elements from the specified set.
 *
 * @param set the set from which all elements are being removed
 */
void nut_hashset_remove_all(HashSet *set)
{
    nut_hashtable_remove_all(set->table);
}

/**
 * Checks whether an element is a part of the specified set.
 *
 * @param[in] set the set being searched for the specified element
 * @param[in] element the element being searched for
 *
 * @return true if the specified element is an element of the set
 */
bool nut_hashset_contains(HashSet *set, void *element)
{
    return nut_hashtable_contains_key(set->table, element);
}

/**
 * Returns the size of the specified set.
 *
 * @param[in] set the set whose size is being returned
 *
 * @return the size of the set
 */
size_t nut_hashset_size(HashSet *set)
{
    return nut_hashtable_size(set->table);
}

/**
 * Returns the capacity of the specified set.
 *
 * @param[in] set the set whose capacity is being returned
 *
 * @return the capacity of the set
 */
size_t nut_hashset_capacity(HashSet *set)
{
    return nut_hashtable_capacity(set->table);
}

/**
 * Applies the function fn to each element of the HashSet.
 *
 * @param[in] set the set on which this operation is being performed
 * @param[in] fn the operation function that is invoked on each element of the
 *               set
 */
void nut_hashset_foreach(HashSet *set, void (*fn) (const void *e))
{
    nut_hashtable_foreach_key(set->table, fn);
}

/**
 * Initializes the set iterator.
 *
 * @param[in] iter the iterator that is being initialized
 * @param[in] set the set on which this iterator will operate
 */
void nut_hashset_iter_init(HashSetIter *iter, HashSet *set)
{
    nut_hashtable_iter_init(&(iter->iter), set->table);
}

/**
 * Advances the iterator and sets the out parameter to the value of the
 * next element.
 *
 * @param[in] iter the iterator that is being advanced
 * @param[out] out Pointer to where the next element is set
 *
 * @return NUT_OK if the iterator was advanced, or CC_ITER_END if the
 * end of the HashSet has been reached.
 */
NutState nut_hashset_iter_next(HashSetIter *iter, void **out)
{
    TableEntry *entry;
    NutState status = nut_hashtable_iter_next(&(iter->iter), &entry);

    if (status != NUT_OK)
        return status;

    if (out)
        *out = entry->key;

    return NUT_OK;
}

/**
 * Removes the last returned entry by <code>nut_hashset_iter_next()</code>
 * function without invalidating the iterator and optionally sets the
 * out parameter to the value of the removed element.
 *
 * @note This Function should only ever be called after a call to <code>
 * nut_hashset_iter_next()</code>.
 *
 * @param[in] iter The iterator on which this operation is performed
 * @param[out] out Pointer to where the removed element is stored, or NULL
 *                 if it is to be ignored
 *
 * @return NUT_OK if the entry was successfully removed, or
 * CC_ERR_VALUE_NOT_FOUND.
 */
NutState nut_hashset_iter_remove(HashSetIter *iter, void **out)
{
    return nut_hashtable_iter_remove(&(iter->iter), out);
}
