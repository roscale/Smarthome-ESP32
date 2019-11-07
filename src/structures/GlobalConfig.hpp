//
// Created by roscale on 2/1/19.
//
#pragma once

#include <EEPROM.h>

struct GlobalConfig {
    char name[256];
    char ssid[256];
    char psk[256];

    static GlobalConfig load();
	static void readName(char *name);

	void save();
};
