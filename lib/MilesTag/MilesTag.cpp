/*
  MilesTag.cpp - Library for infared milestag protical.
  Created by Ethan G. Johnston, April 8, 2018.
  Released into the public domain.
  Using IRremote library
*/

#include "Arduino.h"
#include "MilesTag.h"
#include "IRremote.h"

//transmit code
MilesTagTX::MilesTagTX()
{

}


void MilesTagTX::fireShot(unsigned long teamId, unsigned long playerId, unsigned long dmg)
{
  unsigned long aPlayerId = playerId << 6;
  unsigned long aTeamId = teamId << 4;
  unsigned long aDamage = DamagetoBin(dmg);

  unsigned long Data = aPlayerId | aTeamId;
  Data = Data | aDamage;
  Data = add_parity(Data);
  irTransmit(Data);
}

void MilesTagTX::irTransmit(unsigned long Data) {
  irsend.sendSony(Data, 14);
}

unsigned long MilesTagTX::DamagetoBin(unsigned long dmg) {
  if (dmg >= 100) {
    dmg = 15;
  } else if (dmg >= 75) {
    dmg = 14;
  } else if (dmg >= 50) {
    dmg = 13;
  } else if (dmg >= 40) {
    dmg = 12;
  } else if (dmg >= 35) {
    dmg = 11;
  } else if (dmg >= 30) {
    dmg = 10;
  } else if (dmg >= 25) {
    dmg = 9;
  } else if (dmg >= 20) {
    dmg = 8;
  } else if (dmg >= 17) {
    dmg = 7;
  } else if (dmg >= 15) {
    dmg = 6;
  } else if (dmg >= 10) {
    dmg = 5;
  } else if (dmg >= 7) {
    dmg = 4;
  } else if (dmg >= 5) {
    dmg = 3;
  } else if (dmg >= 4) {
    dmg = 2;
  } else if (dmg >= 2) {
    dmg = 1;
  } else if (dmg >= 1) {
    dmg = 0;
  }
  return dmg;
}

unsigned long MilesTagTX::has_even_parity(unsigned long x){
    unsigned long count = 0, i, b = 1;
    for(i = 0; i < 32; i++){
        if( x & (b << i) ){count++;}
    }
    if( (count % 2) ){return 0;}
    return 1;
}

unsigned long MilesTagTX::add_parity(unsigned long x){
  unsigned long parity = 0;
  if (has_even_parity(x)) {
    parity = 1;
  }
  x = x << 1;
  x = x | parity;
  return x;
}

//Recieve code

MilesTagRX::MilesTagRX()
{
}

MTShotRecieved MilesTagRX::DecodeShotData(unsigned long data) {
  MTShotRecieved decodedData;

  unsigned long dataWp = (data & 0xFFFFFFFE) >> 1;

  unsigned long count = 0, i, b = 1;
  for(i = 0; i < 32; i++){
      if( dataWp & (b << i) ){count++;}
  }
  decodedData.PlayerID = (dataWp & 0x1FC0) >> 6;
  decodedData.TeamID = (dataWp >> 4) & 3;
  decodedData.Damage = BintoDamage(dataWp & 15);
  decodedData.Error = false;
  if (has_even_parity(data)) {
    decodedData.Error = true;
  }
  if (count > 14) {
    decodedData.Error = true;
  }
  if (decodedData.Damage > 100) {
    decodedData.Error = true;
  }
  if (decodedData.TeamID > 3) {
    decodedData.Error = true;
  }
  if (decodedData.PlayerID > 60) {
    decodedData.Error = true;
  }
  if (data > 0x3FFF) {
    decodedData.Error = true;
  }
  return decodedData;
}

unsigned long MilesTagRX::BintoDamage(unsigned long dmg) {
  if (dmg >= 15) {
    dmg = 100;
  } else if (dmg >= 14) {
    dmg = 75;
  } else if (dmg >= 13) {
    dmg = 50;
  } else if (dmg >= 12) {
    dmg = 40;
  } else if (dmg >= 11) {
    dmg = 35;
  } else if (dmg >= 10) {
    dmg = 30;
  } else if (dmg >= 9) {
    dmg = 25;
  } else if (dmg >= 8) {
    dmg = 20;
  } else if (dmg >= 7) {
    dmg = 17;
  } else if (dmg >= 6) {
    dmg = 15;
  } else if (dmg >= 5) {
    dmg = 10;
  } else if (dmg >= 4) {
    dmg = 7;
  } else if (dmg >= 3) {
    dmg = 5;
  } else if (dmg >= 2) {
    dmg = 4;
  } else if (dmg >= 1) {
    dmg = 2;
  } else if (dmg >= 0) {
    dmg = 1;
  }
  return dmg;
}

unsigned long MilesTagRX::has_even_parity(unsigned long x){
    unsigned long count = 0, i, b = 1;
    for(i = 0; i < 32; i++){
        if( x & (b << i) ){count++;}
    }
    if( (count % 2) ){return 0;}
    return 1;
}
