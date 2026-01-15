#include "nrf24l01.h"

bool NRF24_TX::begin(uint8_t cePin, uint8_t csnPin) {
    radio = RF24(cePin, csnPin);

    if (!radio.begin()) return false;

    radio.setPALevel(RF24_PA_MAX);
    radio.setDataRate(RF24_1MBPS);
    radio.setChannel(108);
    radio.setRetries(3, 5);
    radio.openWritingPipe(address);
    radio.stopListening();

    return true;
}

void NRF24_TX::send(uint8_t direction, uint8_t speed) {
    ControlPacket pkt;
    pkt.direction = direction;
    pkt.speed = speed;

    radio.write(&pkt, sizeof(pkt));
}
