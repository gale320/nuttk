
#include "nuttreeset.h"

struct nut_treeset_s {
    TreeTable *t;
    int       *dummy;

    void *(*mem_alloc)  (size_t size);
    void *(*mem_calloc) (size_t blocks, size_t size);
    void  (*mem_free)   (void *block);
};

/**
 * Initializes the fields of the TreeSetConf struct to default values.
 *
 * @param[in, out] conf the configuration struct that is being initialized
 */
void nut_treeset_conf_init(TreeSetConf *conf)
{
    nut_treetable_conf_init(conf);
}

/**
 * Creates a new TreeSet and returns a status code.
 *
 * @param[in] cmp the comparator function used to order elements
 * @param[out] out pointer to where the newly created TreeSet is to be stored
 *
 * @return  NUT_OK if the creation was successful, or NUT_ERR_ALLOC if the memory
 * allocation for the new TreeSet failed.
 */
NutState nut_treeset_new(int (*cmp) (const void*, const void*), TreeSet **set)
{
    TreeSetConf conf;
    nut_treeset_conf_init(&conf);
    conf.cmp = cmp;
    return nut_treeset_new_conf(&conf, set);
}

/**
 * Creates a new TreeSet based on the specified TreeSetConf struct and returns
 * a status code.
 *
 * The TreeSet is allocated using the allocators specified in the TreeSetConf
 * object. The allocation may fail if the underlying allocator fails.
 *
 * @param[in] conf  TreeSet configuration struct. All fields must be initialized.
 * @param[out] out Pointer to where the newly created TreeSet is stored
 *
 * @return NUT_OK if the creation was successful, or NUT_ERR_ALLOC if the memory
 * allocation for the new TreeSet structure failed.
 */
NutState nut_treeset_new_conf(TreeSetConf const * const conf, TreeSet **tset)
{
    TreeSet *set = conf->mem_calloc(1, sizeof(TreeSet));

    if (!set)
        return NUT_ERR_MALLOC;

    TreeTable *table;
    NutState s = nut_treetable_new_conf(conf, &table);

    if (s != NUT_OK) {
        conf->mem_free(set);
        return s;
    }
    set->t          = table;
    set->dummy      = (int*) 1;
    set->mem_alloc  = conf->mem_alloc;
    set->mem_calloc = conf->mem_calloc;
    set->mem_free   = conf->mem_free;

    *tset = set;
    return NUT_OK;
}

/**
 * Destroys the specified TreeSet.
 *
 * @param[in] set the TreeSet to be destroyed
 */
void nut_treeset_destroy(TreeSet *set)
{
    nut_treetable_destroy(set->t);
    set->mem_free(set);
}

/**
 * Adds a new element to the set.
 *
 * @param[in] set the set to which the element is being added
 * @param[in] element the element being added
 *
 * @return NUT_OK if the operation was successful, or NUT_ERR_ALLOC if the
 * memory allocation for the new element failed.
 */
NutState nut_treeset_add(TreeSet *set, void *element)
{
    return nut_treetable_add(set->t, element, set->dummy);
}

/**
 * Removes the specified element from the TreeSet and sets the out
 * parameter to its value.
 *
 * @param[in] set the set from which the element is being removed
 * @param[in] element the element being removed
 * @param[out] out pointer to where the removed value is stored, or NULL
 *                 if it is to be ignored
 *
 * @return NUT_OK if the mapping was successfully removed, or NUT_ERR_VALUE_NOT_FOUND
 * if the value was not found.
 */
NutState nut_treeset_remove(TreeSet *set, void *element, void **out)
{
    if (nut_treetable_remove(set->t, element, out) == NUT_ERR_KEY_NOT_FOUND)
        return NUT_ERR_VALUE_NOT_FOUND;

    return NUT_OK;
}

/**
 * Removes all elements from the specified set.
 *
 * @param set the set from which all elements are being removed
 */
void nut_treeset_remove_all(TreeSet *set)
{
    nut_treetable_remove_all(set->t);
}

/**
 * Returns the first element of the set.
 *
 * @param[in] set the set from which the first element is being returned
 * @param[out] out pointer to where the returned element is stored
 *
 * @return NUT_OK if the element was found, or NUT_ERR_VALUE_NOT_FOUND if not.
 */
NutState nut_treeset_get_first(TreeSet *set, void **out)
{
    if (nut_treetable_get_first_key(set->t, out) == NUT_ERR_KEY_NOT_FOUND)
        return NUT_ERR_VALUE_NOT_FOUND;

    return NUT_OK;
}

/**
 * Returns the last (highest) element of the set.
 *
 * @param[in] set the set from which the last element is being returned
 * @param[out] out pointer to where the returned element is stored
 *
 * @return NUT_OK if the element was found, or NUT_ERR_VALUE_NOT_FOUND if not.
 */
NutState nut_treeset_get_last(TreeSet *set, void **out)
{
    if (nut_treetable_get_last_key(set->t, out) == NUT_ERR_KEY_NOT_FOUND)
        return NUT_ERR_VALUE_NOT_FOUND;

    return NUT_OK;
}

/**
 * Returns the immediate successor of the specified element.
 *
 * @param[in] set the set on which this operation is performed
 * @param[in] element the element whose successor is being returned
 * @param[out] out pointer to where the returned element is stored
 *
 * @return NUT_OK if the element was found, or NUT_ERR_VALUE_NOT_FOUND if not.
 */
NutState nut_treeset_get_greater_than(TreeSet *set, void *element, void **out)
{
    if (nut_treetable_get_greater_than(set->t, element, out) == NUT_ERR_KEY_NOT_FOUND)
        return NUT_ERR_VALUE_NOT_FOUND;

    return NUT_OK;
}

/**
 * Returns the immediate predecessor of the specified element.
 *
 * @param[in] set the set on which this operation is performed
 * @param[in] element the element whose predecessor is being returned
 * @param[out] out pointer to where the returned element is stored
 *
 * @return NUT_OK if the element was found, or NUT_ERR_VALUE_NOT_FOUND if not.
 */
NutState nut_treeset_get_lesser_than(TreeSet *set, void *element, void **out)
{
    if (nut_treetable_get_lesser_than(set->t, element, out) == NUT_ERR_KEY_NOT_FOUND)
        return NUT_ERR_VALUE_NOT_FOUND;

    return NUT_OK;
}

/**
 * Checks whether an element is a part of the specified set.
 *
 * @param[in] set the set being searched for the specified element
 * @param[in] element the element being searched for
 *
 * @return true if the specified element is an element of the set.
 */
bool nut_treeset_contains(TreeSet *set, void *element)
{
    return nut_treetable_contains_key(set->t, element);
}

/**
 * Returns the size of the specified set.
 *
 * @param[in] set the set whose size is being returned
 *
 * @return the size of the set.
 */
size_t nut_treeset_size(TreeSet *set)
{
    return nut_treetable_size(set->t);
}

/**
 * Applies the function fn to each element of the TreeSet.
 *
 * @param[in] set the set on which this operation is being performed
 * @param[in] fn the operation function that is invoked on each element
 *               of the set
 */
void nut_treeset_foreach(TreeSet *set, void (*fn) (const void*))
{
    nut_treetable_foreach_key(set->t, fn);
}

/**
 * Initializes the set iterator.
 *
 * @param[in] iter the iterator that is being initialized
 * @param[in] set the set on which this iterator will operate
 */
void nut_treeset_iter_init(TreeSetIter *iter, TreeSet *set)
{
    nut_treetable_iter_init(&(iter->i), set->t);
}

/**
 * Advances the iterator and sets the out parameter to the value of the
 * next element.
 *
 * @param[in] iter the iterator that is being advanced
 * @param[out] element pointer to where the next element is set
 *
 * @return NUT_OK if the iterator was advanced, or NUT_ITER_END if the
 * end of the TreeSet has been reached.
 */
NutState nut_treeset_iter_next(TreeSetIter *iter, void **element)
{
    TreeTableEntry entry;

    if (nut_treetable_iter_next(&(iter->i), &entry) != NUT_OK)
        return NUT_ITER_END;

    *element = entry.key;
    return NUT_OK;
}

/**
 * Removes the last returned element by <code>nut_treeset_iter_next()</code>
 * function without invalidating the iterator and optionally sets the
 * out parameter to the value of the removed element.
 *
 * @note This Function should only ever be called after a call to <code>
 * nut_treeset_iter_next()</code>.
 *
 * @param[in] iter the iterator on which this operation is performed
 * @param[out] out pointer to where the removed element is stored, or NULL
 *                 if it is to be ignored
 *
 * @return NUT_OK if the element was successfully removed, or
 * NUT_ERR_KEY_NOT_FOUND.
 */
NutState nut_treeset_iter_remove(TreeSetIter *iter, void **out)
{
    return nut_treetable_iter_remove(&(iter->i), out);
}
