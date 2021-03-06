//
// Created by roscale on 2/1/19.
//

#include "TestConnection.hpp"

void TestConnectionCallbacks::onWrite(BLECharacteristic *pCharacteristic) {
	Serial.println("[BLE] Testing connection");
	std::string response = pCharacteristic->getValue();

	buffer << response.c_str() + 1;

	if (response[0] == 0x02) {
		Serial.println(buffer.str().c_str());

		std::string rawString = buffer.str();
		std::string delimiter = "\n";
		std::string ssid = rawString.substr(0, rawString.find(delimiter));
		rawString.erase(0, rawString.find(delimiter) + delimiter.length());
		auto &psk = rawString;

		Network.disconnect();
		Network.connect(ssid.c_str(), psk.c_str());

		int connected = Network.getStatus() == WL_CONNECTED;
		Bluetooth.testConnectionTXCharacteristic->setValue(connected);
		Bluetooth.testConnectionTXCharacteristic->notify();

		buffer.str("");
	}
}
