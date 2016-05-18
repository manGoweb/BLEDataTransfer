#include "COSensors.h"


COSensors::COSensors() {
    dallas.begin();
    dht.begin();
    accelerometer2.init();
}

void COSensors::loop() {
    Serial.println("----------------------------------");
    getExternalTemperature(0);
    getInternalTemperature();
    getHumidity();
}

float COSensors::getExternalTemperature(int index) {
    float v;
    unsigned long t = (millis() - lastUpdateExternalTemperatures);
    if (lastUpdateExternalTemperatures == 0 || t > (REFRESH_RATE * 1000)) {
        Serial.print("Requesting temperatures ... ");
        dallas.requestTemperatures(); // Send the command to get temperatures
        Serial.println("DONE");
        
        v = dallas.getTempCByIndex(index); // Why "byIndex"? You can have more than one IC on the same bus. 0 refers to the first IC on the wire
        if (isnan(v) || v <= DEVICE_DISCONNECTED_C) {
            Serial.printf("Failed to read from sensor %d (devs: %d; isnan(%d))", index, dallas.getDeviceCount(), isnan(v));
            Serial.println("!\n");
            return 0;
        }
        else {
            int count = dallas.getDeviceCount();
            if (count == 0) {
                externalTemperatures[0] = v;
            }
            else {
                for (int i = 0; i < dallas.getDeviceCount(); i++) {
                    externalTemperatures[i] = dallas.getTempCByIndex(i);
                }
            }
            lastUpdateExternalTemperatures = millis();
        }
    }
    else {
        Serial.println("Reading from cache");
        v = externalTemperatures[index];
    }
    Serial.print("Temperature for sensor ");
    Serial.print(index);
    Serial.print(" is: ");
    Serial.print(v);
    Serial.println(" ˚C\n");
    return v;
}

float COSensors::getInternalTemperature() {
    float v;
    unsigned long t = (millis() - lastUpdateInternalTemperature);
    if (lastUpdateInternalTemperature == 0 || t > (REFRESH_RATE * 1000)) {
        v = dht.readTemperature();
        if (isnan(v)) {
            Serial.println("Failed to read temperature from DHT sensor!");
            return 0;
        }
        else {
            internalTemperature = v;
            lastUpdateInternalTemperature = millis();
        }
    }
    else {
        //Serial.println("Reading from cache");
        v = internalTemperature;
    }
    //  Serial.print("Temperature: ");
    //  Serial.print(v);
    //  Serial.println(" ˚C\n");
    return v;
}

float COSensors::getHumidity() {
    float v;
    unsigned long t = (millis() - lastUpdateHumidity);
    if (lastUpdateHumidity == 0 || t > (REFRESH_RATE * 1000)) {
        v = dht.readHumidity();
        if (isnan(v)) {
            Serial.println("Failed to read humidity from DHT sensor!");
            return 0;
        }
        else {
            humidity = v;
            lastUpdateHumidity = millis();
        }
    }
    else {
        Serial.println("Reading from cache");
        v = humidity;
    }
    Serial.print("Humidity: ");
    Serial.print(v);
    Serial.println("%\n");
    return v;
}

COSensorsAccelerometerData COSensors::getAccelerometerData() {
    int8_t x, y, z;
    accelerometer2.getXYZ(&x,&y,&z);
    
    COSensorsAccelerometerData data = COSensorsAccelerometerData();
    data.x = (x * 8);
    data.y = (y * 8);
    data.z = (z * 8);
    return data;
}

void COSensors::refreshData() {
    Serial.println("----------------------------------");
    
    lastUpdateExternalTemperatures = 0;
    lastUpdateInternalTemperature = 0;
    lastUpdateHumidity = 0;
    
    getExternalTemperature(0);
    getInternalTemperature();
    getHumidity();
}

