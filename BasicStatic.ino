// Description: 
//    Modular LED Sign code, very basic example with static colors (no encoder)
//
// Usage:
//    Developed on the Seed Studio XIAO SAMD21 board, but should be compatible with all "Arduino UNO" boards.
//    Make sure to set correct number of LED's (NUM_LEDS), then set colors for each led.
//
// Author: 
//    The Skjegg 27/09/2024

#include <FastLED.h>

FASTLED_USING_NAMESPACE

#define LED_PIN     3
//#define CLK_PIN   4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    9     // Set number of LED's
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          255
#define FRAMES_PER_SECOND  120

void setup() {
  delay(3000); // 3 second delay for recovery
  
  // tell FastLED about the LED strip configuration
  //FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  
  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
}


void loop() {

  // Add lines to match number of LED's
  leds[0] = CRGB::Red;
  leds[1] = CRGB::Green;
  leds[2] = CRGB::Blue;
  leds[3] = CRGB::OrangeRed;
  leds[4] = CRGB::Purple;
  leds[5] = CRGB::DarkMagenta;
  leds[6] = CRGB::Yellow;
  leds[7] = CRGB::DarkGray;
  leds[8] = CRGB::Red;

  FastLED.show();
  while(1){}; // Do nothing

}