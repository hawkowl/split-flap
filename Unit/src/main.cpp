// Based off https://github.com/davidkingsman/split-flap 's Unit.ino

#include "main.hpp"

#include <Arduino.h>
#include <Stepper.h>
#include <ch32v003fun.h>
#include <i2c_slave.h>
#include <stdio.h>

#define SERIAL_ENABLE  // uncomment for serial debug communication

// Pins of I2C address switch
#define ADRESSSW4 PC0
#define ADRESSSW3 PD6
#define ADRESSSW2 PD5
#define ADRESSSW1 PD4

// constants stepper
#define STEPPERPIN1 PC6
#define STEPPERPIN2 PC7
#define STEPPERPIN3 PC3
#define STEPPERPIN4 PC4
#define HALLPIN PD7

#define STEPS 2038  // 28BYJ-48 stepper, number of steps
#define AMOUNTFLAPS 45

// constants others
#define ROTATIONDIRECTION -1  //-1 for reverse direction
#define OVERHEATINGTIMEOUT \
  2  // timeout in seconds to avoid overheating of stepper-> After starting
     // rotation, the counter will start. Stepper won't move again until timeout
     // is passed
unsigned long lastRotation = 0;

// globals
int displayedLetter = 0;  // currently shown letter
int desiredLetter = 0;    // letter to be shown
const char* letters[] = {
    " ", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N",
    "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "Ä", "Ö", "Ü",
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ".", "-", "?", "!"};
Stepper* stepper;

PinStatus lastInd1 = LOW;  // store last status of phase
PinStatus lastInd2 = LOW;  // store last status of phase
PinStatus lastInd3 = LOW;  // store last status of phase
PinStatus lastInd4 = LOW;  // store last status of phase
float missedSteps = 0;  // cummulate steps <1, to compensate via additional step
                        // when reaching >1
int currentlyrotating =
    0;  // 1 = drum is currently rotating, 0 = drum is standing still
int stepperSpeed = 10;  // current speed of stepper, value only for first homing
int calOffset = 74;     // Offset for calibration in steps
int receivedNumber = 0;
bool doCalibrate = false;
int i2cAddress;

// sleep globals
const unsigned long WAIT_TIME =
    100;  // wait time before sleep routine gets executed again in milliseconds
unsigned long previousMillis = 0;  // stores last time sleep was interrupted

volatile uint8_t i2c_registers[32] = {0x00};

// setup
void setup() {
  funGpioInitAll();

  stepper = new Stepper(STEPS, STEPPERPIN1, STEPPERPIN3, STEPPERPIN2,
                        STEPPERPIN4);  // stepper setup

  pinMode(STEPPERPIN1, OUTPUT);
  pinMode(STEPPERPIN2, OUTPUT);
  pinMode(STEPPERPIN3, OUTPUT);
  pinMode(STEPPERPIN4, OUTPUT);

#ifdef SERIAL_ENABLE
  printf("starting unit...\n");
#endif

  // i2c address switch
  pinMode(ADRESSSW1, INPUT_PULLUP);
  pinMode(ADRESSSW2, INPUT_PULLUP);
  pinMode(ADRESSSW3, INPUT_PULLUP);
  pinMode(ADRESSSW4, INPUT_PULLUP);

  // hall sensor
  pinMode(HALLPIN, INPUT);

  i2cAddress = getaddress();  // get I2C Address and save in variable

#ifdef SERIAL_ENABLE
  printf("I2CAddress: %d\n", i2cAddress);
#endif

  funPinMode(PC1, GPIO_CFGLR_OUT_10Mhz_AF_OD);  // SDA
  funPinMode(PC2, GPIO_CFGLR_OUT_10Mhz_AF_OD);  // SCL

  SetupI2CSlave(i2cAddress, i2c_registers, sizeof(i2c_registers), receiveLetter,
                requestEvent, false);
}

void loop() {
  // If we need to recalibrate, do that now
  if (doCalibrate) {
    printf("Doing initial calibration\n");
    doCalibrate = false;
    calibrate(true);
  }

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= WAIT_TIME) {
    delay(WAIT_TIME);
  }  // end of time to sleep

  previousMillis = millis();  // reset sleep counter

  if (displayedLetter != receivedNumber) {
#ifdef SERIAL_ENABLE
    printf("Value over serial received: %d Letter: %s @ speed %d\n",
           receivedNumber, letters[receivedNumber], stepperSpeed);
#endif
    // rotate to new letter
    rotateToLetter(receivedNumber);
  }
}

// rotate to letter
void rotateToLetter(int toLetter) {
  if (lastRotation == 0 ||
      (millis() - lastRotation > OVERHEATINGTIMEOUT * 1000)) {
    lastRotation = millis();
    // get letter position
    int posLetter = -1;
    posLetter = toLetter;
    int posCurrentLetter = -1;
    posCurrentLetter = displayedLetter;
    // int amountLetters = sizeof(letters) / sizeof(String);
#ifdef SERIAL_ENABLE
    printf("go to letter: %s\n", letters[toLetter]);
#endif
    // go to letter, but only if available (>-1)
    if (posLetter > -1) {  // check if letter exists
      // check if letter is on higher index, then no full rotaion is needed
      if (posLetter >= posCurrentLetter) {
#ifdef SERIAL_ENABLE
        printf("direct\n");
#endif
        // go directly to next letter, get steps from current letter to target
        // letter
        int diffPosition = posLetter - posCurrentLetter;
        startMotor();
        stepper->setSpeed(stepperSpeed);
        // doing the rotation letterwise
        for (int i = 0; i < diffPosition; i++) {
          float preciseStep = (float)STEPS / (float)AMOUNTFLAPS;
          int roundedStep = (int)preciseStep;
          missedSteps = missedSteps + ((float)preciseStep - (float)roundedStep);
          if (missedSteps > 1) {
            roundedStep = roundedStep + 1;
            missedSteps--;
          }
          stepper->step(ROTATIONDIRECTION * roundedStep);
        }
      } else {
        // full rotation is needed, good time for a calibration
#ifdef SERIAL_ENABLE
        printf("full rotation incl. calibration\n");
#endif
        calibrate(false);  // calibrate revolver and do not stop motor
        // startMotor();
        stepper->setSpeed(stepperSpeed);
        for (int i = 0; i < posLetter; i++) {
          float preciseStep = (float)STEPS / (float)AMOUNTFLAPS;
          int roundedStep = (int)preciseStep;
          missedSteps = missedSteps + (float)preciseStep - (float)roundedStep;
          if (missedSteps > 1) {
            roundedStep = roundedStep + 1;
            missedSteps--;
          }
          stepper->step(ROTATIONDIRECTION * roundedStep);
        }
      }
      // store new position
      displayedLetter = toLetter;
      // rotation is done, stop the motor
      delay(100);  // important to stop rotation before shutting of the motor to
                   // avoid rotation after switching off current
      stopMotor();
    } else {
#ifdef SERIAL_ENABLE
      printf("letter unknown, go to space\n");
#endif
      desiredLetter = 0;
    }
  }
}

int oldOffset = 0;

void receiveLetter(uint8_t reg, uint8_t length) {
  printf("reg %d over i2c, length %d\n", reg, length);

  switch (reg) {
    case 0x01:
      receivedNumber = i2c_registers[reg];
      printf("set recv number to %d\n", receivedNumber);
      break;
    case 0x02:
      oldOffset = calOffset;
      calOffset = i2c_registers[reg];
      if (calOffset != oldOffset) {
        doCalibrate = true;
        printf("set cal offset to %d\n", calOffset);
      }
      break;
    default:
      break;
  }
  return;
}

void requestEvent(uint8_t reg) {
  printf("reading value over i2c %d\n", reg);

  switch (reg) {
    case 0x03:
      printf("rotating over i2c = %d\n", currentlyrotating);
      i2c_registers[reg] = currentlyrotating;
      break;
    default:
      break;
  }
  return;
}

// returns the adress of the unit as int from 0-15
int getaddress() {
  int address = !digitalRead(ADRESSSW4) + (!digitalRead(ADRESSSW3) * 2) +
                (!digitalRead(ADRESSSW2) * 4) + (!digitalRead(ADRESSSW1) * 8);
  return address;
}

// doing a calibration of the revolver using the hall sensor
int calibrate(bool initialCalibration) {
#ifdef SERIAL_ENABLE
  printf("calibrate revolver\n");
#endif
  currentlyrotating = 1;  // set active state to active
  bool reachedMarker = false;
  stepper->setSpeed(stepperSpeed);
  int i = 0;
  while (!reachedMarker) {
    int currentHallValue = digitalRead(HALLPIN);
    if (currentHallValue == 1 && i == 0) {  // already in zero position move out
                                            // a bit and do the calibration {
      // not reached yet
      i = 50;
      stepper->step(ROTATIONDIRECTION *
                    50);  // move 50 steps to get out of scope of hall
    } else if (currentHallValue == 1) {
      // not reached yet
      stepper->step(ROTATIONDIRECTION * 1);
    } else {
      // reached marker, go to calibrated offset position
      reachedMarker = true;
      stepper->step(ROTATIONDIRECTION * calOffset);
      displayedLetter = 0;
      missedSteps = 0;
#ifdef SERIAL_ENABLE
      printf("revolver calibrated\n");
#endif
      // Only stop motor for initial calibration
      if (initialCalibration) {
        stopMotor();
      }
      return i;
    }
    if (i > 3 * STEPS) {
      // seems that there is a problem with the marker or the sensor. turn off
      // the motor to avoid overheating.
      displayedLetter = 0;
      desiredLetter = 0;
      reachedMarker = true;
#ifdef SERIAL_ENABLE
      printf("calibration revolver failed\n");
#endif
      stopMotor();
      return -1;
    }
    i++;
  }
  return i;
}

// switching off the motor driver
void stopMotor() {
  lastInd1 = digitalRead(STEPPERPIN1);
  lastInd2 = digitalRead(STEPPERPIN2);
  lastInd3 = digitalRead(STEPPERPIN3);
  lastInd4 = digitalRead(STEPPERPIN4);

  digitalWrite(STEPPERPIN1, LOW);
  digitalWrite(STEPPERPIN2, LOW);
  digitalWrite(STEPPERPIN3, LOW);
  digitalWrite(STEPPERPIN4, LOW);
#ifdef SERIAL_ENABLE
  printf("Motor Stop\n");
#endif
  currentlyrotating = 0;  // set active state to not active
  delay(100);
}

void startMotor() {
#ifdef SERIAL_ENABLE
  printf("Motor Start\n");
#endif
  currentlyrotating = 1;  // set active state to active
  digitalWrite(STEPPERPIN1, lastInd1);
  digitalWrite(STEPPERPIN2, lastInd2);
  digitalWrite(STEPPERPIN3, lastInd3);
  digitalWrite(STEPPERPIN4, lastInd4);
}
