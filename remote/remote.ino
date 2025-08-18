#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define CE_PIN 7
#define CSN_PIN 8

RF24 radio(CE_PIN, CSN_PIN);
const byte address[6] = "00001";

#define VRX_PIN A0
#define VRY_PIN A1

struct Data {
  int x;
  int y;
};

Data joystickData;

void setup() {
  pinMode(VRX_PIN, INPUT);
  pinMode(VRY_PIN, INPUT);

  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening();
}

void loop() {
  joystickData.x = analogRead(VRX_PIN);
  joystickData.y = analogRead(VRY_PIN);

  radio.write(&joystickData, sizeof(joystickData));
  delay(50);
}