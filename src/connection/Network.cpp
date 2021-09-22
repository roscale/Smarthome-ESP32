//
// Created by roscale on 2/1/19.
//

#include "Network.hpp"
#include <structures/GlobalConfig.hpp>
#include <util.hpp>
#include <tiny-json.h>

NetworkClass Network;

uint8_t NetworkClass::data[dataLength];

NetworkClass::NetworkClass() : server(COMMAND_PORT) {
}

void NetworkClass::connect(const char* ssid, const char* psk) {
	Serial.print("Connecting to the WiFi network: '");
	Serial.print(ssid);
	Serial.print("'");
	if (psk != nullptr) {
		Serial.print(" with the PSK: '");
		Serial.print(psk);
		Serial.println("'");
	} else {
		Serial.println(" without password");
	}

	WiFi.begin(ssid, psk);

	if ((status = WiFi.waitForConnectResult()) == WL_CONNECTED) {
		udpListener.begin(DISCOVERY_LISTEN_PORT);
		server.begin();

		Serial.print("Connection successful, local IP: ");
		Serial.println(WiFi.localIP());
	} else {
		Serial.println("Connection failed.");
	}
}

void NetworkClass::disconnect() {
	udpListener.stop();
	server.stop();
	WiFi.disconnect();
	status = WL_DISCONNECTED;
}

uint8_t NetworkClass::getStatus() {
	return status;
}

void NetworkClass::handleCommands() {
	if (Network.getStatus() != WL_CONNECTED) {
		return;
	}

	// Discovery through UDP, broadcasting using TCP is not possible
	int udpMsgLength = udpListener.parsePacket();
	if (udpMsgLength != 0) {
		// Parse the packet (a string) and append the null character in the end
		char udpPacket[udpMsgLength + 1];
		udpListener.read(udpPacket, udpMsgLength);
		udpPacket[udpMsgLength] = '\0';

		Serial.println("Reading packet");

		if (strcmp(udpPacket, "discovery") != 0) {
			udpListener.flush();
			return;
		}

		Serial.println(udpPacket);
		Serial.println(udpListener.remoteIP());

		if (client.connect(udpListener.remoteIP(), DISCOVERY_SEND_PORT)) {
			std::string message = createDiscoveryJson();

			client.print(message.c_str());
			client.flush();
			client.stop();

			Serial.print("Sent: ");
			Serial.println(message.c_str());

		} else {
			Serial.println("[UDP] Couldn't report back discovery");
		}

		// Receive commands through TCP
	} else {
		WiFiClient client = server.available();
		if (client) {
			Serial.print("[TCP] New client: ");
			Serial.println(client.remoteIP());

			while (client.connected()) {
				if (client.available()) {
					// Append the null character at the end of the message
					int len = client.read(data, dataLength - 1);
					data[len] = '\0';

					char* jsonStr = (char*) data;

					auto printJsonError = [&]() {
						Serial.print("Couldn't parse json: ");
						Serial.println(jsonStr);
						client.flush();
						client.stop();
					};

					json_t mem[32];
					const json_t* json = json_create(jsonStr, mem, sizeof mem / sizeof *mem);
					if (!json) {
						printJsonError();
						return;
					}

					auto& cfg = GlobalConfig::instance();

					const json_t* powerProperty = json_getProperty(json, "power");
					if (powerProperty != nullptr) {
						if (json_getType(powerProperty) != JSON_BOOLEAN) {
							printJsonError();
							return;
						}
						bool powerValue = json_getBoolean(powerProperty);
						if (powerValue) {
							digitalWrite(LIGHT_PIN, HIGH);
							cfg.power = true;
							cfg.save();
							Serial.println("ON");
						} else {
							digitalWrite(LIGHT_PIN, LOW);
							cfg.power = false;
							cfg.save();
							Serial.println("OFF");
						}
					}

					const json_t* wifiInfoProperty = json_getProperty(json, "wifi_info");
					if (wifiInfoProperty != nullptr) {
						std::string info = createWiFiInfoJson();
						client.print(info.c_str());
					}
				}
			}
		}
	}
}
