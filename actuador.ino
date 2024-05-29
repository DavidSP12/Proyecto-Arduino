#include <ESP8266WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>
#include <Servo.h>

#define wifi_ssid "David"
#define wifi_pass "1qaz2wsx"

// Dirección del broker público de Eclipse
#define mqtt_servidor "test.mosquitto.org"
#define mqtt_puerto 1883

#define topicoActuador "Sensor/Accion"
#define SERVO_PIN 4

Servo servo;
WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topico, byte* payload, unsigned int length) {
    Serial.print("Mensaje recibido en el topico: ");
    Serial.println(topico);
    char msj[length + 1];
    for (int i = 0; i < length; i++) {
        msj[i] = (char)payload[i];
    }
    msj[length] = '\0';
    Serial.print("Payload: ");
    Serial.println(msj);

    // Verificar el valor recibido y girar el servo
    if (strcmp(msj, "1") == 0) {
        servo.write(90); // Girar 90 grados si el mensaje es "1"
        delay(3000);
        servo.write(0); // Volver a la posición inicial
    }
}

void setup() {
    Serial.begin(9600);
    setup_wifi();
    client.setServer(mqtt_servidor, mqtt_puerto);
    client.setCallback(callback);
    servo.attach(SERVO_PIN);
}

void setup_wifi() {
    delay(10);
    Serial.println();
    Serial.print("Conectando a ");
    Serial.println(wifi_ssid);
    WiFi.begin(wifi_ssid, wifi_pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("Conectado exitosamente!");
    Serial.println("IP: ");
    Serial.println(WiFi.localIP());
}

void reconnect() {
    while (!client.connected()) {
        Serial.print("Intentando la conexión MQTT...");
        if (client.connect("ESP8266Client")) {
            Serial.println("Conectado");
            client.subscribe(topicoActuador);
        } else {
            Serial.print("fallo =");
            Serial.print(client.state());
            Serial.println(" se intentará de nuevo en 3 segundos");
            delay(3000);
        }
    }
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop();
    delay(1000);
}