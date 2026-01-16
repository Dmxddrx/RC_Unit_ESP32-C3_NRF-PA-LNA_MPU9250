#include <Arduino.h>
#include "led.h"

LEDController::LEDController(uint8_t pin, uint8_t channel, uint32_t freq, uint8_t res)
    : _pin(pin), _channel(channel), _freq(freq), _res(res) {}

void LEDController::begin() {
    ledcSetup(_channel, _freq, _res);
    ledcAttachPin(_pin, _channel);
    ledcWrite(_channel, 0);  // LED off initially
}

void LEDController::update(bool nrfOk) {
    if (nrfOk) {
        // NRF connected → steady brightness
        ledcWrite(_channel, _brightness);
    } else {
        // NRF not connected → blink LED
        if (millis() - _lastToggle > _blinkInterval) {
            _lastToggle = millis();
            _ledState = !_ledState;
            ledcWrite(_channel, _ledState ? _brightness : 0);
        }
    }
}
