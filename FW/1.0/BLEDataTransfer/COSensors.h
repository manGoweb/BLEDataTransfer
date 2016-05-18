
#ifndef COSensors_h
#define COSensors_h


#include "config.h"
#include <DHT.h>
#include <DallasTemperature.h>
#include <HMC5883L.h>
#include <MMA7660.h>


struct COSensorsAccelerometerData 
{
  int8_t x = 0;
  int8_t y = 0;
  int8_t z = 0;
};


class COSensors {

  public:
    COSensors();

    void loop();

    /*
     * Reading DallasTemperatures sensors
     * Note: For multiple sensores on the same wire select it's index
     */
    float getExternalTemperature(int index);
    
    /*
     * Reading enviromental temperature using DHT22 (AM2302)
     */
    float getInternalTemperature();
    
    /*
     * Reading humidity using DHT22 (AM2302)
     */
    float getHumidity();

    /*
     * Reading accelerometer values
     */
    COSensorsAccelerometerData getAccelerometerData(); 

    /*
     * Refresh all sensor data
     */
    void refreshData();

  private:

    // Caching
    unsigned long lastUpdateExternalTemperatures = 0;
    float externalTemperatures[];

    unsigned long lastUpdateInternalTemperature = 0;
    float internalTemperature = 0;

    unsigned long lastUpdateHumidity = 0;
    float humidity = 0;

    // DHT22
    DHT dht = DHT(DHT22_DATA_PIN, DHT22); // Pin (defined in config) for DHT22 (AM2302)

    // DallasTemperatures

    OneWire oneWire = OneWire(DALLAS_DATA_PIN);
    DallasTemperature dallas = DallasTemperature(&oneWire);
    HMC5883L accelerometer = HMC5883L();
    MMA7660 accelerometer2;
    
};


#endif
