#ifndef WIRELESS_H
#define WIRELESS_H

#include <Arduino.h>
#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

class Wireless {
public:
    // WiFi
    void initWiFi(const char* ssid, const char* password);
    bool isWiFiConnected();
    void printWiFiStatus();

    // BLE
    void initBLE(const char* deviceName, const char* serviceUUID, const char* charUUID);
    void sendBLEMessage(const std::string &message);

private:
    // BLE internal
    BLEServer* pServer = nullptr;
    BLECharacteristic* pCharacteristic = nullptr;
    bool deviceConnected = false;
    
    class MyServerCallbacks : public BLEServerCallbacks {
        void onConnect(BLEServer* pServer) {
            deviceConnected = true;
        }
        void onDisconnect(BLEServer* pServer) {
            deviceConnected = false;
        }
    };
};

#endif
