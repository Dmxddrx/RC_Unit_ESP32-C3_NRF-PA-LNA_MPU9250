#include <Wire.h>
#include <MPU9250_asukiaaa.h>

#define SDA_PIN 6
#define SCL_PIN 7

MPU9250_asukiaaa mpu;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);

  Serial.println("Starting MPU9250...");

  mpu.setWire(&Wire);
  mpu.beginAccel();
  mpu.beginGyro();
  mpu.beginMag();

  Serial.println("✅ MPU9250 initialized");
}

void loop() {
  mpu.accelUpdate();
  mpu.gyroUpdate();
  mpu.magUpdate();

  Serial.print("ACC: ");
  Serial.print(mpu.accelX(), 2);
  Serial.print(", ");
  Serial.print(mpu.accelY(), 2);
  Serial.print(", ");
  Serial.print(mpu.accelZ(), 2);

  Serial.print(" | GYRO: ");
  Serial.print(mpu.gyroX(), 2);
  Serial.print(", ");
  Serial.print(mpu.gyroY(), 2);
  Serial.print(", ");
  Serial.print(mpu.gyroZ(), 2);

  Serial.print(" | MAG: ");
  Serial.print(mpu.magX(), 2);
  Serial.print(", ");
  Serial.print(mpu.magY(), 2);
  Serial.print(", ");
  Serial.print(mpu.magZ(), 2);

  Serial.println();
  delay(100);
}
