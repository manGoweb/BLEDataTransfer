/*
 * Configuration file
 * 
 */

#ifndef config_h
#define config_h


#define DEFAULT_DEVICE_NAME                 "Cooler Table"

 
// Server configuration

// Diagnostics
#define DIGITAL_OUT_PIN             D7      // Pin for diagnostic LED

// Sensors
#define REFRESH_RATE                2      // Sensor data refresh rate in seconds
#define DHT22_DATA_PIN              D6      // Pin for DHT22 sensor
#define DALLAS_DATA_PIN             D2      // Pin for DallasTemperature (one wire) sensor

// Server reporting
#define SERVER_API_ADDR             "api.seriousconcrete.com"
#define SERVER_API_PORT             80

// WiFi
#define WIFI_CONNECTION_RETRIES     5

//// Bluetooth
//#define CHARACTERISTIC1_MAX_LEN     3
//#define CHARACTERISTIC2_MAX_LEN     3

// Motors
#define MOTOR_PAN_MAX_SPEED         100
#define MOTOR_ROTATE_MAX_SPEED      100


#endif
