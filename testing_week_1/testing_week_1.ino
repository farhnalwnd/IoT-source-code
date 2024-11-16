#define LED 2

void setup() {
  pinMode(LED, OUTPUT);
  Serial.begin(115200); // Mulai komunikasi serial dengan baud rate 115200 bps
}

void loop() {
  digitalWrite(LED, HIGH);
  Serial.println("LED ON"); // Output serial ketika LED dinyalakan
  delay(5000);
  digitalWrite(LED, LOW);
  Serial.println("LED OFF"); // Output serial ketika LED dimatikan
  delay(5000);
}

// #define LED 1 // Mengubah pin yang ditetapkan untuk LED menjadi pin 1

// void setup() {
//  pinMode(LED, OUTPUT);
// }

// void loop() {
//  digitalWrite(LED, HIGH);
//  delay(1000);
//  digitalWrite(LED, LOW);
//  delay(1000);
// }
