#include "COSystem.h"
#include "config.h"


COSystem::COSystem() {
  //Serial.println("Init system");
}

String COSystem::deviceName() {
  return String(DEFAULT_DEVICE_NAME);
}

void COSystem::setDeviceName(String deviceName) {
  
}

String valueForKey(COSystemEEPROMKey key) {
  
}

void saveValueForKey(String value, COSystemEEPROMKey key) {
  
}

size_t freePersistentMemory() {
  
}

