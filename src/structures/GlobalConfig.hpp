//
// Created by roscale on 2/1/19.
//
#pragma once

#include <EEPROM.h>

struct GlobalConfig {
	std::string name;
	std::string ssid;
	std::string psk;
	bool power;

	void load();

	void save();

	static GlobalConfig& instance();

private:
	static GlobalConfig* singleton;
};
