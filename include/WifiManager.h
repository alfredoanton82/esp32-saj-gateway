// =================================================================================================
// WifiManager.h
// esp32-saj-gateway: Copyright 2023 by Alfredo M. Anton
// MIT license - see license.md for details
// =================================================================================================
#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include <TickerScheduler.h>

#include <vector>

#include "LedManager.h"

// WIFI Configuration
const char WIFI_SSID[] = "";
const char WIFI_PSWD[] = "!";

// Network configuration
const bool      WIFI_STATIC_IP = false;
const IPAddress WIFI_IP_ADDR(10, 4, 4, 70);
const IPAddress WIFI_GATEWAY(10, 4, 4, 1);
const IPAddress WIFI_SUBNET(255, 255, 255, 0);
const IPAddress WIFI_P_DNS(10, 4, 4, 4);
const IPAddress WIFI_S_DNS(10, 4, 4, 4);

// Wifi reconnect timer
const long WIFI_RECONNECT = 5000L;
const long WIFI_TIMEOUT   = 5000L;

// LED configuration
const int WIFI_GPIO  = 18;
const int WIFI_PWM_C = 0;    // Channel
const int WIFI_PWM_F = 1000; // 1 KHz
const int WIFI_PWM_R = 8;    // 8 bits
const int WIFI_PWM_D = 25;   // Duty

// Callback definition
typedef std::function<void()> onWifiEventFunction;

// Functions
void wifiSetup();

void wifiStart();
void wifiConnect();

// Header (WifiManager.h)
void wifiOnEvent(arduino_event_id_t event, arduino_event_info_t info);
void wifiAddOnConnectCallback(onWifiEventFunction callback);
void wifiAddOnDisconnectCallback(onWifiEventFunction callback);

void wifiLoop();
