#include <semaphore.h>
#include "io4edge_internal.h"

static const char *TAG = "streamq";

// Create a new stream queue
// @param nentries: capacity of the queue
// @param q_p: pointer to the new queue
io4e_err_t io4e_streamq_new(size_t nentries, streamq_t **q_p)
{
    streamq_t *q = calloc(1, sizeof(streamq_t));
    if (q == NULL) {
        return IO4E_ERR_NO_MEM;
    }
    q->msg = calloc(nentries, sizeof(void *));
    if (q->msg == NULL) {
        free(q);
        return IO4E_ERR_NO_MEM;
    }
    q->nentries = nentries;
    q->write_idx = 0;
    q->read_idx = 0;
    q->mutex = (pthread_mutex_t)PTHREAD_MUTEX_INITIALIZER;
    sem_init(&q->write_sem, 0, nentries);
    sem_init(&q->read_sem, 0, 0);
    *q_p = q;
    return IO4E_OK;
}

// Free a stream queue
void io4e_streamq_free(streamq_t *q)
{
    if (q == NULL)
        return;
    if (q->msg != NULL)
        free(q->msg);
    free(q);
}

// advance index
static inline size_t advance_idx(streamq_t *q, size_t idx)
{
    return (idx + 1) % q->nentries;
}

// Push a message into the queue
// @param msg: message to push
void io4e_streamq_push(streamq_t *q, void *msg)
{
    sem_wait(&q->write_sem);
    pthread_mutex_lock(&q->mutex);
    q->msg[q->write_idx] = msg;
    q->write_idx = advance_idx(q, q->write_idx);
    pthread_mutex_unlock(&q->mutex);
    sem_post(&q->read_sem);
}

// Pop a message from the queue
// @param msg_p: pointer to store the message
// @param timeout: timeout in seconds
io4e_err_t io4e_streamq_pop(streamq_t *q, void **msg_p, int timeout)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += timeout;
    if (sem_timedwait(&q->read_sem, &ts) == -1) {
        IO4E_LOGE(TAG, "pop timed out");
        return IO4E_ERR_TIMEOUT;
    }

    sem_wait(&q->read_sem);
    pthread_mutex_lock(&q->mutex);
    *msg_p = q->msg[q->read_idx];
    q->read_idx = advance_idx(q, q->read_idx);
    pthread_mutex_unlock(&q->mutex);
    sem_post(&q->write_sem);
}
