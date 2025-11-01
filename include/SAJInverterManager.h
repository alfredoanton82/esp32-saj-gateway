// =================================================================================================
// esp32-saj-gateway: Copyright 2023 by Alfredo M. Anton
//               MIT license - see license.md for details
// =================================================================================================
#pragma once

#include <Arduino.h>
#include <ModbusServerRTU.h>
#include <SAJInverterData.h>
#include <LedManager.h>

// Workers
typedef std::function<void(SAJInverterData data)> SAJInverterWorker;

namespace SAJInverterManager {
   
    // Serial configuration
    const int  RTU_SERIAL = 1;
    const long RTU_BAUD   = 9600;
    const int  RTU_RXD    = 2;
    const int  RTU_TXD    = 0;

    // RTU configuration
    const int RTU_TIMEOUT  = 2000;   // Msg timeout ms
    const int RTU_INTERVAL = 50000;  // Gap between messages us
    const int RTU_MSG_SIZE = 157;

    // Debug flag
    const bool SERIAL_DBG_FLAG = false;

    // LED GPIO
    const int LED_GPIO  = 21;
    const int LED_PWM_C = 4;    // Channel
    const int LED_PWM_F = 1000; // 1 KHz
    const int LED_PWM_R = 8;    // 8 bits
    const int LED_PWM_D = 25;   // Duty

}

// Setup method
void sajInverterSetup();
// Service management
void sajInverterStart();
void sajInverterStop();

// RTU data handler
void sajInverterOnDataHandler(ModbusMessage response);

// Register worker
void sajInverterRegisterWorker(SAJInverterWorker worker);

void sajInverterLoop();
