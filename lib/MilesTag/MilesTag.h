/*
Copyright <2018> <Ethan Johnston>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#ifndef MilesTag_h
#define MilesTag_h

#include "Arduino.h"
#include <driver/rmt.h>

class MilesTagTX
{
  public:
    MilesTagTX();
    void fireShot(unsigned long teamId, unsigned long playerId, unsigned long dmg);
    rmt_config_t configTx;
    rmt_item32_t items[15];
  private:
    void irTransmit(unsigned long Buffer);
    unsigned long DamagetoBin(unsigned long dmg);
    unsigned long has_even_parity(unsigned long x);
    unsigned long add_parity(unsigned long x);
};

typedef struct MTShotRecieved {
  unsigned long PlayerID;
  unsigned long TeamID;
  unsigned long Damage;
  bool Error = true;
} MTShotRecieved;

class MilesTagRX
{
  public:
    MilesTagRX();
    MTShotRecieved DecodeShotData(unsigned long data);
    void ClearHits();
    void BufferPull();
    rmt_config_t configRx;
    MTShotRecieved Hits[20];
    int Datacount;
  private:
    unsigned long BintoDamage(unsigned long dmg);
    unsigned long has_even_parity(unsigned long x);
};
#endif
