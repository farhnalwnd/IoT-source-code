 #include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#include <NewPing.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

// Pin definitions for the L298N module
const int IN1_A = 7;
const int IN2_A = 6;
const int ENA_A = A2; 
const int IN3_A = 5;
const int IN4_A = 4;
const int ENB_A = A3; 

#define TRIGGER_PIN_FRONT 12
#define ECHO_PIN_FRONT 11
#define TRIGGER_PIN_LEFT 13
#define ECHO_PIN_LEFT 8
#define TRIGGER_PIN_RIGHT 9
#define ECHO_PIN_RIGHT 10
#define MAX_DISTANCE 1000 

const int LED_PIN = 0; 

NewPing sonarFront(TRIGGER_PIN_FRONT, ECHO_PIN_FRONT, MAX_DISTANCE);
NewPing sonarLeft(TRIGGER_PIN_LEFT, ECHO_PIN_LEFT, MAX_DISTANCE);
NewPing sonarRight(TRIGGER_PIN_RIGHT, ECHO_PIN_RIGHT, MAX_DISTANCE);

Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 

Adafruit_DCMotor *motor1 = AFMS.getMotor(1);
Adafruit_DCMotor *motor2 = AFMS.getMotor(2);
Adafruit_DCMotor *motor3 = AFMS.getMotor(3);
Adafruit_DCMotor *motor4 = AFMS.getMotor(4);

// Create MPU6050 object
Adafruit_MPU6050 mpu;

// PID constants
const float Kp = 1.0;
const float Ki = 0.1;
const float Kd = 0.01;

float previousError = 0;
float integral = 0;

void setup() {
  Serial.begin(9600);           
  Serial.println("Adafruit Motorshield v2 - DC Motor test!");

  AFMS.begin();  

  // Set all control pins to output mode
  pinMode(IN1_A, OUTPUT);
  pinMode(IN2_A, OUTPUT);
  pinMode(ENA_A, OUTPUT);
  pinMode(IN3_A, OUTPUT);
  pinMode(IN4_A, OUTPUT);
  pinMode(ENB_A, OUTPUT);
  
  pinMode(LED_PIN, OUTPUT); 

  // Initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (1) {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
  mpu.setGyroRange(MPU6050_RANGE_250_DEG);
  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

  delay(100);
}

void loop() {
  float distanceFront = getDistance(sonarFront);
  float distanceLeft = getDistance(sonarLeft);
  float distanceRight = getDistance(sonarRight);

  Serial.print("Distance Front: ");
  Serial.println(distanceFront);

  // Read pitch from MPU6050
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float pitch = atan2(a.acceleration.y, a.acceleration.z) * 180 / PI;
  Serial.print("Pitch: ");
  Serial.println(pitch);

  // Adjust speed based on pitch using PID controller
  int adjustedSpeed = adjustSpeedWithPID(pitch);

  // Check pitch and handle movement accordingly
  if (pitch > 10 || pitch < -10) { // Adjust pitch threshold
    moveBackward(adjustedSpeed);
    while (pitch > 5 || pitch < -5) { // Adjust pitch threshold for stopping backward
      mpu.getEvent(&a, &g, &temp);
      pitch = atan2(a.acceleration.y, a.acceleration.z) * 180 / PI;
      Serial.print("Pitch: ");
      Serial.println(pitch);
      delay(100);
    }
    Serial.println("Deciding direction to turn.");
    determineDirection(distanceLeft, distanceRight, adjustedSpeed);
    delay(250);
  } else if (distanceFront > 0.00 && distanceFront < 20.00) { 
    stopAllMotors(); 
    Serial.println("Obstacle detected! Stopping for a moment.");
    digitalWrite(LED_PIN, HIGH); 
    Serial.println("LED is ON");
    delay(1000); 
    digitalWrite(LED_PIN, LOW); 
    Serial.println("LED is Off");
    Serial.println("Deciding direction to turn.");
    determineDirection(distanceLeft, distanceRight, adjustedSpeed);
    delay(250); 
  } else {
    moveForward(adjustedSpeed);
  }

  delay(100); // Short delay to allow sensor readings
}

float getDistance(NewPing sonar) {
  delay(50); 
  return sonar.ping_cm();
}

void moveForward(int speed) {
  Serial.println("Moving forward");

  // Adafruit Motor Shield
  motor1->run(FORWARD);
  motor2->run(FORWARD);
  motor3->run(FORWARD);
  motor4->run(FORWARD);
  motor1->setSpeed(speed);
  motor2->setSpeed(speed);
  motor3->setSpeed(speed);
  motor4->setSpeed(speed);

  // L298N module
  digitalWrite(IN1_A, HIGH);
  digitalWrite(IN2_A, LOW);
  analogWrite(ENA_A, speed * 2.55); 
  digitalWrite(IN3_A, LOW);
  digitalWrite(IN4_A, HIGH);
  analogWrite(ENB_A, speed * 2.55); 
}

void moveBackward(int speed) {
  Serial.println("Moving backward");

  motor1->run(BACKWARD);
  motor2->run(BACKWARD);
  motor3->run(BACKWARD);
  motor4->run(BACKWARD);
  motor1->setSpeed(speed);
  motor2->setSpeed(speed);
  motor3->setSpeed(speed);
  motor4->setSpeed(speed);

  digitalWrite(IN1_A, LOW);
  digitalWrite(IN2_A, HIGH);
  analogWrite(ENA_A, speed * 2.55); 
  digitalWrite(IN3_A, HIGH);
  digitalWrite(IN4_A, LOW);
  analogWrite(ENB_A, speed * 2.55); 
}

void turnRight(int speed) {
  Serial.println("Turning right");

  // Adafruit Motor Shield
  motor1->run(FORWARD);
  motor2->run(FORWARD);
  motor3->run(BACKWARD);
  motor4->run(BACKWARD);
  motor1->setSpeed(speed);
  motor2->setSpeed(speed);
  motor3->setSpeed(speed);
  motor4->setSpeed(speed);

  // L298N module
  digitalWrite(IN1_A, HIGH);
  digitalWrite(IN2_A, LOW);
  analogWrite(ENA_A, speed * 2.55); 
  digitalWrite(IN3_A, HIGH);
  digitalWrite(IN4_A, LOW);
  analogWrite(ENB_A, speed * 2.55); 
}

void turnLeft(int speed) {
  Serial.println("Turning left");

  // Adafruit Motor Shield
  motor1->run(BACKWARD);
  motor2->run(BACKWARD);
  motor3->run(FORWARD);
  motor4->run(FORWARD);
  motor1->setSpeed(speed);
  motor2->setSpeed(speed);
  motor3->setSpeed(speed);
  motor4->setSpeed(speed);

  // L298N module
  digitalWrite(IN1_A, LOW);
  digitalWrite(IN2_A, HIGH);
  analogWrite(ENA_A, speed * 2.55); 
  digitalWrite(IN3_A, LOW);
  digitalWrite(IN4_A, HIGH);
  analogWrite(ENB_A, speed * 2.55); 
}

void stopAllMotors() {
  Serial.println("Stopping all motors");

  motor1->run(RELEASE);
  motor2->run(RELEASE);
  motor3->run(RELEASE);
  motor4->run(RELEASE);
  
  digitalWrite(IN1_A, LOW);
  digitalWrite(IN2_A, LOW);
  analogWrite(ENA_A, 0);
  digitalWrite(IN3_A, LOW);
  digitalWrite(IN4_A, LOW);
  analogWrite(ENB_A, 0);
}

void determineDirection(float distanceLeft, float distanceRight, int speed) {

  Serial.print("Distance Left: ");
  Serial.print(distanceLeft);
  Serial.print(" cm, Distance Right: ");
  Serial.println(distanceRight);

  if (distanceRight > distanceLeft) {
    turnRight(speed);
  } else {
    turnLeft(speed);
  }
  delay(500); 
  stopAllMotors();
}

int adjustSpeedWithPID(float pitch) {
  float setPoint = 0; 
  float error = setPoint - pitch;
  integral += error;
  float derivative = error - previousError;
  float output = Kp * error + Ki * integral + Kd * derivative;

  // Adjust motor speed based on PID output
  int baseSpeed = 50; 
  int adjustedSpeed = baseSpeed + output;
  int speed = constrain(adjustedSpeed, 0, 120); 

  previousError = error;

  Serial.print("Adjusted Speed: ");
  Serial.println(speed);
  
  return speed;
}