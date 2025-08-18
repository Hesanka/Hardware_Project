#include <FastLED.h>

#define NUM_LEDS 300     // Number of LEDs in your strip
#define DATA_PIN 6       // Pin connected to DIN of LED strip

SoftwareSerial mySerial(10, 11);
DFRobotDFPlayerMini myDFPlayer;

CRGB leds[NUM_LEDS];

void setup() {
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(100);  // Optional: 0 to 255

  mySerial.begin(9600);   // For DFPlayer
  Serial.begin(9600);     // For debugging

  Serial.println("Initializing DFPlayer...");

  if (!myDFPlayer.begin(mySerial)) {
    Serial.println("❌ Unable to begin DFPlayer. Check connections and SD card.");
    while (true);
  }

  Serial.println("✅ DFPlayer Mini online.");

  myDFPlayer.volume(20);  // Set volume (0 to 30)
  myDFPlayer.play(1);
}
void loop() {
  static uint8_t hue = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(hue + i * 10, 255, 255);
  }
  FastLED.show();
  hue++;
  delavoid loop() {
  static uint8_t hue = 0;
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(hue + i * 10, 255, 255);
  }
  FastLED.show();
  hue++;
  delay(20);
}