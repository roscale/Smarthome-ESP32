//
// Created by roscale on 2/1/19.
//

#pragma once

#include <BLECharacteristic.h>
#include <sstream>
#include <HardwareSerial.h>
#include <connection/Network.hpp>
#include <connection/bluetooth/Bluetooth.hpp>

class TestConnectionCallbacks : public BLECharacteristicCallbacks {
    Bluetooth *bluetooth;
    Network *network;
    std::stringstream buffer;

public:
    TestConnectionCallbacks(Bluetooth *bluetooth, Network *network);

    void onWrite(BLECharacteristic *pCharacteristic) override;
};
