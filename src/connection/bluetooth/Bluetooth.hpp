//
// Created by roscale on 2/1/19.
//

#pragma once

#include <BLECharacteristic.h>
#include <connection/Network.hpp>

class Bluetooth {
    friend class TestConnectionCallbacks; // Send back connection result
    friend class SaveConfigCallbacks; // Send back connection result

    Network *network;

    BLECharacteristic *nameCharacteristic;
    BLECharacteristic *ssidCharacteristic;
    BLECharacteristic *testConnectionRXCharacteristic;
    BLECharacteristic *testConnectionTXCharacteristic;
    BLECharacteristic *saveConfigRXCharacteristic;

public:
    void init(Network *network, const char *name, const char *ssid);
};
