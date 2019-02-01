//
// Created by roscale on 2/1/19.
//
#pragma once

#include <EEPROM.h>

struct Data {
    char name[256];
    char ssid[256];
    char psk[256];
};

Data readData();

void readName(char *name);

void writeData(Data *data);
