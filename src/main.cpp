#include <Arduino.h>
#include <MilesTag.h>
MilesTagTX mttx;
MilesTagRX mtrx;

void setup() {
    Serial.begin(115200);
}

void loop() {
  delay(500);
  // teamid, playerid, Damage.
  mttx.fireShot(1, 21, 100);
  mttx.fireShot(1, 21, 75);
  delay(2000);
  mtrx.BufferPull();
  Serial.print("H1 E: ");
  Serial.print(mtrx.Hits[0].Error);
  Serial.print(" P: ");
  Serial.print(mtrx.Hits[0].PlayerID);
  Serial.print(" T: ");
  Serial.print(mtrx.Hits[0].TeamID);
  Serial.print(" D: ");
  Serial.println(mtrx.Hits[0].Damage);

  Serial.print("H2 E: ");
  Serial.print(mtrx.Hits[1].Error);
  Serial.print(" P: ");
  Serial.print(mtrx.Hits[1].PlayerID);
  Serial.print(" T: ");
  Serial.print(mtrx.Hits[1].TeamID);
  Serial.print(" D: ");
  Serial.println(mtrx.Hits[1].Damage);
  mtrx.ClearHits();
}
