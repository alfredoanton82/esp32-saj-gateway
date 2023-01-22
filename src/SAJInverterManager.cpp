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

  Serial.printf("Setting up inverter RTU: RX = %d / TX = %d\n", RTU_RXD, RTU_TXD);

  pinMode(LED_GPIO, OUTPUT);

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

void sajInverterBlinkLed() {
  analogWrite(LED_GPIO, LED_DUTY);
  delay(20);
  analogWrite(LED_GPIO, LOW);
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
      Serial.printf("ERROR: invalid data / %s", data.json().c_str());
    }
  }

}