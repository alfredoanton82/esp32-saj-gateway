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

}

using namespace MqttManager;

AsyncMqttClient* mqttSetup(const char* clientId) {
  
  Serial.printf("Setting up MQTT: %s:%d\n", MQTT_HOST, MQTT_PORT);

  mqttSetupLed();

  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  mqttClient.setCredentials(MQTT_USER, MQTT_PSWD);
  mqttClient.setClientId(clientId);

  mqttClient.onConnect(mqttOnConnect);
  mqttClient.onDisconnect(mqttOnDisconnect);
  mqttClient.onMessage(mqttOnMessage);
  mqttClient.setKeepAlive(30);
  mqttClient.setMaxTopicLength(256);

  mqttTicker.add(0, MQTT_RECONNECT, [&](void *) { mqttConnect(); }, nullptr, false); 
  mqttTicker.disableAll();

  return &mqttClient;
}

void mqttStart() {
  mqttTicker.enable(0);
}

void mqttStop() {
  mqttTicker.disableAll();
  mqttClient.disconnect();
}

void mqttConnect() {

    Serial.printf("Connecting MQTT: %s:%d\n", MQTT_HOST, MQTT_PORT);
    mqttClient.connect();
    mqttBlinkLed();

}

// -------------------- LED PWM functions using modern HAL --------------------

void mqttSetupLed() {
  // Attach pin to PWM with given frequency & resolution
  // No need to manage channels manually
  ledcAttach(MQTT_GPIO, MQTT_PWM_F, MQTT_PWM_R);
}

void mqttBlinkLed() {
  ledcWrite(MQTT_GPIO, MQTT_PWM_D);
  delay(20);
  ledcWrite(MQTT_GPIO, 0); // LOW replaced with 0 for duty cycle
}

void mqttOnLed() {
  ledcWrite(MQTT_GPIO, MQTT_PWM_D);
}

void mqttOffLed() {
  ledcWrite(MQTT_GPIO, 0);
}

// -------------------- MQTT callbacks --------------------

void mqttOnConnect(bool sessionPresent) {

  mqttTicker.disable(0);
  mqttOnLed();

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

}

void mqttOnDisconnect(AsyncMqttClientDisconnectReason reason) {

  Serial.printf("MQTT disconnected: %d\n", reason);
  for(auto callback : onDisconnectCallbacks) {
    callback();
  }
  mqttTicker.enable(0);
  mqttOffLed();

}

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

void mqttLoop(){
  mqttTicker.update();
}
