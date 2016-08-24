#ifndef BDTMessageService_h
#define BDTMessageService_h

#include "Arduino.h"

#include <string.h>
#include "BDTMD5.h"

class BDTMessageService {

  bool addString(String str) {
    currentMsg += str;
    return isCurrentMsgValid();
  }

  void setIncommingHash(String hash) {
    incommingHash = hash;
    currentMsg = "";
  }

public:
	BDTMessageService(){
	};

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

  bool isCurrentMsgValid() {
 
    if (incommingHash == "" || currentMsg.length() == 0) {
      Serial.print("hash: [");
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

  void sendStringOnParts(String string, uint16_t msg_handler) {

    if (string.length() > 0 && ble.attServerCanSendPacket()) {

      const uint8_t max_packet_len = 20;
      const unsigned int strLen = string.length();

      Serial.print("Message len: ");
      Serial.println(strLen);
      uint8_t infoMsg[max_packet_len] ;
      // 4B starting sequence
      infoMsg[0] = 0xce;
      infoMsg[1] = 0xce;
      infoMsg[2] = 0xce;
      infoMsg[3] = 0xce;

      // 4B message len
      infoMsg[4] = strLen & 0x000000ff;
      infoMsg[5] = (strLen & 0x0000ff00) >> 8;
      infoMsg[6] = (strLen & 0x00ff0000) >> 16;
      infoMsg[7] = (strLen & 0xff000000) >> 24;

      char charArr[9];
      for(uint8_t i = 0; i < 8; i++) {
        if(infoMsg[i] == 0x00) {
          charArr[i] = 0x30;
        } else { 
          charArr[i] = infoMsg[i];
        }
      }

      charArr[8] = 0x0;


      // 12B md5 of prev 8B
      unsigned char* hash1 = BDTMD5::make_hash(charArr);
      char *md5str1 = BDTMD5::make_digest(hash1, 16);
      for (uint8_t i = 8; i < max_packet_len; i++) {
        infoMsg[i] = md5str1[i-8];
      }
      
      ble.sendNotify(msg_handler, infoMsg, max_packet_len);
      
      
      char charArr2[strLen];
      string.toCharArray(charArr2, strLen+1);
      
      //send info message
      unsigned char* hash2 = BDTMD5::make_hash(charArr2);
      char *md5str2 = BDTMD5::make_digest(hash2, 16);
      
      uint8_t infoMsg2[max_packet_len] ;

      for (uint8_t i = 0; i < max_packet_len; i++) {
        infoMsg2[i] = md5str2[i];
      }

      ble.sendNotify(msg_handler, infoMsg2, max_packet_len);
      
      //send data messages
      bool notFinished = true;
      uint16_t m = 0;
      
      while (notFinished) {
        uint8_t msg[max_packet_len] = {};
        uint8_t len = 0;
        
        for (uint8_t n = 0; n < max_packet_len;  n++) {
        
          if (strLen <= ((m*max_packet_len)+n) ) {
            notFinished = false;
            break;
          }
          else {
            msg[n] = charArr2[(m*max_packet_len)+n];
          }
            len++;
        }
        m++;

        ble.sendNotify(msg_handler, msg, len);
      }
      free(hash2);
      free(hash1);
      free(md5str2);
      free(md5str1);

    }
    else {
      Serial.println("BLE can't send data");
    }
}
};

#endif
