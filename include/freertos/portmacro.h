#pragma once

/* portMUX — spinlock type.  On native there is no real concurrent ISR context,
 * so the type is a plain int and all critical-section macros are no-ops. */
typedef int portMUX_TYPE;
#define portMUX_INITIALIZER_UNLOCKED 0

#define portENTER_CRITICAL(mux) ((void)(mux))
#define portEXIT_CRITICAL(mux) ((void)(mux))
#define portENTER_CRITICAL_ISR(mux) ((void)(mux))
#define portEXIT_CRITICAL_ISR(mux) ((void)(mux))

/* portYIELD_FROM_ISR — no-op on native (no scheduler preemption). */
#ifndef portYIELD_FROM_ISR
#define portYIELD_FROM_ISR(x) ((void)(x))
#endif
