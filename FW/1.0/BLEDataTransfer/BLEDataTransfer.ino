#if defined(ARDUINO) 
SYSTEM_MODE(MANUAL); //do not connect to cloud
#else
SYSTEM_MODE(AUTOMATIC); //connect to cloud
#endif


#include "config.h"
#include "COSystem.h"
#include "CONetwork.h"
#include "COSensors.h"
#include "BDTBluetooth.h"
#include "BDTMD5.h"
#include "BDTMessageService.h"


//BDTBluetooth bt = BDTBluetooth();
COSensors sensors = COSensors();
CONetwork net = CONetwork();

void setup() {
  Serial.begin(115200);
  delay(500);

  setupBLE();
}

void loop() {
  //sensors.getExternalTemperature(0);

  RGB.control(true);
  RGB.color(0, 255, 0);
  delay(500);
  RGB.color(255, 255, 255);
  delay(500);
  RGB.color(0, 0, 255);

  // this should be deleted after testing
  sendStringOnParts("This should be deleted after testing of sending messages via bluetooth");
  delay(5000);


  
  //net.myMacAddress();

//  net.updateAvailableNetworks();
//  Serial.println(net.availableNetworks);
//  
//  //CONetworkCredentials credentials = net.netCredentialsFromString(String("{\"ssid\":\"Ondrej's iPhone 6 Plus\",\"pass\":\"aaaaaaaa\",\"sec\":\"WPA2\"}"));
//  //CONetworkCredentials credentials = net.netCredentialsFromString(String("{\"ssid\":\"Virgin Media net\",\"pass\":\"\",\"sec\":\"UNSEC\"}"));
//    // "{\"ssid\":\"Rafiki Wi-Fi Network\",\"pass\":\"\aaaaaaaa\",\"sec\":\"UNSEC\"}"
//  char creds[100] = "{\"ssid\": \"Rafiki Wi-Fi Network\",\"pass\": \"\aaaaaaaa\",\"sec\":\"UNSEC\"}";
//  Serial.print("Network: ");
//  Serial.println(creds);
//  //Serial.println(String(creds));
//  CONetworkCredentials credentials = net.wifiCredentialsFromJsonString(String(creds));
//  
//  Serial.print("Connecting to: '");
//  Serial.print(credentials.ssid);
//  Serial.print("' - '");
//  Serial.print(credentials.password);
//  Serial.print("' - '");
//  Serial.print(credentials.security);
//  Serial.println("'");
//  
//  net.connectToWiFi(credentials);

//  COSensorsAccelerometerData data = sensors.getAccelerometerData();
//  
//  Serial.print("X:");
//  Serial.print(data.x);
//  Serial.print("; Y:");
//  Serial.print(data.y);
//  Serial.print("; Z:");
//  Serial.println(data.z);

  

  delay(500);
  RGB.control(false);
  
}
