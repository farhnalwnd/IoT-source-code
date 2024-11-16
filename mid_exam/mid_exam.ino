#include <WiFi.h>
#include <WiFiClient.h>
#include "PubSubClient.h"

#define LED1 2  // Built-in LED
#define LED2 4  // Additional LED

char* SSID = "bayar";
char* PASS = "qwertyuiop";

// Use IPAddress segment for MQTT_SERVER. NOTE: CHANGE THIS TO YOUR COMPUTER IP
IPAddress MQTT_SERVER(10, 10, 177, 145);
int MQTT_PORT = 1883;

// State initiation
unsigned long timePassed;
int ledState1 = 0;
int ledState2 = 1;  // Initial opposite state

// Create WiFi client and inject into PubSubClient
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Function to connect to WiFi
void initConnection() {
  Serial.print("Connecting to: ");
  Serial.println(SSID);
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nConnected to address: ");
  Serial.println(WiFi.localIP());
}

// Function to connect to MQTT Broker
void connectToBroker(String clientName) {
  // Set server and port
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(callback);
  Serial.println("Connecting to MQTT Broker");
  String clientId = "ESP32Client-" + clientName;
  mqttClient.connect(clientId.c_str());
  while (!mqttClient.connected()) {
    Serial.print("Failed: ");
    Serial.println(mqttClient.state());
    Serial.println("Attempting to reconnect");
    delay(500);
  }
  Serial.print("Connected to ");
  Serial.println(clientName);
  mqttClient.subscribe("esp32/led1");
  mqttClient.subscribe("esp32/led2");
}

void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Topic: ");
  Serial.print(topic);
  Serial.print(" Message: ");
  String msgTemp;

  for (int i = 0; i < length; i++) {
    Serial.print((char)message[i]);
    msgTemp += (char)message[i];
  }
  Serial.println();

  if (String(topic) == "esp32/led1") {
    if (msgTemp == "ON") {
      digitalWrite(LED1, HIGH);
      Serial.println("LED1 ON");
    }
    if (msgTemp == "OFF") {
      digitalWrite(LED1, LOW);
      Serial.println("LED1 OFF");
    }
  }

  if (String(topic) == "esp32/led2") {
    if (msgTemp == "ON") {
      digitalWrite(LED2, HIGH);
      Serial.println("LED2 ON");
    }
    if (msgTemp == "OFF") {
      digitalWrite(LED2, LOW);
      Serial.println("LED2 OFF");
    }
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  initConnection();
  connectToBroker("my-localhost");
}

void loop() {
  if (!mqttClient.connected()) {
    connectToBroker("my-localhost");
  }
  mqttClient.loop();
}
