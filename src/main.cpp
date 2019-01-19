#include <stddef.h>
#include <sstream>
#include <string>

#include <WiFi.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <EEPROM.h>
#include <BLE2902.h>

#include "helper.hpp"

const char *SERVICE_UUID = "b1d109ed-eb34-4421-8780-841efba77469";
const char *NAME_CHARACTERISTIC_UUID = "6cba5470-a064-4479-a4e9-c3a3b479d402";
const char *SSID_CHARACTERISTIC_UUID = "8cdceb63-9875-43a1-af5e-eee545068327";
const char *TEST_CONNECTION_RX_CHARACTERISTIC_UUID = "bc147d05-ecdd-47bb-a2bd-55edaf2bd1e0";
const char *TEST_CONNECTION_TX_CHARACTERISTIC_UUID = "0799315c-2ad0-46d4-8070-6fc3539174c2";

const char *SSID = "Nicoleta";
const char *PASSWORD = "A21D2CCAD9";
const char *UUID = "black";

const int COMMAND_PORT = 8088;
const int DISCOVERY_LISTEN_PORT = 9997;
const int DISCOVERY_SEND_PORT = 9001;

const int LIGHT_PIN = 2;

uint8_t networkStatus;
WiFiUDP udpListener;
WiFiServer server(COMMAND_PORT);
WiFiClient client;

const int dataLength = 50;
uint8_t data[dataLength];

enum PowerState {
    OFF = 0,
    ON = 1
} powerState = PowerState::OFF;


struct Data {
    char name[256];
    char ssid[256];
    char psk[256];
};

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
    EEPROM.writeChar(offsetof(Data, name) + sizeof(Data::name) - 1, '\0');

    EEPROM.writeString(offsetof(Data, ssid), data->ssid);
    EEPROM.writeChar(offsetof(Data, ssid) + sizeof(Data::ssid) - 1, '\0');

    EEPROM.writeString(offsetof(Data, psk), data->psk);
    EEPROM.writeChar(offsetof(Data, psk) + sizeof(Data::psk) - 1, '\0');
}


uint8_t connectToNetwork(const char* ssid, const char* psk = nullptr) {
    uint8_t status;

    Serial.print("Connecting to the WiFi network: '");
    Serial.print(ssid);
    Serial.print("' with the PSK: '");
    Serial.print(psk);
    Serial.println("'");

    WiFi.begin();
    if ((status = WiFi.waitForConnectResult()) == WL_CONNECTED) {
        udpListener.begin(DISCOVERY_LISTEN_PORT);
        server.begin();

        Serial.print("Connection successful, local IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("Connection failed.");
    }
    return status;
}

void disconnectFromNetwork() {
    server.stop();
    udpListener.stop();
    Serial.println("Disconnecting from network.");
    WiFi.disconnect();
}


// TODO BIG refactoring
BLECharacteristic* testConnectionTXCharacteristic;


class TestConnectionCallbacks : public BLECharacteristicCallbacks {
    std::stringstream buffer;

public:
    void onWrite(BLECharacteristic *pCharacteristic) override {
        std::string response = pCharacteristic->getValue();

        buffer << response.c_str() + 1;

        if (response[0] == 0x02) {
            Serial.println(buffer.str().c_str());

            std::string s = buffer.str();
            std::string delimiter = "\n";
            std::string ssid = s.substr(0, s.find(delimiter));
            s.erase(0, s.find(delimiter) + delimiter.length());
            auto& psk = s;

            disconnectFromNetwork();
            networkStatus = connectToNetwork(ssid.c_str(), psk.c_str());
            int connected = networkStatus == WL_CONNECTED;

            testConnectionTXCharacteristic->setValue(connected);
            testConnectionTXCharacteristic->notify();

            buffer.str("");
        }
    }
};


void setup() {
    Serial.begin(115200);
    pinMode(LIGHT_PIN, OUTPUT);

    // EEPROM
    if (!EEPROM.begin(4096)) {
        Serial.println("Failed to initialise EEPROM");
        Serial.println("Restarting...");
        delay(1000);
        ESP.restart();
    }

    Data dataToWrite = {"Smartbulb", "bbox2-2669", ""};
    writeData(&dataToWrite);
    EEPROM.commit();

    Data data = readData();

    // WiFi
    networkStatus = connectToNetwork(data.ssid, data.psk);

    // BLE
    BLEDevice::init(data.name);
    BLEServer *pServer = BLEDevice::createServer();
    BLEService *pService = pServer->createService(SERVICE_UUID);

    BLECharacteristic *nameCharacteristic = pService->createCharacteristic(
            NAME_CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE
    );
    nameCharacteristic->setValue(data.name);

    BLECharacteristic *ssidCharacteristic = pService->createCharacteristic(
            SSID_CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE
    );
    ssidCharacteristic->setValue(data.ssid);

    BLECharacteristic *testConnectionRXCharacteristic = pService->createCharacteristic(
            TEST_CONNECTION_RX_CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_WRITE
    );
    testConnectionRXCharacteristic->setCallbacks(new TestConnectionCallbacks());

    testConnectionTXCharacteristic = pService->createCharacteristic(
            TEST_CONNECTION_TX_CHARACTERISTIC_UUID,
            BLECharacteristic::PROPERTY_NOTIFY
    );
    testConnectionTXCharacteristic->addDescriptor(new BLE2902());

    pService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();

    Serial.println("Characteristic defined! Now you can read it in your phone!");
}


void loop() {
    if (networkStatus != WL_CONNECTED) {
        delay(1000);
        return;
    }

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

                    char *dataStr = (char *) data;

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



