#include "systick.h"

#include <ch32v003fun.h>

#ifdef __cplusplus
extern "C" {
#endif

unsigned long micros(void) { return SysTick->CNT / DELAY_US_TIME; }
unsigned long millis(void) { return SysTick->CNT / DELAY_MS_TIME; }

#ifdef __cplusplus
}
#endif
