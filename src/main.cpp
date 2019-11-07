#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <EEPROM.h>
#include <connection/Network.hpp>
#include <connection/bluetooth/Bluetooth.hpp>

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
    strcpy(cfg.name, "Other");
    strcpy(cfg.ssid, "Nicoleta");
    strcpy(cfg.psk, "A21D2CCAD9");

	Bluetooth.init(cfg.name, cfg.ssid);
    Network.connect(cfg.ssid, cfg.psk);
}


void loop() {
    if (Network.getStatus() != WL_CONNECTED) {
        delay(1000);
        return;
    }

    Network.handleCommands();
}
