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

enum Effects { SOLID_COLOR, SINELON, SINELON_LINGER, HUE_RIPPLE, RAINBOW, COLOR_RIPPLE, BPM, NUM_EFFECTS };
int currentEffect = 0;           // Current effect index
unsigned long lastButtonPress = 0; // Time of the last button press
bool lastButtonState = LOW;      // Previous state of the button
bool currentButtonState = LOW;   // Current state of the button

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);  // Set the button pin as input with pull-up
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(100);  // Default brightness
  Serial.begin(115200);  // Initialize the serial communication
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

  switch (currentEffect) {
    case SOLID_COLOR:
      solidColorEffect(hue);  
    break;
    case SINELON:
      sinelonEffect(hue);  
    break;
    case SINELON_LINGER:
      sinelonLingerEffect(hue); 
    break;
    case HUE_RIPPLE:
      hueRippleEffect(hue);
    break;
    case RAINBOW:
      rainbowEffect();
      break;
    case COLOR_RIPPLE:
      colorRippleEffect(hue);
    break;
    case BPM:
      bpmEffect();
      break;
  }

  FastLED.show();  // Display the current effect
  
  // // Send LED data to serial (send the RGB values of each LED)
  // for (int i = 0; i < NUM_LEDS; i++) {
  //   Serial.print(leds[i].r); Serial.print(",");
  //   Serial.print(leds[i].g); Serial.print(",");
  //   Serial.print(leds[i].b);
  //   if (i < NUM_LEDS - 1) Serial.print("|");  // Separator for each LED
  // }
  // Serial.println();  // End the line to mark end of LED data

  delay(20);  // Small delay for smoother animation
}

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

void sinelonEffect(int hue) {
  fadeToBlackBy(leds, NUM_LEDS, 20);
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  leds[pos] += CHSV(hue, 255, 192);  // Moving dot with hue control
}

void bpmEffect() {
  uint8_t beat = beatsin8(62, 64, 255);  // Beats per minute
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CHSV(beat - (i * 2), 255, 255);  // Color wave
  }
}

void sinelonLingerEffect(int hue) {
  for (int i = 0; i < NUM_LEDS; i++) {
    CHSV hsv = rgb2hsv_approximate(leds[i]);
    hsv.hue += 1;
    hsv.val = scale8(hsv.val, 250);  // Adjust fading speed here
    leds[i] = hsv;
  }
  int pos = beatsin16(13, 0, NUM_LEDS - 1);
  leds[pos] = CHSV(hue, 255, 255);
}


void colorRippleEffect(int hue) {
  static int center = random16(NUM_LEDS);  
  static int rippleStep = 0;  
  int rippleSpeed = 5;

  fadeToBlackBy(leds, NUM_LEDS, 20);
  for (int i = 0; i < NUM_LEDS; i++) {
    int distance = abs(i - center);  
    if (distance == rippleStep) {
      int rippleHue = hue + (distance * 5) % 255;
      leds[i] = CHSV(rippleHue, 255, 255);
    }
  }
  rippleStep++;
  if (rippleStep >= NUM_LEDS / 2) {
    center = random16(NUM_LEDS);  
    rippleStep = 0;
  }
  delay(rippleSpeed);
}

void hueRippleEffect(int hue) {
  int hueRange = 100;
  static int center = random16(NUM_LEDS);  
  static int rippleStep = 0;  
  int rippleSpeed = 3; // Speed of the ripple effect

  fadeToBlackBy(leds, NUM_LEDS, 20);
  for (int i = 0; i < NUM_LEDS; i++) {
    int distance = abs(i - center);  
    if (distance == rippleStep) {
      int rippleHue = hue + (distance * hueRange / (NUM_LEDS / 2)) % 255; // Modifying hue based on the distance from center
      if (rippleHue > 255) rippleHue -= 255; // Ensure hue is within 0-255
      leds[i] = CHSV(rippleHue, 255, 255); // Set color with full saturation and brightness
    }
  }
  rippleStep++;
  if (rippleStep >= NUM_LEDS / 2) {
    center = random16(NUM_LEDS);  
    rippleStep = 0; // Reset ripple step and randomly choose a new center
  }
  delay(rippleSpeed);
}
