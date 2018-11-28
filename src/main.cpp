#include <WiFi.h>
#include "helper.hpp"


const char* SSID = "Nicoleta";
const char* PASSWORD =  "A21D2CCAD9";
const char* UUID = "kf5hf536sa";

const int COMMAND_PORT = 8088;
const int DISCOVERY_LISTEN_PORT = 9997;
const int DISCOVERY_SEND_PORT = 9001;

const int LIGHT_PIN = 2;


WiFiUDP udpListener;
WiFiServer server(COMMAND_PORT);
WiFiClient client;

const int dataLength = 50;
uint8_t data[dataLength];

enum PowerState {
	OFF = 0,
	ON = 1
} powerState = PowerState::OFF;


void setup() {
    Serial.begin(115200);
	pinMode(LIGHT_PIN, OUTPUT);

    WiFi.begin(SSID, PASSWORD);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi..");
    }
    Serial.print("Connected to the WiFi network: ");
    Serial.println(WiFi.localIP());

	udpListener.begin(DISCOVERY_LISTEN_PORT);
	server.begin();
}


void loop() {

	// Discovery through UDP
	int udpMsgLength = udpListener.parsePacket();
	if (udpMsgLength != 0) {
		char udpPacket[udpMsgLength+1];
		udpListener.read(udpPacket, udpMsgLength);
		udpPacket[udpMsgLength] = 0x0;

		if (strcmp(udpPacket, "discovery") != 0) {
			udpListener.flush();
			return;
		}

		Serial.println(udpPacket);
		Serial.println(udpListener.remoteIP());

		if (client.connect(udpListener.remoteIP(), DISCOVERY_SEND_PORT)) {
			String message = createDiscoveryMessage(UUID, powerState);

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
					if (len < dataLength){
						data[len] = '\0';
					} else {
						data[dataLength] = '\0';
					}

					char* dataStr = (char*) data;

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



