#pragma once
#include <Arduino.h>

void receiveLetter(uint8_t reg, uint8_t length);
void requestEvent(uint8_t reg);
int getaddress();
int calibrate(bool initialCalibration);
void stopMotor();
void startMotor();
void rotateToLetter(int toLetter);
