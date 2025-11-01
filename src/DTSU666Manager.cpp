// =================================================================================================
// esp32-saj-gateway: Copyright 2023 by Alfredo M. Anton
//               MIT license - see license.md for details
// =================================================================================================

#include <DTSU666Manager.h>

namespace DTSU666Manager {

  // Define service instance for Modbus RTU communication
  HardwareSerial  rtuSerial(RTU_SERIAL);
  ModbusServerRTU rtuServer(rtuSerial, RTU_TIMEOUT);

  // List of workers to process inverter data
  std::vector<DTSU666Worker> workerList;

  // LED manager instance for inverter status
  LedManager dtsu666Led(LED_GPIO, LED_PWM_F, LED_PWM_R, LED_PWM_D);

}

using namespace DTSU666Manager;

// =====================================================
// DTSU666 Meter RTU Setup
// =====================================================
void dtsu666Setup() {

  Serial.printf("Setting up meter RTU: Serial = %d / RX = %d / TX = %d\n", RTU_SERIAL, RTU_RXD, RTU_TXD);


  // Configure and start serial port for RTU
  rtuSerial.begin(RTU_BAUD, SERIAL_8N1, RTU_RXD, RTU_TXD);

  // Register Modbus sniffer (called on every message received)
  rtuServer.registerSniffer(dtsu666OnDataHandler);

}

// =====================================================
// Blink LED for inverter
// =====================================================
void dtsu666BlinkLed() {
  // Blink LED for 50 ms to indicate data received
  dtsu666Led.startBlink(50);
}

// =====================================================
// Register a worker to handle inverter data
// =====================================================
void dtsu666RegisterWorker(DTSU666Worker worker) {
  workerList.push_back(worker);
}

// =====================================================
// Modbus RTU Data Handler
// =====================================================
void dtsu666OnDataHandler (ModbusMessage msg) {

  // Optional debug output
  if (SERIAL_DBG_FLAG) {
    Serial.printf("Received Modbus message (%d bytes): ", msg.size());
    for (auto& byte : msg) {
      Serial.printf("%02X,", byte);
    }
    Serial.println();
  }

  // Only process messages of expected size
  if (msg.size() == RTU_MSG_SIZE) {

    // Parse response into structured data
    DTSU666Data data = DTSU666Data(msg);

    // Blink LED to indicate data received
    dtsu666BlinkLed();

    if (SERIAL_DBG_FLAG) {
      Serial.println(data.json().c_str());
    }

    // Call all registered worker functions
    if (data.isValid()) {
      for(auto worker : workerList){
        worker(data);
      }
    } else {
      Serial.printf("ERROR: invalid data / %s\n", data.json().c_str());
    }
  }

    // Update LED state (in case blinking is active)
  dtsu666Led.update();
}

void dtsu666Loop() {
  dtsu666Led.update(); // Update LED blinking
}
