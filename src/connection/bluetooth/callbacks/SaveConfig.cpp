//
// Created by roscale on 2/1/19.
//

#include "SaveConfig.hpp"
#include <HardwareSerial.h>
#include <structures/Data.hpp>

SaveConfigCallbacks::SaveConfigCallbacks(Bluetooth *bluetooth, Network *network) : bluetooth(bluetooth),
                                                                                   network(network) {
}

void SaveConfigCallbacks::onWrite(BLECharacteristic *pCharacteristic) {
    Serial.println("[BLE] Saving settings");
    std::string response = pCharacteristic->getValue();

    buffer << response.c_str() + 1;

    if (response[0] == 0x02) {
        Serial.println(buffer.str().c_str());

        std::string rawString = buffer.str();
        std::string delimiter = "\n";

        std::string name = rawString.substr(0, rawString.find(delimiter));
        rawString.erase(0, rawString.find(delimiter) + delimiter.length());

        std::string ssid = rawString.substr(0, rawString.find(delimiter));
        rawString.erase(0, rawString.find(delimiter) + delimiter.length());

        auto &psk = rawString;

        Data data{};
        strcpy(data.name, name.c_str());
        strcpy(data.ssid, ssid.c_str());
        strcpy(data.psk, psk.c_str());

        writeData(&data);

        bluetooth->nameCharacteristic->setValue(data.name);
        bluetooth->ssidCharacteristic->setValue(data.ssid);

        network->disconnect();
        network->connect(ssid.c_str(), psk.c_str());

        buffer.str("");
    }
}