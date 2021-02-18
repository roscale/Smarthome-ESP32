//
// Created by roscale on 2/1/19.
//

#include "SaveConfig.hpp"
#include <HardwareSerial.h>
#include <structures/GlobalConfig.hpp>

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

		GlobalConfig cfg{};
		strcpy(cfg.name, name.c_str());
		strcpy(cfg.ssid, ssid.c_str());
		strcpy(cfg.psk, psk.c_str());

		cfg.save();

		Bluetooth.nameCharacteristic->setValue(cfg.name);
		Bluetooth.ssidCharacteristic->setValue(cfg.ssid);

		Network.disconnect();
		Network.connect(ssid.c_str(), psk.c_str());

		buffer.str("");
	}
}