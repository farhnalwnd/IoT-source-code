#include<WiFi.h>
#include<AsyncTCP.h>
#include<ESPAsyncWebServer.h>
#define LED 1

char *ssid = "bayar";
char *password = "qwertyuiop";

AsyncWebServer server(80);

const char *index_html = R"rawliteral(
 <html>
 <body>
 <h1>ESP32 Web Server</h2>
 %BUTTON_PLACEHOLDER%
 <script>
   function toggle(e) {
     var xhr = new XMLHttpRequest();
     if (e.checked) {
       xhr.open("GET", "/update?output=" + e.id + "&state=1", true);
     } else {
       xhr.open("GET", "/update?output=" + e.id + "&state=0", true);
     }
     xhr.send();
   }
 </script>
 </body>
 </html>
)rawliteral";

String processor(const String &var) {
 if (var == "BUTTON_PLACEHOLDER") {
   String buttons = "";
   buttons += "<h4>Toggle <input type=\"checkbox\" id=\"1\" onchange=\"toggle(this)\"" + outputState(LED) + " /></h4>";
   return buttons;
 }
 return String();
}

String outputState(int output) {
 if (digitalRead(output)) {
   return "checked";
 } else {
   return "";
 }
}

void initConnection() {
 Serial.print("Connecting to ");
 Serial.println(ssid);
 WiFi.begin(ssid, password);
 while(WiFi.status() != WL_CONNECTED) {
   delay(500);
   Serial.print(".");
 }

 Serial.println("");
 Serial.print("Established Connection: ");
 Serial.println(WiFi.localIP());

 server.begin();
}

void handleRoot(AsyncWebServerRequest *request) {
 request->send_P(200, "text/html", index_html, processor);
}

void handleToggle(AsyncWebServerRequest *request) {
 String msgIn1 = "No Message", msgIn2 = "No Message";
 if (request->hasParam("output") && request->hasParam("state")) {
   msgIn1 = request->getParam("output")->value();
   msgIn2 = request->getParam("state")->value();
   digitalWrite(msgIn1.toInt(), msgIn2.toInt());
 }

 Serial.print("PIN: ");
 Serial.println(msgIn1);
 Serial.print("State: ");
 Serial.println(msgIn2);
 request->send(200, "text/html", "OK");
}

void setupRoute() {
 server.on("/", HTTP_GET, handleRoot);
 server.on("/update", HTTP_GET, handleToggle);
}

void setup() {
 Serial.begin(9600);
 pinMode(LED, OUTPUT);
 initConnection();
 setupRoute();
}

void loop() {
 // nothing here
}

// #include <WiFi.h>
// #include <AsyncTCP.h>
// #include <ESPAsyncWebServer.h>

// #define LED 1

// char *ssid = "bayar";
// char *password = "qwertyuiop";

// AsyncWebServer server(80);

// const char *index_html = R"rawliteral(
// <html>
// <body>
// <h1>ESP32 Web Server</h1>
// %BUTTON_PLACEHOLDER%
// <script>
//   function toggle(e) {
//     var xhr = new XMLHttpRequest();
//     if (e.checked) {
//       xhr.open("GET", "/update?output=" + e.id + "&state=1", true);
//     } else {
//       xhr.open("GET", "/update?output=" + e.id + "&state=0", true);
//     }
//     xhr.send();
//   }
// </script>
// </body>
// </html>
// )rawliteral";

// String processor(const String &var) {
//   if (var == "BUTTON_PLACEHOLDER") {
//     String buttons = "";
//     // Jika ID LED adalah 1, maka atur statusnya menjadi checked untuk menyala
//     if (LED == 1) {
//       buttons += "<h4>Toggle <input type=\"checkbox\" id=\"1\" onchange=\"toggle(this)\" checked /></h4>";
//     } else {
//       buttons += "<h4>Toggle <input type=\"checkbox\" id=\"1\" onchange=\"toggle(this)\"" + outputState(LED) + " /></h4>";
//     }
//     return buttons;
//   }
//   return String();
// }

// String outputState(int output) {
//   if (digitalRead(output)) {
//     return "checked";
//   } else {
//     return "";
//   }
// }

// void initConnection() {
//   Serial.print("Connecting to ");
//   Serial.println(ssid);
//   WiFi.begin(ssid, password);
//   while(WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("");
//   Serial.print("Established Connection: ");
//   Serial.println(WiFi.localIP());
//   server.begin();
// }

// void handleRoot(AsyncWebServerRequest *request) {
//   request->send_P(200, "text/html", index_html, processor);
// }

// void handleToggle(AsyncWebServerRequest *request) {
//   String msgIn1 = "No Message", msgIn2 = "No Message";
//   if (request->hasParam("output") && request->hasParam("state")) {
//     msgIn1 = request->getParam("output")->value();
//     msgIn2 = request->getParam("state")->value();
    
//     // Menyalakan atau mematikan LED berdasarkan nilai yang diterima
//     digitalWrite(msgIn1.toInt(), msgIn2.toInt() == 1 ? HIGH : LOW);
    
//     // Jika ID LED adalah 1, maka pastikan lampu tetap menyala
//     if (msgIn1.toInt() == 1 && msgIn2.toInt() == 1) {
//       digitalWrite(LED, HIGH);
//     }
//   }
//   Serial.print("PIN: ");
//   Serial.println(msgIn1);
//   Serial.print("State: ");
//   Serial.println(msgIn2);
//   request->send(200, "text/html", "OK");
// }

// void setupRoute() {
//   server.on("/", HTTP_GET, handleRoot);
//   server.on("/update", HTTP_GET, handleToggle);
// }

// void setup() {
//   Serial.begin(9600);
//   pinMode(LED, OUTPUT);
//   initConnection();
//   setupRoute();
// }

// void loop() {
//   // Tidak ada yang perlu ditambahkan di sini karena semua proses berada dalam HTTP request
// }
