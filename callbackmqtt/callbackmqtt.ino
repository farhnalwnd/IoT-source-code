#include<WiFi.h>
#include<WiFiClient.h>
#include "PubSubClient.h"

#define LED 2  // built-in LED

char *MQTT_SERVER = "broker.emqx.io";
int MQTT_PORT = 1883;
char *SSID = "bayar";
char *PASS = "qwertyuiop";

// create wifi client and inject into PubSubClient
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// function to connect to WiFi
void connectToWiFi() {
 Serial.print("Connecting to: ");
 Serial.println(SSID);
 WiFi.begin(SSID, PASS);
 while (WiFi.status() != WL_CONNECTED) {
   Serial.print(".");
   delay(500);
 }
 Serial.print("\nConnected to address : ");
 Serial.println(WiFi.localIP());
}

// function to connect to MQTT Broker
void connectToBroker(String clientName) {
 mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
 mqttClient.setCallback(callback);  // for reading message

 String clientId = "ESP32Client-" + clientName;
 mqttClient.connect(clientId.c_str());
  Serial.println("Connecting to MQTT Broker");
  while (!mqttClient.connected()) {
   Serial.print("Failed: ");
   Serial.println(mqttClient.state());
   Serial.println("Attempting to reconnect");
   delay(500);
 }

 // if connected, subscribe to topic
 if (mqttClient.connected()) {
   doSubscribe();
 }
}

void doSubscribe() {
 // NOTE: you could subscribe to other topics
 mqttClient.subscribe("esp32/ledtoggle");
}

// function to publish to a topic with certain payload
void doPublish(String topic, float payload) {
 char tmp[8];
 dtostrf(payload, 1, 2, tmp);
 mqttClient.publish(topic.c_str(), tmp);
 Serial.print(topic);
 Serial.print(" ==> ");
 Serial.println(payload);
}

void callback(String topic, byte *message, unsigned int length) {
 String msgTemp;

 // convert byte to char
 for (int i = 0; i < length; i++) {
   msgTemp += (char)message[i];
 }

 // NOTE: Read topic and message then process
 if (topic == "esp32/ledtoggle") {
   if (msgTemp == "ON") {
     Serial.println("ON");
     digitalWrite(LED, HIGH);
   }
   if (msgTemp == "OFF") {
     Serial.println("OFF");
     digitalWrite(LED, LOW);
   }
 }
}

void setup() {
 Serial.begin(9600);
 pinMode(LED, OUTPUT);
 connectToWiFi();
 connectToBroker("my-localhost");
}

void loop() {
 mqttClient.loop();  // always loop client
}
