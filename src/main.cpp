#include <WiFi.h>
#include <EEPROM.h>
#include "BluetoothSerial.h"
#include <connection/Network.hpp>
#include <connection/Bluetooth.hpp>
#include <tiny-json.h>
#include "structures/GlobalConfig.hpp"

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

	auto& cfg = GlobalConfig::instance();
	cfg.load();

	if (cfg.power) {
		digitalWrite(LIGHT_PIN, HIGH);
	}

	Bluetooth.init("Smarthome-1");
	Network.connect(cfg.ssid.c_str(), cfg.psk.c_str());
}

void loop() {
	Bluetooth.handleCommands();
	Network.handleCommands();
	delay(100);
}
