#include<WiFi.h>
#include<WiFiClient.h>
#include "PubSubClient.h"

#define LED 2  // built-in LED
#define TRIG_PIN 23
#define ECHO_PIN 22

char *SSID = "bayar";
char *PASS = "qwertyuiop";

// use IPAddress segment for MQTT_SERVER. NOTE: CHANGE THIS TO YOUR COMPUTER IP
IPAddress MQTT_SERVER(10, 10, 177, 145);
int MQTT_PORT = 1883;

// state initiation
unsigned long timePassed;
int ledState = 0;
unsigned long prevMic;
unsigned long prev;

// create wifi client and inject into PubSubClient
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
// function to connect to WiFi
void initConnection() {
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
 // set server and port
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
}
void doPublish(String topic, float payload) {
 char tmp[8];
 dtostrf(payload, 1, 2, tmp);
 mqttClient.publish(topic.c_str(), tmp);
 Serial.print(topic);
 Serial.print(" ==> ");
 Serial.println(payload);
}

void callback(String topic, byte *message, unsigned int length) {
 Serial.print("Topic: ");
 Serial.print(topic);
 Serial.print(" Message: ");
 String msgTemp;

 for (int i = 0; i < length; i++) {
   Serial.print((char)message[i]);
   msgTemp += (char)message[i];
 }
 Serial.println();

 if (topic == "esp32/led") {
   if (msgTemp == "ON") {
     Serial.println("ON");
   }
   if (msgTemp == "OFF") {
     Serial.println("OFF");
   }
 }
}
long getDistanceFromMicro(long microsec) {
 return microsec / 29 / 2;
}

long getPingDuration() {
 pinMode(TRIG_PIN, OUTPUT);  // put here to always clean
 pinMode(ECHO_PIN, INPUT);

 digitalWrite(TRIG_PIN, LOW);  // make low
 if (micros() - prevMic >= 2) {
   digitalWrite(TRIG_PIN, HIGH);
   prevMic = micros();
 }
 if (micros() - prevMic >= 10) {
   digitalWrite(TRIG_PIN, LOW);
   prevMic = micros();
 }

 return pulseIn(ECHO_PIN, HIGH);
}
void setup() {
 Serial.begin(9600);
 pinMode(LED, OUTPUT);
 pinMode(TRIG_PIN, OUTPUT);
 pinMode(ECHO_PIN, INPUT);
 initConnection();
 connectToBroker("my-localhost");
}

void loop() {
 if (millis() - timePassed >= 1000) {
   long pingDuration = getPingDuration();
   long distance = getDistanceFromMicro(pingDuration);
   doPublish("esp32/led", ledState);
   doPublish("esp32/ultra", distance);
   digitalWrite(LED, ledState);
   ledState = !ledState;
   timePassed = millis();
 }
}
