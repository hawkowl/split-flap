#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint8_t byte;
typedef uint16_t word;

typedef uint8_t pin_size_t;

typedef enum {
  LOW = 0,
  HIGH = 1,
  CHANGE = 2,
  FALLING = 3,
  RISING = 4,
} PinStatus;

typedef enum {
  INPUT = 0x0,
  OUTPUT = 0x1,
  INPUT_PULLUP = 0x2,
  INPUT_PULLDOWN = 0x3,
  OUTPUT_OPENDRAIN = 0x4,
} PinMode;

void pinMode(pin_size_t pinNumber, PinMode pinMode);
void digitalWrite(pin_size_t pinNumber, PinStatus status);
PinStatus digitalRead(pin_size_t pinNumber);

#ifdef __cplusplus
}
#endif
