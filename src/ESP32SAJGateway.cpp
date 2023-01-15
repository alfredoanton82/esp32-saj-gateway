// =================================================================================================
// esp32-saj-gateway: Copyright 2023 by Alfredo M. Anton
//               MIT license - see license.md for details
// =================================================================================================
#include <ESP32SAJGateway.h>

// Counter
unsigned long dtsuLastMeasTime = 0L;
unsigned long sajInvLastMeasTime = 0L;

// Moving average
DTSU666Data dtsuAveraged;
SAJInverterData sajInvAveraged;

// Number of samples (DTSU666 sends 1 measurement per second)
int dtsuNumSamples = 1;
int sajInvNumSamples = 1;

// MqttClient
AsyncMqttClient *mqtt;

// Setup main program
void setup()
{

  // Start logging
  loggingSetup();

  // Setup wif
  wifiSetup();

  // Setup mqtt
  mqtt = mqttSetup(ESP32_ID);

  // Add Wifi connect callback
  wifiAddOnConnectCallback(mqttStart);

  // Connect wifi
  wifiStart();

  // Configure dtsu666
  dtsu666Setup();
  dtsu666RegisterWorker(onDTSU666DataHandler);
  
  // Add callbacks (only start and stop on wifi and mqtt connection)
  mqttAddOnConnectCallback(dtsu666Start);
  wifiAddOnDisconnectCallback(dtsu666Stop);
  mqttAddOnDisconnectCallback(dtsu666Stop);

  // // Start dtsu666
  sajInverterSetup();
  sajInverterRegisterWorker(onSAJInverterDataHandler);
  
  // Add callbacks (only start and stop on wifi and mqtt connection)
  mqttAddOnConnectCallback(sajInverterStart);
  mqttAddOnDisconnectCallback(sajInverterStop);
  mqttAddOnDisconnectCallback(sajInverterStop);

  // Configure freeds
  freeDSSetup(mqtt);
  dtsu666RegisterWorker(freeDSPwMeterOnDataHandler);
  sajInverterRegisterWorker(freeDSInverterOnDataHandler);

}

void loop() {

  wifiLoop();
  mqttLoop();
  delay(1000); // Only used for Tickers update, other services are asynchronous.

}

void onDTSU666DataHandler(DTSU666Data data) {

    // First initialisation
    if (dtsuAveraged.isValid()) {

      // Update moving average
      dtsuNumSamples++;
      dtsuAveraged = dtsuAveraged + data;

      // Publish every MEAS_INTERVAL (historical)
      if (millis() - dtsuLastMeasTime >= MEAS_INTERVAL) {

        dtsuLastMeasTime = millis();

        dtsuAveraged = dtsuAveraged.scale(1.0/dtsuNumSamples);
        
        mqtt->publish(MQTT_PWMETER_AVE, 0, false, dtsuAveraged.json().c_str());

        // Reset current average
        dtsuNumSamples = 1;
        dtsuAveraged = data;

      }

    } else {

      // Initialising
      dtsuNumSamples = 1;
      dtsuAveraged = data;

    }
    
}

void onSAJInverterDataHandler(SAJInverterData data) {

    // First initialisation
    if (sajInvAveraged.isValid()) {

      // Update moving average
      sajInvNumSamples++;
      sajInvAveraged = sajInvAveraged + data;
    
      // Publish every MEAS_INTERVAL (historical)
      if (millis() - sajInvLastMeasTime >= MEAS_INTERVAL) {

        sajInvLastMeasTime = millis();

        sajInvAveraged = sajInvAveraged.scale(1.0/sajInvNumSamples);

        mqtt->publish(MQTT_INVERTER_AVE, 0, false, sajInvAveraged.json().c_str());

        // Reset current average
        sajInvNumSamples = 1;
        sajInvAveraged = data;

      }

    } else {

      // Initialising
      sajInvNumSamples = 1;
      sajInvAveraged = data;

    }
    
}
