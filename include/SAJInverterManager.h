// =================================================================================================
// esp32-saj-gateway: Copyright 2023 by Alfredo M. Anton
//               MIT license - see license.md for details
// =================================================================================================
#include <Arduino.h>
#include <HardwareSerial.h>
#include <ModbusServerRTU.h>

#include <SAJInverterData.h>

#include <ESP32SAJGatewayLogging.h>

// Workers
typedef std::function<void(SAJInverterData data)> SAJInverterWorker;

namespace SAJInverterManager {
  
  // Serial configuration
  const int  RTU_SERIAL = 1;
  const long RTU_BAUD   = 9600;
  const int  RTU_RXD    = 02;
  const int  RTU_TXD    = 00;

  // RTU configuration
  const int RTU_TIMEOUT  = 2000; // Msg timeout ms
  const int RTU_INTERVAL = 50000; // Gap between messages us
  const int RTU_MSG_SIZE = 157;

  // Logging
  const bool SERIAL_DBG_FLAG = false;

  // Led GPIO
  const int LED_GPIO = 21;
  const int LED_DUTY = 80;

}

// Setup method
void sajInverterSetup();

// Service management
void sajInverterStart();
void sajInverterStop();

// Led handler
void sajInverterBlinkLed();

// RTU data handler
void sajInverterOnDataHandler (ModbusMessage response);

// Register worker
void sajInverterRegisterWorker(SAJInverterWorker worker);
