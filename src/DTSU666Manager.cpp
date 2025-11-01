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

  Serial.printf("Setting up meter RTU: Serial = %d / RX = %d / TX = %d\n", RTU_SERIAL, RTU_RXD, RTU_TXD);

  dtsu666SetupLed();

  // Configure and Start serial
  rtuSerial.begin(RTU_BAUD, SERIAL_8N1, RTU_RXD, RTU_TXD);

  // Configure and Start ModBusRTU
  rtuServer.registerSniffer(dtsu666OnDataHandler);

}

// -------------------- LED PWM functions using modern HAL --------------------

void dtsu666SetupLed() {
  // Modern HAL: attach pin to PWM
  ledcAttach(LED_GPIO, LED_PWM_F, LED_PWM_R);
}

void dtsu666BlinkLed() {
  ledcWrite(LED_GPIO, LED_PWM_D);
  delay(50);
  ledcWrite(LED_GPIO, 0); // LOW replaced with 0
}

// -------------------- Worker registration / data handling --------------------

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
