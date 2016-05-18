#include "Packeteer.h"

Packeteer::Packeteer() {
  
}

PacketeerData Packeteer::dataFromStringObject(String stringObject, char identifier[4]) {
  PacketeerData data = PacketeerData();
  data.initialData[0] = '>';
  int8_t len = stringObject.length();
  char charArray[len];
  stringObject.toCharArray(charArray, len);
  uint8_t noPackets = ceil(len / 20);
  data.initialData[1] = noPackets;
  for (int i = 0; i < 4; i++) {
    char ch = identifier[i];
    data.identifier[i] = ch;
    data.initialData[(i + 2)] = ch;
  }
  return data;
}

