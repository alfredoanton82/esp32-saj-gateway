// =================================================================================================
// esp32-saj-gateway: Copyright 2023 by Alfredo M. Anton
//               MIT license - see license.md for details
// =================================================================================================

#include <WifiManager.h>

namespace WifiManager {

  // Initialize the WiFi client (for MQTT or other network usage)
  WiFiClient wifiClient;

  // Ticker scheduler for manual WiFi reconnect
  TickerScheduler wifiTicker(1);

  // Lists of registered callbacks
  std::vector<onWifiEventFunction> onConnectCallbacks;
  std::vector<onWifiEventFunction> onDisconnectCallbacks;

}

using namespace WifiManager;

// =====================================================
// WiFi Setup
// =====================================================
void wifiSetup() {
  Serial.printf("Setting up WiFi: %s\n", WIFI_SSID);

  // Setup LED for WiFi status
  wifiSetupLed();

  // Configure WiFi as Station
  WiFi.mode(WIFI_STA);

  // Configure static IP if needed
  if (WIFI_STATIC_IP) {
    WiFi.config(WIFI_IP_ADDR, WIFI_GATEWAY, WIFI_SUBNET, WIFI_P_DNS, WIFI_S_DNS);
  }

  // Register event handlers for modern Arduino ESP32 WiFi events
  WiFi.onEvent(wifiOnEvent, ARDUINO_EVENT_WIFI_STA_GOT_IP);
  WiFi.onEvent(wifiOnEvent, ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

  // Disable automatic reconnect; we manage reconnection manually
  WiFi.setAutoReconnect(false);

  // Configure reconnect ticker
  wifiTicker.add(0, WIFI_RECONNECT, [&](void *) { wifiConnect(); }, nullptr, false); 
  wifiTicker.disableAll();
}

// =====================================================
// LED Setup for WiFi
// =====================================================
void wifiSetupLed() {
  // Attach the LED pin to a PWM channel using new ledcAttach
  ledcAttach(WIFI_GPIO, WIFI_PWM_F, WIFI_PWM_R);
}

// =====================================================
// LED Controls
// =====================================================
void wifiBlinkLed() {
  ledcWrite(WIFI_GPIO, WIFI_PWM_D); // Half or full brightness
  delay(20);
  ledcWrite(WIFI_GPIO, 0);          // Turn off
}

void wifiOnLed() {
  ledcWrite(WIFI_GPIO, WIFI_PWM_D); // Full brightness
}

void wifiOffLed() {
  ledcWrite(WIFI_GPIO, 0);          // Off
}

// =====================================================
// Start WiFi reconnect ticker
// =====================================================
void wifiStart() {
  wifiTicker.enable(0);
}

// =====================================================
// Connect to WiFi network
// =====================================================
void wifiConnect() {
  Serial.printf("Connecting to WiFi: %s\n", WIFI_SSID);

  WiFi.setSleep(false);
  WiFi.persistent(false);

  wifiBlinkLed(); // Blink LED during connection attempt

  WiFi.begin(WIFI_SSID, WIFI_PSWD);
  WiFi.waitForConnectResult(WIFI_TIMEOUT);
}

// =====================================================
// WiFi Event Handler
// =====================================================
void wifiOnEvent(arduino_event_id_t event, arduino_event_info_t info) {
  Serial.printf("WiFi Event: %i\n", event);

  switch(event) {
    case ARDUINO_EVENT_WIFI_STA_GOT_IP: {
      // WiFi connected and got IP address
      IPAddress ip(info.got_ip.ip_info.ip.addr);
      Serial.printf("WiFi connected: %s\n", ip.toString().c_str());

      // Call all registered onConnect callbacks
      for(auto callback : onConnectCallbacks) {
        callback();
      }

      // Stop reconnect ticker and turn LED on
      wifiTicker.disable(0);
      wifiOnLed();
      break;
    }

    case ARDUINO_EVENT_WIFI_STA_DISCONNECTED: {
      // WiFi disconnected
      Serial.println("WiFi lost connection");

      // Call all registered onDisconnect callbacks
      for(auto callback : onDisconnectCallbacks) {
        callback();
      }

      wifiOffLed();            // Turn off LED
      delay(WIFI_RECONNECT);   // Wait before reconnect
      wifiTicker.enable(0);    // Start reconnect ticker
      break;
    }

    default:
      // Other events can be added here if needed
      break;
  }
}

// =====================================================
// Register callbacks
// =====================================================
void wifiAddOnConnectCallback(onWifiEventFunction callback) {
  onConnectCallbacks.push_back(callback);
}

void wifiAddOnDisconnectCallback(onWifiEventFunction callback) {
  onDisconnectCallbacks.push_back(callback);
}

// =====================================================
// WiFi ticker update (call in loop)
// =====================================================
void wifiLoop() {
  wifiTicker.update();
}
