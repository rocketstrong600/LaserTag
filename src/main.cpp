#include <Arduino.h>
#include <MilesTag.h>
#include <IRremote.h>
MilesTagTX mttx;
bool fired = true;
int shot_timer = 0;
unsigned long previousMillis = 0;

void setup() {
  pinMode(4, INPUT_PULLUP);
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= 160) {
    fired = false;
  }
  if (digitalRead(4) == LOW && !fired) {
    previousMillis = currentMillis;
    mttx.fireShot(1, 21, 100);
    fired = true;
  }
}
