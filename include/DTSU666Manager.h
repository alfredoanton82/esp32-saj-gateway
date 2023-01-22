// =================================================================================================
// esp32-saj-gateway: Copyright 2023 by Alfredo M. Anton
//               MIT license - see license.md for details
// =================================================================================================
#include <Arduino.h>
#include <ModbusServerRTU.h>
#include <DTSU666Data.h>

#include <ESP32SAJGatewayLogging.h>

// Workers
typedef std::function<void(DTSU666Data data)> DTSU666Worker;

namespace DTSU666Manager {

  // Serial configuration
  const int  RTU_SERIAL = 2;
  const long RTU_BAUD   = 9600;
  const int  RTU_RXD    = 16;
  const int  RTU_TXD    = 17;

  // RTU configuration
  const int RTU_TIMEOUT  = 2000; // Msg timeout ms
  const int RTU_INTERVAL = 30000; // Gap between messages us
  const int RTU_MSG_SIZE = 143;

  // Debug flag
  const bool SERIAL_DBG_FLAG = false;

  // Led GPIO
  const int LED_GPIO = 21;
  const int LED_DUTY = 80;

}

// Setup method
void dtsu666Setup();

// Service management
void dtsu666Start();
void dtsu666Stop();

// Led handler
void dtsu666BlinkLed();

// RTU data handler
void dtsu666OnDataHandler (ModbusMessage response);

// Register worker
void dtsu666RegisterWorker(DTSU666Worker worker);
