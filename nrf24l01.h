#ifndef NRF24L01_H
#define NRF24L01_H

#include <SPI.h>
#include <RF24.h>

typedef struct {
    uint8_t direction;
    uint8_t speed;
} ControlPacket;

class NRF24_TX {
public:
    bool begin(uint8_t cePin, uint8_t csnPin);
    bool send(uint8_t direction, uint8_t speed);
    bool isOk();

private:
    RF24 radio = RF24(0, 0);
    bool initialized = false;
    const byte address[6] = "CAR01";
};

#endif
