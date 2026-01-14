#include "MPU9250.h"
#include <math.h>

MPU9250 mpu;

void setup() {
    Serial.begin(115200);
    Wire.begin(8, 9); // SDA = GPIO 8, SCL = GPIO 9 for ESP32-C3

    delay(2000);

    if (!mpu.setup(0x68)) {  // Change to 0x69 if your jumper is set high
        while (1) {
            Serial.println("MPU9250 connection failed. Check wiring!");
            delay(5000);
        }
    }
    
    Serial.println("MPU9250 Initialized.");
}

void loop() {
    if (mpu.update()) {
        // 1. Get Raw/Scaled Values
        float ax = mpu.getAccX();
        float ay = mpu.getAccY();
        float az = mpu.getAccZ();
        
        float gx = mpu.getGyroX();
        float gy = mpu.getGyroY();
        float gz = mpu.getGyroZ();
        
        float mx = mpu.getMagX();
        float my = mpu.getMagY();
        float mz = mpu.getMagZ();

        // 2. Calculate Pitch & Roll from Accelerometer
        // Roll = atan2(Y, Z) | Pitch = atan2(-X, sqrt(Y^2 + Z^2))
        float roll  = atan2(ay, az) * 180.0 / PI;
        float pitch = atan2(-ax, sqrt(ay * ay + az * az)) * 180.0 / PI;

        // 3. Print Everything
        Serial.print("--- TILT ---  ");
        Serial.print("P: "); Serial.print(pitch, 1);
        Serial.print("  R: "); Serial.println(roll, 1);

        Serial.print("ACCEL: "); Serial.print(ax); Serial.print(", "); Serial.print(ay); Serial.print(", "); Serial.println(az);
        Serial.print("GYRO:  "); Serial.print(gx); Serial.print(", "); Serial.print(gy); Serial.print(", "); Serial.println(gz);
        Serial.print("MAG:   "); Serial.print(mx); Serial.print(", "); Serial.print(my); Serial.print(", "); Serial.println(mz);
        
        Serial.println("-----------------------------------------");
    }
    delay(200); // Small delay to make it readable
}