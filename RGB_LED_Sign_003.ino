// Description: 
//    Modular LED Sign code, based on DemoReel100.ino from the FastLED library. Added encoder control and freeColor mode to 
//    allow user to set colors by encoder. Encoder functions:
//    Short press: Toggles sign ON/OFF
//    Long press: Increments modes
//    Turning: Normally sets sign brightness, if in freeColor mode it selects color (last modes)
//
// Usage:
//    Developed on the Seeed Studio XIAO SAMD21 board, but should be compatible with all "Arduino UNO" boards.
//    Make sure to set correct number of LED's, and define arrays for characters and colors.
//
// Author: 
//    The Skjegg 28/09/2024
//
// Versions:
//    v001: First version
//    v002: Tested freeColor mode for each led only (not chars)
//    v003: Added freeColorChars and freeColorLeds modes, both as separate modes

#include <FastLED.h>
#include <Encoder.h>

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define LED_PIN     3
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    14  // Total number of LED's
CRGB leds[NUM_LEDS];
#define BRIGHTNESS          255
#define FRAMES_PER_SECOND  60
#define ENC_SWITCH  10

int FirstLedInChar[] = {0,2,4,6,8,10,12,NUM_LEDS}; // Enter first LED in each char, starting from 0. Let last array item be total number of LEDS.
CRGB color[] = {CRGB::White, CRGB::Red, CRGB::Lime, CRGB::Blue, CRGB::DodgerBlue, CRGB::Yellow, CRGB::Plum, CRGB::LightSalmon, CRGB::Fuchsia, CRGB::Gold, CRGB::Magenta, CRGB::Brown, CRGB::OrangeRed};
int colorSetChars[] = {1,5,5,5,5,5,1}; // Defines startup colors for the freeColorChars mode. Make sure to have one array element per char.
int colorSetLeds[] = {1,3,1,3,1,3,1,3,1,3,1,3,1,3}; // Defines startup colors for the freeColorLeds mode. Make sure to have one array element per LED.
unsigned long timer = 0;

Encoder Enc(8, 9);
long newPosition = 0;
long oldPosition  = 0;
volatile byte SW_state = 0;
int mode = 1;
bool onOff = true;
uint8_t brightness = 10;

void setup() {
  delay(3000); // 3 second delay for recovery
  pinMode(ENC_SWITCH, INPUT_PULLUP);
  // tell FastLED about the LED strip configuration
  //FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);
  
  attachInterrupt(digitalPinToInterrupt(ENC_SWITCH),EncSW,FALLING);
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

void EncSW(){
  SW_state++;
}
  
void loop()
{
  if(mode == 1){
    EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
  }
  else if((mode > 1) && (mode < 7)){
    gCurrentPatternNumber = mode - 2; // fixed pattern
  }
  else if(mode == 7){
    drawFreeColorsChars();
    setFreeColorsChars();
  }
  else if(mode == 8)
    drawFreeColorsChars();

  else if(mode == 9){
    drawFreeColorsLeds();
    setFreeColorsLeds();
  }
  else if(mode == 10)
    drawFreeColorsLeds();

  if(mode < 7)
    gPatterns[gCurrentPatternNumber](); // execute patterns

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow

  newPosition = Enc.read(); // Check for encoder change
  updateBrightness();
  checkSwitch();
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void drawFreeColorsChars(){ // Draw current colorset for chars
  int k = 1;
  for(int s=0;s<NUM_LEDS;s++){
    if(s<FirstLedInChar[k])
      leds[s] = color[colorSetChars[k-1]];
    else{
      k++;
      leds[s] = color[colorSetChars[k-1]];
    }
    FastLED.show();
  }
}

void setFreeColorsChars(){ // Lets user set color on each character/symbol
  int toutCount = 0;
  int k = 1;
  timer = millis();
  while((k < (ARRAY_SIZE(FirstLedInChar))) && (toutCount < 10)){
    toutCount = 0;
    while((SW_state == 0) && (toutCount < 10)){
      if((millis() - timer) < 800){
        for(int s=FirstLedInChar[k-1];s<FirstLedInChar[k];s++)
            leds[s] = color[colorSetChars[k-1]];
      }
      else if((millis() - timer) < 1000){
        for(int s=FirstLedInChar[k-1];s<FirstLedInChar[k];s++)
            leds[s] = CRGB::Black;
      }
      else{
        timer = millis();
        toutCount++;
      }
      FastLED.show();
      newPosition = Enc.read(); // Check for encoder change..
      delay(200);
      newPosition = Enc.read(); // ..discard double increment
      if(newPosition < oldPosition){
        if(colorSetChars[k-1] > 0)
          colorSetChars[k-1] = colorSetChars[k-1] - 1;
        timer = millis();
      }
      else if(newPosition > oldPosition){
        if(colorSetChars[k-1] < (ARRAY_SIZE(color) - 1))
          colorSetChars[k-1] = colorSetChars[k-1] + 1;
        timer = millis();
      }
      oldPosition = newPosition;
    }
    // Re-write color if clicked when black
    for(int s=FirstLedInChar[k-1];s<FirstLedInChar[k];s++)
      leds[s] = color[colorSetChars[k-1]];
    k++;
    delay(200); // Avoid double jump
    SW_state = 0;
  }
  mode = 8;
}

void drawFreeColorsLeds(){ // Draw current colorset for each led
  for(int s=0;s<NUM_LEDS;s++)
    leds[s] = color[colorSetLeds[s]];
  FastLED.show();
}

void setFreeColorsLeds(){ // Lets user set color on each LED
  int toutCount = 0;
  int k = 0;
  timer = millis();
  while((k < NUM_LEDS) && (toutCount < 10)){
    toutCount = 0;
    while((SW_state == 0) && (toutCount < 10)){
      if((millis() - timer) < 800){
        leds[k] = color[colorSetLeds[k]];
      }
      else if((millis() - timer) < 1000){
        leds[k] = CRGB::Black;
      }
      else{
        timer = millis();
        toutCount++;
      }
      FastLED.show();
      newPosition = Enc.read(); // Check for encoder change..
      delay(200);
      newPosition = Enc.read(); // ..discard double increment
      if(newPosition < oldPosition){
        if(colorSetLeds[k] > 0)
          colorSetLeds[k] = colorSetLeds[k] - 1;
        timer = millis();
      }
      else if(newPosition > oldPosition){
        if(colorSetLeds[k] < (ARRAY_SIZE(color) - 1))
          colorSetLeds[k] = colorSetLeds[k] + 1;
        timer = millis();
      }
      oldPosition = newPosition;
    }
    // Re-write color if clicked when black
    leds[k] = color[colorSetLeds[k]];
    k++;
    delay(200); // Avoid double jump
    SW_state = 0;
  }
  mode = 10;
}

void checkSwitch(){
  if(SW_state > 0){
    delay(500);
    if(!digitalRead(ENC_SWITCH)){ // Long press
      if(mode > 9)
        mode = 1;
      else
        mode++;

      FastLED.clear(true);
      for(int s=0;s<mode;s++){
        fill_solid( leds, NUM_LEDS, CRGB::Red);
        FastLED.show();
        delay(200);
        fill_solid( leds, NUM_LEDS, CRGB::Black);
        FastLED.show();
        delay(200);
      }
      SW_state = 0;
    }
    else{ // Short click: toggle ON/OFF
      if(onOff){
        for(int i = 0;i<100;i++){ // Turn OFF sign
          fadeToBlackBy( leds, NUM_LEDS, 10);
          FastLED.show();
          delay(10);
        }
      }
      else{
        if(mode == 8) // If in freeColorChar mode, let user re-set colors
          mode = 7;
        else if(mode == 10) // If in freeColorLed mode, let user re-set colors
          mode = 9;
      }
    onOff = !onOff;
    SW_state = 0;

    while(!onOff && (SW_state == 0)){}
    }
  }
}

void updateBrightness(){
    if((newPosition < oldPosition) && (brightness > 0))
      brightness--;
    else if ((newPosition > oldPosition) && (brightness < 50))
      brightness++;

    if(brightness >= 35) // A bit of expo feeling..
      FastLED.setBrightness(map(brightness, 35, 50, 10, 1));
    else
      FastLED.setBrightness(map(brightness, 0, 35, 255, 10));

    oldPosition = newPosition;
}

void nextPattern(){
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow(){
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter(){
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter){
  if( random8() < chanceOfGlitter){
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti(){
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon(){
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm(){
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle(){
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}