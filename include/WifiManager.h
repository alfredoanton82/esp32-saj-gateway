// =================================================================================================
// esp32-saj-gateway: Copyright 2023 by Alfredo M. Anton
//               MIT license - see license.md for details
// =================================================================================================
#include <Arduino.h>
#include <WiFi.h>
#include <TickerScheduler.h>

#include <vector>

#include<ESP32SAJGatewayLogging.h>

// WIFI Configuration
const char WIFI_SSID[] = "";
const char WIFI_PSWD[] = "";

// Network configuration
const bool      WIFI_STATIC_IP = false;
const IPAddress WIFI_IP_ADDR(10, 4, 4, 70);
const IPAddress WIFI_GATEWAY(10, 4, 4, 1);
const IPAddress WIFI_SUBNET(255, 255, 255, 0);
const IPAddress WIFI_P_DNS(10, 4, 4, 4);
const IPAddress WIFI_S_DNS(10, 4, 4, 4);

// Wifi reconnect timer 
const long WIFI_RECONNECT = 5000L;
const long WIFI_TIMEOUT = 5000L;

// Callback definition
typedef std::function<void()> onWifiEventFunction;

// Functions
void wifiSetup();
void wifiStart();
void wifiConnect();
void wifiOnEvent(WiFiEvent_t event);
void wifiAddOnConnectCallback(onWifiEventFunction callback);
void wifiAddOnDisconnectCallback(onWifiEventFunction callback);
void wifiLoop();
