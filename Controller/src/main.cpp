#include <Arduino.h>
#include <Wire.h>

#include "SafeString.h"
#include "SafeStringReader.h"

const int FLAPAMOUNT = 53;

const int CALIBRATION_VALS[] = {
    73,
    73,
    70,
};

const int UNITS = 3;

const char* letters[] = {
    " ", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N",
    "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "Ä", "Ö", "Ü",
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ".", "-", "?", "!"};

String text = "";
int textcount = 0;

createSafeStringReader(sfReader, 10, "\r\n");

int idx = 0;

// translates char to letter position
int translateLettertoInt(char letterchar) {
  for (int i = 0; i < FLAPAMOUNT; i++) {
    if (letterchar == *letters[i]) {
      return i;
    }
  }
  return -1;
}

void transmitLetter(int target, char letter) {
  Serial.printf(
      "UNIT %d: sending %c to unit @ speed 10, calibration offset %d\n", target,
      letter, CALIBRATION_VALS[target - 1]);
  Wire1.beginTransmission(target);
  Wire1.write(0x02);
  Wire1.write(CALIBRATION_VALS[target - 1]);
  Wire1.endTransmission();

  Wire1.beginTransmission(target);
  Wire1.write(0x01);
  Wire1.write(translateLettertoInt(letter));
  Wire1.endTransmission();

  Wire1.flush();
}

bool getTurning(int target) {
  Serial.printf("UNIT %d: checking turning\n", target);

  byte out;

  Wire1.beginTransmission(target);
  Wire1.write(0x03);
  Wire1.endTransmission();

  Wire1.requestFrom(target, 0x01);
  while (Wire1.available()) {
    out = Wire1.read();
  }

  Serial.printf("UNIT %d: got %d\n", target, out);
  return out == 0;
}

void waitForStopped() {
  // Wait for it to settle
  delay(250);

  for (int i = 1; i < UNITS + 1; i++) {
    while (!getTurning(i)) {
      delay(250);
    }
  }
  Serial.printf("all units stopped!\n");
}

void reset() {
  for (int i = 0; i < UNITS; i++) {
    transmitLetter(i + 1, ' ');
  }
}

void setup() {
  Serial.begin();

  SafeString::setOutput(Serial);
  sfReader.connect(Serial);
  sfReader.echoOn();

  Wire1.setClock(100000);
  Wire1.begin();

  delay(2500);

  reset();
  waitForStopped();

  delay(5000);

  Serial.println("off we go");
}

void loop() {
  if (sfReader.read()) {
    textcount = sfReader.length();
    text = sfReader.c_str();
    text.toUpperCase();
  }

  for (int i = 0; i < UNITS; i++) {
    char letter = ' ';

    if (idx + i < textcount) {
      letter = text[idx + i];
    }

    transmitLetter(i + 1, letter);
  }
  waitForStopped();

  if (textcount > 0) {
    idx = idx + UNITS;
    if (idx >= textcount) {
      idx = 0;
      delay(7500);
      reset();
      textcount = 0;
    } else {
      delay(7500);
    }
  }
}
