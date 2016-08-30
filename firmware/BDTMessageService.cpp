#include "BDTMessageService.h"

#define STARTING_SYMBOL 0xCE
#define PACKET_LENGTH 20

bool BDTMessageService::BDTMessageService::isStartingPacket(uint8_t *buffer) {

    for(uint8_t i = 0; i < 4; i++) {
        if(buffer[i] != STARTING_SYMBOL) {
            return false;
        }
    }
    return true;

}

bool BDTMessageService::BDTMessageService::processFirstPacket(uint8_t *buffer, uint16_t size) {
    //debug("started processing 1st packet", 0);
    Serial.println("processing 1st packet START");
    // 4th - 8th byte message length
    this->recievedMsgLen = (this->recievedMsgLen << 8) + buffer[7];
    this->recievedMsgLen = (this->recievedMsgLen << 8) + buffer[6];
    this->recievedMsgLen = (this->recievedMsgLen << 8) + buffer[5];
    this->recievedMsgLen = (this->recievedMsgLen << 8) + buffer[4];


    // 9th to 20th byte md5 of previous 8 bytes
    uint8_t md5len = PACKET_LENGTH - 8;
    char md5hash[md5len];
    for(uint8_t i = 0; i < md5len; i++) {
        md5hash[i] = buffer[i+8];
    }

    // compute md5 and compare
    char char_arr[9];
    for(uint8_t i = 0; i < 8; i++) {
        if(buffer[i] == 0x00) {
            char_arr[i] = 0x30;
        } else {
            char_arr[i] = buffer[i];
        }
    }
    char_arr[8] = 0x0;
    unsigned char* computedHash = BDTMD5::make_hash(char_arr);
    char *md5computed = BDTMD5::make_digest(computedHash, 12);

    if(!this->areEqual(md5computed, md5hash, 12)) {
        //debug("Invalid md5", 0);
        return false;
    }

    free(computedHash);
    free(md5computed);
    //debug("finished 1st packed", 0);
    Serial.println("Processing 1st packet END");
    return true;

}

bool BDTMessageService::BDTMessageService::processSecondPacket(uint8_t *buffer, uint16_t size) {

    for(uint8_t i = 0; i < PACKET_LENGTH; i++) {
        this->recievedMessageMd5[i] = buffer[i];
    }

    return true;
}

void BDTMessageService::BDTMessageService::addToMessage(uint8_t *buffer, uint16_t size) {

    for(uint8_t i = 0; i < size; i++) {
        this->messageBuffer[i+this->actualMsgLen] = buffer[i];
    }
    this->actualMsgLen += size;

}

void BDTMessageService::BDTMessageService::reset() {
    // debug("Message reset", 0);
    Serial.println("MSG reset");
    this->recievedMsgLen = 0;
    this->actualMsgLen = 0;
    this->firstPacketComplete = false;
    this->secondPacketComplete = false;
    this->messageComplete = false;
    this->messageValid = false;

    if(this->messageBuffer != NULL)
        free(this->messageBuffer);
    Serial.println("MSG reset end");
    //debug("End reset", 0);
}



BDTMessageService::BDTMessageService::BDTMessageService(){
    Serial.println("YOP");
    this->recievedMsgLen = 0;
    this->actualMsgLen = 0;
    this->recievedMessageMd5 = new char[PACKET_LENGTH];
    this->firstPacketComplete = false;
    this->secondPacketComplete = false;
    this->messageComplete = false;
    this->messageValid = false;
}

BDTMessageService::BDTMessageService::~BDTMessageService(){
    if(this->messageBuffer != NULL)
        free(this->messageBuffer);
}

bool BDTMessageService::BDTMessageService::isMessageComplete() {
    return this->messageComplete;
}

bool BDTMessageService::BDTMessageService::BDTMessageService::isMessageValid() {
    return this->messageValid;
}

char* BDTMessageService::BDTMessageService::getMessageBuffer() {
    return this->messageBuffer;
}

bool BDTMessageService::BDTMessageService::areEqual(char *hash1, char *hash2, uint8_t size) {
    for(uint8_t i = 0; i < size; i++) {
        if(hash1[i] != hash2[i]) {
            return false;
        }
    }
    return true;
}

bool BDTMessageService::BDTMessageService::processMessage(uint8_t *buffer, uint16_t size) {

    if(this->isStartingPacket(buffer)) {
        // debug("This is starting packet", 0);
        Serial.println("This is 1st packet");
        this->reset();
        //debug("HOHOO", 0);
        this->firstPacketComplete = this->processFirstPacket(buffer, size);

    } else {

        // process second packet
        if(this->firstPacketComplete) {
            //debug("Processing second packet", 0);
            Serial.println("Processing second packet");
            this->secondPacketComplete = this->processSecondPacket(buffer, size);

            this->firstPacketComplete = false;
        }
            // process third to n-th data packets
        else {

            if(this->secondPacketComplete) {
                // allocate memory

                this->messageBuffer = (char *) malloc(this->recievedMsgLen);
                if(this->messageBuffer == NULL) {
                    //debug("Can't allocate the memory", 0);
                    return false;
                }

                this->secondPacketComplete = false;
            }

            this->addToMessage(buffer, size);
            if(this->actualMsgLen > this->recievedMsgLen) {
                //debug("Invalid message length", 0);
                return false;
            }

            if(this->actualMsgLen == this->recievedMsgLen) {
                this->messageComplete = true;

                // add null terminator to message
                char *msgWithNull = (char *) malloc(this->recievedMsgLen + 1);
                if(msgWithNull == NULL) {
                    //debug("Can't allocate the memory", 0);
                    return false;
                }
                strcpy(msgWithNull,this->messageBuffer);
                msgWithNull[this->recievedMsgLen] = 0x0;

                unsigned char* hash2 = BDTMD5::make_hash(msgWithNull);
                char *md5str2 = BDTMD5::make_digest(hash2, 20);
                this->messageValid = this->areEqual(this->recievedMessageMd5, md5str2,PACKET_LENGTH);

                free(hash2);
                free(md5str2);
                free(msgWithNull);
            }


        }

    }

    return true;

}

void BDTMessageService::BDTMessageService::sendStringOnParts(String string, uint16_t msg_handler) {

    if (string.length() > 0 && ble.attServerCanSendPacket()) {

        const uint8_t max_packet_len = 20;
        const unsigned int strLen = string.length();

        //debug("Message len: %d", strLen);
        //debug(string, 0);
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
        //debug("First packet sent", 0);

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
        //debug("Second packet sent", 0);
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
            //debug("data packet sent", 0);
        }
        free(hash2);
        free(hash1);
        free(md5str2);
        free(md5str1);

    }
    else {
        //debug("BLE can't send data", 0);
    }
}

