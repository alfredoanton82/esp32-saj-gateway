// =================================================================================================
// esp32-saj-gateway: Copyright 2023 by Alfredo M. Anton
//               MIT license - see license.md for details
// =================================================================================================
#include <SAJInverterManager.h>

namespace SAJInverterManager {

  // --------------------------
  // Serial and RTU setup
  // --------------------------
  HardwareSerial  rtuSerial(RTU_SERIAL);            // Hardware Serial instance
  ModbusServerRTU rtuServer(rtuSerial, RTU_TIMEOUT); // Modbus RTU server instance

  // --------------------------
  // Registered workers
  // --------------------------
  std::vector<SAJInverterWorker> workerList;

}

// --------------------------
// Use namespace for convenience
// --------------------------
using namespace SAJInverterManager;

// =====================================================
// Setup inverter RTU and LED
// =====================================================
void sajInverterSetup() {

  Serial.printf("Setting up inverter RTU: Serial = %d / RX = %d / TX = %d\n", RTU_SERIAL, RTU_RXD, RTU_TXD);

  // Setup LED for activity
  sajInverterSetupLed();

  // Configure and start the serial port
  rtuSerial.begin(RTU_BAUD, SERIAL_8N1, RTU_RXD, RTU_TXD);

  // Register Modbus sniffer (called on every message received)
  rtuServer.registerSniffer(sajInverterOnDataHandler);
}

// =====================================================
// LED Setup and Controls
// =====================================================
void sajInverterSetupLed() {
  // Attach LED pin to PWM using new LEDC API
  // LED_GPIO: pin number
  // LED_PWM_F: frequency (Hz)
  // LED_PWM_R: resolution (bits)
  ledcAttach(LED_GPIO, LED_PWM_F, LED_PWM_R);
}

void sajInverterBlinkLed() {
  // Blink LED for activity indication
  ledcWrite(LED_PWM_C, LED_PWM_D); // Turn LED on (duty)
  delay(50);                        // Short delay
  ledcWrite(LED_PWM_C, 0);          // Turn LED off
}

// =====================================================
// Register a worker callback
// =====================================================
void sajInverterRegisterWorker(SAJInverterWorker worker) {
  workerList.push_back(worker);
}

// =====================================================
// Modbus Data Handler
// =====================================================
void sajInverterOnDataHandler(ModbusMessage msg) {

  // --------------------------
  // Optional debug printing
  // --------------------------
  if (SERIAL_DBG_FLAG) {
    Serial.printf("Received Modbus message (%d bytes): ", msg.size());
    for (auto& byte : msg) {
      Serial.printf("%02X,", byte);
    }
    Serial.println();
  }

  // --------------------------
  // Process valid messages
  // --------------------------
  if (msg.size() == RTU_MSG_SIZE) {

    // Parse response into SAJInverterData
    SAJInverterData data(msg);

    // Indicate activity with LED
    sajInverterBlinkLed();

    if (SERIAL_DBG_FLAG) {
      Serial.println(data.json().c_str());
    }

    // --------------------------
    // Call all registered workers
    // --------------------------
    if (data.isValid()) {
      for (auto worker : workerList) {
        worker(data);
      }
    } else {
      Serial.printf("ERROR: invalid data / %s\n", data.json().c_str());
    }
  }
}
