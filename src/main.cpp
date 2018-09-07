#include <Arduino.h>
#include <MilesTag.h>
MilesTagTX mttx;

void setup() {
}

void loop() {
  mttx.fireShot(1, 21, 100);
  delay(500);
}
