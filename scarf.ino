#include <FastLED.h>

// LEDs
#define LED_PIN     6       // Pin connected to the data pin of the LED strip
#define NUM_LEDS    72      // Number of LEDs in the strip
#define LED_TYPE    WS2812B // Type of LED strip
#define COLOR_ORDER GRB     // Color order
#define DEBOUNCE_DELAY 50   // Debounce time for button press
CRGB leds[NUM_LEDS];

// Controls
const int buttonPin = 7;
const int huePot = A0;
const int brightPot = A1;

enum Effects { RAINBOW, COLOR_WIPE, BPM, SINELON, NUM_EFFECTS };
int currentEffect = 0;           // Current effect index
unsigned long lastButtonPress = 0; // Time of the last button press
bool lastButtonState = LOW;      // Previous state of the button
bool currentButtonState = LOW;   // Current state of the button

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);  // Set the button pin as input with pull-up
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(100);  // Default brightness
}

void loop() {
  // Read the potentiometer values
  int brightnessValue = analogRead(brightPot);
  int brightness = map(brightnessValue, 0, 1023, 0, 255);  // Map brightness
  FastLED.setBrightness(brightness);  // Set the brightness dynamically

  // Read the hue pot and map the value to a hue (0-255)
  int hueValue = analogRead(huePot);
  int hue = map(hueValue, 0, 1023, 0, 255);  // Map hue

  // Check if the button is pressed
  currentButtonState = digitalRead(buttonPin);

  if (currentButtonState == LOW && lastButtonState == HIGH && (millis() - lastButtonPress) > DEBOUNCE_DELAY) {
    currentEffect = (currentEffect + 1) % NUM_EFFECTS;  // Move to the next effect when button is pressed
    lastButtonPress = millis();
  }

  lastButtonState = currentButtonState;  // Update the button state

  // Call the current effect
  switch (currentEffect) {
    case RAINBOW:
      rainbowEffect();
      break;
    case COLOR_WIPE:
      colorWipeEffect(hue);  // Pass the dynamic hue to colorWipeEffect
      break;
    case BPM:
      bpmEffect();
      break;
    case SINELON:
      sinelonEffect(hue);  // Pass the dynamic hue to sinelonEffect
      break;
  }

  FastLED.show();  // Display the current effect
  delay(20);       // Small delay for smoother animation
}

// Existing Effects:

void rainbowEffect() {
  static uint8_t hue = 0;
  fill_rainbow(leds, NUM_LEDS, hue, 7);
  hue++;
}

void colorWipeEffect(int hue) {
  static int ledIndex = 0;  
  leds[ledIndex] = CHSV(hue, 255, 255);  // Use the hue value from the potentiometer
  ledIndex = (ledIndex + 1) % NUM_LEDS;
  if (ledIndex == 0) {
    FastLED.clear();  // Clear strip after full wipe
  }
}

// BPM Pulse Effect
void bpmEffect() {
  uint8_t beat = beatsin8(62, 64, 255);  // Beats per minute
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(beat - (i * 2), 255, 255);  // Color wave
  }
}

// Sinelon Effect with hue control
void sinelonEffect(int hue) {
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  leds[pos] += CHSV(hue, 255, 192);  // Moving dot with hue control
}
