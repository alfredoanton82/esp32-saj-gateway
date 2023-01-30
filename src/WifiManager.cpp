// =================================================================================================
// esp32-saj-gateway: Copyright 2023 by Alfredo M. Anton
//               MIT license - see license.md for details
// =================================================================================================
#include<WifiManager.h>

namespace WifiManager {

  // Initialize the MQTT library
  WiFiClient wifiClient;

  // Ticker scheduler
  TickerScheduler wifiTicker(1);

  // List of callbacks
  std::vector<onWifiEventFunction> onConnectCallbacks;
  std::vector<onWifiEventFunction> onDisconnectCallbacks;

}

using namespace WifiManager;

void wifiSetup() {

  Serial.printf("Setting up Wifi: %s\n", WIFI_SSID);

  wifiSetupLed();

  WiFi.mode(WIFI_STA);
  if (WIFI_STATIC_IP) {
    WiFi.config(WIFI_IP_ADDR, WIFI_GATEWAY, WIFI_SUBNET, WIFI_P_DNS, WIFI_S_DNS);
  }
  WiFi.onEvent(wifiOnEvent);
  WiFi.setAutoConnect(false);
  WiFi.setAutoReconnect(false);

  wifiTicker.add(0, WIFI_RECONNECT,  [&](void *) { wifiConnect(); }, nullptr, false); 
  wifiTicker.disableAll();

}

void wifiSetupLed() {
  ledcSetup(WIFI_PWM_C, WIFI_PWM_F, WIFI_PWM_R);
  ledcAttachPin(WIFI_GPIO, WIFI_PWM_C);
}

void wifiBlinkLed() {
  ledcWrite(WIFI_PWM_C, WIFI_PWM_D);
  delay(20);
  ledcWrite(WIFI_PWM_C, LOW);
}

void wifiOnLed() {
  ledcWrite(WIFI_PWM_C, WIFI_PWM_D);
}

void wifiOffLed() {
  ledcWrite(WIFI_PWM_C, LOW);
}

void wifiStart() {
  wifiTicker.enable(0);
}

void wifiConnect() {

  // Connecting to a WiFi network
  Serial.printf("Connecting to Wifi: %s\n", WIFI_SSID);

  WiFi.setSleep(false);
  WiFi.persistent(false);

  wifiBlinkLed();

  WiFi.begin(WIFI_SSID, WIFI_PSWD);
  WiFi.waitForConnectResult(WIFI_TIMEOUT);
  
}

void wifiOnEvent(WiFiEvent_t event)
{

  Serial.printf(" WiFi Event : %i\n", event);

  switch (event) 
  {
  case SYSTEM_EVENT_STA_GOT_IP:

    Serial.printf("WiFi connected : %s\n", WiFi.localIP().toString());
    
    for(auto callback : onConnectCallbacks) {
      callback();
    }

    break;
  case SYSTEM_EVENT_STA_CONNECTED:

    Serial.printf("WiFi connected : %s\n", WiFi.localIP().toString());

    wifiTicker.disable(0);
    wifiOnLed();

    break;
  case SYSTEM_EVENT_STA_LOST_IP:
  case SYSTEM_EVENT_STA_DISCONNECTED:

    Serial.printf("WiFi lost connection\n");
    for(auto callback : onDisconnectCallbacks) {
      callback();
    }

    wifiOffLed();    
    delay(WIFI_RECONNECT);

    wifiTicker.enable(0);

    break;
  case SYSTEM_EVENT_WIFI_READY:
  case SYSTEM_EVENT_SCAN_DONE:
  case SYSTEM_EVENT_STA_START:
  case SYSTEM_EVENT_STA_STOP:
  case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
  case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
  case SYSTEM_EVENT_STA_WPS_ER_FAILED:
  case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
  case SYSTEM_EVENT_STA_WPS_ER_PIN:
  case SYSTEM_EVENT_STA_WPS_ER_PBC_OVERLAP:
  case SYSTEM_EVENT_AP_START:
  case SYSTEM_EVENT_AP_STOP:
  case SYSTEM_EVENT_AP_STACONNECTED:
  case SYSTEM_EVENT_AP_STADISCONNECTED:
  case SYSTEM_EVENT_AP_STAIPASSIGNED:
  case SYSTEM_EVENT_AP_PROBEREQRECVED:
  case SYSTEM_EVENT_GOT_IP6:
  case SYSTEM_EVENT_ETH_START:
  case SYSTEM_EVENT_ETH_STOP:
  case SYSTEM_EVENT_ETH_CONNECTED:
  case SYSTEM_EVENT_ETH_DISCONNECTED:
  case SYSTEM_EVENT_ETH_GOT_IP:
  case SYSTEM_EVENT_MAX:
    break;
  }
}

void wifiAddOnConnectCallback(onWifiEventFunction callback) {
  onConnectCallbacks.push_back(callback);
}

void wifiAddOnDisconnectCallback(onWifiEventFunction callback) {
  onDisconnectCallbacks.push_back(callback);
}

void wifiLoop(){
  wifiTicker.update();
}
