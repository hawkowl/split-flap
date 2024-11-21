#pragma once

#include <math.h>

#include "ch32v003fun.h"
#include "pinout.h"

#ifdef __cplusplus
extern "C" {
#endif

void yield(void);

unsigned long millis(void);
unsigned long micros(void);
void delay(unsigned long);
void delayMicroseconds(unsigned int us);

void init(void);
void initVariant(void);

#ifndef HOST
int atexit(void (*func)()) __attribute__((weak));
#endif
int main() __attribute__((weak));

void setup(void);
void loop(void);

#ifdef __cplusplus
}
#endif
