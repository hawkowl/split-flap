#include <Arduino.h>
#include <Wire.h>

const int FLAPAMOUNT = 53;

const int UNITS = 2;

const char* letters[] = {
    " ", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N",
    "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "Ä", "Ö", "Ü",
    "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", ":", ".", "-", "?", "!"};

const char text[] = {'B', 'R', 'I', 'B', 'D', '!'};
const int textcount = 6;

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
  Serial.printf("sending %c to unit %d\n", letter, target);
  Wire1.beginTransmission(target);
  Wire1.write(10);
  Wire1.write(translateLettertoInt(letter));
  Wire1.endTransmission();
}

void reset() {
  for (int i = 0; i < UNITS; i++) {
    transmitLetter(i + 1, ' ');
  }
}

void setup() {
  Serial.begin();
  Wire1.setClock(100000);
  Wire1.begin();  // join i2c bus (address optional for master)

  delay(2500);

  reset();
  delay(2000);

  Serial.println("off we go");
}

void loop() {
  for (int i = 0; i < UNITS; i++) {
    char letter = ' ';

    Serial.printf("id %d\n", idx + i);

    if (idx + i < textcount) {
      letter = text[idx + i];
    }

    transmitLetter(i + 1, letter);
  }

  idx = idx + UNITS;
  if (idx >= textcount) {
    idx = 0;
    delay(10000);
    reset();
    delay(999999999);
  }
  delay(7500);
}
