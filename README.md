# BLEDataTransfer
Set of libraries for iOS (Swift), Android and BLE enabled Arduino based devices

## Transfer protocol
BLE notification messages are limited to 20 bytes for each packet. This protocol defines communication rules. The size
limit for message is 2^32 (4 294 967 296) characters.

### The packets

#### First packet
1. `4B` starting sequence (4x symbol `0xCE`)
2. `4B` message length (4 bytes integer)
3. `12B` md5 of previous 8B

#### Second packet
1. `20B` md5 of message to transfer

#### Third to n-th packet
1. `20B` of data

If message length isn't modulo 20, the unused last packet's bytes are filled by null terminator (`0x00)`).