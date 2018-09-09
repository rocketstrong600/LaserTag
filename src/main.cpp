#include <Arduino.h>
#include <MilesTag.h>
MilesTagTX mttx;
MilesTagRX mtrx;

void setup() {
  Serial.begin(115200);
  delay(500);
  // teamid, playerid, Damage.
  mttx.fireShot(1, 21, 100);
  delay(500);
  mtrx.BufferPull();
  Serial.println(mtrx.Hits[0].Error);
  Serial.println(mtrx.Hits[0].PlayerID);
  Serial.println(mtrx.Hits[0].TeamID);
  Serial.println(mtrx.Hits[0].Damage);
  mtrx.ClearHits();
}

void loop() {

}
