#ifndef BDTMessageService_h
#define BDTMessageService_h

#include "Arduino.h"

#include <string.h>
#include "BDTMD5.h"

class BDTMessageService
{
public:
	BDTMessageService(){}

  String currentMsg;
  String incommingHash;

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
