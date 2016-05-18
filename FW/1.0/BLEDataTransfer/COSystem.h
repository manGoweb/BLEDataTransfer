#ifndef COSystem_h
#define COSystem_h


#include "Arduino.h"


typedef enum COSystemEEPROMKey {
  COS_EEPROM_DEVICE_NAME_KEY = 0,
  COS_EEPROM_UUID_KEY,
  COS_EEPROM_LON_LAT_KEY
};


class COSystem {

  public:
    COSystem();

    String deviceName();
    void setDeviceName(String deviceName);
    
    String valueForKey(COSystemEEPROMKey key);
    void saveValueForKey(String value, COSystemEEPROMKey key);

    size_t freePersistentMemory();
    
  private:
    
    
};


#endif
