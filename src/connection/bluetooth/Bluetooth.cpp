//
// Created by roscale on 2/1/19.
//

#include "Bluetooth.hpp"
#include <HardwareSerial.h>
#include <constants.hpp>
#include <util.hpp>
#include <tiny-json.h>
#include <structures/PowerState.hpp>

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
			String info = createDiscoveryMessage(UUID, "NEW Prototype", 1);
			btSerial.print(info);
			btSerial.flush();
			return;
//			btSerial.disconnect();
		}

		const json_t* powerStateProperty = json_getProperty(json, "power_state");
		if (powerStateProperty != nullptr) {
			if (json_getType(powerStateProperty) != JSON_INTEGER) {
				printJsonError();
				return;
			}
			int64_t powerStateValue = json_getInteger(powerStateProperty);
			Serial.print("Power State: ");
			Serial.println(powerStateValue);
			if (powerStateValue == 1) {
				powerState = PowerState::ON;
				digitalWrite(LIGHT_PIN, HIGH);
			} else {
				powerState = PowerState::OFF;
				digitalWrite(LIGHT_PIN, LOW);
			}
		}

		btSerial.flush();
		btSerial.disconnect();
	}
}
