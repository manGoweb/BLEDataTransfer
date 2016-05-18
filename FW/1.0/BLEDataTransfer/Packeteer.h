#ifndef Packeteer_h
#define Packeteer_h


#include "Arduino.h"


struct PacketeerData {
  uint8_t totalLength;
  uint8_t dataLength;
  uint8_t identifier[4];
  
  uint8_t initialData[6];
  uint8_t data[];
};


class Packeteer {

  public:
    Packeteer();

    PacketeerData dataFromStringObject(String string, char identifier[4]);
    
  private:
    
    
};


#endif
