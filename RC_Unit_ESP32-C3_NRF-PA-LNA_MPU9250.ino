#include "mpu6500.h"

#define SDA_PIN 6
#define SCL_PIN 7

MPU6500 imu;

// ---------------- CONFIG ----------------
const float DEAD_ZONE = 0.16;

enum Direction {
  STOP,
  FWD, FWD_RIGHT, RIGHT, BACK_RIGHT,
  BACK, BACK_LEFT, LEFT, FWD_LEFT
};

// ----------------------------------------
float mapLinear(float v, float inMin, float inMax, float outMin, float outMax) {
  v = constrain(v, inMin, inMax);
  return outMin + (v - inMin) * (outMax - outMin) / (inMax - inMin);
}

// ----------------------------------------
int speedSingleAxis(float v) {
  v = fabs(v);

  if (v <= 0.16) return 0;
  if (v >= 0.85) return 70;
  if (v >= 0.6)  return mapLinear(v, 0.6, 0.85, 60, 70);
  return mapLinear(v, 0.16, 0.6, 30, 60);
}

// ----------------------------------------
int speedDualAxis(float v) {
  v = constrain(v, 0.1, 0.7);

  if (v >= 0.6) return 70;
  if (v >= 0.5) return mapLinear(v, 0.5, 0.6, 60, 70);
  return mapLinear(v, 0.1, 0.5, 30, 60);
}

// ----------------------------------------
Direction getMajorDirection(float x, float y) {
  if (fabs(x) > fabs(y)) {
    return (x < 0) ? FWD : BACK;
  } else {
    return (y > 0) ? RIGHT : LEFT;
  }
}

// ----------------------------------------
Direction getCrossDirection(float x, float y) {
  if (x < 0 && y > 0) return FWD_RIGHT;
  if (x > 0 && y > 0) return BACK_RIGHT;
  if (x > 0 && y < 0) return BACK_LEFT;
  if (x < 0 && y < 0) return FWD_LEFT;
  return STOP;
}

// ----------------------------------------
void setup() {
  Serial.begin(115200);
  imu.begin(&Wire, SDA_PIN, SCL_PIN);
  Serial.println("MPU6500 Ready");
}

// ----------------------------------------
void loop() {
  imu.update();

  float x = imu.x();
  float y = imu.y();

  Serial.print("X: "); Serial.print(x, 2);
  Serial.print(" Y: "); Serial.print(y, 2);

  // -------- STOP CONDITION --------
  if (fabs(x) < DEAD_ZONE && fabs(y) < DEAD_ZONE) {
    Serial.println(" | STOP | Speed: 0%");
    delay(100);
    return;
  }

  bool xActive = fabs(x) >= DEAD_ZONE;
  bool yActive = fabs(y) >= DEAD_ZONE;

  Direction dir = STOP;
  int speed = 0;

  // -------- SINGLE AXIS --------
  if (xActive ^ yActive) {
    dir = getMajorDirection(x, y);
    speed = speedSingleAxis(xActive ? x : y);
  }

  // -------- DOUBLE AXIS --------
  else {
    float ax = fabs(x);
    float ay = fabs(y);

    float r1 = ax / ay;
    float r2 = ay / ax;

    if (r1 >= 0.45 && r1 <= 2.15 && r2 >= 0.45 && r2 <= 2.15) {
      dir = getCrossDirection(x, y);
      speed = speedDualAxis((ax + ay) / 2.0);
    } else {
      dir = getMajorDirection(x, y);
      speed = speedSingleAxis(ax > ay ? x : y);
    }
  }

  // -------- OUTPUT --------
  Serial.print(" | DIR: ");
  Serial.print(dir);
  Serial.print(" | Speed: ");
  Serial.print(speed);
  Serial.println("%");

  delay(100);
}
