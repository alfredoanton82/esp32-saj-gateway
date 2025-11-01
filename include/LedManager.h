// =================================================================================================
// LedManager: Handles LED blinking and on/off state using TickerScheduler
// esp32-saj-gateway: Copyright 2023 by Alfredo M. Anton
// MIT license - see license.md for details
// =================================================================================================
#pragma once

#include <Arduino.h>
#include <TickerScheduler.h>
#include <esp32-hal-ledc.h> // Required for LED PWM functions

class LedManager {
private:
    uint8_t pin;              // GPIO pin
    uint32_t freq;            // PWM frequency
    uint8_t resolution;       // PWM resolution in bits
    uint32_t duty;            // PWM duty cycle

    bool connected;           // LED on/off state
    bool blinking;            // Blinking state
    uint32_t blinkInterval;   // Blink interval in milliseconds

    TickerScheduler ticker;   // Internal scheduler for blinking

    // Internal toggle function called by TickerScheduler
    void toggleLed(void*);

public:
    // Constructor
    LedManager(uint8_t pin, uint32_t freq, uint8_t resolution, uint32_t duty);

    // Initialize the LED (attach to PWM)
    void setup();

    // Start blinking with given interval (ms)
    void startBlink(uint32_t interval);

    // Stop blinking
    void stopBlink();

    // Permanently turn LED on or off
    void setConnected(bool state);

    // Update function (optional if you want to manually tick)
    void update();
};
