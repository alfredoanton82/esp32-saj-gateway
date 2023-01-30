// =================================================================================================
// esp32-saj-gateway: Copyright 2023 by Alfredo M. Anton
//               MIT license - see license.md for details
// =================================================================================================
#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <TickerScheduler.h>

#include <vector>
#include <string>

#include <ESP32SAJGatewayLogging.h>

// MQTT Configuration
const char MQTT_HOST[] = "";
const int  MQTT_PORT   = 1883;
const char MQTT_USER[] = "";
const char MQTT_PSWD[] = "";

// Led GPIO
const int MQTT_GPIO  = 19;
const int MQTT_PWM_C = 2;    // Channel
const int MQTT_PWM_F = 1000; // 1 KHz
const int MQTT_PWM_R = 8;    // 8 bits
const int MQTT_PWM_D = 50;  // Duty

// MQTT Log
const char MQTT_LOG[] = "esp32pwmeter/log";

// MQTT Reconnect
const int MQTT_RECONNECT = 5000;

// Callback definition
typedef std::function<void()> onMqttEventFunction;

// Functions
AsyncMqttClient *mqttSetup(const char* clientId);

void mqttStart();
void mqttStop();

void mqttConnect();
void mqttOnConnect(bool sessionPresent);
void mqttOnDisconnect(AsyncMqttClientDisconnectReason reason);
void mqttOnMessage(char *topic, char *payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total);

void mqttLoop();

void mqttAddTopic(const char* topic);
void mqttAddOnConnectCallback(onMqttEventFunction callback);
void mqttAddOnDisconnectCallback(onMqttEventFunction callback);

void mqttSetupLed();
void mqttBlinkLed();
void mqttOnLed();
void mqttOffLed();
