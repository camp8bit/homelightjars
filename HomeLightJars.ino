#include "FastLED.h"

/**
 * Light-jars project for ambient in-home decoration
 * Sitting in Sam's piano
 */

FASTLED_USING_NAMESPACE

#define DATA_PIN    3
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_JARS    2
#define JAR_LENGTH    8
#define NUM_LEDS    JAR_LENGTH    *NUM_JARS

CRGB leds[NUM_LEDS];

int colorA = 0, colorB = 1;

CRGBPalette16 currentPalette(CRGB::Black);
CRGBPalette16 targetPalette(CloudColors_p);

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

void setup() {
  delay(100);
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
 // FastLED.setBrightness(BRIGHTNESS);

  nextPattern();
}

fract8 chanceOfGlitter=0;

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { colorPair /*, rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm*/ };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
  
void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();
  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  nblendPaletteTowardPalette( currentPalette, targetPalette, 12);

  // do some periodic updates
  EVERY_N_MILLISECONDS( 100 ) {
    if(random8() == 1) {
      chanceOfGlitter = 100;
    }
    if(chanceOfGlitter > 0) {
      chanceOfGlitter -= 10;
    }
  }

  // Change every 2 minutes
  /*
  EVERY_N_SECONDS( 1 ) {
  //  nextPattern();
  }
  */

  EVERY_N_SECONDS( 30 ) {
    nextPalette();
  }
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{

  colorA = random8(16);
  colorB = random8(16);

  /*
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
  */
}

/**
 * Choose a new palette
 */
void nextPalette()
{
  for( int i = 0; i < 2; i++) {
    targetPalette[i] = CHSV(random8(171), 255, 255);
  }
}

void colorPair() {  
  fill_solid(leds, JAR_LENGTH, ColorFromPalette(currentPalette, colorA));
  fill_solid(leds+JAR_LENGTH, JAR_LENGTH, ColorFromPalette(currentPalette, colorB));

  if(chanceOfGlitter) {
    addGlitter(chanceOfGlitter);
  }
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(13,0,NUM_LEDS);
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16(i+7,0,NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

