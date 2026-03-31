#include "freertos/semphr.h"

#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    pthread_mutex_t mtx;
    pthread_cond_t  cv;
    UBaseType_t     count;
    UBaseType_t     max;
} SemCB;

static void ms_to_abstime(uint32_t ms, struct timespec *ts) {
    clock_gettime(CLOCK_REALTIME, ts);
    ts->tv_sec  += (time_t)(ms / 1000u);
    ts->tv_nsec += (long)(ms % 1000u) * 1000000L;
    if (ts->tv_nsec >= 1000000000L) {
        ts->tv_sec++;
        ts->tv_nsec -= 1000000000L;
    }
}

static SemCB *sem_alloc(UBaseType_t initial, UBaseType_t max) {
    SemCB *s = (SemCB *)calloc(1, sizeof(SemCB));
    if (!s) return NULL;
    pthread_mutex_init(&s->mtx, NULL);
    pthread_cond_init(&s->cv, NULL);
    s->count = initial;
    s->max   = max;
    return s;
}

SemaphoreHandle_t xSemaphoreCreateBinary(void) {
    return (SemaphoreHandle_t)sem_alloc(0, 1);
}

SemaphoreHandle_t xSemaphoreCreateMutex(void) {
    return (SemaphoreHandle_t)sem_alloc(1, 1);
}

SemaphoreHandle_t xSemaphoreCreateCounting(UBaseType_t max_count, UBaseType_t initial_count) {
    return (SemaphoreHandle_t)sem_alloc(initial_count, max_count);
}

void vSemaphoreDelete(SemaphoreHandle_t sem) {
    SemCB *s = (SemCB *)sem;
    if (!s) return;
    pthread_mutex_destroy(&s->mtx);
    pthread_cond_destroy(&s->cv);
    free(s);
}

BaseType_t xSemaphoreTake(SemaphoreHandle_t sem, TickType_t wait) {
    SemCB *s = (SemCB *)sem;
    if (!s) return pdFALSE;

    struct timespec deadline;
    if (wait != portMAX_DELAY) ms_to_abstime(wait, &deadline);

    pthread_mutex_lock(&s->mtx);
    while (s->count == 0) {
        if (wait == 0) { pthread_mutex_unlock(&s->mtx); return pdFALSE; }
        int rc = (wait == portMAX_DELAY)
                     ? pthread_cond_wait(&s->cv, &s->mtx)
                     : pthread_cond_timedwait(&s->cv, &s->mtx, &deadline);
        if (rc == ETIMEDOUT) { pthread_mutex_unlock(&s->mtx); return pdFALSE; }
    }
    s->count--;
    pthread_mutex_unlock(&s->mtx);
    return pdTRUE;
}

BaseType_t xSemaphoreGive(SemaphoreHandle_t sem) {
    SemCB *s = (SemCB *)sem;
    if (!s) return pdFALSE;
    pthread_mutex_lock(&s->mtx);
    if (s->count >= s->max) { pthread_mutex_unlock(&s->mtx); return pdFALSE; }
    s->count++;
    pthread_cond_signal(&s->cv);
    pthread_mutex_unlock(&s->mtx);
    return pdTRUE;
}

BaseType_t xSemaphoreGiveFromISR(SemaphoreHandle_t sem, BaseType_t *higher_priority_woken) {
    if (higher_priority_woken) *higher_priority_woken = pdFALSE;
    return xSemaphoreGive(sem);
}

UBaseType_t uxSemaphoreGetCount(SemaphoreHandle_t sem) {
    SemCB *s = (SemCB *)sem;
    if (!s) return 0;
    pthread_mutex_lock(&s->mtx);
    UBaseType_t n = s->count;
    pthread_mutex_unlock(&s->mtx);
    return n;
}
