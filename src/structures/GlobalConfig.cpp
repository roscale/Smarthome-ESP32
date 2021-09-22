//
// Created by roscale on 2/1/19.
//

#include "GlobalConfig.hpp"

GlobalConfig* GlobalConfig::singleton = nullptr;

const size_t str_size = 256;

void GlobalConfig::load() {
	static char buffer[str_size];

	EEPROM.readString(0 * str_size, buffer, str_size);
	name = buffer;
	EEPROM.readString(1 * str_size, buffer, str_size);
	ssid = buffer;
	EEPROM.readString(2 * str_size, buffer, str_size);
	psk = buffer;
	power = EEPROM.readBool(3 * str_size);
}

void GlobalConfig::save() {
	EEPROM.writeString(0 * str_size, name.c_str());
	EEPROM.writeString(1 * str_size, ssid.c_str());
	EEPROM.writeString(2 * str_size, psk.c_str());
	EEPROM.writeBool(3 * str_size, power);

	EEPROM.commit();
}

GlobalConfig& GlobalConfig::instance() {
	if (singleton == nullptr) {
		singleton = new GlobalConfig;
	}
	return *singleton;
}
