#include <Arduino.h>
#include <Wire.h>

void setup() {
  Wire1.setClock(100000);
  Wire1.begin();  // join i2c bus (address optional for master)
}

void loop() {
  Wire1.beginTransmission(4);
  Wire1.write(10);  // sends one byte
  Wire1.write(20);
  Wire1.endTransmission();  // stop transmitting
  delay(5000);
}
