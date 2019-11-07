//
// Created by roscale on 2/1/19.
//

#pragma once

#include <BLECharacteristic.h>
#include <connection/Network.hpp>

class BluetoothClass {
    friend class TestConnectionCallbacks; // Send back connection result
    friend class SaveConfigCallbacks; // Send back connection result

    BLECharacteristic *nameCharacteristic;
    BLECharacteristic *ssidCharacteristic;
    BLECharacteristic *testConnectionRXCharacteristic;
    BLECharacteristic *testConnectionTXCharacteristic;
    BLECharacteristic *saveConfigRXCharacteristic;

public:
    void init(const char *name, const char *ssid);
};

extern BluetoothClass Bluetooth;