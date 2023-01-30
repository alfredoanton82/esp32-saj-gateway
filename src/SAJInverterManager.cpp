// =================================================================================================
// esp32-saj-gateway: Copyright 2023 by Alfredo M. Anton
//               MIT license - see license.md for details
// =================================================================================================
#include <SAJInverterManager.h>

namespace SAJInverterManager {

  // Define service instance
  HardwareSerial  rtuSerial(RTU_SERIAL);
  ModbusServerRTU rtuServer(rtuSerial, RTU_TIMEOUT);

  // List of workers
  std::vector<SAJInverterWorker> workerList;

}

using namespace SAJInverterManager;

void sajInverterSetup() {

  Serial.printf("Setting up inverter RTU: Serial = %d / RX = %d / TX = %d\n", RTU_SERIAL, RTU_RXD, RTU_TXD);

  sajInverterSetupLed();

  // Configure and Start serial
  rtuSerial.begin(RTU_BAUD, SERIAL_8N1, RTU_RXD, RTU_TXD);

  // Configure and Start ModBusRTU
  rtuServer.registerSniffer(sajInverterOnDataHandler);

}

void sajInverterStart() {
    rtuServer.start(-1, RTU_INTERVAL); 
}

void sajInverterStop() {
    rtuServer.stop(); 
}

void sajInverterSetupLed() {
  ledcSetup(LED_PWM_C, LED_PWM_F, LED_PWM_R);
  ledcAttachPin(LED_GPIO, LED_PWM_C);
}

void sajInverterBlinkLed() {
  ledcWrite(LED_PWM_C, LED_PWM_D);
  delay(50);
  ledcWrite(LED_PWM_C, LOW);
}

void sajInverterRegisterWorker(SAJInverterWorker worker) {
  workerList.push_back(worker);
}

void sajInverterOnDataHandler (ModbusMessage msg) {

  if (SERIAL_DBG_FLAG) {

    Serial.println(msg.size());
    for (auto& byte : msg) {
      Serial.printf("%02X,", byte);
    }
    Serial.println();
  }

  // Process results
  if (msg.size() == RTU_MSG_SIZE) {

    // Parse response
    SAJInverterData data = SAJInverterData(msg);

    sajInverterBlinkLed();

    if (SERIAL_DBG_FLAG) {
      Serial.println(data.json().c_str());
    }

    // Call workers
    if (data.isValid()) {
      for(auto worker : workerList){
        worker(data);
      }
    } else {
      Serial.printf("ERROR: invalid data / %s\n", data.json().c_str());
    }
  }

}