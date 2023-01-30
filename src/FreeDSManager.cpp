
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

    array<float,3> acV = data.getV();
    root["ENERGY"]["Voltage"]       = (acV[0]+acV[1]+acV[2]) / 3.0; // [V]

    array<float,3> acI = data.getI();
    root["ENERGY"]["Current"]       = (acI[0]+acI[1]+acI[2]); // [A]

    root["ENERGY"]["Frequency"]     = data.getF(); // [Hz]

    root["ENERGY"]["Power"]         = data.getP()[0]; // [W]
    root["ENERGY"]["ApparentPower"] = data.getS()[0]; // [VA]
    root["ENERGY"]["ReactivePower"] = data.getQ()[0]; // [VAR]
    root["ENERGY"]["Factor"]        = data.getPf()[0];

    // Dummy values
    root["ENERGY"]["TotalStartTime"] = "2000-01-01T00:00:00";
    root["ENERGY"]["Today"]          = 0.0;
    root["ENERGY"]["Yesterday"]      = 0.0;
    root["ENERGY"]["Period"]         = 0.0;
    root["ENERGY"]["ExportActive"]   = 0.0;
    root["ENERGY"]["ImportActive"]   = 0.0;
    root["ENERGY"]["ExportReactive"] = 0.0;
    root["ENERGY"]["ImportReactive"] = 0.0;
    root["ENERGY"]["PhaseAngle"]     = 0.0;

//    root["ENERGY"]["TotalTariff"]    = [0.0,0.0];
//    root["ENERGY"]["ExportTariff"]    = [0.0,0.0];
//    root["ENERGY"]["ImportTariff"]    = [0.0,0.0];

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
