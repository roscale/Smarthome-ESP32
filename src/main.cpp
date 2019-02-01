#include <cstddef>
#include <sstream>
#include <string>

#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <EEPROM.h>
#include <BLE2902.h>
#include <connection/Network.hpp>
#include <connection/bluetooth/Bluetooth.hpp>

#include "structures/Data.hpp"
#include "helper.hpp"

Network* network;
Bluetooth* bluetooth;

void setup() {
    Serial.begin(115200);
    pinMode(LIGHT_PIN, OUTPUT);

    // Storage
    if (!EEPROM.begin(4096)) {
        Serial.println("Failed to initialise EEPROM");
        Serial.println("Restarting...");
        delay(1000);
        ESP.restart();
    }

    network = new Network();
    bluetooth = new Bluetooth();

    Data data = readData();
    bluetooth->init(network, data.name, data.ssid);
    network->connect(data.ssid, data.psk);
}


void loop() {
    if (network.getStatus() != WL_CONNECTED) {
        delay(1000);
        return;
    }

    network.handleCommands();
}
