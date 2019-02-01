//
// Created by roscale on 2/1/19.
//

#include "Network.hpp"
#include <structures/Data.hpp>
#include <structures/PowerState.hpp>
#include <helper.hpp>

uint8_t Network::data[dataLength];

Network::Network() : server(COMMAND_PORT) {
}

void Network::connect(const char *ssid, const char *psk) {
    Serial.print("Connecting to the WiFi network: '");
    Serial.print(ssid);
    Serial.print("' with the PSK: '");
    Serial.print(psk);
    Serial.println("'");

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

void Network::disconnect() {
    udpListener.stop();
    server.stop();
    WiFi.disconnect();
    status = WL_DISCONNECTED;
}

uint8_t Network::getStatus() {
    return status;
}

void Network::handleCommands() {
    // Discovery through UDP
    int udpMsgLength = udpListener.parsePacket();
    if (udpMsgLength != 0) {
        char udpPacket[udpMsgLength + 1];
        udpListener.read(udpPacket, udpMsgLength);
        udpPacket[udpMsgLength] = 0x0;

        Serial.println("Reading packet");

        if (strcmp(udpPacket, "discovery") != 0) {
            udpListener.flush();
            return;
        }

        Serial.println(udpPacket);
        Serial.println(udpListener.remoteIP());

        if (client.connect(udpListener.remoteIP(), DISCOVERY_SEND_PORT)) {
            static char name[256];
            memset(name, 0, strlen(name)); // Fix remaining characters from the past

            readName(name);
            Serial.println(name);
            String message = createDiscoveryMessage(UUID, name, powerState);

            client.print(message);
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
                    int len = client.read(data, dataLength);
                    if (len < dataLength) {
                        data[len] = '\0';
                    } else {
                        data[dataLength - 1] = '\0';
                    }

                    auto *dataStr = (char *) data;

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
