#include "mpu6500.h"

bool MPU6500::begin(TwoWire* wire, int sda, int scl) {
    wire->begin(sda, scl);
    wire->setClock(100000);

    mpu.setWire(wire);
    mpu.beginAccel();

    return true;
}

void MPU6500::update() {
    mpu.accelUpdate();
}

float MPU6500::x() { return mpu.accelX(); }
float MPU6500::y() { return mpu.accelY(); }
float MPU6500::z() { return mpu.accelZ(); }
