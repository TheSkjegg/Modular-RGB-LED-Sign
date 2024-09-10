// Description: 
//    Modular LED Sign code, based on DemoReel100.ino from the FastLED library. Added encoder control and freeColor mode to 
//    allow user to set colors by encoder. Encoder functions:
//    Short press: Toggles sign ON/OFF
//    Long press: Increments modes
//    Turning: Normally sets sign brightness, if in freeColor mode it selects color
//
// Usage:
//    Developed on the Seed Studio XIAO SAMD21 board, but should be compatible with all "Arduino UNO" boards.
//    Make sure to set correct number of LED's, and define arrays for characters and colors.
//
// Author: 
//    The Skjegg 09/09/2024
//
// Versions:
//    v001: First version

#include <FastLED.h>
#include <Encoder.h>

FASTLED_USING_NAMESPACE

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define LED_PIN     3
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    14
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          255
#define FRAMES_PER_SECOND  60

#define ENC_SWITCH  10

// Enter first LED in each char, starting from 0. Let last array item be total number of LEDS.
int FirstLedInChar[] = {0,2,4,6,8,10,12,NUM_LEDS};
CRGB color[] = {CRGB::White, CRGB::Red, CRGB::Lime, CRGB::Blue, CRGB::DodgerBlue, CRGB::Yellow, CRGB::Plum, CRGB::LightSalmon, CRGB::Fuchsia, CRGB::Gold, CRGB::Magenta, CRGB::Brown, CRGB::Purple};
// Array defines startup colors for the freeColor mode. Make sure to have one array element per char.
int colorSet[] = {0,1,1,1,1,1,0};
unsigned long timer = 0;

Encoder Enc(8, 9);
long newPosition = 0;
long oldPosition  = 0;
volatile byte SW_state = 0;
int mode = 1;
bool onOff = true;
uint8_t brightness = 40;

void setup() {
  Serial.begin(9600); // Debug use
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
    drawFreeColors();
    setFreeColors();
  }
  else if(mode == 8)
    drawFreeColors();

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

void drawFreeColors(){ // Draw current colorset
  int k = 1;
  for(int s=0;s<NUM_LEDS;s++){
    if(s<FirstLedInChar[k])
      leds[s] = color[colorSet[k-1]];
    else{
      k++;
      leds[s] = color[colorSet[k-1]];
    }
    FastLED.show();
  }
}

void setFreeColors(){
  int toutCount = 0;
  int k = 1;
  timer = millis();
  while((k < (ARRAY_SIZE(FirstLedInChar))) && (toutCount < 10)){
    toutCount = 0;
    while((SW_state == 0) && (toutCount < 10)){
      if((millis() - timer) < 800){
        for(int s=FirstLedInChar[k-1];s<FirstLedInChar[k];s++)
            leds[s] = color[colorSet[k-1]];
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
        if(colorSet[k-1] > 0)
          colorSet[k-1] = colorSet[k-1] - 1;
        timer = millis();
      }
      else if(newPosition > oldPosition){
        if(colorSet[k-1] < (ARRAY_SIZE(color) - 1))
          colorSet[k-1] = colorSet[k-1] + 1;
        timer = millis();
      }
      oldPosition = newPosition;
    }
    // Re-write color if clicked when black
    for(int s=FirstLedInChar[k-1];s<FirstLedInChar[k];s++)
      leds[s] = color[colorSet[k-1]];
    k++;
    delay(200); // Avoid double jump
    SW_state = 0;
  }
  mode = 8;
}

void checkSwitch(){
  if(SW_state > 0){
    delay(500);
    if(!digitalRead(ENC_SWITCH)){ // Long press
      if(mode > 7)
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
        if(mode == 8) // If in freeColor mode, let user re-set colors
          mode = 7;
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
    oldPosition = newPosition;
      FastLED.setBrightness(map(brightness, 0, 50, 255, 0));
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