#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN 7
#define CSN_PIN 8

RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "00001";

#define IN1 3
#define IN2 4
#define IN3 6
#define IN4 9
#define ENA 5 // PWM for Left Motor
#define ENB 10 // PWM for Right Motor

struct Data {
  int x;
  int y;
};

Data joystickData;

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_LOW);
  radio.startListening();

  Serial.begin(9600);
}

void loop() {
  if (radio.available()) {
    radio.read(&joystickData, sizeof(joystickData));

    int x = joystickData.x - 512;
    int y = joystickData.y - 512;
    Serial.print("X: ");
    Serial.print(x);
    Serial.print("    y: ");
    Serial.println(y);

    //int leftSpeed = constrain(abs(y + x), 0, 255);
    //int rightSpeed = constrain(abs(y - x), 0, 255);

    //analogWrite(ENA, leftSpeed);
    //analogWrite(ENB, rightSpeed);
    analogWrite(ENA, 100);
    analogWrite(ENB, 100);

    if (x<200 && y<200 ) { 
      if(x>0 && y<0){ //forward
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
      }
      else if(x<0 && y>0){//left
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, HIGH);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
      }
      else{//stop
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
      }
    }
    else if (x<200 && y>200) { // Backward
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, HIGH);
    } 
    else if (x>200 && y<200) { // Right
      digitalWrite(IN1, HIGH);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, HIGH);
      digitalWrite(IN4, LOW);
    }
    else { // Stop
      digitalWrite(IN1, LOW);
      digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW);
      digitalWrite(IN4, LOW);
    }
  }
}