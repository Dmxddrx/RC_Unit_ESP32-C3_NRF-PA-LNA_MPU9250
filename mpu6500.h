#ifndef MPU6500_H
#define MPU6500_H

#include <Wire.h>
#include <MPU9250_asukiaaa.h>

class MPU6500 {
public:
    bool begin(TwoWire* wire, int sda, int scl);
    void update();
    float x();
    float y();
    float z();

private:
    MPU9250_asukiaaa mpu;
};

#endif
