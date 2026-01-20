#include "wireless.h"

// WiFi setup
void Wireless::initWiFi(const char* ssid, const char* password) {
    Serial.print("Connecting to WiFi: ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    int retries = 0;
    while (WiFi.status() != WL_CONNECTED && retries < 20) {
        delay(500);
        Serial.print(".");
        retries++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi connected!");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nWiFi connection failed!");
    }
}

bool Wireless::isWiFiConnected() {
    return WiFi.status() == WL_CONNECTED;
}

void Wireless::printWiFiStatus() {
    if (isWiFiConnected()) {
        Serial.print("WiFi connected: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("WiFi not connected.");
    }
}

// BLE setup
void Wireless::initBLE(const char* deviceName, const char* serviceUUID, const char* charUUID) {
    BLEDevice::init(deviceName);

    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    BLEService* pService = pServer->createService(serviceUUID);
    pCharacteristic = pService->createCharacteristic(
        charUUID,
        BLECharacteristic::PROPERTY_NOTIFY
    );

    pCharacteristic->addDescriptor(new BLE2902());
    pService->start();

    BLEAdvertising* pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(serviceUUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();

    Serial.println("BLE device started and advertising.");
}

void Wireless::sendBLEMessage(const std::string &message) {
    if (pCharacteristic && deviceConnected) {
        pCharacteristic->setValue(message);
        pCharacteristic->notify();
        Serial.print("BLE message sent: ");
        Serial.println(message.c_str());
    }
}
