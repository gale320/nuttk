#include "nutconf.h"
#include "nutport.h"
#include "nutmem.h"
#include "nutarray.h"

#define DEFAULT_CAPACITY 8
#define DEFAULT_EXPANSION_FACTOR 2

struct nut_array_s {
    size_t   size;
    size_t   capacity;
    float    exp_factor;
    void   **buffer;

    /*
    void *(*mem_alloc)  (size_t size);
    void *(*mem_calloc) (size_t blocks, size_t size);
    void  (*mem_free)   (void *block);
     */
};

static NutState expand_capacity(Array *ar);


/**
 * Creates a new empty array and returns a status code.
 *
 * @param[out] out pointer to where the newly created Array is to be stored
 *
 * @return NUT_OK if the creation was successful, or NUT_ERR_MALLOC if the
 * memory allocation for the new Array structure failed.
 */
NutState nut_array_new(Array **out)
{
    ArrayConf c;
    nut_array_conf_init(&c);
    return nut_array_new_conf(&c, out);
}

/**
 * Creates a new empty Array based on the specified ArrayConf struct and
 * returns a status code.
 *
 * The Array is allocated using the allocators specified in the ArrayConf
 * struct. The allocation may fail if underlying allocator fails. It may also
 * fail if the values of exp_factor and capacity in the ArrayConf do not meet
 * the following condition: <code>exp_factor < (CC_MAX_ELEMENTS / capacity)</code>.
 *
 * @param[in] conf array configuration structure
 * @param[out] out pointer to where the newly created Array is to be stored
 *
 * @return NUT_OK if the creation was successful, NUT_ERR_INVALID_CAPACITY if
 * the above mentioned condition is not met, or NUT_ERR_MALLOC if the memory
 * allocation for the new Array structure failed.
 */
NutState nut_array_new_conf(ArrayConf const * const conf, Array **out)
{
    float ex;

    /* The expansion factor must be greater than one for the
     * array to grow */
    if (conf->exp_factor <= 1)
        ex = DEFAULT_EXPANSION_FACTOR;
    else
        ex = conf->exp_factor;

    /* Needed to avoid an integer overflow on the first resize and
     * to easily check for any future overflows. */
    if (!conf->capacity || ex >= NUT_MAX_ELEMENTS / conf->capacity)
        return NUT_ERR_INVALID_CAPACITY;

    Array *ar = nut_mem_calloc(1, sizeof(Array));

    if (!ar)
        return NUT_ERR_MALLOC;

    void **buff = nut_mem_alloc(conf->capacity * sizeof(void*));

    if (!buff) {
        conf->mem_free(ar);
        return NUT_ERR_MALLOC;
    }

    ar->buffer     = buff;
    ar->exp_factor = ex;
 /*
    ar->capacity   = conf->capacity;
    ar->mem_alloc  = conf->mem_alloc;
    ar->mem_calloc = conf->mem_calloc;
    ar->mem_free   = conf->mem_free;
*/
    *out = ar;
    return NUT_OK;
}

/**
 * Initializes the fields of the ArrayConf struct to default values.
 *
 * @param[in, out] conf ArrayConf structure that is being initialized
 */
void nut_array_conf_init(ArrayConf *conf)
{
    conf->exp_factor = DEFAULT_EXPANSION_FACTOR;
    conf->capacity   = DEFAULT_CAPACITY;
    conf->mem_alloc  = &nut_mem_malloc;
    conf->mem_calloc = &nut_mem_calloc;
    conf->mem_free   = &nut_mem_free;
}

/**
 * Destroys the Array structure, but leaves the data it used to hold intact.
 *
 * @param[in] ar the array that is to be destroyed
 */
void nut_array_destroy(Array *ar)
{
    nut_mem_free(ar->buffer);
    nut_mem_free(ar);
}

/**
 * Destroys the Array structure along with all the data it holds.
 *
 * @note
 * This function should not be called on a array that has some of its elements
 * allocated on the stack.
 *
 * @param[in] ar the array that is being destroyed
 */
void nut_array_destroy_cb(Array *ar, void (*cb) (void*))
{
    size_t i;
    for (i = 0; i < ar->size; i++)
        cb(ar->buffer[i]);

    nut_array_destroy(ar);
}

/**
 * Adds a new element to the Array. The element is appended to the array making
 * it the last element (the one with the highest index) of the Array.
 *
 * @param[in] ar the array to which the element is being added
 * @param[in] element the element that is being added
 *
 * @return NUT_OK if the element was successfully added, NUT_ERR_MALLOC if the
 * memory allocation for the new element failed, or CC_ERR_MAX_CAPACITY if the
 * array is already at maximum capacity.
 */
NutState nut_array_add(Array *ar, void *element)
{
    if (ar->size >= ar->capacity) {
        NutState status = expand_capacity(ar);
        if (status != NUT_OK)
            return status;
    }

    ar->buffer[ar->size] = element;
    ar->size++;

    return NUT_OK;
}

/**
 * Adds a new element to the array at a specified position by shifting all
 * subsequent elements by one. The specified index must be within the bounds
 * of the array. This function may also fail if the memory allocation for
 * the new element was unsuccessful.
 *
 * @param[in] ar the array to which the element is being added
 * @param[in] element the element that is being added
 * @param[in] index the position in the array at which the element is being
 *            added
 *
 * @return NUT_OK if the element was successfully added, NUT_ERR_OUT_RANGE if
 * the specified index was not in range, NUT_ERR_MALLOC if the memory
 * allocation for the new element failed, or CC_ERR_MAX_CAPACITY if the
 * array is already at maximum capacity.
 */
NutState nut_array_add_at(Array *ar, void *element, size_t index)
{
    if (index == ar->size)
        return nut_array_add(ar, element);

    if ((ar->size == 0 && index != 0) || index > (ar->size - 1))
        return NUT_ERR_OUT_RANGE;

    if (ar->size >= ar->capacity) {
        NutState status = expand_capacity(ar);
        if (status != NUT_OK)
            return status;
    }

    size_t shift = (ar->size - index) * sizeof(void*);

    memmove(&(ar->buffer[index + 1]),
            &(ar->buffer[index]),
            shift);

    ar->buffer[index] = element;
    ar->size++;

    return NUT_OK;
}

/**
 * Replaces an array element at the specified index and optionally sets the out
 * parameter to the value of the replaced element. The specified index must be
 * within the bounds of the Array.
 *
 * @param[in]  ar      array whose element is being replaced
 * @param[in]  element replacement element
 * @param[in]  index   index at which the replacement element should be inserted
 * @param[out] out     pointer to where the replaced element is stored, or NULL if
 *                     it is to be ignored
 *
 * @return NUT_OK if the element was successfully replaced, or NUT_ERR_OUT_RANGE
 *         if the index was out of range.
 */
NutState nut_array_replace_at(Array *ar, void *element, size_t index, void **out)
{
    if (index >= ar->size)
        return NUT_ERR_OUT_RANGE;

    if (out)
        *out = ar->buffer[index];

    ar->buffer[index] = element;

    return NUT_OK;
}

NutState nut_array_swap_at(Array *ar, size_t index1, size_t index2)
{
    void *tmp;
    if(index1 >= ar->size || index2 >= ar->size)
        return NUT_ERR_OUT_RANGE;
    tmp = ar->buffer[index1];
    ar->buffer[index1] = ar->buffer[index2];
    ar->buffer[index2] = tmp;
    return NUT_OK;
}

/**
 * Removes the specified element from the Array if such element exists and
 * optionally sets the out parameter to the value of the removed element.
 *
 * @param[in] ar array from which the element is being removed
 * @param[in] element element being removed
 * @param[out] out pointer to where the removed value is stored, or NULL
 *                 if it is to be ignored
 *
 * @return NUT_OK if the element was successfully removed, or
 * NUT_ERR_NOT_FIND if the element was not found.
 */
NutState nut_array_remove(Array *ar, void *element, void **out)
{
    size_t index;
    NutState status = nut_array_index_of(ar, element, &index);

    if (status == NUT_ERR_OUT_RANGE)
        return NUT_ERR_NOT_FIND;

    if (index != ar->size - 1) {
        size_t block_size = (ar->size - index) * sizeof(void*);

        memmove(&(ar->buffer[index]),
                &(ar->buffer[index + 1]),
                block_size);
    }
    ar->size--;

    if (out)
        *out = element;

    return NUT_OK;
}

/**
 * Removes an Array element from the specified index and optionally sets the
 * out parameter to the value of the removed element. The index must be within
 * the bounds of the array.
 *
 * @param[in] ar the array from which the element is being removed
 * @param[in] index the index of the element being removed.
 * @param[out] out  pointer to where the removed value is stored,
 *                  or NULL if it is to be ignored
 *
 * @return NUT_OK if the element was successfully removed, or NUT_ERR_OUT_RANGE
 * if the index was out of range.
 */
NutState nut_array_remove_at(Array *ar, size_t index, void **out)
{
    if (index >= ar->size)
        return NUT_ERR_OUT_RANGE;

    if (out)
        *out = ar->buffer[index];

    if (index != ar->size - 1) {
        size_t block_size = (ar->size - index) * sizeof(void*);

        memmove(&(ar->buffer[index]),
                &(ar->buffer[index + 1]),
                block_size);
    }
    ar->size--;

    return NUT_OK;
}

/**
 * Removes an Array element from the end of the array and optionally sets the
 * out parameter to the value of the removed element.
 *
 * @param[in] ar the array whose last element is being removed
 * @param[out] out pointer to where the removed value is stored, or NULL if it is
 *                 to be ignored
 *
 * @return NUT_OK if the element was successfully removed, or NUT_ERR_OUT_RANGE
 * if the Array is already empty.
 */
NutState nut_array_remove_last(Array *ar, void **out)
{
    return nut_array_remove_at(ar, ar->size - 1, out);
}

/**
 * Removes all elements from the specified array. This function does not shrink
 * the array capacity.
 *
 * @param[in] ar array from which all elements are to be removed
 */
void nut_array_remove_all(Array *ar)
{
    ar->size = 0;
}

/**
 * Removes and frees all elements from the specified array. This function does
 * not shrink the array capacity.
 *
 * @param[in] ar array from which all elements are to be removed
 */
void nut_array_remove_all_free(Array *ar)
{
    size_t i;
    for (i = 0; i < ar->size; i++)
        nut_mem_free(ar->buffer[i]);

    nut_array_remove_all(ar);
}

/**
 * Gets an Array element from the specified index and sets the out parameter to
 * its value. The specified index must be within the bounds of the array.
 *
 * @param[in] ar the array from which the element is being retrieved
 * @param[in] index the index of the array element
 * @param[out] out pointer to where the element is stored
 *
 * @return NUT_OK if the element was found, or NUT_ERR_OUT_RANGE if the index
 * was out of range.
 */
NutState nut_array_get_at(Array *ar, size_t index, void **out)
{
    if (index >= ar->size)
        return NUT_ERR_OUT_RANGE;

    *out = ar->buffer[index];
    return NUT_OK;
}

/**
 * Gets the last element of the array or the element at the highest index
 * and sets the out parameter to its value.
 *
 * @param[in] ar the array whose last element is being returned
 * @param[out] out pointer to where the element is stored
 *
 * @return NUT_OK if the element was found, or NUT_ERR_NOT_FIND if the
 * Array is empty.
 */
NutState nut_array_get_last(Array *ar, void **out)
{
    if (ar->size == 0)
        return NUT_ERR_NOT_FIND;

    return nut_array_get_at(ar, ar->size - 1, out);
}

/**
 * Returns the underlying array buffer.
 *
 * @note Any direct modification of the buffer may invalidate the Array.
 *
 * @param[in] ar array whose underlying buffer is being returned
 *
 * @return array's internal buffer.
 */
const void * const* nut_array_get_buffer(Array *ar)
{
    return (const void* const*) ar->buffer;
}

/**
 * Gets the index of the specified element. The returned index is the index
 * of the first occurrence of the element starting from the beginning of the
 * Array.
 *
 * @param[in] ar array being searched
 * @param[in] element the element whose index is being looked up
 * @param[out] index  pointer to where the index is stored
 *
 * @return NUT_OK if the index was found, or CC_OUT_OF_RANGE if not.
 */
NutState nut_array_index_of(Array *ar, void *element, size_t *index)
{
    size_t i;
    for (i = 0; i < ar->size; i++) {
        if (ar->buffer[i] == element) {
            *index = i;
            return NUT_OK;
        }
    }
    return NUT_ERR_OUT_RANGE;
}

/**
 * Creates a subarray of the specified Array, ranging from <code>b</code>
 * index (inclusive) to <code>e</code> index (inclusive). The range indices
 * must be within the bounds of the Array, while the <code>e</code> index
 * must be greater or equal to the <code>b</code> index.
 *
 * @note The new Array is allocated using the original Array's allocators
 *       and it also inherits the configuration of the original Array.
 *
 * @param[in] ar array from which the subarray is being created
 * @param[in] b the beginning index (inclusive) of the subarray that must be
 *              within the bounds of the array and must not exceed the
 *              the end index
 * @param[in] e the end index (inclusive) of the subarray that must be within
 *              the bounds of the array and must be greater or equal to the
 *              beginning index
 * @param[out] out pointer to where the new sublist is stored
 *
 * @return NUT_OK if the subarray was successfully created, CC_ERR_INVALID_RANGE
 * if the specified index range is invalid, or NUT_ERR_MALLOC if the memory allocation
 * for the new subarray failed.
 */
NutState nut_array_subarray(Array *ar, size_t b, size_t e, Array **out)
{
    if (b > e || e >= ar->size)
        return NUT_ERR_INVALID_RANGE;

    Array *sub_ar = nut_mem_calloc(1, sizeof(Array));

    if (!sub_ar)
        return NUT_ERR_MALLOC;

    /* Try to allocate the buffer */
    if (!(sub_ar->buffer = nut_mem_alloc(ar->capacity * sizeof(void*)))) {
        nut_mem_free(sub_ar);
        return NUT_ERR_MALLOC;
    }
/*
    sub_ar->mem_alloc  = ar->mem_alloc;
    sub_ar->mem_calloc = ar->mem_calloc;
    sub_ar->mem_free   = ar->mem_free;
    */
    sub_ar->size       = e - b + 1;
    sub_ar->capacity   = sub_ar->size;

    memcpy(sub_ar->buffer,
           &(ar->buffer[b]),
           sub_ar->size * sizeof(void*));

    *out = sub_ar;
    return NUT_OK;
}

/**
 * Creates a shallow copy of the specified Array. A shallow copy is a copy of
 * the Array structure, but not the elements it holds.
 *
 * @note The new Array is allocated using the original Array's allocators
 *       and it also inherits the configuration of the original array.
 *
 * @param[in] ar the array to be copied
 * @param[out] out pointer to where the newly created copy is stored
 *
 * @return NUT_OK if the copy was successfully created, or NUT_ERR_MALLOC if the
 * memory allocation for the copy failed.
 */
NutState nut_array_copy_shallow(Array *ar, Array **out)
{
    Array *copy = nut_mem_alloc(sizeof(Array));

    if (!copy)
        return NUT_ERR_MALLOC;

    if (!(copy->buffer = nut_mem_calloc(ar->capacity, sizeof(void*)))) {
        nut_mem_free(copy);
        return NUT_ERR_MALLOC;
    }
    copy->exp_factor = ar->exp_factor;
    copy->size       = ar->size;
    copy->capacity   = ar->capacity;
    /*
    copy->mem_alloc  = ar->mem_alloc;
    copy->mem_calloc = ar->mem_calloc;
    copy->mem_free   = ar->mem_free;
*/
    memcpy(copy->buffer,
           ar->buffer,
           copy->size * sizeof(void*));

    *out = copy;
    return NUT_OK;
}

/**
 * Creates a deep copy of the specified Array. A deep copy is a copy of
 * both the Array structure and the data it holds.
 *
 * @note The new Array is allocated using the original Array's allocators
 *       and it also inherits the configuration of the original Array.
 *
 * @param[in] ar   array to be copied
 * @param[in] cp   the copy function that should return a pointer to the copy of
 *                 the data
 * @param[out] out pointer to where the newly created copy is stored
 *
 * @return NUT_OK if the copy was successfully created, or NUT_ERR_MALLOC if the
 * memory allocation for the copy failed.
 */
NutState nut_array_copy_deep(Array *ar, void *(*cp) (void *), Array **out)
{
    Array *copy = nut_mem_alloc(sizeof(Array));

    if (!copy)
        return NUT_ERR_MALLOC;

    if (!(copy->buffer = nut_mem_calloc(ar->capacity, sizeof(void*)))) {
        nut_mem_free(copy);
        return NUT_ERR_MALLOC;
    }

    copy->exp_factor = ar->exp_factor;
    copy->size       = ar->size;
    copy->capacity   = ar->capacity;
    /*
    copy->mem_alloc  = ar->mem_alloc;
    copy->mem_calloc = ar->mem_calloc;
    copy->mem_free   = ar->mem_free;
*/
    size_t i;
    for (i = 0; i < copy->size; i++)
        copy->buffer[i] = cp(ar->buffer[i]);

    *out = copy;

    return NUT_OK;
}

/**
 * Filters the Array by modifying it. It removes all elements that don't
 * return true on pred(element).
 *
 * @param[in] ar   array that is to be filtered
 * @param[in] pred predicate function which returns true if the element should
 *                 be kept in the Array
 *
 * @return NUT_OK if the Array was filtered successfully, or NUT_ERR_OUT_RANGE
 * if the Array is empty.
 */
NutState nut_array_filter_mut(Array *ar, bool (*pred) (const void*))
{
    if (ar->size == 0)
        return NUT_ERR_OUT_RANGE;

    size_t rm   = 0;
    size_t keep = 0;

    /* Look for clusters of non matching elements before moving
     * in order to minimize the number of memmoves */
    for (size_t i = ar->size - 1; i != ((size_t) - 1); i--) {
        if (!pred(ar->buffer[i])) {
            rm++;
            continue;
        }
        if (rm > 0) {
            if (keep > 0) {
                size_t block_size = keep * sizeof(void*);
                memmove(&(ar->buffer[i + 1]),
                        &(ar->buffer[i + 1 + rm]),
                        block_size);
            }
            ar->size -= rm;
            rm = 0;
        }
        keep++;
    }
    /* Remove any remaining elements*/
    if (rm > 0) {
        size_t block_size = keep * sizeof(void*);
        memmove(&(ar->buffer[0]),
                &(ar->buffer[rm]),
                block_size);

        ar->size -= rm;
    }
    return NUT_OK;
}

/**
 * Filters the Array by creating a new Array that contains all elements from the
 * original Array that return true on pred(element) without modifying the original
 * Array.
 *
 * @param[in] ar   array that is to be filtered
 * @param[in] pred predicate function which returns true if the element should
 *                 be kept in the filtered array
 * @param[out] out pointer to where the new filtered Array is to be stored
 *
 * @return NUT_OK if the Array was filtered successfully, NUT_ERR_OUT_RANGE
 * if the Array is empty, or NUT_ERR_MALLOC if the memory allocation for the
 * new Array failed.
 */
NutState nut_array_filter(Array *ar, bool (*pred) (const void*), Array **out)
{
    if (ar->size == 0)
        return NUT_ERR_OUT_RANGE;

    Array *filtered = nut_mem_alloc(sizeof(Array));

    if (!filtered)
        return NUT_ERR_MALLOC;

    if (!(filtered->buffer = nut_mem_calloc(ar->capacity, sizeof(void*)))) {
        nut_mem_free(filtered);
        return NUT_ERR_MALLOC;
    }

    filtered->exp_factor = ar->exp_factor;
    filtered->size       = 0;
    filtered->capacity   = ar->capacity;
/*
    filtered->mem_alloc  = ar->mem_alloc;
    filtered->mem_calloc = ar->mem_calloc;
    filtered->mem_free   = ar->mem_free;
*/
    size_t f = 0;
    for (size_t i = 0; i < ar->size; i++) {
        if (pred(ar->buffer[i])) {
            filtered->buffer[f++] = ar->buffer[i];
            filtered->size++;
        }
    }
    *out = filtered;

    return NUT_OK;
}

/**
 * Reverses the order of elements in the specified array.
 *
 * @param[in] ar array that is being reversed
 */
void nut_array_reverse(Array *ar)
{
    size_t i;
    size_t j;
    for (i = 0, j = ar->size - 1; i < (ar->size - 1) / 2; i++, j--) {
        void *tmp = ar->buffer[i];
        ar->buffer[i] = ar->buffer[j];
        ar->buffer[j] = tmp;
    }
}

/**
 * Trims the array's capacity, in other words, it shrinks the capacity to match
 * the number of elements in the Array, however the capacity will never shrink
 * below 1.
 *
 * @param[in] ar array whose capacity is being trimmed
 *
 * @return NUT_OK if the capacity was trimmed successfully, or NUT_ERR_MALLOC if
 * the reallocation failed.
 */
NutState nut_array_trim_capacity(Array *ar)
{
    if (ar->size == ar->capacity)
        return NUT_OK;

    void **new_buff = nut_mem_calloc(ar->size, sizeof(void*));

    if (!new_buff)
        return NUT_ERR_MALLOC;

    size_t size = ar->size < 1 ? 1 : ar->size;

    memcpy(new_buff, ar->buffer, size * sizeof(void*));
    nut_mem_free(ar->buffer);

    ar->buffer   = new_buff;
    ar->capacity = ar->size;

    return NUT_OK;
}

/**
 * Returns the number of occurrences of the element within the specified Array.
 *
 * @param[in] ar array that is being searched
 * @param[in] element the element that is being searched for
 *
 * @return the number of occurrences of the element.
 */
size_t nut_array_contains(Array *ar, void *element)
{
    return nut_array_contains_value(ar, element, nut_common_cmp_ptr);
}

/**
 * Returns the number of occurrences of the value pointed to by <code>e</code>
 * within the specified Array.
 *
 * @param[in] ar array that is being searched
 * @param[in] element the element that is being searched for
 * @param[in] cmp comparator function which returns 0 if the values passed to it are equal
 *
 * @return the number of occurrences of the value.
 */
size_t nut_array_contains_value(Array *ar, void *element, int (*cmp) (const void*, const void*))
{
    size_t o = 0;
    size_t i;
    for (i = 0; i < ar->size; i++) {
        if (cmp(element, ar->buffer[i]) == 0)
            o++;
    }
    return o;
}

/**
 * Returns the size of the specified Array. The size of the array is the
 * number of elements contained within the Array.
 *
 * @param[in] ar array whose size is being returned
 *
 * @return the the number of element within the Array.
 */
size_t nut_array_size(Array *ar)
{
    return ar->size;
}

/**
 * Returns the capacity of the specified Array. The capacity of the Array is
 * the maximum number of elements an Array can hold before it has to be resized.
 *
 * @param[in] ar array whose capacity is being returned
 *
 * @return the capacity of the Array.
 */
size_t nut_array_capacity(Array *ar)
{
    return ar->capacity;
}

/**
 * Sorts the specified array.
 *
 * @note
 * Pointers passed to the comparator function will be pointers to the array
 * elements that are of type (void*) ie. void**. So an extra step of
 * dereferencing will be required before the data can be used for comparison:
 * eg. <code>my_type e = *(*((my_type**) ptr));</code>.
 *
 * @code
 * enum cc_stat mycmp(const void *e1, const void *e2) {
 *     MyType el1 = *(*((enum cc_stat**) e1));
 *     MyType el2 = *(*((enum cc_stat**) e2));
 *
 *     if (el1 < el2) return -1;
 *     if (el1 > el2) return 1;
 *     return 0;
 * }
 *
 * ...
 *
 * nut_array_sort(array, mycmp);
 * @endcode
 *
 * @param[in] ar  array to be sorted
 * @param[in] cmp the comparator function that must be of type <code>
 *                enum cc_stat cmp(const void e1*, const void e2*)</code> that
 *                returns < 0 if the first element goes before the second,
 *                0 if the elements are equal and > 0 if the second goes
 *                before the first
 */
void nut_array_sort(Array *ar, int (*cmp) (const void*, const void*))
{
    qsort(ar->buffer, ar->size, sizeof(void*), cmp);
}

/**
 * Expands the Array capacity. This might fail if the the new buffer
 * cannot be allocated. In case the expansion would overflow the index
 * range, a maximum capacity buffer is allocated instead. If the capacity
 * is already at the maximum capacity, no new buffer is allocated.
 *
 * @param[in] ar array whose capacity is being expanded
 *
 * @return NUT_OK if the buffer was expanded successfully, NUT_ERR_MALLOC if
 * the memory allocation for the new buffer failed, or CC_ERR_MAX_CAPACITY
 * if the array is already at maximum capacity.
 */
static NutState expand_capacity(Array *ar)
{
    if (ar->capacity == NUT_MAX_ELEMENTS)
        return NUT_ERR_MAX_CAPACITY;

    size_t new_capacity = ar->capacity * ar->exp_factor;

    /* As long as the capacity is greater that the expansion factor
     * at the point of overflow, this is check is valid. */
    if (new_capacity <= ar->capacity)
        ar->capacity = NUT_MAX_ELEMENTS;
    else
        ar->capacity = new_capacity;

    void **new_buff = nut_mem_alloc(new_capacity * sizeof(void*));

    if (!new_buff)
        return NUT_ERR_MALLOC;

    memcpy(new_buff, ar->buffer, ar->size * sizeof(void*));

    nut_mem_free(ar->buffer);
    ar->buffer = new_buff;

    return NUT_OK;
}

/**
 * Applies the function fn to each element of the Array.
 *
 * @param[in] ar array on which this operation is performed
 * @param[in] fn operation function that is to be invoked on each Array
 *               element
 */
void nut_array_map(Array *ar, void (*fn) (void *e))
{
    size_t i;
    for (i = 0; i < ar->size; i++)
        fn(ar->buffer[i]);
}

/**
 * A fold/reduce function that collects all of the elements in the array
 * together. For example, if we have an array of [a,b,c...] the end result
 * will be (...((a+b)+c)+...).
 *
 * @param[in] ar the array on which this operation is performed
 * @param[in] fn the operation function that is to be invoked on each array
 *               element
 * @param[in] result the pointer which will collect the end result
 */
void nut_array_reduce(Array *ar, void (*fn) (void*, void*, void*), void *result)
{
    if (ar->size == 1) {
        fn(ar->buffer[0], NULL, result);
        return;
    }
    if (ar->size > 1)
        fn(ar->buffer[0], ar->buffer[1], result);

    for (size_t i = 2; i < ar->size; i++)
        fn(result, ar->buffer[i], result);
}

/**
 * Initializes the iterator.
 *
 * @param[in] iter the iterator that is being initialized
 * @param[in] ar the array to iterate over
 */
void nut_array_iter_init(ArrayIter *iter, Array *ar)
{
    iter->ar    = ar;
    iter->index = 0;
    iter->last_removed = false;
}

/**
 * Advances the iterator and sets the out parameter to the value of the
 * next element in the sequence.
 *
 * @param[in] iter the iterator that is being advanced
 * @param[out] out pointer to where the next element is set
 *
 * @return NUT_OK if the iterator was advanced, or NUT_TIER_END if the
 * end of the Array has been reached.
 */
NutState nut_array_iter_next(ArrayIter *iter, void **out)
{
    if (iter->index >= iter->ar->size)
        return NUT_ITER_END;


    *out = iter->ar->buffer[iter->index];

    iter->index++;
    iter->last_removed = false;

    return NUT_OK;
}

/**
 * Removes the last returned element by <code>nut_array_iter_next()</code>
 * function without invalidating the iterator and optionally sets the out
 * parameter to the value of the removed element.
 *
 * @note This function should only ever be called after a call to <code>
 * nut_array_iter_next()</code>.

 * @param[in] iter the iterator on which this operation is being performed
 * @param[out] out pointer to where the removed element is stored, or NULL
 *                 if it is to be ignored
 *
 * @return NUT_OK if the element was successfully removed, or
 * NUT_ERR_NOT_FIND.
 */
NutState nut_array_iter_remove(ArrayIter *iter, void **out)
{
    NutState status = NUT_ERR_NOT_FIND;

    if (!iter->last_removed) {
        status = nut_array_remove_at(iter->ar, iter->index - 1, out);
        if (status == NUT_OK)
            iter->last_removed = true;
    }
    return status;
}

/**
 * Adds a new element to the Array after the last returned element by
 * <code>nut_array_iter_next()</code> function without invalidating the
 * iterator.
 *
 * @note This function should only ever be called after a call to <code>
 * nut_array_iter_next()</code>.
 *
 * @param[in] iter the iterator on which this operation is being performed
 * @param[in] element the element being added
 *
 * @return NUT_OK if the element was successfully added, NUT_ERR_MALLOC if the
 * memory allocation for the new element failed, or CC_ERR_MAX_CAPACITY if
 * the array is already at maximum capacity.
 */
NutState nut_array_iter_add(ArrayIter *iter, void *element)
{
    return nut_array_add_at(iter->ar, element, iter->index++);
}

/**
 * Replaces the last returned element by <code>nut_array_iter_next()</code>
 * with the specified element and optionally sets the out parameter to
 * the value of the replaced element.
 *
 * @note This function should only ever be called after a call to <code>
 * nut_array_iter_next()</code>.
 *
 * @param[in] iter the iterator on which this operation is being performed
 * @param[in] element the replacement element
 * @param[out] out pointer to where the replaced element is stored, or NULL
 *                if it is to be ignored
 *
 * @return NUT_OK if the element was replaced successfully, or
 * NUT_ERR_OUT_RANGE.
 */
NutState nut_array_iter_replace(ArrayIter *iter, void *element, void **out)
{
    return nut_array_replace_at(iter->ar, element, iter->index - 1, out);
}

/**
 * Returns the index of the last returned element by <code>nut_array_iter_next()
 * </code>.
 *
 * @note
 * This function should not be called before a call to <code>nut_array_iter_next()
 * </code>.
 *
 * @param[in] iter the iterator on which this operation is being performed
 *
 * @return the index.
 */
size_t nut_array_iter_index(ArrayIter *iter)
{
    return iter->index - 1;
}

/**
 * Initializes the zip iterator.
 *
 * @param[in] iter iterator that is being initialized
 * @param[in] ar1  first array
 * @param[in] ar2  second array
 */
void nut_array_zip_iter_init(ArrayZipIter *iter, Array *ar1, Array *ar2)
{
    iter->ar1 = ar1;
    iter->ar2 = ar2;
    iter->index = 0;
    iter->last_removed = false;
}

/**
 * Outputs the next element pair in the sequence and advances the iterator.
 *
 * @param[in]  iter iterator that is being advanced
 * @param[out] out1 output of the first array element
 * @param[out] out2 output of the second array element
 *
 * @return NUT_OK if a next element pair is returned, or NUT_TIER_END if the end of one
 * of the arrays has been reached.
 */
NutState nut_array_zip_iter_next(ArrayZipIter *iter, void **out1, void **out2)
{
    if (iter->index >= iter->ar1->size || iter->index >= iter->ar2->size)
        return NUT_ITER_END;

    *out1 = iter->ar1->buffer[iter->index];
    *out2 = iter->ar2->buffer[iter->index];

    iter->index++;
    iter->last_removed = false;

    return NUT_OK;
}

/**
 * Removes and outputs the last returned element pair by <code>nut_array_zip_iter_next()
 * </code> without invalidating the iterator.
 *
 * @param[in]  iter iterator on which this operation is being performed
 * @param[out] out1 output of the removed element from the first array
 * @param[out] out2 output of the removed element from the second array
 *
 * @return NUT_OK if the element was successfully removed, NUT_ERR_OUT_RANGE if the
 * state of the iterator is invalid, or NUT_ERR_NOT_FIND if the element was
 * already removed.
 */
NutState nut_array_zip_iter_remove(ArrayZipIter *iter, void **out1, void **out2)
{
    if ((iter->index - 1) >= iter->ar1->size || (iter->index - 1) >= iter->ar2->size)
        return NUT_ERR_OUT_RANGE;

    if (!iter->last_removed) {
        nut_array_remove_at(iter->ar1, iter->index - 1, out1);
        nut_array_remove_at(iter->ar2, iter->index - 1, out2);
        iter->last_removed = true;
        return NUT_OK;
    }
    return NUT_ERR_NOT_FIND;
}

/**
 * Adds a new element pair to the arrays after the last returned element pair by
 * <code>nut_array_zip_iter_next()</code> and immediately before an element pair
 * that would be returned by a subsequent call to <code>nut_array_zip_iter_next()</code>
 * without invalidating the iterator.
 *
 * @param[in] iter iterator on which this operation is being performed
 * @param[in] e1   element added to the first array
 * @param[in] e2   element added to the second array
 *
 * @return NUT_OK if the element pair was successfully added to the arrays, or
 * NUT_ERR_MALLOC if the memory allocation for the new elements failed.
 */
NutState nut_array_zip_iter_add(ArrayZipIter *iter, void *e1, void *e2)
{
    size_t index = iter->index++;
    Array  *ar1  = iter->ar1;
    Array  *ar2  = iter->ar2;

    /* Make sure both array buffers have room */
    if ((ar1->size == ar1->capacity && (expand_capacity(ar1) != NUT_OK)) ||
            (ar2->size == ar2->capacity && (expand_capacity(ar2) != NUT_OK)))
        return NUT_ERR_MALLOC;

    nut_array_add_at(ar1, e1, index);
    nut_array_add_at(ar2, e2, index);

    return NUT_OK;
}

/**
 * Replaces the last returned element pair by <code>nut_array_zip_iter_next()</code>
 * with the specified replacement element pair.
 *
 * @param[in] iter  iterator on which this operation is being performed
 * @param[in]  e1   first array's replacement element
 * @param[in]  e2   second array's replacement element
 * @param[out] out1 output of the replaced element from the first array
 * @param[out] out2 output of the replaced element from the second array
 *
 * @return NUT_OK if the element was successfully replaced, or NUT_ERR_OUT_RANGE.
 */
NutState nut_array_zip_iter_replace(ArrayZipIter *iter, void *e1, void *e2, void **out1, void **out2)
{
    if ((iter->index - 1) >= iter->ar1->size || (iter->index - 1) >= iter->ar2->size)
        return NUT_ERR_OUT_RANGE;

    nut_array_replace_at(iter->ar1, e1, iter->index - 1, out1);
    nut_array_replace_at(iter->ar2, e2, iter->index - 1, out2);

    return NUT_OK;
}

/**
 * Returns the index of the last returned element pair by <code>nut_array_zip_iter_next()</code>.
 *
 * @param[in] iter iterator on which this operation is being performed
 *
 * @return current iterator index.
 */
size_t nut_array_zip_iter_index(ArrayZipIter *iter)
{
    return iter->index - 1;
}
