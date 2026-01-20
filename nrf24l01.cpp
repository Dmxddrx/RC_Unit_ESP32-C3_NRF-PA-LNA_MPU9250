#include "nrf24l01.h"

bool NRF24_TX::begin(uint8_t cePin, uint8_t csnPin) {
    radio = RF24(cePin, csnPin);

    if (!radio.begin()) {
        initialized = false;
        return false;
    }

    radio.setPALevel(RF24_PA_MAX);
    radio.setDataRate(RF24_1MBPS);
    radio.setChannel(108);
    radio.setAutoAck(false);
    radio.setRetries(0,0);
    radio.openWritingPipe(address);
    radio.stopListening();

    initialized = radio.isChipConnected();
    return initialized;
}

bool NRF24_TX::send(uint8_t direction, uint8_t speed) {
    if (!initialized) return false;

    ControlPacket pkt;
    pkt.direction = direction;
    pkt.speed = speed;

    return radio.write(&pkt, sizeof(pkt));
}

bool NRF24_TX::isOk() {
    return initialized && radio.isChipConnected();
}
