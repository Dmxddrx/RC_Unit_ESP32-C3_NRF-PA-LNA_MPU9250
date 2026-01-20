#ifndef WIRELESS_H
#define WIRELESS_H

#include <Arduino.h>
#include <WiFi.h>

class Wireless {
public:
    // ---------- WiFi ----------
    void initWiFi(const char* ssid, const char* password);
    bool isWiFiConnected();
    void printWiFiStatus();
};

#endif
