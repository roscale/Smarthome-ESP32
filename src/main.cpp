#include <WiFi.h>
#include <EEPROM.h>
#include "BluetoothSerial.h"
#include <connection/Network.hpp>
#include <connection/bluetooth/Bluetooth.hpp>
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

//    GlobalConfig cfg = GlobalConfig::load();
	GlobalConfig cfg = GlobalConfig{};

	strcpy(cfg.name, "NEW Prototype");
	strcpy(cfg.ssid, "Nicoleta");
	strcpy(cfg.psk, "A21D2CCAD9");

	Bluetooth.init("Smarthome-1");
	Network.connect(cfg.ssid, cfg.psk);
}

int i = 0;

void loop() {
	Bluetooth.handleCommands();
	Network.handleCommands();
	delay(100);
}
