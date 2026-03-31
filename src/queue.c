#include "freertos/queue.h"

#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    pthread_mutex_t mtx;
    pthread_cond_t cv_not_full;
    pthread_cond_t cv_not_empty;
    UBaseType_t capacity;
    UBaseType_t item_size;
    UBaseType_t count;
    UBaseType_t head; /* read index  */
    UBaseType_t tail; /* write index */
    uint8_t *buf;     /* capacity * item_size bytes */
} QueueCB;

static void ms_to_abstime(uint32_t ms, struct timespec *ts) {
    clock_gettime(CLOCK_REALTIME, ts);
    ts->tv_sec += (time_t)(ms / 1000u);
    ts->tv_nsec += (long)(ms % 1000u) * 1000000L;
    if (ts->tv_nsec >= 1000000000L) {
        ts->tv_sec++;
        ts->tv_nsec -= 1000000000L;
    }
}

QueueHandle_t xQueueCreate(UBaseType_t length, UBaseType_t item_size) {
    if (length == 0 || item_size == 0) return NULL;
    QueueCB *q = (QueueCB *)calloc(1, sizeof(QueueCB));
    if (!q) return NULL;
    q->buf = (uint8_t *)malloc((size_t)length * item_size);
    if (!q->buf) {
        free(q);
        return NULL;
    }
    q->capacity  = length;
    q->item_size = item_size;
    q->count = q->head = q->tail = 0;
    pthread_mutex_init(&q->mtx, NULL);
    pthread_cond_init(&q->cv_not_full, NULL);
    pthread_cond_init(&q->cv_not_empty, NULL);
    return (QueueHandle_t)q;
}

void vQueueDelete(QueueHandle_t queue) {
    QueueCB *q = (QueueCB *)queue;
    if (!q) return;
    pthread_mutex_destroy(&q->mtx);
    pthread_cond_destroy(&q->cv_not_full);
    pthread_cond_destroy(&q->cv_not_empty);
    free(q->buf);
    free(q);
}

static BaseType_t send_impl(QueueHandle_t queue, const void *item, TickType_t wait, int to_front) {
    QueueCB *q = (QueueCB *)queue;
    if (!q || !item) return pdFALSE;

    struct timespec deadline;
    if (wait != portMAX_DELAY) ms_to_abstime(wait, &deadline);

    pthread_mutex_lock(&q->mtx);
    while (q->count >= q->capacity) {
        if (wait == 0) {
            pthread_mutex_unlock(&q->mtx);
            return pdFALSE;
        }
        int rc = (wait == portMAX_DELAY)
                     ? pthread_cond_wait(&q->cv_not_full, &q->mtx)
                     : pthread_cond_timedwait(&q->cv_not_full, &q->mtx, &deadline);
        if (rc == ETIMEDOUT) {
            pthread_mutex_unlock(&q->mtx);
            return pdFALSE;
        }
    }

    uint8_t *slot;
    if (to_front) {
        q->head = (q->head == 0) ? q->capacity - 1 : q->head - 1;
        slot    = q->buf + q->head * q->item_size;
    } else {
        slot    = q->buf + q->tail * q->item_size;
        q->tail = (q->tail + 1) % q->capacity;
    }
    memcpy(slot, item, q->item_size);
    q->count++;
    pthread_cond_signal(&q->cv_not_empty);
    pthread_mutex_unlock(&q->mtx);
    return pdTRUE;
}

BaseType_t xQueueSend(QueueHandle_t queue, const void *item, TickType_t wait) {
    return send_impl(queue, item, wait, 0);
}

BaseType_t xQueueSendToFront(QueueHandle_t queue, const void *item, TickType_t wait) {
    return send_impl(queue, item, wait, 1);
}

BaseType_t xQueueReceive(QueueHandle_t queue, void *buf, TickType_t wait) {
    QueueCB *q = (QueueCB *)queue;
    if (!q || !buf) return pdFALSE;

    struct timespec deadline;
    if (wait != portMAX_DELAY) ms_to_abstime(wait, &deadline);

    pthread_mutex_lock(&q->mtx);
    while (q->count == 0) {
        if (wait == 0) {
            pthread_mutex_unlock(&q->mtx);
            return pdFALSE;
        }
        int rc = (wait == portMAX_DELAY)
                     ? pthread_cond_wait(&q->cv_not_empty, &q->mtx)
                     : pthread_cond_timedwait(&q->cv_not_empty, &q->mtx, &deadline);
        if (rc == ETIMEDOUT) {
            pthread_mutex_unlock(&q->mtx);
            return pdFALSE;
        }
    }
    uint8_t *slot = q->buf + q->head * q->item_size;
    memcpy(buf, slot, q->item_size);
    q->head = (q->head + 1) % q->capacity;
    q->count--;
    pthread_cond_signal(&q->cv_not_full);
    pthread_mutex_unlock(&q->mtx);
    return pdTRUE;
}

UBaseType_t uxQueueMessagesWaiting(QueueHandle_t queue) {
    QueueCB *q = (QueueCB *)queue;
    if (!q) return 0;
    pthread_mutex_lock(&q->mtx);
    UBaseType_t n = q->count;
    pthread_mutex_unlock(&q->mtx);
    return n;
}

UBaseType_t uxQueueSpacesAvailable(QueueHandle_t queue) {
    QueueCB *q = (QueueCB *)queue;
    if (!q) return 0;
    pthread_mutex_lock(&q->mtx);
    UBaseType_t n = q->capacity - q->count;
    pthread_mutex_unlock(&q->mtx);
    return n;
}

BaseType_t xQueueReset(QueueHandle_t queue) {
    QueueCB *q = (QueueCB *)queue;
    if (!q) return pdFALSE;
    pthread_mutex_lock(&q->mtx);
    q->count = q->head = q->tail = 0;
    pthread_cond_broadcast(&q->cv_not_full);
    pthread_mutex_unlock(&q->mtx);
    return pdTRUE;
}
