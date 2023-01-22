// =================================================================================================
// esp32-saj-gateway: Copyright 2023 by Alfredo M. Anton
//               MIT license - see license.md for details
// =================================================================================================
#include <DTSU666Manager.h>

namespace DTSU666Manager {

  // Define service instance
  HardwareSerial  rtuSerial(RTU_SERIAL);
  ModbusServerRTU rtuServer(rtuSerial, RTU_TIMEOUT);

  // List of workers
  std::vector<DTSU666Worker> workerList;

}

using namespace DTSU666Manager;

void dtsu666Setup() {

  Serial.printf("Setting up meter RTU: RX = %d / TX = %d\n", RTU_RXD, RTU_TXD);

  pinMode(LED_GPIO, OUTPUT);

  // Configure and Start serial
  rtuSerial.begin(RTU_BAUD, SERIAL_8N1, RTU_RXD, RTU_TXD);

  // Configure and Start ModBusRTU
  rtuServer.registerSniffer(dtsu666OnDataHandler);

}

void dtsu666Start() {
    rtuServer.start(-1, RTU_INTERVAL); 
}

void dtsu666Stop() {
    rtuServer.stop(); 
}

void dtsu666BlinkLed() {
  analogWrite(LED_GPIO, LED_DUTY);
  delay(20);
  analogWrite(LED_GPIO, LOW);
}

void dtsu666RegisterWorker(DTSU666Worker worker) {
  workerList.push_back(worker);
}

void dtsu666OnDataHandler (ModbusMessage msg) {

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
    DTSU666Data data = DTSU666Data(msg);

    dtsu666BlinkLed();

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