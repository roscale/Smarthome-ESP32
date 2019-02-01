//
// Created by roscale on 2/1/19.
//

#include "Bluetooth.hpp"
#include <HardwareSerial.h>
#include <BLEDevice.h>
#include <BLE2902.h>
#include <constants.hpp>
#include <connection/bluetooth/callbacks/TestConnection.hpp>
#include <connection/bluetooth/callbacks/SaveConfig.hpp>

void Bluetooth::init(Network *network, const char *name, const char *ssid) {
    this->network = network;
    BLEDevice::init(name);

    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);

    nameCharacteristic = pService->createCharacteristic(
            NAME_CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE
    );
    nameCharacteristic->setValue(name);

    ssidCharacteristic = pService->createCharacteristic(
            SSID_CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE
    );
    ssidCharacteristic->setValue(ssid);

    testConnectionRXCharacteristic = pService->createCharacteristic(
            TEST_CONNECTION_RX_CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_WRITE
    );
    testConnectionRXCharacteristic->setCallbacks(new TestConnectionCallbacks(this, network));

    testConnectionTXCharacteristic = pService->createCharacteristic(
            TEST_CONNECTION_TX_CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_NOTIFY
    );
    testConnectionTXCharacteristic->addDescriptor(new BLE2902());

    saveConfigRXCharacteristic = pService->createCharacteristic(
            SAVE_CONFIG_RX_CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_WRITE
    );
    saveConfigRXCharacteristic->setCallbacks(new SaveConfigCallbacks(nullptr, network));

    pService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();

    Serial.println("[BLE] Started.");
}
