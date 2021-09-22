//
// Created by roscale on 2/1/19.
//

#pragma once

#include "BluetoothSerial.h"

class BluetoothClass {
	BluetoothSerial btSerial;
public:
	void init(const char* name);
	void handleCommands();
};

extern BluetoothClass Bluetooth;