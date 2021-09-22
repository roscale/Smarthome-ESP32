//
// Created by roscale on 2/1/19.
//

#include "Bluetooth.hpp"
#include <HardwareSerial.h>
#include <constants.hpp>
#include <util.hpp>
#include <tiny-json.h>
#include <structures/GlobalConfig.hpp>

BluetoothClass Bluetooth;

void BluetoothClass::init(const char* name) {
	btSerial = BluetoothSerial();
	if (!btSerial.begin(name)) {
		Serial.println("Failed to initialize bluetooth serial.");
	}
}

void BluetoothClass::handleCommands() {
	if (btSerial.available()) {
		String jsonString = btSerial.readString();

		auto printJsonError = [&]() {
			Serial.print("Couldn't parse json: ");
			Serial.println(jsonString.c_str());
			btSerial.flush();
			btSerial.disconnect();
		};

		char* jsonStr = &jsonString[0];
		json_t mem[32];
		const json_t* json = json_create(jsonStr, mem, sizeof mem / sizeof *mem);
		if (!json) {
			printJsonError();
			return;
		}

		// Client requests device information
		if (json_getProperty(json, "info") != nullptr) {
			std::string info = createDiscoveryJson();
			btSerial.print(info.c_str());
			btSerial.flush();
			return;
		}

		if (json_getProperty(json, "wifi_info") != nullptr) {
			std::string info = createWiFiInfoJson();
			btSerial.print(info.c_str());
			btSerial.flush();
			return;
		}

		const json_t* powerProperty = json_getProperty(json, "power");
		if (powerProperty != nullptr) {
			if (json_getType(powerProperty) != JSON_BOOLEAN) {
				printJsonError();
				return;
			}
			bool powerValue = json_getBoolean(powerProperty);
			Serial.print("Power: ");
			Serial.println(powerValue);

			auto& cfg = GlobalConfig::instance();
			if (powerValue) {
				digitalWrite(LIGHT_PIN, HIGH);
				cfg.power = true;
				cfg.save();
			} else {
				digitalWrite(LIGHT_PIN, LOW);
				cfg.power = false;
				cfg.save();
			}
		}

		btSerial.flush();
		btSerial.disconnect();
	}
}
