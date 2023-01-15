
// =================================================================================================
// esp32-saj-gateway: Copyright 2023 by Alfredo M. Anton
//               MIT license - see license.md for details
// =================================================================================================
#include<FreeDSManager.h>

namespace FreeDSManager {

  // MQTT client
  AsyncMqttClient *mqttClient;

}

using namespace FreeDSManager;

void freeDSSetup(AsyncMqttClient *mqttClient) {
  FreeDSManager::mqttClient = mqttClient;
}

// Handlers
void freeDSPwMeterOnDataHandler(DTSU666Data data) {

  if (data.isValid()) {

    // Prepare JSON
    StaticJsonDocument<512> root;
    root["ENERGY"]["Power"] = data.getP()[0]; // [W]

    array<float,3> acV = data.getV();
    root["ENERGY"]["Voltage"] = (acV[0]+acV[1]+acV[2]) / 3.0;

    // Convert to JSON
    String json;
    serializeJson(root, json);

    mqttClient->publish(PWMETER_TOPIC, 0, false, json.c_str());

  }

}

void freeDSInverterOnDataHandler(SAJInverterData data){

  if (data.isValid()) {

    // Prepare JSON
    StaticJsonDocument<512> root;
    array<float,3> dcI = data.getdcI();
    root["ENERGY"]["Pv1Current"] = dcI[0];
    root["ENERGY"]["Pv2Current"] = dcI[1];
    root["ENERGY"]["Pv3Current"] = dcI[2];

    array<float,3> dcV = data.getdcV();
    root["ENERGY"]["Pv1Voltage"] = dcV[0];
    root["ENERGY"]["Pv2Voltage"] = dcV[1];
    root["ENERGY"]["Pv3Voltage"] = dcV[2];

    array<float,3> dcP = data.getdcP();
    root["ENERGY"]["Pv1Power"]   = dcP[0];
    root["ENERGY"]["Pv2Power"]   = dcP[1];
    root["ENERGY"]["Pv3Power"]   = dcP[2];

    root["ENERGY"]["Power"]       = data.getP();
    root["ENERGY"]["Temperature"] = data.getTemp();
    root["ENERGY"]["Today"] = 0.0;

    // Convert to JSON
    String json;
    serializeJson(root, json);

    mqttClient->publish(INVERTER_TOPIC, 0, false, json.c_str());

  }

}
