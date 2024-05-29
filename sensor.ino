#include <ESP8266WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN D4
#define RST_PIN D3
MFRC522 mfrc522(SS_PIN, RST_PIN);

#define WIFI_SSID "David"
#define WIFI_PASSWORD "1qaz2wsx"

// Dirección del broker público de Eclipse
#define MQTT_SERVER "test.mosquitto.org"
#define MQTT_PORT 1883

#define TOPIC_SENSOR "Sensor/Identidad"

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
    Serial.begin(9600);
    setupWifi();
    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setCallback(callback);
    SPI.begin();
    mfrc522.PCD_Init();
}

void setupWifi() {
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("Connected successfully!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (client.connect("ESP8266Client")) {
            Serial.println("Connected");
        } else {
            Serial.print("Failed, rc=");
            Serial.print(client.state());
            Serial.println(" Trying again in 3 seconds");
            delay(3000);
        }
    }
}

void callback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
        String id = "";
        for (byte i = 0; i < mfrc522.uid.size; i++) {
            id.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
            id.concat(String(mfrc522.uid.uidByte[i], HEX));
        }
        id.toUpperCase();
        if (id.isEmpty()) {
            Serial.println("Error reading card");
            return;
        }
        Serial.print("ID read: ");
        Serial.println(id);
        client.publish(TOPIC_SENSOR, id, true);
        mfrc522.PICC_HaltA();
    }
    delay(1000);
}