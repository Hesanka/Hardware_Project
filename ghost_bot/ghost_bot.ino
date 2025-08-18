#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Encoder.h>
#include <Wire.h>

// -------------------- Radio --------------------
#define CE_PIN 4
#define CSN_PIN 5
RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "00002";

// -------------------- IR -----------------------
#define IR_PIN 32

// -------------------- L298N pins ---------------
#define ENA 25
#define IN1 26
#define IN2 15
#define ENB 27
#define IN3 33
#define IN4 14
    
// -------------------- Encoders -----------------
#define ENCODER_A1 34
#define ENCODER_A2 35
#define ENCODER_B1 16
#define ENCODER_B2 17

// -------------------- Kinematics ---------------
const float WHEEL_DIAMETER_CM = 4.0;   // measure rubber OD
const float TICKS_PER_REV     = 300.0;
const float MY_PI             = 3.14159265358979;
const float TRACK_WIDTH_CM    = 10.4;  // center-to-center of wheels

// -------------------- Globals ------------------
volatile long encoderCountA = 0;
volatile long encoderCountB = 0;

int currentValue = 0;
int whites = 10;

// -------------------- Helpers ------------------
long turnTicksFromAngle(float angle_deg) {
  float turn_circumference = MY_PI * TRACK_WIDTH_CM;
  float arc_len = (angle_deg / 360.0f) * turn_circumference;
  float wheel_circ = MY_PI * WHEEL_DIAMETER_CM;
  float revs = arc_len / wheel_circ;
  return (long)(revs * TICKS_PER_REV);
}

// Small-angle trimming (you can tune these)
float turnScaleFor(float angle_deg) {
  float s90 = 0.97f, s180 = 1.00f;
  if (angle_deg <= 90)  return s90;
  if (angle_deg >= 180) return s180;
  return s90 + (s180 - s90) * ((angle_deg - 90.0f) / 90.0f);
}

int rampSpeed(int base, long done, long target, int minOut = 35) {
  if (target <= 0) return base;
  float remain = max(0L, target - done);
  float frac   = remain / (float)target;  // 1 → 0
  float scale  = 0.25f + 0.75f * frac;
  int v = (int)(base * scale);
  return constrain(v, minOut, 255);
}

// -------------------- Setup --------------------
void setup() {
  Serial.begin(9600);
  delay(1000);

  Serial.println("Initializing nRF24L01...");
  if (!radio.begin()) {
    Serial.println("nRF24L01 NOT detected. Check wiring!");
    while (1);
  }
  Serial.println("nRF24L01 initialized successfully!");
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);

  pinMode(IR_PIN, INPUT);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(ENCODER_A1, INPUT);
  pinMode(ENCODER_A2, INPUT);
  pinMode(ENCODER_B1, INPUT);
  pinMode(ENCODER_B2, INPUT);

  attachInterrupt(digitalPinToInterrupt(ENCODER_A1), countEncoderA, RISING);
  attachInterrupt(digitalPinToInterrupt(ENCODER_B1), countEncoderB, RISING);
}

// -------------------- Loop ---------------------
void loop() {
  radio.startListening();  
  int receivedValue = 0;
  if (radio.available()) {
    radio.read(&receivedValue, sizeof(receivedValue));
    Serial.print("Received Integer: ");
    Serial.println(receivedValue);
  }

  if (receivedValue >= 1 && receivedValue <= 8) currentValue = receivedValue;

  if      (currentValue == 1) { Serial.println("pattern 7 running"); pattern7(); }
  else if (currentValue == 2) { Serial.println("pattern 8 running"); pattern8(); }
  else if (currentValue == 3) { Serial.println("pattern 6 running"); pattern6(); }
  else if (currentValue == 4) { Serial.println("pattern 5 running"); pattern5(); }
  else if (currentValue == 5) { Serial.println("pattern 2 running"); pattern2(); }
  else if (currentValue == 6) { Serial.println("pattern 4 running"); pattern4(); }
  else if (currentValue == 7) { Serial.println("pattern 3 running"); pattern3(); }
  else if (currentValue == 8) { Serial.println("pattern 1 running"); pattern1(); }
  else if (currentValue == 0) { stopMotors(); }
  else { Serial.println("Invalid input. Send a number between 0 and 8."); }

  delay(200);
}

// -------------------- ISRs ---------------------
void countEncoderA() { encoderCountA++; }
void countEncoderB() { encoderCountB++; }

// -------------------- IR (debounced) -----------
bool isDetected() {
  int total = 0;
  for (int i = 0; i < 3; i++) {
    total += analogRead(IR_PIN);
    delay(5);
  }
  int avgValue = total / 3;
  Serial.println(avgValue);
  return avgValue < 1500;  // tune threshold to your maze
}

// -------------------- Brakes -------------------
void hardBrake(int ms = 50) {          // shorter & gentler
  // brief active-brake at reduced drive, then coast
  digitalWrite(IN1, HIGH); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, HIGH);
  analogWrite(ENA, 180);                 // reduced from full
  analogWrite(ENB, 180);
  delay(ms);
  // coast (all low)
  digitalWrite(IN1, LOW);  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  delay(60);                             // settle before any direction change
}

void stopMotors() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
  analogWrite(ENA, 0); analogWrite(ENB, 0);
}

// ---------------- Motion ----------------

void moveForward(float distance_cm) {
  long targetTicks = (distance_cm / (MY_PI * WHEEL_DIAMETER_CM)) * TICKS_PER_REV;
  encoderCountA = 0; encoderCountB = 0;

  // Softer PID
  float kp = 0.8, ki = 0.01, kd = 0.10;
  float errorSum = 0, lastError = 0;

  const int baseSpeedNominal = 150;

  // Direction forward (A forward, B forward)
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);

  while ((encoderCountA + encoderCountB) / 2 < targetTicks) {
    long ticksA = encoderCountA, ticksB = encoderCountB;
    long avg = (ticksA + ticksB) / 2;

    int baseSpeed = rampSpeed(baseSpeedNominal, avg, targetTicks);

    float error = (float)(ticksA - ticksB);
    errorSum = constrain(errorSum + error, -600, 600);
    float dError = error - lastError; lastError = error;

    float out = kp*error + ki*errorSum + kd*dError;

    int speedA = constrain(baseSpeed - (int)(out/2), 0, 255);
    int speedB = constrain(baseSpeed + (int)(out/2), 0, 255);

    analogWrite(ENA, speedA);
    analogWrite(ENB, speedB);

    if (isDetected()) {
      hardBrake();                               // <- quick quiet stop
      radio.openWritingPipe(address); radio.stopListening();
      for (int i = 0; i < 40; i++) { radio.write(&whites, sizeof(whites)); delay(10); }
      return;
    }
    delay(20);
  }
  hardBrake();                                   // stop without squeal
}

void turnLeft(int angle_deg) {
  long targetTicks = (long)(turnTicksFromAngle(angle_deg) * turnScaleFor(angle_deg));
  encoderCountA = 0; encoderCountB = 0;

  float kp = 0.8, ki = 0.01, kd = 0.10;          // softer PID
  float errorSum = 0, lastError = 0;

  const int baseSpeedNominal = 130;

  // A backward, B forward
  digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);

  while ( ((labs(encoderCountA) + labs(encoderCountB)) / 2) < targetTicks ) {
    long ticksA = labs(encoderCountA), ticksB = labs(encoderCountB);
    long avg   = (ticksA + ticksB) / 2;

    int base = rampSpeed(baseSpeedNominal, avg, targetTicks);

    float error = (float)(ticksA - ticksB);
    errorSum = constrain(errorSum + error, -600, 600);
    float dError = error - lastError; lastError = error;
    float out = kp*error + ki*errorSum + kd*dError;

    int speedA = constrain(base - (int)(out/2), 0, 255);
    int speedB = constrain(base + (int)(out/2), 0, 255);

    analogWrite(ENA, speedA);
    analogWrite(ENB, speedB);

    if (isDetected()) {
      hardBrake();
      radio.openWritingPipe(address); radio.stopListening();
      for (int i=0;i<40;i++){ radio.write(&whites, sizeof(whites)); delay(10); }
      return;
    }
    delay(20);
  }
  hardBrake();
}

void turnRight(int angle_deg) {
  long targetTicks = (long)(turnTicksFromAngle(angle_deg) * turnScaleFor(angle_deg));
  encoderCountA = 0; encoderCountB = 0;

  float kp = 0.8, ki = 0.01, kd = 0.10;          // softer PID
  float errorSum = 0, lastError = 0;

  const int baseSpeedNominal = 130;

  // A forward, B backward
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);  digitalWrite(IN4, HIGH);

  while ( ((labs(encoderCountA) + labs(encoderCountB)) / 2) < targetTicks ) {
    long ticksA = labs(encoderCountA), ticksB = labs(encoderCountB);
    long avg   = (ticksA + ticksB) / 2;

    int base = rampSpeed(baseSpeedNominal, avg, targetTicks);

    float error = (float)(ticksA - ticksB);
    errorSum = constrain(errorSum + error, -600, 600);
    float dError = error - lastError; lastError = error;
    float out = kp*error + ki*errorSum + kd*dError;

    int speedA = constrain(base - (int)(out/2), 0, 255);
    int speedB = constrain(base + (int)(out/2), 0, 255);

    analogWrite(ENA, speedA);
    analogWrite(ENB, speedB);

    if (isDetected()) {
      hardBrake();
      radio.openWritingPipe(address); radio.stopListening();
      for (int i=0;i<40;i++){ radio.write(&whites, sizeof(whites)); delay(10); }
      return;
    }
    delay(20);
  }
  hardBrake();
}

void turnAround() { turnLeft(180); }

// -------------------- Patterns -----------------
// Longer gaps between actions to protect the H-bridge & motors
// after direction changes. (was 500 ms → 800 ms)
void pattern1() {
  moveForward(32); delay(800);
  turnLeft(90);    delay(800);
  moveForward(32); delay(800);
  turnAround();    delay(800);
  moveForward(32); delay(800);
  turnRight(90);   delay(800);
  moveForward(32); delay(800);
  turnAround();    delay(800);
}

void pattern2() {
  moveForward(32); delay(800);
  turnRight(90);   delay(800);
  moveForward(48); delay(800); turnAround(); delay(800);
  moveForward(48); delay(800);
  turnLeft(90);    delay(800);
  moveForward(32); delay(800);
  turnAround();    delay(800);
}

void pattern3() {
  turnRight(90);   delay(800);
  moveForward(48); delay(800);
  turnAround();    delay(800);
  moveForward(48); delay(800);
  turnRight(90);   delay(800);
}

void pattern4() {
  turnLeft(90);    delay(800);
  moveForward(16); delay(800);
  turnLeft(90);    delay(800);
  moveForward(32); delay(800);
  turnAround();    delay(800);
  moveForward(32); delay(800);
  turnRight(90);   delay(800);
  moveForward(16); delay(800);
  turnLeft(90);    delay(800);
}

void pattern5() {
  turnLeft(90);    delay(800);
  moveForward(48); delay(800);
  turnLeft(90);    delay(800);
  moveForward(32); delay(800);
  turnAround();    delay(800);
  moveForward(32); delay(800); turnRight(90); delay(800);
  moveForward(48); delay(800);
  turnLeft(90);    delay(800);
}

void pattern6() {
  turnRight(90);   delay(800);
  moveForward(32); delay(800);
  turnRight(90);   delay(800);
  moveForward(48); delay(800);
  turnAround();    delay(800); moveForward(48); delay(800);
  turnLeft(90);    delay(800);
  moveForward(32); delay(800);
  turnRight(90);   delay(800);
}

void pattern7() {
  turnLeft(90);    delay(800);
  moveForward(48); delay(800);
  turnAround();    delay(800);
  moveForward(48); delay(800);
  turnLeft(90);    delay(800);
}

void pattern8() {
  turnRight(90);   delay(800);
  moveForward(32); delay(800);
  turnLeft(90);    delay(800);
  moveForward(48); delay(800);
  turnAround();    delay(800); moveForward(48); delay(800);
  turnRight(90);   delay(800);
  moveForward(32); delay(800);
  turnRight(90);   delay(800);
}
