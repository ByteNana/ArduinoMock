#include "freertos/timers.h"

#include <pthread.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_TIMERS 32

typedef struct {
    char name[16];
    TickType_t period;
    int auto_reload;
    int active;
    void *timer_id;
    TimerCallbackFunction_t callback;
    struct timespec fire_at;
} TimerImpl;

static TimerImpl *s_timers[MAX_TIMERS];
static size_t s_count        = 0;
static pthread_mutex_t s_mtx = PTHREAD_MUTEX_INITIALIZER;

static void set_fire_at(TimerImpl *t) {
    clock_gettime(CLOCK_MONOTONIC, &t->fire_at);
    t->fire_at.tv_sec += (time_t)(t->period / 1000u);
    t->fire_at.tv_nsec += (long)(t->period % 1000u) * 1000000L;
    if (t->fire_at.tv_nsec >= 1000000000L) {
        t->fire_at.tv_sec++;
        t->fire_at.tv_nsec -= 1000000000L;
    }
}

static int timespec_ge(const struct timespec *a, const struct timespec *b) {
    if (a->tv_sec != b->tv_sec) return a->tv_sec > b->tv_sec;
    return a->tv_nsec >= b->tv_nsec;
}

TimerHandle_t xTimerCreate(const char *name, TickType_t period, BaseType_t auto_reload,
                           void *timer_id, TimerCallbackFunction_t cb) {
    TimerImpl *t = (TimerImpl *)calloc(1, sizeof(TimerImpl));
    if (!t) return NULL;
    strncpy(t->name, name ? name : "", sizeof(t->name) - 1);
    t->period      = period;
    t->auto_reload = (auto_reload != pdFALSE);
    t->active      = 0;
    t->timer_id    = timer_id;
    t->callback    = cb;

    pthread_mutex_lock(&s_mtx);
    if (s_count < MAX_TIMERS) {
        s_timers[s_count++] = t;
    } else {
        free(t);
        t = NULL;
    }
    pthread_mutex_unlock(&s_mtx);
    return (TimerHandle_t)t;
}

BaseType_t xTimerStart(TimerHandle_t timer, TickType_t wait) {
    (void)wait;
    TimerImpl *t = (TimerImpl *)timer;
    if (!t) return pdFALSE;
    pthread_mutex_lock(&s_mtx);
    t->active = 1;
    set_fire_at(t);
    pthread_mutex_unlock(&s_mtx);
    return pdPASS;
}

BaseType_t xTimerStop(TimerHandle_t timer, TickType_t wait) {
    (void)wait;
    TimerImpl *t = (TimerImpl *)timer;
    if (!t) return pdFALSE;
    pthread_mutex_lock(&s_mtx);
    t->active = 0;
    pthread_mutex_unlock(&s_mtx);
    return pdPASS;
}

BaseType_t xTimerReset(TimerHandle_t timer, TickType_t wait) {
    (void)wait;
    TimerImpl *t = (TimerImpl *)timer;
    if (!t) return pdFALSE;
    pthread_mutex_lock(&s_mtx);
    t->active = 1;
    set_fire_at(t);
    pthread_mutex_unlock(&s_mtx);
    return pdPASS;
}

BaseType_t xTimerDelete(TimerHandle_t timer, TickType_t wait) {
    (void)wait;
    TimerImpl *t = (TimerImpl *)timer;
    if (!t) return pdFALSE;
    pthread_mutex_lock(&s_mtx);
    for (size_t i = 0; i < s_count; i++) {
        if (s_timers[i] == t) {
            s_timers[i] = s_timers[--s_count];
            break;
        }
    }
    pthread_mutex_unlock(&s_mtx);
    free(t);
    return pdPASS;
}

BaseType_t xTimerIsTimerActive(TimerHandle_t timer) {
    TimerImpl *t = (TimerImpl *)timer;
    if (!t || !t->active) return pdFALSE;

    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    pthread_mutex_lock(&s_mtx);
    if (timespec_ge(&now, &t->fire_at)) {
        if (t->callback) t->callback(timer);
        if (t->auto_reload) {
            set_fire_at(t);
        } else {
            t->active = 0;
        }
    }
    int active = t->active;
    pthread_mutex_unlock(&s_mtx);
    return active ? pdTRUE : pdFALSE;
}

BaseType_t xTimerChangePeriod(TimerHandle_t timer, TickType_t new_period, TickType_t wait) {
    (void)wait;
    TimerImpl *t = (TimerImpl *)timer;
    if (!t) return pdFALSE;
    pthread_mutex_lock(&s_mtx);
    t->period = new_period;
    pthread_mutex_unlock(&s_mtx);
    return pdPASS;
}

void *pvTimerGetTimerID(TimerHandle_t timer) {
    TimerImpl *t = (TimerImpl *)timer;
    return t ? t->timer_id : NULL;
}

const char *pcTimerGetName(TimerHandle_t timer) {
    TimerImpl *t = (TimerImpl *)timer;
    return t ? t->name : NULL;
}

void mockTimerProcess(void) {
    pthread_mutex_lock(&s_mtx);
    for (size_t i = 0; i < s_count; i++) {
        TimerImpl *t = s_timers[i];
        if (t->active && t->callback) {
            t->callback((TimerHandle_t)t);
            if (!t->auto_reload) t->active = 0;
        }
    }
    pthread_mutex_unlock(&s_mtx);
}

void mockTimerFire(TimerHandle_t timer) {
    TimerImpl *t = (TimerImpl *)timer;
    if (!t) return;
    pthread_mutex_lock(&s_mtx);
    if (t->callback) {
        t->callback(timer);
        if (!t->auto_reload) t->active = 0;
    }
    pthread_mutex_unlock(&s_mtx);
}

size_t mockTimerCount(void) {
    pthread_mutex_lock(&s_mtx);
    size_t n = s_count;
    pthread_mutex_unlock(&s_mtx);
    return n;
}

void mockTimerClear(void) {
    pthread_mutex_lock(&s_mtx);
    for (size_t i = 0; i < s_count; i++) free(s_timers[i]);
    s_count = 0;
    pthread_mutex_unlock(&s_mtx);
}
