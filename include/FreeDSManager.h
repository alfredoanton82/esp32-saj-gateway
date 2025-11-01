// =================================================================================================
// esp32-saj-gateway: Copyright 2023 by Alfredo M. Anton
//               MIT license - see license.md for details
// =================================================================================================
#include <Arduino.h>
#include <AsyncMqttClient.h>
#include <ArduinoJson.h>
#include <DTSU666Data.h>
#include <SAJInverterData.h>

#include <iostream>
#include <array>
#include <string>

using namespace std;

namespace FreeDSManager {

  // MQTT Power Meter topic
  const char PWMETER_TOPIC[] = "freeds/pwmeter";

  // MQTT Inverter topic
  const char INVERTER_TOPIC[] = "freeds/inverter";

}

// Setup method
void freeDSSetup( AsyncMqttClient *mqttClient);

// Handlers
void freeDSPwMeterOnDataHandler(DTSU666Data data);
void freeDSInverterOnDataHandler(SAJInverterData data);
