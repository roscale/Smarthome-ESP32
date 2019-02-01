//
// Created by roscale on 2/1/19.
//

#include "Data.hpp"

Data readData() {
    Data data = {};
    EEPROM.readString(offsetof(Data, name), data.name, sizeof(Data::name));
    EEPROM.readString(offsetof(Data, ssid), data.ssid, sizeof(Data::ssid));
    EEPROM.readString(offsetof(Data, psk), data.psk, sizeof(Data::psk));
    return data;
}

void readName(char *name) {
    EEPROM.readString(offsetof(Data, name), name, sizeof(name));
}

void writeData(Data *data) {
    EEPROM.writeString(offsetof(Data, name), data->name);
    EEPROM.writeString(offsetof(Data, ssid), data->ssid);
    EEPROM.writeString(offsetof(Data, psk), data->psk);

    EEPROM.commit();
}