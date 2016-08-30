
#ifndef BDTMessageService_h
#define BDTMessageService_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#elif defined(SPARK)
#include "application.h"
#endif

#include <string.h>
#include "BDTMD5.h"



class BDTMessageService {

    bool isStartingPacket(uint8_t *buffer);

    bool processFirstPacket(uint8_t *buffer, uint16_t size);

    bool processSecondPacket(uint8_t *buffer, uint16_t size);

    void addToMessage(uint8_t *buffer, uint16_t size);

    void reset();

    unsigned int recievedMsgLen;
    unsigned int actualMsgLen;
    char *recievedMessageMd5;
    bool firstPacketComplete;
    bool secondPacketComplete;
    bool messageComplete;
    bool messageValid;
    char *messageBuffer;


public:
    BDTMessageService();

    ~BDTMessageService();

    bool isMessageComplete();

    bool isMessageValid();

    char* getMessageBuffer();

    static bool areEqual(char *hash1, char *hash2, uint8_t size);

    bool processMessage(uint8_t *buffer, uint16_t size);

    void sendStringOnParts(String string, uint16_t msg_handler);

};

#endif