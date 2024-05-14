/*
 * SPDX-FileCopyrightText: 2023 Ci4Rail GmbH
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include <semaphore.h>
#include "io4edge_internal.h"

static const char *TAG = "streamq";

// Create a new stream queue
// @param capacity: capacity of the queue
// @param q_p: pointer to the new queue
io4e_err_t io4e_streamq_new(size_t capacity, streamq_t **q_p)
{
    streamq_t *q = calloc(1, sizeof(streamq_t));
    if (q == NULL) {
        return IO4E_ERR_NO_MEM;
    }
    q->msg = calloc(capacity, sizeof(void *));
    if (q->msg == NULL) {
        free(q);
        return IO4E_ERR_NO_MEM;
    }
    q->capacity = capacity;
    q->write_idx = 0;
    q->read_idx = 0;
    q->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    sem_init(&q->write_sem, 0, capacity);
    sem_init(&q->read_sem, 0, 0);
    *q_p = q;
    return IO4E_OK;
}

// Delete stream queue
void io4e_streamq_delete(streamq_t **q_p)
{
    streamq_t *q = *q_p;
    *q_p = NULL;
    if (q == NULL)
        return;
    if (q->msg != NULL)
        free(q->msg);
    free(q);
}

// advance index
static inline size_t advance_idx(streamq_t *q, size_t idx)
{
    return (idx + 1) % q->capacity;
}

static struct timespec get_timeout(uint64_t timeout)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    uint64_t until = ts.tv_sec + timeout;

    // ensure end of the timeout doesn't exceed the maximum time value on 32 bit systems
    if(sizeof(time_t)==4 && until > 0x7FFFFFFF){
        until = 0x7FFFFFFF;
    }
    ts.tv_sec = until;
    return ts;
}

// Push a message into the queue. Blocks if the queue is full.
// @param msg: message to push
io4e_err_t io4e_streamq_push(streamq_t *q, void *msg, uint64_t timeout)
{
    struct timespec ts = get_timeout(timeout);
    if (sem_timedwait(&q->write_sem, &ts) == -1) {
        IO4E_LOGD(TAG, "push timed out");
        return IO4E_ERR_TIMEOUT;
    }
    pthread_mutex_lock(&q->mutex);
    q->msg[q->write_idx] = msg;
    q->write_idx = advance_idx(q, q->write_idx);
    q->nentries++;
    pthread_mutex_unlock(&q->mutex);
    sem_post(&q->read_sem);
    return IO4E_OK;
}

// Pop a message from the queue
// @param msg_p: pointer to store the message
// @param timeout: timeout in seconds
// @return IO4E_OK on success, IO4E_ERR_TIMEOUT on timeout
io4e_err_t io4e_streamq_pop(streamq_t *q, void **msg_p, uint64_t timeout)
{
    struct timespec ts = get_timeout(timeout);

    if (sem_timedwait(&q->read_sem, &ts) == -1) {
        IO4E_LOGD(TAG, "pop timed out");
        return IO4E_ERR_TIMEOUT;
    }

    pthread_mutex_lock(&q->mutex);
    *msg_p = q->msg[q->read_idx];
    q->read_idx = advance_idx(q, q->read_idx);
    q->nentries--;
    pthread_mutex_unlock(&q->mutex);
    sem_post(&q->write_sem);
    return IO4E_OK;
}

// Get the number of messages in the queue
size_t io4e_streamq_entries(streamq_t *q)
{
    size_t size;
    pthread_mutex_lock(&q->mutex);
    size = q->nentries;
    pthread_mutex_unlock(&q->mutex);
    return size;
}
