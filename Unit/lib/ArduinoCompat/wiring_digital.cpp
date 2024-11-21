/*
  wiring_digital.cpp - digital input and output functions
  Part of Arduino - http://www.arduino.cc/

  Copyright (c) 2018-2019 Arduino SA

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA
*/

#ifdef __cplusplus
extern "C" {
#endif

#include "Arduino.h"
#include "ch32v003fun.h"
#include "stdio.h"

void pinMode(pin_size_t pin, PinMode mode) {
  // Configure pin
  switch (mode) {
    case INPUT:
      funPinMode(pin, GPIO_CFGLR_IN_FLOAT);
      break;
    case OUTPUT:
      funPinMode(pin, GPIO_CFGLR_OUT_10Mhz_PP);
      break;
    case INPUT_PULLUP:
      funPinMode(pin, GPIO_CFGLR_IN_PUPD);
      funDigitalWrite(pin, 1);
      break;
    case INPUT_PULLDOWN:
    default:
      funPinMode(pin, GPIO_CFGLR_IN_PUPD);
      funDigitalWrite(pin, 0);
      break;
  }
}

void digitalWrite(pin_size_t pin, PinStatus val) {
  switch (val) {
    case PinStatus::HIGH:
      // printf("writing HIGH to %d\n", pin);
      funDigitalWrite(pin, 1);
      break;
    case PinStatus::LOW:
      // printf("writing LOW to %d\n", pin);
      funDigitalWrite(pin, 0);
      break;
    default:
      printf("writing nothing to %d\n", pin);
      break;
  }
}

PinStatus digitalRead(pin_size_t pin) {
  switch (funDigitalRead(pin)) {
    case 0:
      return PinStatus::LOW;
      break;
    case 1:
      return PinStatus::HIGH;
      break;
    default:
      printf("fell through on read");
      return PinStatus::LOW;
  }
}

#ifdef __cplusplus
}
#endif
