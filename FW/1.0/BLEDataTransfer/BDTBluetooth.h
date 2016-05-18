
#ifndef BDTBluetooth_h
#define BDTBluetooth_h


#include "COSensors.h"
#include "CONetwork.h"
#include "Packeteer.h"
#include "BDTMD5.h"


extern COSensors sensors;
extern CONetwork net;


#define SKIP_UNCOMMON_DATA_STEPS        30

#define SWITCH_DATA_MAX_LEN             3
#define NOTIFICATION_DATA_MAX_LEN       3
#define BATTERY_DATA_MAX_LEN            2
#define TEMPERATURE_DATA_MAX_LEN        7
#define HUMIDITY_DATA_MAX_LEN           3
#define LIGHTING_DATA_MAX_LEN           3
#define ACCELEROMETER_DATA_MAX_LEN      3
#define CONNECTION_DATA_MAX_LEN         20
#define SETTINGS_DATA_MAX_LEN           1

#define DIGITAL_OUT_PIN                 D7
//#define SERVO_PIN                       D4
#define ANALOG_IN_PIN                   A5

//Servo                                   myservo;

/******************************************************
 *               Variable Definitions
 ******************************************************/
static uint8_t service1_uuid[16]                  = {0x71,0x3d,0x00,0x00,0x50,0x3e,0x4c,0x75,0xba,0x94,0x31,0x48,0xf1,0x8d,0x94,0x1e};
static uint8_t switchDataUUID[16]                 = {0x71,0x3d,0x00,0x03,0x50,0x3e,0x4c,0x75,0xba,0x94,0x31,0x48,0xf1,0x8d,0x94,0x1e};
static uint8_t notificationsUUID[16]              = {0x71,0x3d,0x00,0x02,0x50,0x3e,0x4c,0x75,0xba,0x94,0x31,0x48,0xf1,0x8d,0x94,0x1e};
static uint8_t temperatureNotificationsUUID[16]   = {0x73,0x3d,0x00,0x02,0x50,0x3e,0x4c,0x75,0xba,0x94,0x31,0x48,0xf1,0x8d,0x94,0x1e};
static uint8_t humidityNotificationsUUID[16]      = {0x74,0x3d,0x00,0x02,0x50,0x3e,0x4c,0x75,0xba,0x94,0x31,0x48,0xf1,0x8d,0x94,0x1e};
static uint8_t lightingNotificationsUUID[16]      = {0x75,0x3d,0x00,0x02,0x50,0x3e,0x4c,0x75,0xba,0x94,0x31,0x48,0xf1,0x8d,0x94,0x1e};
static uint8_t accelerometerNotificationsUUID[16] = {0x76,0x3d,0x00,0x02,0x50,0x3e,0x4c,0x75,0xba,0x94,0x31,0x48,0xf1,0x8d,0x94,0x1e};
static uint8_t connectionNotificationsUUID[16]    = {0x77,0x3d,0x00,0x02,0x50,0x3e,0x4c,0x75,0xba,0x94,0x31,0x48,0xf1,0x8d,0x94,0x1e};
static uint8_t settingsNotificationsUUID[16]    = {0x77,0x3d,0x00,0x02,0x50,0x3e,0x4c,0x75,0xba,0x94,0x31,0x48,0xf1,0x8d,0x94,0x1e};

static uint8_t  appearance[1]    = {0x180};
static uint8_t  change[2]        = {0x00, 0x00};
static uint8_t  conn_param[8]    = {0x28, 0x00, 0x90, 0x01, 0x00, 0x00, 0x90, 0x01};

static uint16_t deviceNameHandler     = 0x0000;
static uint16_t batteryHandler        = 0x0000;
static uint16_t switchHandler         = 0x0000;
static uint16_t notificationHandler   = 0x0000;
static uint16_t temperatureHandler    = 0x0000;
static uint16_t humidityHandler       = 0x0000;
static uint16_t lightingHandler       = 0x0000;
static uint16_t accelerometerHandler  = 0x0000;
static uint16_t connectionHandler     = 0x0000;
static uint16_t settingsHandler       = 0x0000;

static uint8_t switchData[SWITCH_DATA_MAX_LEN] = {0x01};
static uint8_t notificationsData[NOTIFICATION_DATA_MAX_LEN] = {0x00};
static uint8_t temperatureData[TEMPERATURE_DATA_MAX_LEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static uint8_t batteryData[BATTERY_DATA_MAX_LEN] = {0x00, 0x00};
static uint8_t connectionData[CONNECTION_DATA_MAX_LEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static uint8_t settingsData[SETTINGS_DATA_MAX_LEN] = {0x00};

static btstack_timer_source_t notificationTimer;


static advParams_t adv_params;
static uint8_t adv_data[] = {0x02,0x01,0x06,0x08,0x08,'m','a','n','G','o','B','T',0x11,0x07,0x1e,0x94,0x8d,0xf1,0x48,0x31,0x94,0xba,0x75,0x4c,0x3e,0x50,0x00,0x00,0x3d,0x71};

static uint8_t currentSendDataSkipStep = 0;

/******************************************************
 *               Function Definitions
 ******************************************************/

//trimms string if too long and sends it in messages to connected device
void sendStringOnParts(String string) {
    if (string.length() > 0 && ble.attServerCanSendPacket()) {

      const uint8_t max_msg_len = 20;
      const uint8_t strLen =  string.length();
      char charString[strLen];
      string.toCharArray(charString, strLen);
      
      //send info message
      unsigned char* hash = BDTMD5::make_hash(charString);
      char *md5str = BDTMD5::make_digest(hash, 16);
//      Serial.println(md5str);
      
      uint8_t infoMsg[max_msg_len] ;
      infoMsg[0] = 0xce;
      for (uint8_t i = 4; i < max_msg_len; i++) {
        infoMsg[i] = md5str[max_msg_len-i];
      }
      ble.sendNotify(notificationHandler, infoMsg, 20);
      free(md5str);
      
      //send data messages
      bool notFinished = true;
      uint8_t m = 0;
      
      while (notFinished) {
        uint8_t msg[max_msg_len] = {};
        uint16_t len = 0;
        
        for (uint8_t n = 0; n < max_msg_len;  n++) {
        
          if (strLen < ((m*max_msg_len)+n) ) {
            notFinished = false;
            break;
          }
          else {
            msg[n] = charString[(m*max_msg_len)+n];
          }
            len++;
        }
        m++;
//        Serial.write("-----------------");
//        Serial.write(msg, len);
        ble.sendNotify(notificationHandler, msg, len);
      }
      Serial.println("BLE whole string sent");
    }
    else {
      Serial.println("BLE can't send data");
    }
}


// Receiving data

void processNameData(uint8_t *buffer) {
  
}

void processSwitchData(uint8_t *buffer) {
  Serial.print("Data: ");
  Serial.println(switchData[0]);
  // Process the data
  if (switchData[0] == 0x01) { // Command is to control digital out pin
      if (switchData[1] == 1) {
          digitalWrite(DIGITAL_OUT_PIN, HIGH);
      }
      else {
          digitalWrite(DIGITAL_OUT_PIN, LOW);
      }
  }
//  else if (switchData[0] == 0x03) { // Command is to control Servo pin
//      myservo.write(switchData[1]);
//  }
//  else if (switchData[0] == 0x04) {
//      myservo.write(0);
//      analogWrite(PWM_PIN, 0);
//      digitalWrite(DIGITAL_OUT_PIN, LOW);
//  }
}

// BLE Callbacks
 
void deviceConnectedCallback(BLEStatus_t status, uint16_t handle) {
    switch (status) {
        case BLE_STATUS_OK:
            Serial.println("Device connected!");
            break;
        default:
            break;
    }

    // TODO: Clear PIN which will be requested to write
}

void deviceDisconnectedCallback(uint16_t handle) {
    Serial.println("Disconnected.");

    // TODO: Logout PIN
}

int gattWriteCallback(uint16_t value_handler, uint8_t *buffer, uint16_t size) {
    Serial.print("Write value handler: ");
    Serial.print(value_handler, HEX);

    if (deviceNameHandler == value_handler) {
      Serial.print(" - Device name");
      
      processNameData(buffer);
    }
    else if (switchHandler == value_handler) {
      Serial.print(" - Switch");
      uint16_t index;
      //memcpy(characteristic1_data, buf, size);
      Serial.print("Value: ");
      for (uint8_t index = 0; index < size; index++) {
          Serial.print(buffer[index], HEX);
          Serial.print(" ");
      }
      Serial.println(" ");
      
      processSwitchData(buffer);
    }
    Serial.println("");
    return 0;
}

// Sending data

void sendSensorInfo() {
  if (ble.attServerCanSendPacket()) {
    //uint16_t value = sensors.getInternalTemperature();
    uint16_t value = analogRead(ANALOG_IN_PIN);
    notificationsData[0] = (0x0B);
    notificationsData[1] = (value >> 8000);
    notificationsData[2] = (value);
    ble.sendNotify(notificationHandler, notificationsData, NOTIFICATION_DATA_MAX_LEN);
  }
}

void sendSwitchInfo() {
  if (ble.attServerCanSendPacket()) {
    switchData[0] = (0x00);
    switchData[1] = (0x01);
    switchData[1] = (0x01);
    ble.sendNotify(switchHandler, switchData, SWITCH_DATA_MAX_LEN);
  }
}

void sendBatteryInfo() {
  if (ble.attServerCanSendPacket()) {
    batteryData[0] = (0x00);
    batteryData[1] = (0x28);
    ble.sendNotify(batteryHandler, batteryData, BATTERY_DATA_MAX_LEN);
  }
}

void sendTemperatureInfo() {
  if (ble.attServerCanSendPacket()) {
    // Table values
    temperatureData[0] = (0x07);
    temperatureData[1] = (0x14);
    temperatureData[2] = (0x00);
    
    // Basic two benches
    temperatureData[3] = (0x13);
    temperatureData[4] = (0x12);
    
    // Additional benches
    temperatureData[5] = (0x00);
    temperatureData[6] = (0x00);
    ble.sendNotify(temperatureHandler, temperatureData, TEMPERATURE_DATA_MAX_LEN);
  }
}

void sendConnectionInfo() {  
  if (ble.attServerCanSendPacket()) {
    String connections = net.availableNetworksString();

    // Sending first packet containing info about the data to follow
    connectionData[0] = 'L';                    // Length identifier
    connectionData[1] = connections.length();   // Length
    connectionData[2] = net.connectionStatus;   // Connection status
    ble.sendNotify(connectionHandler, connectionData, 3);

    // TODO: Split the connections string into 20 bytes packets and send them through
  }
}

// Notifications

static void sendAllNotificationsLoop(btstack_timer_source_t *ts) {
  sendSwitchInfo();
  sendSensorInfo();

  // Send following data only now and than
  if (currentSendDataSkipStep >= SKIP_UNCOMMON_DATA_STEPS) {
    sendBatteryInfo();
    sendTemperatureInfo();
      
    currentSendDataSkipStep++;
  }
  
  // reset
  ble.setTimer(ts, 200);
  ble.addTimer(ts);
}

// Setup

void setupBLE() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("manGocontrol FW");
    //ble.debugLogger(true);
    ble.init();

    ble.onConnectedCallback(deviceConnectedCallback);
    ble.onDisconnectedCallback(deviceDisconnectedCallback);
    ble.onDataWriteCallback(gattWriteCallback);

    // Generic Access
    ble.addService(0x1800);
    deviceNameHandler = ble.addCharacteristic(0x2A00, ATT_PROPERTY_READ|ATT_PROPERTY_WRITE, (uint8_t*)DEFAULT_DEVICE_NAME, sizeof(DEFAULT_DEVICE_NAME));
    ble.addCharacteristic(0x2A01, ATT_PROPERTY_READ, appearance, sizeof(appearance));
    ble.addCharacteristic(0x2A04, ATT_PROPERTY_READ, conn_param, sizeof(conn_param));
    
    // Device info
    ble.addService(0x180A);
    
    static uint8_t revision[5] = {'0', '.', '1', '.', '0'}; // Firmware Revision String
    ble.addCharacteristic(0x2A26, ATT_PROPERTY_READ, revision, sizeof(revision));

    static uint8_t manufacturer[11] = {'m', 'a', 'n', 'G', 'o', 'w', 'e', 'b', '.', 'c', 'z'}; // Manufacturer Name String
    ble.addCharacteristic(0x2A29, ATT_PROPERTY_READ, manufacturer, sizeof(manufacturer));

    // Model Number String
    static uint8_t model[14] = {'M', 'N', 'G', 'C', 'o', 'n', 'n', 'e', 'c', 't', '_', '1', '.', '0'}; // Manufacturer Name String
    ble.addCharacteristic(0x2A24, ATT_PROPERTY_READ, model, sizeof(model));
    
    // Battery
    ble.addService(0x180F);
    static uint8_t battery[1] = {0x64};
    batteryHandler = ble.addCharacteristic(0x2A19, ATT_PROPERTY_NOTIFY, battery, sizeof(battery));

    // Generic Attributes
    ble.addService(0x1801);
    ble.addCharacteristic(0x2A05, ATT_PROPERTY_INDICATE, change, sizeof(change));
    
    ble.addService(service1_uuid);
    switchHandler = ble.addCharacteristicDynamic(switchDataUUID, ATT_PROPERTY_NOTIFY|ATT_PROPERTY_WRITE|ATT_PROPERTY_WRITE_WITHOUT_RESPONSE, switchData, SWITCH_DATA_MAX_LEN);
    notificationHandler = ble.addCharacteristicDynamic(notificationsUUID, ATT_PROPERTY_NOTIFY, notificationsData, NOTIFICATION_DATA_MAX_LEN);
    temperatureHandler = ble.addCharacteristicDynamic(temperatureNotificationsUUID, ATT_PROPERTY_NOTIFY, notificationsData, TEMPERATURE_DATA_MAX_LEN);
    connectionHandler = ble.addCharacteristicDynamic(connectionNotificationsUUID, ATT_PROPERTY_NOTIFY|ATT_PROPERTY_WRITE, connectionData, CONNECTION_DATA_MAX_LEN);
    
    adv_params.adv_int_min = 0x00A0;
    adv_params.adv_int_max = 0x01A0;
    adv_params.adv_type = 0;
    adv_params.dir_addr_type = 0;
    memset(adv_params.dir_addr, 0,6);
    adv_params.channel_map = 0x07;
    adv_params.filter_policy = 0x00;
    
    ble.setAdvParams(&adv_params);
    
    ble.setAdvData(sizeof(adv_data), adv_data);
    
    ble.startAdvertising();

    pinMode(DIGITAL_OUT_PIN, OUTPUT);
    //pinMode(DIGITAL_IN_PIN, INPUT_PULLUP);
    //pinMode(PWM_PIN, OUTPUT);
    
    // Default to internally pull high, change it if you need
    //digitalWrite(DIGITAL_IN_PIN, HIGH);

    //myservo.attach(SERVO_PIN);
    //myservo.write(0);

    // Set one-shot timer
    notificationTimer.process = &sendAllNotificationsLoop;
    ble.setTimer(&notificationTimer, 500); // 2000ms
    ble.addTimer(&notificationTimer);
    
    Serial.println("BLE start advertising.");
}


#endif
