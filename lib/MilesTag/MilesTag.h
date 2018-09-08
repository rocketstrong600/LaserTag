/*
  MilesTag.h - Library for infared milestag protical.
  Created by Ethan G. Johnston, April 8, 2018.
  Released into the public domain.
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
  boolean Error;
} MTShotRecieved;

class MilesTagRX
{
  public:
    MilesTagRX();
    MTShotRecieved DecodeShotData(unsigned long data);
    rmt_config_t configRx;
    rmt_item32_t items[15];
  private:
    unsigned long BintoDamage(unsigned long dmg);
    unsigned long has_even_parity(unsigned long x);
};
#endif
