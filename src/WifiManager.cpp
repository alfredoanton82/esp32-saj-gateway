// =================================================================================================
// esp32-saj-gateway: Copyright 2023 by Alfredo M. Anton
//               MIT license - see license.md for details
// =================================================================================================
#include <WifiManager.h>

namespace WifiManager {

  WiFiClient wifiClient;
  TickerScheduler wifiTicker(1);

  // List of callbacks
  std::vector<onWifiEventFunction> onConnectCallbacks;
  std::vector<onWifiEventFunction> onDisconnectCallbacks;

}

using namespace WifiManager;

// =====================================================
// WiFi event handler
// =====================================================
void wifiEvent(arduino_event_id_t event, arduino_event_info_t info)
{
  Serial.printf(" WiFi Event: %d\n", event);

  switch(event) 
  {
    case ARDUINO_EVENT_WIFI_STA_CONNECTED:
      Serial.println("WiFi connected");
      wifiOnLed();
      wifiTicker.disable(0);
      break;

    case ARDUINO_EVENT_WIFI_STA_GOT_IP:
      Serial.printf("WiFi IP: %s\n", WiFi.localIP().toString().c_str());
      for (auto &cb : onConnectCallbacks) cb();
      break;

    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
      Serial.println("WiFi lost connection");
      wifiOffLed();
      for (auto &cb : onDisconnectCallbacks) cb();
      wifiTicker.enable(0);
      break;

    default:
      break;
  }
}

// =====================================================
// Setup WiFi
// =====================================================
void wifiSetup()
{
  Serial.printf("Setting up Wifi: %s\n", WIFI_SSID);

  wifiSetupLed();

  WiFi.mode(WIFI_STA);

  if (WIFI_STATIC_IP) {
    WiFi.config(WIFI_IP_ADDR, WIFI_GATEWAY, WIFI_SUBNET, WIFI_P_DNS, WIFI_S_DNS);
  }

  WiFi.onEvent(wifiEvent);

  WiFi.setAutoReconnect(false);

  wifiTicker.add(0, WIFI_RECONNECT, [&](void*) { wifiConnect(); }, nullptr, false);
  wifiTicker.disableAll();
}

// =====================================================
// LED Setup using new ledcAttach() API
// =====================================================
void wifiSetupLed()
{
  // Attach GPIO to PWM using new simplified API
  ledcAttach(WIFI_GPIO, WIFI_PWM_F, WIFI_PWM_R);
}

// =====================================================
// LED controls
// =====================================================
void wifiBlinkLed()
{
  ledcWrite(WIFI_GPIO, WIFI_PWM_D);
  delay(50);
  ledcWrite(WIFI_GPIO, 0);
}

void wifiOnLed()
{
  ledcWrite(WIFI_GPIO, WIFI_PWM_D);
}

void wifiOffLed()
{
  ledcWrite(WIFI_GPIO, 0);
}

// =====================================================
// Start WiFi reconnect ticker
// =====================================================
void wifiStart()
{
  wifiTicker.enable(0);
}

// =====================================================
// Connect to WiFi
// =====================================================
void wifiConnect()
{
  Serial.printf("Connecting to Wifi: %s\n", WIFI_SSID);

  WiFi.setSleep(false);
  WiFi.persistent(false);

  wifiBlinkLed();

  WiFi.begin(WIFI_SSID, WIFI_PSWD);
}

// =====================================================
// Register callbacks
// =====================================================
void wifiAddOnConnectCallback(onWifiEventFunction callback)
{
  onConnectCallbacks.push_back(callback);
}

void wifiAddOnDisconnectCallback(onWifiEventFunction callback)
{
  onDisconnectCallbacks.push_back(callback);
}

// =====================================================
// WiFi ticker update (call inside loop)
// =====================================================
void wifiLoop()
{
  wifiTicker.update();
}
