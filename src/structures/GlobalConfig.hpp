//
// Created by roscale on 2/1/19.
//
#pragma once

#include <EEPROM.h>

struct GlobalConfig {
	char name[256] = {0};
	char ssid[256] = {0};
	char psk[256] = {0};

	static GlobalConfig load();

	static void readName(char *name, size_t len);

	void save();
};
