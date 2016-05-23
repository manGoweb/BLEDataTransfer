#ifndef BDTMessageService_h
#define BDTMessageService_h

#include "Arduino.h"

#include <string.h>
#include "BDTMD5.h"

class BDTMessageService
{

public:
	BDTMessageService(){};

  String currentMsg;
  String incommingHash;
 

  bool processMessage(uint8_t *buffer, uint16_t size) {

    String msg = (char*)buffer;

    Serial.println("_╬_");
    for (uint16_t c = 0; c < size; c++) {
      Serial.println(buffer[c]);
    }
    Serial.println(msg.substring(0,1));
    if ( (uint8_t) buffer[0] == (uint8_t) 0xce) {
      Serial.println("pisu hash");
      Serial.println(msg.substring(4,size));
      setIncommingHash(msg.substring(4,size));
    }
    else {
      Serial.println(msg.substring(0,size));
      return addString(msg.substring(0,size));
    }
    return false;
  }

  void setIncommingHash(String hash) {
    incommingHash = hash;
    currentMsg = "";
  }

  bool addString(String str) {
    currentMsg += str;
    return isCurrentMsgValid();
  }

  bool isCurrentMsgValid() {
 
    if (incommingHash == "" || currentMsg.length() == 0) {
      Serial.print(" [hash");
      Serial.print(incommingHash);
      Serial.print("] ");
      return false;
    }
    const uint8_t strLen =  currentMsg.length();
    char charString[strLen];
    currentMsg.toCharArray(charString, strLen+1);
      
    unsigned char* hash = BDTMD5::make_hash(charString);
      
    for (uint8_t i = 0; i < incommingHash.length(); i++ ) {
      if (incommingHash.charAt(i) != hash[1]) { 
        free(hash);
        return false;
      }
    }
      
    free(hash);
    return true;
  }
};

#endif
