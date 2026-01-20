#include "wireless.h"

// ---------------------- WiFi ----------------------
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

// ---------------------- BLE ----------------------
void Wireless::initBLE(const char* deviceName, const char* serviceUUID, const char* charUUID) {
    // Initialize BLE
    BLEDevice::init(deviceName);

    // Create BLE server
    pServer = BLEDevice::createServer();

    // ✅ Set callbacks for connect/disconnect
    pServer->setCallbacks(new MyServerCallbacks(this));

    // Create service and characteristic
    BLEService* pService = pServer->createService(serviceUUID);
    pCharacteristic = pService->createCharacteristic(
        charUUID,
        BLECharacteristic::PROPERTY_NOTIFY
    );

    // Add descriptor for notifications
    pCharacteristic->addDescriptor(new BLE2902());

    // Start the service
    pService->start();

    // Start advertising
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
