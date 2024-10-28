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

enum Effects { SINELON_LINGER, SINELON, TWINKLE, SOLID_COLOR, RAINBOW, COLOR_WIPE, BPM,  NUM_EFFECTS };
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
    case SINELON_LINGER:
      sinelonLingerEffect(hue);  // Pass the dynamic hue to sinelonEffect
    break;
    case SINELON:
      sinelonEffect(hue);  // Pass the dynamic hue to sinelonEffect
    break;
    case TWINKLE:
      twinkleEffect(hue);  // Pass the dynamic hue to twinkleEffect
    break;
    case SOLID_COLOR:
      solidColorEffect(hue);  // Pass the dynamic hue value from the potentiometer
    break;
    case RAINBOW:
      rainbowEffect();
      break;
    case COLOR_WIPE:
      colorWipeEffect(hue);  // Pass the dynamic hue to colorWipeEffect
      break;
    case BPM:
      bpmEffect();
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

void solidColorEffect(int hue) {
  // Set all LEDs to the same color based on the hue
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(hue, 255, 255);  // Full saturation and brightness for a solid color
  }
}

void colorWipeEffect(int hue) {
  // Fade out all the LEDs gradually
  fadeToBlackBy(leds, NUM_LEDS, 20);
  
  // Get a position that bounces back and forth across the LED strip
  int ledIndex = beatsin16(13, 0, NUM_LEDS - 1);  // Slow bounce effect
  
  // Set the LED at the current position to the hue from the potentiometer
  leds[ledIndex] = CHSV(hue, 255, 255);  // Full brightness and saturation
}


// Sinelon Effect with hue control
void sinelonEffect(int hue) {
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  leds[pos] += CHSV(hue, 255, 192);  // Moving dot with hue control
}

// BPM Pulse Effect
void bpmEffect() {
  uint8_t beat = beatsin8(62, 64, 255);  // Beats per minute
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(beat - (i * 2), 255, 255);  // Color wave
  }
}

void sinelonLingerEffect(int hue) {
  // Gradually fade out all LEDs and shift hue
  for (int i = 0; i < NUM_LEDS; i++) {
    // Convert the current color to HSV so we can manipulate the hue
    CHSV hsv = rgb2hsv_approximate(leds[i]);
    
    // Shift the hue slightly and keep it within 0-255 range
    hsv.hue += 1;

    // Scale down the brightness (like fading out)
    hsv.val = scale8(hsv.val, 250);  // Adjust fading speed here

    // Convert back to RGB and apply to the LED
    leds[i] = hsv;
  }

  // Calculate the position of the moving dot using beatsin16
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  
  // Set the moving LED to a bright color with hue from potentiometer
  leds[pos] = CHSV(hue, 255, 255);  // Full brightness and saturation for the moving LED
}


void twinkleEffect(int hue) {
  int hueRange = 20;  // Range for twinkling hue variation
   int speed = 50;
  
  // Iterate through all LEDs
  for (int i = 0; i < NUM_LEDS; i++) {
    // Random chance for each LED to twinkle
    if (random8() < 10) {  // Adjust the '10' to control the density of twinkles
      // Choose a random hue within the specified range around the main hue
      int randomHue = hue + random8(-hueRange, hueRange + 1);  // Add or subtract up to hueRange

      // Ensure the hue stays within the 0-255 range
      if (randomHue < 0) randomHue += 255;
      if (randomHue > 255) randomHue -= 255;

      // Set the LED to a bright color with a bit of randomness in brightness
      leds[i] = CHSV(randomHue, 255, random8(128, 255));
    } else {
      // Gradually fade out the LEDs
      leds[i].fadeToBlackBy(speed);  // Use speed to control the fade speed
    }
  }
  
  // Introduce a delay to control the speed of the overall twinkling effect
  delay(map(speed, 0, 255, 50, 5));  // Map speed to a delay (higher speed means shorter delay)
}



