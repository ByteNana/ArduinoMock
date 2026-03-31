#include "freertos/task.h"

#include <errno.h>
#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct {
    pthread_t thread;
    pthread_mutex_t mtx;
    pthread_cond_t cv;
    volatile int cancel;
    char name[configMAX_TASK_NAME_LEN];
    UBaseType_t priority;
} TaskImpl;

typedef struct {
    TaskImpl *impl;
    TaskFunction_t fn;
    void *arg;
} ThreadArg;

/* Thread-local storage key for the current task */
static pthread_key_t s_tls_key;
static pthread_once_t s_tls_once = PTHREAD_ONCE_INIT;

static void tls_key_create(void) { pthread_key_create(&s_tls_key, NULL); }

static TaskImpl *current_task(void) {
    pthread_once(&s_tls_once, tls_key_create);
    return (TaskImpl *)pthread_getspecific(s_tls_key);
}

static void *task_thread_fn(void *raw) {
    ThreadArg *ta     = (ThreadArg *)raw;
    TaskImpl *impl    = ta->impl;
    TaskFunction_t fn = ta->fn;
    void *arg         = ta->arg;
    free(ta);

    pthread_once(&s_tls_once, tls_key_create);
    pthread_setspecific(s_tls_key, impl);

    fn(arg);

    pthread_setspecific(s_tls_key, NULL);
    return NULL;
}

static void ms_to_abstime(uint32_t ms, struct timespec *ts) {
    clock_gettime(CLOCK_REALTIME, ts);
    ts->tv_sec += (time_t)(ms / 1000u);
    ts->tv_nsec += (long)(ms % 1000u) * 1000000L;
    if (ts->tv_nsec >= 1000000000L) {
        ts->tv_sec++;
        ts->tv_nsec -= 1000000000L;
    }
}

BaseType_t xTaskCreate(TaskFunction_t fn, const char *name, uint32_t stack_depth, void *params,
                       UBaseType_t priority, TaskHandle_t *handle) {
    (void)stack_depth;
    pthread_once(&s_tls_once, tls_key_create);

    TaskImpl *impl = (TaskImpl *)calloc(1, sizeof(TaskImpl));
    if (!impl) return pdFALSE;

    pthread_mutex_init(&impl->mtx, NULL);
    pthread_cond_init(&impl->cv, NULL);
    strncpy(impl->name, name ? name : "task", sizeof(impl->name) - 1);
    impl->priority = priority;
    impl->cancel   = 0;

    ThreadArg *ta = (ThreadArg *)malloc(sizeof(ThreadArg));
    if (!ta) {
        pthread_mutex_destroy(&impl->mtx);
        pthread_cond_destroy(&impl->cv);
        free(impl);
        return pdFALSE;
    }
    ta->impl = impl;
    ta->fn   = fn;
    ta->arg  = params;

    if (pthread_create(&impl->thread, NULL, task_thread_fn, ta) != 0) {
        free(ta);
        pthread_mutex_destroy(&impl->mtx);
        pthread_cond_destroy(&impl->cv);
        free(impl);
        return pdFALSE;
    }

    if (handle) *handle = (TaskHandle_t)impl;
    return pdPASS;
}

void vTaskDelete(TaskHandle_t handle) {
    if (handle == NULL) {
        /* Self-delete: signal cancel then exit the calling thread */
        TaskImpl *impl = current_task();
        if (impl) {
            pthread_mutex_lock(&impl->mtx);
            impl->cancel = 1;
            pthread_cond_broadcast(&impl->cv);
            pthread_mutex_unlock(&impl->mtx);
        }
        pthread_exit(NULL);
    }

    TaskImpl *impl = (TaskImpl *)handle;
    pthread_mutex_lock(&impl->mtx);
    impl->cancel = 1;
    pthread_cond_broadcast(&impl->cv);
    pthread_mutex_unlock(&impl->mtx);

    pthread_join(impl->thread, NULL);
    pthread_mutex_destroy(&impl->mtx);
    pthread_cond_destroy(&impl->cv);
    free(impl);
}

void vTaskDelay(TickType_t ticks) {
    TaskImpl *impl = current_task();

    if (!impl) {
        /* Outside a task context: plain nanosleep */
        struct timespec ts;
        ts.tv_sec  = ticks / 1000u;
        ts.tv_nsec = (long)(ticks % 1000u) * 1000000L;
        nanosleep(&ts, NULL);
        return;
    }

    if (ticks == 0) return;

    struct timespec deadline;
    ms_to_abstime(ticks, &deadline);

    pthread_mutex_lock(&impl->mtx);
    while (!impl->cancel) {
        int rc = pthread_cond_timedwait(&impl->cv, &impl->mtx, &deadline);
        if (rc == ETIMEDOUT) break;
    }
    int should_exit = impl->cancel;
    pthread_mutex_unlock(&impl->mtx);

    if (should_exit) pthread_exit(NULL);
}

void vTaskDelayUntil(TickType_t *prev, TickType_t increment) {
    vTaskDelay(increment);
    if (prev) *prev += increment;
}

TickType_t xTaskGetTickCount(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (TickType_t)(ts.tv_sec * 1000UL + (uint32_t)(ts.tv_nsec / 1000000UL));
}

void vTaskStartScheduler(void) {}
void vTaskEndScheduler(void) {}
size_t xPortGetFreeHeapSize(void) { return 1024u * 1024u; }
