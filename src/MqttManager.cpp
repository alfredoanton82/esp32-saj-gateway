// =================================================================================================
// esp32-saj-gateway: Copyright 2023 by Alfredo M. Anton
//               MIT license - see license.md for details
// =================================================================================================

#include <MqttManager.h>

namespace MqttManager {

  // MQTT client
  AsyncMqttClient mqttClient;

  // Ticker scheduler
  TickerScheduler mqttTicker(1);

  // List of registered topics
  std::vector<std::string> topics;

  // List of callbacks
  std::vector<onMqttEventFunction> onConnectCallbacks;
  std::vector<onMqttEventFunction> onDisconnectCallbacks;

  // Instantiate LED manager for MQTT
  LedManager mqttLed(MQTT_GPIO, MQTT_PWM_F, MQTT_PWM_R, MQTT_PWM_D);
}

using namespace MqttManager;

AsyncMqttClient* mqttSetup(const char* clientId) {
  
  Serial.printf("Setting up MQTT: %s:%d\n", MQTT_HOST, MQTT_PORT);


  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setCredentials(MQTT_USER, MQTT_PSWD);
  mqttClient.setClientId(clientId);

  mqttClient.onConnect(mqttOnConnect);
  mqttClient.onDisconnect(mqttOnDisconnect);

  mqttTicker.add(0, MQTT_RECONNECT, [&](void *) { mqttConnect(); }, nullptr, false); 
  mqttTicker.disableAll();

  return &mqttClient;
}

// =====================================================
// Start MQTT connection
// =====================================================
void mqttStart() {
  mqttTicker.enable(0);
}

void mqttStop() {
  mqttTicker.disableAll();
  mqttClient.disconnect();
}

void mqttConnect() {
  
  Serial.printf("Connecting MQTT: %s:%d\n", MQTT_HOST, MQTT_PORT);
  
  // Start blinking LED during connection attempt
  mqttLed.startBlink(20);

  mqttClient.connect();
}

// =====================================================
// MQTT Event handlers
// =====================================================
void mqttOnConnect(bool sessionPresent) {

  char outMsg[80];
  sprintf(outMsg, "Connected MQTT: %s:%d", MQTT_HOST, MQTT_PORT);
  mqttClient.publish(MQTT_LOG, 0, false, outMsg);
  Serial.println(outMsg);

  for (auto t : topics) {
    mqttClient.subscribe(t.c_str(), 0);
    sprintf(outMsg, "Subscribed to MQTT topic: %s", t.c_str());
    mqttClient.publish(MQTT_LOG, 0, false, outMsg);
    Serial.println(outMsg);
  }

  for(auto callback : onConnectCallbacks) {
    callback();
  }

  mqttLed.setConnected(true);  // LED off while disconnected
  mqttTicker.disable(0); // Stop reconnect ticker

}

void mqttOnDisconnect(AsyncMqttClientDisconnectReason reason) {
  Serial.printf("MQTT disconnected: %d\n", reason);


  for (auto callback : onDisconnectCallbacks) {
    callback();
  }

  mqttLed.setConnected(false);  // LED off while disconnected
  mqttTicker.enable(0); // Start reconnect ticker

}

// =====================================================
// Register callbacks
// =====================================================
void mqttOnMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total)
{

  char outMsg[120];
  if ( sprintf(outMsg, "Received message topic = '%s', value = '%s'", topic, payload) > 0) {
    Serial.println(outMsg);
    mqttClient.publish(MQTT_LOG, 0, false, outMsg);
  }

}

void mqttAddTopic(const char* topic) {
  topics.push_back(topic);
}

void mqttAddOnConnectCallback(onMqttEventFunction callback) {
  onConnectCallbacks.push_back(callback);
}

void mqttAddOnDisconnectCallback(onMqttEventFunction callback) {
  onDisconnectCallbacks.push_back(callback);
}

// =====================================================
// Loop updates (call in main loop)
// =====================================================
void mqttLoop() {
  mqttTicker.update();
  mqttLed.update(); // Update LED blinking
}
