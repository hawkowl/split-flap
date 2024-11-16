#include "pinout.h"

#include <ch32v003fun.h>

#ifdef __cplusplus
extern "C" {
#endif

void pinMode(pin_size_t pinNumber, PinMode pinMode) {
  switch (pinMode) {
    case INPUT:
      funPinMode(pinNumber, GPIO_Speed_10MHz | GPIO_CNF_IN_FLOATING);
      break;
    case INPUT_PULLDOWN:
      funPinMode(pinNumber, GPIO_Speed_10MHz | GPIO_CNF_IN_PUPD);
      // funDigitalWrite(pinNumber, FUN_HIGH);
      break;
    case INPUT_PULLUP:
      funPinMode(pinNumber, GPIO_Speed_10MHz | GPIO_CNF_IN_PUPD);
      funDigitalWrite(pinNumber, FUN_HIGH);
      break;
    case OUTPUT:
      funPinMode(pinNumber, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);
      break;
    default:
      break;
  }
}

void digitalWrite(pin_size_t pinNumber, PinStatus status) {
  funDigitalWrite(pinNumber, status);
}

PinStatus digitalRead(pin_size_t pinNumber) {
  return funDigitalRead(pinNumber);
}

#ifdef __cplusplus
}
#endif
