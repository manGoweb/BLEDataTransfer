
#ifndef CONetwork_h
#define CONetwork_h


#include "Arduino.h"


#define REQUEST_TYPE_POST     "POST"
#define REQUEST_TYPE_GET      "GET"


typedef struct {
   String ssid;
   int security;
   String password;
} CONetworkCredentials;

typedef enum {
  CONetworkConnectionStatusDisconnected = 0,
  CONetworkConnectionStatusWiFiUnavailable = -1,
  CONetworkConnectionStatusConnected = 1
} CONetworkConnectionStatus;


class CONetwork {

  public:
    CONetwork();

    /*
     * Current connection status
     */
    CONetworkConnectionStatus connectionStatus;

    /*
     * Discovering available WiFi networks
     */
    int numberOfAvailableNetworks = 0;
    String availableNetworks = String("[]");

    bool updateAvailableNetworks();
    String availableNetworksJson();                                             // Format is [{"ssid":"Network","sec":"UNSEC","ch":11,"rssi":-59}]
    String availableNetworksString();                                           // Format is "SSID|SEC|CH|RSSI&..."

    /*
     * Get my MAC address
     */
    String myMacAddress();                                                      // Only returns a valid MAC if connected to a WiFi

    /*
     * Connecting to WiFi
     */
    bool connectToWiFi(CONetworkCredentials credentials);

    /*
     * Object conversions
     */
    String WiFiApToJSON(WiFiAccessPoint ap);                                    // Format is {"ssid":"Network","sec":"UNSEC","ch":11,"rssi":-59}
    String WiFiApToString(WiFiAccessPoint ap);                                  // Format is "SSID|SEC|CH|RSSI"
    CONetworkCredentials wifiCredentialsFromJsonString(String credentialsJsonString);      // Format is {"ssid":"Network","pass":"pass","sec":"WPA2"}, no spaces! no pretty printed JSON!
    CONetworkCredentials wifiCredentialsFromString(String credentialsString);   // Format is "SSID|PASSWORD|SECURITY"

    /*
     * Print info about currently connected network
     */
    void printWifiStatus();

    /*
     * API communication
     */
    bool createRequest(String urlPath, String dataJsonString = NULL);
    
    bool postData(String dataJsonString, String urlPath);
    String postDataWithResponse(String dataJsonString, String urlPath);
    
    bool getData(String urlPath);
    String getDataWithResponse(String urlPath);

  private:

    TCPClient client;

    void updateConnectionStatus(CONetworkConnectionStatus status);
    
    String getEncryptionType(int thisType);
    int getEncryptionFromType(String security);

    CONetworkCredentials currentConnection;
    
};


#endif
