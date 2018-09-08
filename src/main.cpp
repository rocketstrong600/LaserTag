#include <Arduino.h>
#include <MilesTag.h>
MilesTagTX mttx;
MilesTagRX mtrx;

void setup() {
}

void loop() {
  mttx.fireShot(1, 21, 100);
  mtrx.BufferPull();
  delay(500);
}
