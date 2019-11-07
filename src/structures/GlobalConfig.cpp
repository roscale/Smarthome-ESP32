//
// Created by roscale on 2/1/19.
//

#include "GlobalConfig.hpp"

GlobalConfig GlobalConfig::load() {
	GlobalConfig cfg = {};
	EEPROM.readString(offsetof(GlobalConfig, name), cfg.name, sizeof(GlobalConfig::name));
	EEPROM.readString(offsetof(GlobalConfig, ssid), cfg.ssid, sizeof(GlobalConfig::ssid));
	EEPROM.readString(offsetof(GlobalConfig, psk), cfg.psk, sizeof(GlobalConfig::psk));
	return cfg;
}

void GlobalConfig::readName(char *name) {
	EEPROM.readString(offsetof(GlobalConfig, name), name, sizeof(name));
}

void GlobalConfig::save() {
	EEPROM.writeString(offsetof(GlobalConfig, name), this->name);
	EEPROM.writeString(offsetof(GlobalConfig, ssid), this->ssid);
	EEPROM.writeString(offsetof(GlobalConfig, psk), this->psk);

	EEPROM.commit();
}
