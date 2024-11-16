#pragma once
#include <Arduino.h>
#include <i2c_slave.h>

void loop();
void receiveLetter(uint8_t reg, uint8_t length);
void requestEvent();
int getaddress();
void getOffset();
int calibrate(bool initialCalibration);
void stopMotor();
void startMotor();
void rotateToLetter(int toLetter);
