#include "mpu6500.h"
#include "nrf24l01.h"
#include "led.h"
#include "wireless.h"
#include <ArduinoOTA.h>


#define SDA_PIN 6
#define SCL_PIN 7

// ---------- NRF24 PINS (ESP32-C3) ----------
#define NRF_CE  3
#define NRF_CSN 10

#define LED_PIN 0

MPU6500 imu;
NRF24_TX radioTX;
LEDController statusLED(LED_PIN);
Wireless wireless;

// ---------------- CONFIG ----------------
const float DEAD_ZONE = 0.16;

// -------- NRF STATUS --------
bool nrfOk = false;
unsigned long lastNrfPrint = 0;

// -------- DIRECTION ENUM --------
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

  // SPI: SCK, MISO, MOSI, CSN
  SPI.begin(4, 2, 5, NRF_CSN);

  imu.begin(&Wire, SDA_PIN, SCL_PIN);

  // -------- NRF INIT --------
  if (!radioTX.begin(NRF_CE, NRF_CSN)) {
    Serial.println("❌ NRF24 INIT FAILED");
    nrfOk = false;
  } else {
    Serial.println("✅ NRF24 INIT OK");
    nrfOk = true;
  }
  statusLED.begin();
  
 // -------- WIFI INIT --------
wireless.initWiFi("Dialog 4G 079", "BA7b01b7");  // <-- connect first

if (wireless.isWiFiConnected()) {
    Serial.println("WiFi connected successfully! IP:");
    Serial.println(WiFi.localIP());

    // Then setup OTA
    ArduinoOTA.onStart([]() { Serial.println("OTA Start"); });
    ArduinoOTA.onEnd([]() { Serial.println("\nOTA End"); });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("OTA Progress: %u%%\r", (progress / (total / 100)));
    });
    ArduinoOTA.onError([](ota_error_t error) {
        Serial.printf("OTA Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

    ArduinoOTA.begin(); // <-- after WiFi
} else {
    Serial.println("WiFi connection failed!");
}

}

// ----------------------------------------
void loop() {
  ArduinoOTA.handle();

  imu.update();

  float x = imu.x();
  float y = imu.y();

  Direction dir = STOP;
  int speed = 0;

  // -------- NRF STATUS MONITOR --------
  if (millis() - lastNrfPrint > 1000) {
    lastNrfPrint = millis();

    if (radioTX.isOk()) {
      Serial.println("📡 NRF24 STATUS: OK");
      nrfOk = true;
    } else {
      Serial.println("❌ NRF24 STATUS: NOT RESPONDING");
      nrfOk = false;
    }
  }
  statusLED.update(nrfOk);

  // -------- STOP CONDITION --------
  if (fabs(x) < DEAD_ZONE && fabs(y) < DEAD_ZONE) {

    dir = STOP;
    speed = 0;

    radioTX.send(dir, speed);   // ✅ SEND STOP
    Serial.println(" | STOP | Speed: 0%");
    delay(30);
    return;
  }

  bool xActive = fabs(x) >= DEAD_ZONE;
  bool yActive = fabs(y) >= DEAD_ZONE;


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

  // -------- SEND DATA --------
  radioTX.send(dir, speed);   // ✅ SEND VIA NRF24
  
  // -------- OUTPUT --------
  Serial.print("X: "); Serial.print(x, 2);
  Serial.print(" Y: "); Serial.print(y, 2);
  Serial.print(" | DIR: "); Serial.print(dir);
  Serial.print(" | Speed: "); Serial.print(speed);
  Serial.println("%");

  delay(30);
}
