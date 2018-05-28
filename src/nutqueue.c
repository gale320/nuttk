
#include "nutconf.h"
#include "nutport.h"
#include "nutinc.h"
#include "nutmem.h"
#include "nutqueue.h"

struct nut_queue_s {
    Deque *d;

    void *(*mem_alloc)  (size_t size);
    void *(*mem_calloc) (size_t blocks, size_t size);
    void  (*mem_free)   (void *block);
};

/**
 * Initializes the fields of the QueueConf struct to default values.
 *
 * @param[in, out] conf the configuration struct that is being initialized
 */
void nut_queue_conf_init(QueueConf *conf)
{
    nut_deque_conf_init(conf);
}

/**
 * Creates a new empty queue and returns a status code.
 *
 * @param[out] out pointer to where the newly created Queue is to be stored
 *
 * @return  CC_OK if the creation was successful, or NUT_ERR_MALLOC if the
 * memory allocation for the new Queue structure failed.
 */
NutState nut_queue_new(Queue **queue)
{
    QueueConf conf;
    nut_queue_conf_init(&conf);
    return nut_queue_new_conf(&conf, queue);
}

/**
 * Creates a new empty Queue based on the specified QueueConf object and
 * returns a status code.
 *
 * The Queue is allocated using the allocators specified in the QueueConf struct.
 * The allocation may fail if the underlying allocator fails.
 *
 * @param[in] conf Queue configuration structure. All fields must be initialized
 *                 with appropriate values.
 * @param[out] out Pointer to where the newly created Queue is to be stored.
 *
 * @return CC_OK if the creation was successful, or NUT_ERR_MALLOC if the memory
 * allocation for the new Queue structure failed.
 */
NutState nut_queue_new_conf(QueueConf const * const conf, Queue **q)
{
    Queue *queue = conf->mem_calloc(1, sizeof(Queue));

    if (!queue)
        return NUT_ERR_MALLOC;

    Deque *deque;
    nut_deque_new_conf(conf, &deque);

    if (!deque) {
        conf->mem_free(queue);
        return NUT_ERR_MALLOC;
    }

    queue->d          = deque;
    queue->mem_alloc  = conf->mem_alloc;
    queue->mem_calloc = conf->mem_calloc;
    queue->mem_free   = conf->mem_free;

    *q = queue;

    return NUT_OK;
}

/**
 * Destroys the queue structure, but leaves the data it used to hold intact.
 *
 * @param[in] queue the queue that is to be destroyed
 */
void nut_queue_destroy(Queue *queue)
{
    nut_deque_destroy(queue->d);
    queue->mem_free(queue);
}

/**
 * Destroys the queue structure along with all the data it holds.
 *
 * @note This function should not be called on a queue that has some of its
 *       elements allocated on the stack.
 *
 * @param[in] queue the queue that is to be destroyed
 */
void nut_queue_destroy_cb(Queue *queue, void (*cb) (void*))
{
    nut_deque_destroy_cb(queue->d, cb);
    free(queue);
}

/**
 * Gets the element at the front of the queue and sets the out
 * parameter to its value.
 *
 * @param[in] queue the Queue whose element is being returned
 * @param[out] out pointer to where the element is stored
 *
 * @return CC_OK if the element was found, or CC_ERR_OUT_OF_RANGE if the
 * Queue is empty.
 */
NutState nut_queue_peek(Queue const * const queue, void **out)
{
    return nut_deque_get_last(queue->d, out);
}

/**
 * Gets and removes the element at the front of the queue and optionally
 * sets the out parameter to the value of the removed element.
 *
 * @param[in] queue the queue on which this operation is performed
 * @param[out] out pointer to where the removed element is stored
 *
 * @return CC_OK if the element was found, or CC_ERR_OUT_OF_RANGE if the
 * Queue is empty.
 */
NutState nut_queue_poll(Queue *queue, void **out)
{
    return nut_deque_remove_last(queue->d, out);
}

/**
 * Appends an element to the back of the queue. This operation may
 * fail if the memory allocation for the new element fails.
 *
 * @param[in] queue the queue on which this operation is performed
 * @param[in] element the element being enqueued
 *
 * @return CC_OK if the element was successfully added, or NUT_ERR_MALLOC
 * if the memory allocation for the new element failed.
 */
NutState nut_queue_enqueue(Queue *queue, void *element)
{
    return nut_deque_add_first(queue->d, element);
}

/**
 * Returns the size of the specified queue. The size of the queue is
 * the number of elements contained within the queue.
 *
 * @param[in] queue the queue whose size is being returned
 *
 * @return the number of elements within the queue.
 */
size_t nut_queue_size(Queue const * const queue)
{
    return nut_deque_size(queue->d);
}

/**
 * Applies the function fn to each element of the Queue.
 *
 * @param[in] queue the queue on which this operation is performed
 * @param[in] fn the operation function that is to be invoked on each queue element
 */
void nut_queue_foreach(Queue *queue, void (*fn) (void*))
{
    nut_deque_foreach(queue->d, fn);
}

/**
 * Initializes the iterator.
 *
 * @param[in] iter the iterator that is being initialized
 * @param[in] queue the queue to iterate over
 */
void nut_queue_iter_init(QueueIter *iter, Queue *queue)
{
    nut_deque_iter_init(&(iter->i), queue->d);
}

/**
 * Advances the iterator and sets the out parameter to the value of the
 * next element in the sequence.
 *
 * @param[in] iter the iterator that is being advanced
 * @param[out] out pointer to where the next element is set
 *
 * @return CC_OK if the iterator was advanced, or CC_ITER_END if the
 * end of the Queue has been reached.
 */
NutState nut_queue_iter_next(QueueIter *iter, void **out)
{
    return nut_deque_iter_next(&(iter->i), out);
}

/**
 * Replaces the last returned element by <code>nut_queue_iter_next()</code>
 * with the specified element and optionally sets the out parameter to
 * the value of the replaced element.
 *
 * @note This function should only ever be called after a call to <code>
 *       nut_queue_iter_next()</code>.
 *
 * @param[in] iter the iterator on which this operation is being performed
 * @param[in] element the replacement element
 * @param[out] out pointer to where the replaced element is stored, or NULL
 *                 if it is to be ignored
 *
 * @return  CC_OK if the element was replaced successfully, or
 * CC_ERR_OUT_OF_RANGE.
 */
NutState nut_queue_iter_replace(QueueIter *iter, void *replacement, void **out)
{
    return nut_deque_iter_replace(&(iter->i), replacement, out);
}

/**
 * Initializes the zip iterator.
 *
 * @param[in] iter iterator that is being initialized
 * @param[in] q1   first queue
 * @param[in] q2   second queue
 */
void nut_queue_zip_iter_init(QueueZipIter *iter, Queue *q1, Queue *q2)
{
    nut_deque_zip_iter_init(&(iter->i), q1->d, q2->d);
}

/**
 * Outputs the next element pair in the sequence and advances the iterator.
 *
 * @param[in]  iter iterator that is being advanced
 * @param[out] out1 output of the first queue element
 * @param[out] out2 output of the second queue element
 *
 * @return CC_OK if a next element pair is returned, or CC_ITER_END if the end
 * of one of the queues has been reached.
 */
NutState nut_queue_zip_iter_next(QueueZipIter *iter, void **out1, void **out2)
{
    return nut_deque_zip_iter_next(&(iter->i), out1, out2);
}

/**
 * Replaces the last returned element pair by <code>nut_queue_zip_iter_next()</code>
 * with the specified replacement element pair.
 *
 * @param[in] iter  iterator on which this operation is being performed
 * @param[in]  e1   first queue's replacement element
 * @param[in]  e2   second queue's replacement element
 * @param[out] out1 output of the replaced element from the first queue
 * @param[out] out2 output of the replaced element from the second queue
 *
 * @return CC_OK if the element was successfully replaced, or CC_ERR_OUT_OF_RANGE.
 */
NutState nut_queue_zip_iter_replace(QueueZipIter *iter, void *e1, void *e2, void **out1, void **out2)
{
    return nut_deque_zip_iter_replace(&(iter->i), e1, e2, out1, out2);
}
