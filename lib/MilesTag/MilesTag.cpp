/*
Copyright <2018> <Ethan Johnston>

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "Arduino.h"
#include "MilesTag.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include <driver/rmt.h>

#define IR_PIN  GPIO_NUM_17
#define REC_PIN  GPIO_NUM_16

#define DEBUG_SCALE 1
#define CDEBUG 0

#define HEADER_US 2400
#define SPACE_US 600
#define ONE_US 1200
#define ZERO_US 600
#define OFFSET 100


//transmit code
MilesTagTX::MilesTagTX(uint32_t freq=56)
{
  // put your setup code here, to run once:
  configTx.rmt_mode = RMT_MODE_TX;
  configTx.channel = RMT_CHANNEL_0;
  configTx.gpio_num = IR_PIN;
  configTx.mem_block_num = 1;
  configTx.tx_config.loop_en = 0;
  configTx.tx_config.carrier_duty_percent = 50;
  configTx.tx_config.carrier_freq_hz = freq*1000;
  configTx.tx_config.carrier_en = CDEBUG;
  configTx.tx_config.idle_output_en = 1;
  configTx.tx_config.idle_level = RMT_IDLE_LEVEL_LOW;
  configTx.tx_config.carrier_level = RMT_CARRIER_LEVEL_HIGH;
  configTx.clk_div = 80; // 80MHx / 80 = 1MHz 0r 1uS per count

  rmt_config(&configTx);
  rmt_driver_install(configTx.channel, 0, 0);  //  rmt_driver_install(rmt_channel_t channel, size_t rx_buf_size, int rmt_intr_num)

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

void MilesTagTX::irTransmit(unsigned long data) {
  int nbits = 14;
  //header
  items[0].duration0 = HEADER_US*DEBUG_SCALE;
  items[0].level0 = 1;
  items[0].duration1 = SPACE_US*DEBUG_SCALE;
  items[0].level1 = 0;

	// Data
  int i = 1;
	for (unsigned long  mask = 1UL << (nbits - 1);  mask;  mask >>= 1) {
		if (data & mask) {
      items[i].duration0 = ONE_US*DEBUG_SCALE;
      items[i].level0 = 1;
      items[i].duration1 = SPACE_US*DEBUG_SCALE;
      items[i].level1 = 0;
		} else {
      items[i].duration0 = ZERO_US*DEBUG_SCALE;
      items[i].level0 = 1;
      items[i].duration1 = SPACE_US*DEBUG_SCALE;
      items[i].level1 = 0;
    }
    i++;
  }
  rmt_write_items(configTx.channel, items, 15, 1);
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
  // put your setup code here, to run once:
  configRx.rmt_mode = RMT_MODE_RX;
  configRx.channel = RMT_CHANNEL_1;
  configRx.gpio_num = REC_PIN;
  configRx.mem_block_num = 1;
  configRx.rx_config.filter_en = true;
  configRx.rx_config.filter_ticks_thresh = 200;
  configRx.rx_config.idle_threshold = HEADER_US + OFFSET;
  configRx.clk_div = 80; // 80MHx / 80 = 1MHz 0r 1uS per count

  rmt_config(&configRx);
  rmt_driver_install(configRx.channel, 1000, 0);
  rmt_rx_start(RMT_CHANNEL_1, 1);
}

void MilesTagRX::ClearHits() {
  for (int i = 0; i < 20; i++) {
    Hits[i].PlayerID = 0;
    Hits[i].Damage = 0;
    Hits[i].TeamID = 0;
    Hits[i].Error = true;
  }
  HitCount = 0;
}

void MilesTagRX::BufferPull() {
  unsigned long data = 0;
  RingbufHandle_t rb = NULL;
  rmt_get_ringbuf_handle(RMT_CHANNEL_1, &rb);
  while(rb) {
      size_t rx_size = 0;
      rmt_item32_t* item = (rmt_item32_t*) xRingbufferReceive(rb, &rx_size, 1000);
      if (item) {
        rmt_item32_t* itemproc = item;
        for(size_t i=0; i < (rx_size / 4); i++) {
          if(itemproc->duration0 < (HEADER_US+OFFSET) && itemproc->duration0 > (HEADER_US-OFFSET)) {
            for(int i=14; i >= 1; i--) {
              if (itemproc[i].duration0 < (ONE_US+OFFSET) && itemproc[i].duration0 > (ONE_US-OFFSET)) {
                data = data | 1 << (14 - i);
              } else if(itemproc[i].duration0 < (ZERO_US+OFFSET) && itemproc[i].duration0 > (ZERO_US-OFFSET)) {
                data = data | 0 << (14 - i);
              }
            }
            Hits[HitCount] = DecodeShotData(data);
            HitCount++;
            data = 0;
          }
          ++itemproc;
        }
        vRingbufferReturnItem(rb, (void*) item);
      } else {
        break;
      }
    }
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
  unsigned long dmgarray[16] = {1, 2, 4, 5, 7, 10, 15, 17, 20, 25, 30, 35, 40, 50, 75, 100};
  return dmgarray[dmg];
}

unsigned long MilesTagRX::has_even_parity(unsigned long x){
    unsigned long count = 0, i, b = 1;
    for(i = 0; i < 32; i++){
        if( x & (b << i) ){count++;}
    }
    if( (count % 2) ){return 0;}
    return 1;
}
