#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <Servo.h>
#include <Wire.h> 

#define CE_PIN 29
#define CSN_PIN 31
// Create RF24 object
RF24 radio(CE_PIN, CSN_PIN);

int number = 0;

// Define the address for communication
const byte address[6] = "00002";

// Create servo objects
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;
Servo servo5;
Servo servo6;
Servo servo7;
Servo servo8;

// Define pins for IR sensors
const int IR1 = 12; // rotate doors after trophy

const int IR2 = 13; // change pattern and rotate doors

const int IR3 = 22;// change pattern
const int IR4 = 24;// change pattern
const int IR5 = 26;// change pattern
const int IR6 = 28;// change pattern
const int IR7 = 30;// change pattern
const int IR8 = 32;// change pattern

const int IR9 = 34; // out doors
const int IR10 = 36; // out doors

const int LED1 = 33; 
const int LED2 = 35;

const int LED_TROPHY = 49;

unsigned long previousMillis = 0; // Store the last time the LED state changed
const unsigned long interval = 60000; // 1-minute interval in milliseconds
bool isLED1On = false; // State of the first LED

// Define pins for servo motors
const int SERVO1_PIN = 4;  // Servo 1 connected to pin 9
const int SERVO2_PIN = 5; // Servo 2 connected to pin 10
const int SERVO3_PIN = 6;
const int SERVO4_PIN = 7;
const int SERVO5_PIN = 8;
const int SERVO6_PIN = 9;
const int SERVO7_PIN = 10;
const int SERVO8_PIN = 11;


// Variables to track if the servos have already operated
bool servo1Activated = false;
bool servo2Activated = false;
bool servo3Activated = false;
bool servo4Activated = false;
bool servo5Activated = false;
bool servo6Activated = false;
bool servo7Activated = false;
bool servo8Activated = false;

const int reset1 = 2;
const int reset2 = 3;


void setup() {
  // Attach servos to respective pins
  servo1.attach(SERVO1_PIN);
  servo2.attach(SERVO2_PIN);
  servo3.attach(SERVO3_PIN);
  servo4.attach(SERVO4_PIN);
  servo5.attach(SERVO5_PIN);
  servo6.attach(SERVO6_PIN);
  servo7.attach(SERVO7_PIN);
  servo8.attach(SERVO8_PIN);


  // Set IR sensor pins as input
  pinMode(IR1, INPUT);
  pinMode(IR2, INPUT);

  pinMode(IR3, INPUT);
  pinMode(IR4, INPUT);
  pinMode(IR5, INPUT);
  pinMode(IR6, INPUT);
  pinMode(IR7, INPUT);
  pinMode(IR8, INPUT);

  pinMode(IR9, INPUT);
  pinMode(IR10, INPUT);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  pinMode(LED_TROPHY, OUTPUT);

  pinMode(reset1, INPUT);
  pinMode(reset2, INPUT);
  

  // Set initial servo positions
  servo1.write(90); // Servo 1 initial position
  servo2.write(0); // Servo 2 initial position
  servo3.write(0); // Servo 1 initial position
  servo4.write(0);
  servo5.write(90); 
  servo6.write(0);
  servo7.write(0); 
  servo8.write(0);

  // Start serial communication for debugging (optional)
  Serial.begin(9600);
  // Initialize the NRF24L01 module
  if (radio.begin()) {
    Serial.println("NRF24L01 initialized!");
  } else {
    Serial.println("NRF24L01 initialization failed!");
    
  }

  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW); 
  radio.openReadingPipe(0, address);
  radio.stopListening();
}

void resetIR1(bool logic1){
  if(!logic1){
    servo1.write(90); // Rotate servo 1 to 90 degrees
    delay(100);      // Wait for a second
    servo2.write(0); // Rotate servo 1 to 90 degrees
    delay(100);
    servo3.write(0); // Rotate servo 1 to 90 degrees
    delay(100);
    servo4.write(0); // Rotate servo 1 to 90 degrees
    delay(100);
    servo5.write(90); // Rotate servo 1 to 90 degrees
    delay(100);
    servo6.write(0); // Rotate servo 1 to 90 degrees
    delay(100); 
    Serial.println("resetIR1 activated!");
  }
}

void resetIR2(bool logic2){
  if(!logic2){
    servo7.write(0); // Rotate servo 1 to 90 degrees
    delay(100);      // Wait for a second
    servo8.write(0); // Rotate servo 1 to 90 degrees
    delay(100);
    Serial.println("resetIR2 activated!");
  }
}

bool Trophy = false;

void loop() {

  if(!Trophy){
    digitalWrite(LED_TROPHY,HIGH);
  }

  int receivedValue = 0;
  radio.startListening();
  if (radio.available()) {
      radio.read(&receivedValue, sizeof(receivedValue));
      if (receivedValue==10){
        Serial.println("Game over");
        delay(1000);
      }
  }
  // Read values from IR sensors
  delay(100);
  bool logic_1 = digitalRead(reset1);
  bool logic_2 = digitalRead(reset2);
  // Serial.print("logic1: ");
  // Serial.print(logic_1);

  // Serial.print("\t");
  // Serial.print("logic2: ");
  // Serial.println(logic_2);
  // delay(100);



  int IR1Value = digitalRead(IR1);
  int IR2Value = digitalRead(IR2);
  delay(100);

  int IR3Value = digitalRead(IR3);
  int IR4Value = digitalRead(IR4);
  delay(100);

  int IR5Value = digitalRead(IR5);
  int IR6Value = digitalRead(IR6);
  delay(100);

  int IR7Value = digitalRead(IR7);
  int IR8Value = digitalRead(IR8);
  delay(100);

  int IR9Value = digitalRead(IR9);
  int IR10Value = digitalRead(IR10);
  delay(100);

  resetIR1(logic_1);
  resetIR2(logic_2);

  // Serial.print("IR1: ");
  // Serial.println(IR1Value);
  delay(100);
  // Check if IR sensor 1 is triggered and the servo is not yet activated
  //////////////////////////////
  if (IR1Value == LOW && !servo1Activated) {
    Serial.println("IR 1 detected");
    servo1.write(0); // Rotate servo 1 to 90 degrees
    delay(100);      // Wait for a second
    servo2.write(90); // Rotate servo 1 to 90 degrees
    delay(100);
    servo4.write(90); // Rotate servo 1 to 90 degrees
    delay(100);
    servo5.write(0); // Rotate servo 1 to 90 degrees
    delay(100);
    servo7.write(90); // Rotate servo 1 to 90 degrees
    delay(100);
    servo8.write(90); // Rotate servo 1 to 90 degrees
    delay(100);
    Trophy = true;
    isLED1On = true;
    digitalWrite(LED1, HIGH);
    digitalWrite(LED_TROPHY,LOW);
    // servo1.write(0);  // Return servo 1 to 0 degrees
    servo1Activated = true; // Mark servo 1 as activated
  }else{
    // Serial.println("IR Sensor 1 not detected movement");
    servo1Activated = false; // Mark servo 1 as activated
  }

  if( Trophy == true ){
    unsigned long currentMillis = millis();

    // Check if it's time to toggle the LEDs
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis; // Update the last change time
      // Toggle LEDs
      if (isLED1On) {
        digitalWrite(LED1, LOW); // Turn off the first LED
        digitalWrite(LED2, HIGH); // Turn on the second LED
      } else {
        digitalWrite(LED1, HIGH); // Turn on the first LED
        digitalWrite(LED2, LOW); // Turn off the second LED
      }

      isLED1On = !isLED1On; // Flip the LED state
  }

  }
/////////////////////////////////////////////////
  // Check if IR sensor 2 is triggered and the servo is not yet activated
  if (IR2Value == LOW && !servo3Activated && Trophy && isLED1On) {
    servo3.write(90); // Rotate servo 3 to 90 degrees
    delay(100);      // Wait for a second
    servo3Activated = true; // Mark servo 3 as activated
      radio.stopListening();
      number = 1;
      radio.write(&number, sizeof(number));
      Serial.println("IR Sensor 2 detected ");
  }else{
    servo3Activated = false; // Mark servo 3 as activated
  }
  if (IR2Value == LOW && Trophy && isLED1On){
    radio.stopListening();
    number = 1;
    radio.write(&number, sizeof(number));
    Serial.println("IR Sensor 2 detected ");
  }

  if (IR2Value == LOW && !servo6Activated && Trophy && !isLED1On) {
    servo6.write(90); // Rotate servo 6 to 90 degrees
    delay(100);      // Wait for a second
    servo6Activated = true; // Mark servo 6 as activated
    radio.stopListening();
    number = 2;
    radio.write(&number, sizeof(number));
  }else{
    servo6Activated = false; // Mark servo 6 as activated
  }
  if (IR2Value == LOW && Trophy && isLED1On){
    radio.stopListening();
    number = 2;
    radio.write(&number, sizeof(number));
    Serial.println("IR Sensor 2 detected ");
  }

  if (Trophy == true){
    if (IR4Value == LOW) {
      radio.stopListening();
      number = 4;
      radio.write(&number, sizeof(number));
      delay(100);
      Serial.println("IR Sensor 4 detected ");

    }
    if (IR3Value == LOW) {
      radio.stopListening();
      number = 3;
      radio.write(&number, sizeof(number));
      delay(100);
      Serial.println("IR Sensor 3 detected ");

    }
  }else{
    if (IR8Value == LOW) {
      radio.stopListening();
      number = 8;
      radio.write(&number, sizeof(number));
      delay(100);
      Serial.println("IR Sensor 8 detected ");

    }
    delay(100);

    if (IR5Value == LOW) {
      radio.stopListening();
      number = 5;
      radio.write(&number, sizeof(number));
      Serial.println("IR Sensor 5 detected ");
    }
    delay(100);
    if (IR6Value == LOW) {
      radio.stopListening();
      number = 6;
      radio.write(&number, sizeof(number));
      delay(100);
      Serial.println("IR Sensor 6 detected ");

    }
    if (IR7Value == LOW) {
      radio.stopListening(); 
      number = 7;
      radio.write(&number, sizeof(number));
      delay(100);
      Serial.println("IR Sensor 7 detected ");

    }
   
  }

  if (IR9Value == LOW) {
    if(isLED1On && Trophy){
      Serial.println("You Won!");
    }
    else{
      Serial.println("You Lost!");
    }
  }
  delay(100);
  if (IR10Value == LOW) {
    if(!isLED1On && Trophy){
      Serial.println("You Won!");
    }
    else{
      Serial.println("You Lost!");
    }
  }

}