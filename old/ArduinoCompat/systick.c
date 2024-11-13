#include "systick.h"

#include <stdio.h>

#include "ch32v003fun.h"

#ifdef __cplusplus
extern "C" {
#endif

unsigned long micros(void) { return SysTick->CNT / DELAY_US_TIME; }

#define delay(ms) Delay_Ms(ms)

#ifdef __cplusplus
}
#endif
