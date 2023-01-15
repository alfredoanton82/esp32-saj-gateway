// =================================================================================================
// esp32-saj-gateway: Copyright 2023 by Alfredo M. Anton
//               MIT license - see license.md for details
// =================================================================================================
#include <Arduino.h>
#include <WifiManager.h>
#include <MqttManager.h>
#include <DTSU666Manager.h>
#include <SAJInverterManager.h>
#include <FreeDSManager.h>

#include <ESP32SAJGatewayLogging.h>

// ESP 32 Identification 
const char ESP32_ID[] = "ESP32_PWMETER";

// Publish MQTT
const char MQTT_PWMETER_AVE[]  = "esp32pwmeter/pwmeter_ave";
const char MQTT_INVERTER_AVE[] = "esp32pwmeter/inverter_ave";

// Interval change (30s)
const int MEAS_INTERVAL = 30000;

// Data proccess
void onDTSU666DataHandler(DTSU666Data data);
void onSAJInverterDataHandler(SAJInverterData data);
