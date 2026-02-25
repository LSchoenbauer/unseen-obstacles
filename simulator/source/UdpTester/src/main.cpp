// for disabling brown-out detector
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems

#include <net/Ip4Address.h>
#include <utils/Log.h>

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiUdp.h>

using namespace Net;

// WiFi credentials
const char* ssid = "Unseen-Obstacles";
const char* password = "unseenobstaclesEKLS";

// UDP settings
WiFiUDP udp;
const Ip4Address remoteIP(192, 168, 7, 3);
const uint32_t remotePort = 4210; // Change to your target port

const uint32_t pinLed = 13; // GPIO13, built-in LED on many ESP32 boards
#define turnOnLed() digitalWrite(pinLed, LOW)
#define turnOffLed() digitalWrite(pinLed, HIGH)

// Data buffer: 8 uint32_t values (32 bytes)
uint32_t data[8] = {0};
uint32_t lastSendTime = 0;
const uint32_t sendInterval = 1000; // 1 Hz

void setup() {
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0); //disable brownout detector

    Logger::Init(Logger::LogSink::Serial);
    pinMode(pinLed, OUTPUT);
    for (int i = 0; i < 3; i++) {
        turnOnLed();
        delay(100);
        turnOffLed();
        delay(100);
    }

    LogInfo("Connecting to WiFi '%s'...", ssid);

    uint32_t retryCount = 0;
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        if (retryCount == 0) {
            LogInfo(""); // line break
        }
        LogMsgStr(".");
        if (retryCount % 2 == 0) {
            // turnOnLed();
            digitalWrite(pinLed, LOW);
        } else {
            // turnOffLed();
            digitalWrite(pinLed, HIGH);
        }
        retryCount++;
        if (retryCount == 80) {
            LogMsgStr("\n");
            retryCount = 0;
        }
    }

    LogInfo("\nWiFi connected");
    LogInfo("IP address: %s", WiFi.localIP().toString().c_str());
    turnOnLed();
    
    // Initialize UDP
    delay(1000);
    LogInfo("\nConnecting to UDP server at %s:%d...", remoteIP.toString().c_str(), remotePort);
    udp.begin(WiFi.localIP(), remotePort);
}

void loop() {
    unsigned long currentMillis = millis();

    // Send data every 1 second
    if (currentMillis - lastSendTime >= sendInterval) {
        lastSendTime = currentMillis;

        // Increment each value
        for (int i = 0; i < 8; i++) {
            data[i]++;
        }

        // Send data
        turnOffLed();
        LogInfo("Sending data ..."); 
        udp.beginPacket(remoteIP, remotePort);
        size_t bytesSent = udp.write((uint8_t*)data, sizeof(data));
        udp.endPacket();
        LogInfo("Sent data: %u, %u, %u, %u, %u, %u, %u, %u (bytes sent: %zu)", data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], bytesSent);
        turnOnLed();
    }
}
