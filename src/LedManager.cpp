// =================================================================================================
// LedManager: Handles LED blinking and on/off state using TickerScheduler
// esp32-saj-gateway: Copyright 2023 by Alfredo M. Anton
// MIT license - see license.md for details
// =================================================================================================

#include "LedManager.h"

// =====================================================
// Constructor
// =====================================================
LedManager::LedManager(uint8_t pin, uint32_t freq, uint8_t resolution, uint32_t duty)
: pin(pin), freq(freq), resolution(resolution), duty(duty),
  connected(false), blinking(false), blinkInterval(500), ticker(1)
{
}

// =====================================================
// Setup LED
// =====================================================
void LedManager::setup() {
    ledcAttach(pin, freq, resolution); // Attach pin to PWM
    ledcWrite(pin, 0);                 // Initially off
}

// =====================================================
// Start blinking LED
// =====================================================
void LedManager::startBlink(uint32_t interval) {
    blinkInterval = interval;
    if (!blinking) {
        blinking = true;
        ticker.add(0, blinkInterval, [&](void*){ toggleLed(nullptr); }, nullptr, true);
    }
}

// =====================================================
// Stop blinking LED
// =====================================================
void LedManager::stopBlink() {
    if (blinking) {
        ticker.disable(0);
        blinking = false;
        ledcWrite(pin, 0); // Ensure LED is off
    }
}

// =====================================================
// Set LED state (connected / permanent on)
// =====================================================
void LedManager::setConnected(bool state) {
    connected = state;
    if (connected) {
        stopBlink();       // Stop any blinking
        ledcWrite(pin, duty); // Turn LED on
    } else {
        ledcWrite(pin, 0);    // Turn LED off
    }
}

// =====================================================
// Internal toggle function called by TickerScheduler
// =====================================================
void LedManager::toggleLed(void*) {
    if (!connected) {          // Only toggle if not permanently on
        static bool ledState = false;
        ledState = !ledState;
        ledcWrite(pin, ledState ? duty : 0);
    }
}

// =====================================================
// Update function (optional for manual ticking)
// =====================================================
void LedManager::update() {
    ticker.update();
}
