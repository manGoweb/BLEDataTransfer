#include <stdio.h>
#include "BDTMD5.h"
#include "BDTMessageService.h"


#if defined(ARDUINO)
SYSTEM_MODE(SEMI_AUTOMATIC);
#endif

#define MIN_CONN_INTERVAL          0x0028 // 50ms.
#define MAX_CONN_INTERVAL          0x0190 // 500ms.
#define SLAVE_LATENCY              0x0000 // No slave latency.
#define CONN_SUPERVISION_TIMEOUT   0x03E8 // 10s.

#define BLE_PERIPHERAL_APPEARANCE  BLE_APPEARANCE_UNKNOWN

#define BLE_DEVICE_NAME            "BLE_Peripheral"

// Length of characteristic value.
#define CHARACTERISTIC1_MAX_LEN    15
#define CHARACTERISTIC2_MAX_LEN    15
#define TXRX_BUF_LEN               15

/******************************************************
                 Variable Definitions
 ******************************************************/
// Primary service 128-bits UUID
static uint8_t service1_uuid[16] = { 0x71, 0x3d, 0x00, 0x00, 0x50, 0x3e, 0x4c, 0x75, 0xba, 0x94, 0x31, 0x48, 0xf1, 0x8d, 0x94, 0x1e };
// Characteristics 128-bits UUID
static uint8_t service1_tx_uuid[16] = { 0x71,0x3d,0x00,0x03,0x50,0x3e,0x4c,0x75,0xba,0x94,0x31,0x48,0xf1,0x8d,0x94,0x1e };
static uint8_t service1_rx_uuid[16] = { 0x71,0x3d,0x00,0x02,0x50,0x3e,0x4c,0x75,0xba,0x94,0x31,0x48,0xf1,0x8d,0x94,0x1e };

// GAP and GATT characteristics value
static uint8_t  appearance[2] = {
  LOW_BYTE(BLE_PERIPHERAL_APPEARANCE),
  HIGH_BYTE(BLE_PERIPHERAL_APPEARANCE)
};

static uint8_t  change[4] = {
  0x00, 0x00, 0xFF, 0xFF
};

static uint8_t  conn_param[8] = {
  LOW_BYTE(MIN_CONN_INTERVAL), HIGH_BYTE(MIN_CONN_INTERVAL),
  LOW_BYTE(MAX_CONN_INTERVAL), HIGH_BYTE(MAX_CONN_INTERVAL),
  LOW_BYTE(SLAVE_LATENCY), HIGH_BYTE(SLAVE_LATENCY),
  LOW_BYTE(CONN_SUPERVISION_TIMEOUT), HIGH_BYTE(CONN_SUPERVISION_TIMEOUT)
};


static advParams_t adv_params = {
  .adv_int_min   = 0x0030,
  .adv_int_max   = 0x0030,
  .adv_type      = BLE_GAP_ADV_TYPE_ADV_IND,
  .dir_addr_type = BLE_GAP_ADDR_TYPE_PUBLIC,
  .dir_addr      = {0, 0, 0, 0, 0, 0},
  .channel_map   = BLE_GAP_ADV_CHANNEL_MAP_ALL,
  .filter_policy = BLE_GAP_ADV_FP_ANY
};

// BLE peripheral advertising data
static uint8_t adv_data[] = {
  0x02,
  BLE_GAP_AD_TYPE_FLAGS,
  BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE,

  0x11,
  BLE_GAP_AD_TYPE_128BIT_SERVICE_UUID_COMPLETE,
  0x1e, 0x94, 0x8d, 0xf1, 0x48, 0x31, 0x94, 0xba, 0x75, 0x4c, 0x3e, 0x50, 0x00, 0x00, 0x3d, 0x71
};

// BLE peripheral scan respond data
static uint8_t scan_response[] = {
  0x0F,
  BLE_GAP_AD_TYPE_COMPLETE_LOCAL_NAME,
  'V', 'Y',  'J', 'E', 'T', 'A', '-', 'P','O','K','L','I','C','E'
};


static uint16_t conn_handle = 0xFFFF;

// Characteristic value handle
static uint16_t character1_handle = 0x0000;
static uint16_t character2_handle = 0x0000;

// Buffer of characterisitc value.
static uint8_t characteristic1_data[CHARACTERISTIC1_MAX_LEN] = { 0x01 };
static uint8_t characteristic2_data[CHARACTERISTIC2_MAX_LEN] = { 0x01 };

static btstack_timer_source_t characteristic2;

char rx_buf[TXRX_BUF_LEN];
static uint8_t rx_buf_num;
static uint8_t rx_state = 0;

BDTMessageService msgService = BDTMessageService();

static void deviceConnectedCallback(BLEStatus_t status, uint16_t handle) {
  switch (status) {
    case BLE_STATUS_OK:
      Serial.print("BLE device connection established! Connection handle: ");
      Serial.println(handle, HEX);
      conn_handle = handle;
      break;
    default: 
      Serial.println("Failed to establish connection with peer device!");
      break;
  }
}

static void deviceDisconnectedCallback(uint16_t handle) {
  Serial.println("Disconnected.");
}


int gattWriteCallback(uint16_t value_handle, uint8_t *buffer, uint16_t size) {
  Serial.print("Write value handler: ");
  Serial.println(value_handle, HEX);

  if (character1_handle == value_handle) {
      Serial.print(" - Message | ");
      if (msgService.processMessage(buffer, size)) {
        Serial.print("There is a valid message: ");
        Serial.println(msgService.currentMsg);
      }
  
    /*
    memcpy(characteristic1_data, buffer, size);
    Serial.print("Characteristic1 write value: ");
    for (uint8_t index = 0; index < size; index++) {
      Serial.write(characteristic1_data[index]);

      Serial.print(" ");
    }*/
    Serial.println(" ");
  }
  
  return 0;
}

static void  characteristic2_notify(btstack_timer_source_t *ts) {   
  if (Serial.available()) {
    //read the serial command into a buffer
    uint8_t rx_len = min(Serial.available(), CHARACTERISTIC2_MAX_LEN);
    Serial.readBytes(rx_buf, rx_len);
    //send the serial command to the server
    Serial.print("Sent: ");
    Serial.println(rx_buf);
    rx_state = 1;
  }
  if (rx_state != 0) {
    ble.sendNotify(character2_handle, (uint8_t*)rx_buf, CHARACTERISTIC2_MAX_LEN);
    memset(rx_buf, 0x00, 20);
    rx_state = 0;
  }
  // reset
  ble.setTimer(ts, 200);
  ble.addTimer(ts);
}


//trimms string if too long and sends it in messages to connected device





/**
   @brief Setup.
*/
void setup() {
  Serial.begin(115200);
  delay(5000);
  Serial.println("BLE peripheral demo.");

  // Initialize ble_stack.
  ble.init();

  ble.onConnectedCallback(deviceConnectedCallback);
  ble.onDisconnectedCallback(deviceDisconnectedCallback);
  ble.onDataWriteCallback(gattWriteCallback);

   
  ble.addService(BLE_UUID_GAP);
  ble.addCharacteristic(BLE_UUID_GAP_CHARACTERISTIC_DEVICE_NAME, ATT_PROPERTY_READ, (uint8_t*)BLE_DEVICE_NAME, sizeof(BLE_DEVICE_NAME));
  ble.addCharacteristic(BLE_UUID_GAP_CHARACTERISTIC_APPEARANCE, ATT_PROPERTY_READ, appearance, sizeof(appearance));
  ble.addCharacteristic(BLE_UUID_GAP_CHARACTERISTIC_PPCP, ATT_PROPERTY_READ, conn_param, sizeof(conn_param));

  ble.addService(BLE_UUID_GATT);
  ble.addCharacteristic(BLE_UUID_GATT_CHARACTERISTIC_SERVICE_CHANGED, ATT_PROPERTY_INDICATE, change, sizeof(change));

  ble.addService(service1_uuid);
  character1_handle = ble.addCharacteristicDynamic(service1_tx_uuid, ATT_PROPERTY_NOTIFY|ATT_PROPERTY_WRITE|ATT_PROPERTY_WRITE_WITHOUT_RESPONSE, characteristic1_data, CHARACTERISTIC1_MAX_LEN);
  character2_handle = ble.addCharacteristicDynamic(service1_rx_uuid, ATT_PROPERTY_NOTIFY, characteristic2_data, CHARACTERISTIC2_MAX_LEN);




  // Set BLE advertising parameters
  ble.setAdvertisementParams(&adv_params);

  // Set BLE advertising and scan respond data
  ble.setAdvertisementData(sizeof(adv_data), adv_data);
  ble.setScanResponseData(sizeof(scan_response), scan_response);

  // Start advertising.
  ble.startAdvertising();
  Serial.println("BLE start advertising.");

  // set one-shot timer
  characteristic2.process = &characteristic2_notify;
  ble.setTimer(&characteristic2, 500);//100ms
  ble.addTimer(&characteristic2);
  

}

/**
   @brief Loop.
*/
void loop() {


      delay(5000);
      msgService.sendStringOnParts("Ahoj tohle je very first message timle podivnym peklostrojem! Dlouha jako krava a plna podivnych znaku {\"json\":\"adasdasasdasasdasd\"}", character2_handle);


}
