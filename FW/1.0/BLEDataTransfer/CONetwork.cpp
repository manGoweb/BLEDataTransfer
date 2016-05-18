
#import "CONetwork.h"
// TODO: Import bluetooth for updating WiFi status
//#import "BDTBluetooth.h"
#include <ArduinoJson.h>
#import "config.h"


CONetwork::CONetwork() {
  WiFi.on();
}

void CONetwork::updateConnectionStatus(CONetworkConnectionStatus status) {
  connectionStatus = status;

  // TODO: Update WiFi status over BT
  //sendConnectionInfo();
}

bool CONetwork::updateAvailableNetworks() {
  Serial.println("** Scan Networks **");
  
  WiFiAccessPoint aps[30];
  
  int found = WiFi.scan(aps, 30);
  
  if (found <= 0) {
    Serial.println("Couldn't find any WiFi networks");
    availableNetworks = String("[]");
    return false;
  }
  
  // print the list of networks seen:
  Serial.print("Number of available networks:");
  Serial.println(found);
  
  numberOfAvailableNetworks = found;
  
  availableNetworks = String("[");
  
  for (int i = 0; i < found; i++) {
    if (availableNetworks.length() > 1) {
      availableNetworks.concat(",");
    }
    WiFiAccessPoint &ap = aps[i];
    availableNetworks.concat(WiFiApToJSON(ap));
  }
  availableNetworks.concat("]");
  
  return true;
}

String CONetwork::availableNetworksJson() {
  updateAvailableNetworks();
  return availableNetworks;
}

String CONetwork::availableNetworksString() {
  Serial.println("** Scan Networks **");

  WiFiAccessPoint aps[20];
  
  int found = WiFi.scan(aps, 20);
  
  if (found <= 0) {
    Serial.println("Couldn't find any WiFi networks");
    return NULL;
  }

  // print the list of networks seen:
  Serial.print("Number of available networks:");
  Serial.println(found);
  
  String networks = String();
  
  for (int i = 0; i < found; i++) {
    if (networks.length() > 1) {
      networks.concat("&");
    }
    WiFiAccessPoint &ap = aps[i];
    networks.concat(WiFiApToString(ap));
  }

  return networks;
}

String CONetwork::getEncryptionType(int thisType) {
  switch (thisType) {
  case UNSEC:
    return String("UNSEC");
    break;
  case WEP:
    return String("WEP");
    break;
  case WPA:
    return String("WPA");
    break;
  case WPA2:
    return String("WPA2");
    break;
  }
}

int CONetwork::getEncryptionFromType(String security) {
  if (security.equals("WPA2")) {
    return 3;
  }
  else if (security.equals("UNSEC")) {
    return 0;
  }
  else if (security.equals("WPA")) {
    return 2;
  }
  else if (security.equals("WEP")) {
    return 1;
  }
  return -1;
}

String CONetwork::myMacAddress() {
  byte mac[6];
  WiFi.macAddress(mac);

  String macAddr = String(String(mac[5], HEX) + ":" + String(mac[4], HEX) + ":" + String(mac[3], HEX) + ":" + String(mac[2], HEX) + ":" + String(mac[1], HEX) + ":" + String(mac[0], HEX));
  Serial.println(macAddr);
  return macAddr;
}

bool CONetwork::connectToWiFi(CONetworkCredentials credentials) {
  currentConnection = credentials;
  
  WiFi.clearCredentials();
  WiFi.setCredentials(credentials.ssid, credentials.password, credentials.security);
  WiFi.connect();

  IPAddress localIP = WiFi.localIP();

  int retry = 0;
  while (localIP[0] == 0 && retry < WIFI_CONNECTION_RETRIES) {
      localIP = WiFi.localIP();
      Serial.println("Waiting for an IP address");
      delay(1000);

      retry++;
      Serial.println(retry);
      
      // TODO: Send BT update
  }
  if (localIP[0] == 0 || retry == WIFI_CONNECTION_RETRIES) {
    Serial.println("Disconnecting from WiFi");
    //WiFi.disconnect();
    
     // TODO: Send BT update
  }
  else {
    // TODO: Save my credentials
  }

  char addr[16];
  sprintf(addr, "%u.%u.%u.%u", localIP[0], localIP[1], localIP[2], localIP[3]);
  
  Serial.println(addr);
}

String CONetwork::WiFiApToJSON(WiFiAccessPoint ap) {
  String wifiJson = String("{\"ssid\":\"");
  wifiJson.concat(ap.ssid);
  wifiJson.concat("\",");
  
  wifiJson.concat("\"sec\":\"");
  wifiJson.concat(getEncryptionType(ap.security));
  wifiJson.concat("\",");

  wifiJson.concat("\"ch\":");
  wifiJson.concat(ap.channel);
  wifiJson.concat(",");
  
  wifiJson.concat("\"rssi\":");
  wifiJson.concat(ap.rssi);

  wifiJson.concat("}");

  return wifiJson;
}

String CONetwork::WiFiApToString(WiFiAccessPoint ap) {
  String wifiString = String();
  wifiString.concat(ap.ssid);
  wifiString.concat("|");
  wifiString.concat(getEncryptionType(ap.security));
  wifiString.concat("|");
  wifiString.concat(ap.channel);
  wifiString.concat("|");
  wifiString.concat(ap.rssi);
  return wifiString;
}

CONetworkCredentials CONetwork::wifiCredentialsFromJsonString(String credentialsJsonString) {
  CONetworkCredentials credentials;
  
  int len = credentialsJsonString.length();
  char jsonChars[len];
  credentialsJsonString.toCharArray(jsonChars, len);

  StaticJsonBuffer<200> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(jsonChars);

  // TODO: The JSON is not for whatever reason being parsed?!
  Serial.print("JSON: ");
  root.printTo(Serial);
  Serial.println("");
  
  // TODO: Verify the varible exist
  const char* ssid = root["ssid"];
  credentials.ssid = String(ssid);
  const char* password = root["pass"];
  credentials.password = String(password);
  const char* security = root["sec"];
  credentials.security = getEncryptionFromType(String(security));
  
  return credentials;
}

CONetworkCredentials CONetwork::wifiCredentialsFromString(String credentialsString) {
  int separatorIndex = credentialsString.indexOf("\",");
  int secondSeparatorIndex = credentialsString.indexOf("\",\"pass\":");
  int thirdSeparatorIndex = credentialsString.indexOf("\",\"sec\":");
  int endIndex = credentialsString.indexOf("\"}", (separatorIndex + 0));

  CONetworkCredentials credentials;
  credentials.ssid = credentialsString.substring(9, separatorIndex);
  credentials.password = credentialsString.substring((separatorIndex + 10), thirdSeparatorIndex);
  credentials.security = getEncryptionFromType(credentialsString.substring((thirdSeparatorIndex + 9), endIndex));

  return credentials;
}

void CONetwork::printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  
  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

// API services

bool CONetwork::createRequest(String urlPath, String dataJsonString) {
  IPAddress ip = WiFi.localIP();
  if (ip[0] != 0) {
    if (client.connect(SERVER_API_ADDR, SERVER_API_PORT)) {
      Serial.println("Connected to server");
      // Make a HTTP request:
      client.print((dataJsonString.length() > 0) ? REQUEST_TYPE_POST : REQUEST_TYPE_GET);
      client.print(" ");
      client.print(urlPath);
      client.println(" HTTP/1.1");
      client.println("Host: example.com");
      client.println("Connection: close");
      client.println();
      // TODO: How do I pass the response?
      return true;
    }
  }
  return false;
}

bool CONetwork::postData(String dataJsonString, String urlPath) {
  return false;
}

String postDataWithResponse(String dataJsonString, String urlPath) {
  return NULL;
}

bool getData(String dataJsonString, String urlPath) {
  return false;
}

String getDataWithResponse(String dataJsonString, String urlPath) {
  return NULL;
}

