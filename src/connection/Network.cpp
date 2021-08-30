//
// Created by roscale on 2/1/19.
//

#include "Network.hpp"
#include <structures/GlobalConfig.hpp>
#include <structures/PowerState.hpp>
#include <util.hpp>

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
			static char name[256];
			/* Reset leftover characters
			 * If we first set a long name and then a shorter one, the
			 * characters that are not overwritten remain in the string */
			memset(name, 0, strlen(name));

			GlobalConfig::readName(name, 256);
			Serial.println(name);
			String message = createDiscoveryMessage(UUID, name, powerState);

			client.print(message);
			client.flush();
			client.stop();

			Serial.print("Sent: ");
			Serial.println(message);

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

					auto* dataStr = (char*) data;

					if (strcmp(dataStr, "on") == 0) {
						Serial.println("ON");
						powerState = PowerState::ON;
						digitalWrite(LIGHT_PIN, HIGH);

					} else if (strcmp(dataStr, "off") == 0) {
						Serial.println("OFF");
						powerState = PowerState::OFF;
						digitalWrite(LIGHT_PIN, LOW);
					}
				}
			}
		}
	}
}
