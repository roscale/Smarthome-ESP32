//
// Created by roscale on 2/1/19.
//

#pragma once

#include <cstdint>
#include <WiFi.h>
#include <WiFiServer.h>
#include <WiFiUdp.h>
#include "constants.hpp"

class NetworkClass {
    uint8_t status;
    WiFiClient client;
    WiFiServer server;
    WiFiUDP udpListener;

    // Incoming data buffer
    static constexpr int dataLength = 50;
    static uint8_t data[dataLength];

public:
    NetworkClass();

    void connect(const char *ssid, const char *psk = nullptr);

    void disconnect();

    uint8_t getStatus();

    void handleCommands();
};

extern NetworkClass Network;
