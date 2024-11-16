#pragma once

#include <ch32v003fun.h>

#ifdef __cplusplus
extern "C" {
#endif

unsigned long micros(void);
unsigned long millis(void);

#define delay(ms) Delay_Ms(ms)

#ifdef __cplusplus
}
#endif
