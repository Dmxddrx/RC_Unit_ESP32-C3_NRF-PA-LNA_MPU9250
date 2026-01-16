#ifndef LED_H
#define LED_H

#include <stdint.h>

class LEDController {
public:
    LEDController(uint8_t pin, uint8_t channel = 0, uint32_t freq = 5000, uint8_t res = 8);

    void begin();
    void update(bool nrfOk); // call every loop, true = steady, false = blink
private:
    uint8_t _pin;
    uint8_t _channel;
    uint32_t _freq;
    uint8_t _res;

    unsigned long _lastToggle = 0;
    bool _ledState = false;
    const uint8_t _brightness = 128;
    const unsigned long _blinkInterval = 500; // ms
};

#endif
