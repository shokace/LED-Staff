#include "FastLED.h"

FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few 
// of the kinds of animation patterns you can quickly and easily 
// compose using FastLED.  
//
// This example also shows one easy way to define multiple 
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

int ButtonValue = LOW;
int Button = 6;


//********************************CUSTOM********************************


#define qsubd(x, b)  ((x>b)?b:0)                              // A digital unsigned subtraction macro. if result <0, then => 0. Otherwise, take on fixed value.
#define qsuba(x, b)  ((x>b)?x-b:0)                            // Unsigned subtraction macro. if result <0, then => 0

uint8_t max_bright = 128;                                     // Overall brightness definition. It can be changed on the fly.
unsigned long previousMillis;                                 // Store last time the strip was updated.

uint8_t thishue;                                              // You can change the starting hue value for the first wave.
uint8_t thathue;                                              // You can change the starting hue for other wave.
uint8_t thisrot;                                              // You can change how quickly the hue rotates for this wave. Currently 0.
uint8_t thatrot;                                              // You can change how quickly the hue rotates for the other wave. Currently 0.
uint8_t allsat;                                               // I like 'em fully saturated with colour.
uint8_t thisdir;
uint8_t thatdir;
uint8_t alldir;                                               // You can change direction.
int8_t thisspeed;                                             // You can change the speed.
int8_t thatspeed;                                             // You can change the speed.
uint8_t allfreq;                                              // You can change the frequency, thus overall width of bars.
int thisphase;                                                // Phase change value gets calculated.
int thatphase;                                                // Phase change value gets calculated.
uint8_t thiscutoff;                                           // You can change the cutoff value to display this wave. Lower value = longer wave.
uint8_t thatcutoff;                                           // You can change the cutoff value to display that wave. Lower value = longer wave.
int thisdelay;                                                // Standard delay. . 
uint8_t fadeval;                                              // Use to fade the led's of course.

CRGBPalette16 thisPalette;
CRGBPalette16 thatPalette;
TBlendType    currentBlending;                                // NOBLEND or LINEARBLEND







//********************************CUSTOM********************************
#define DATA_PIN    3
//#define CLK_PIN   4
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    72
CRGB leds[NUM_LEDS];



#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  1000



void setup() {
  pinMode(Button, INPUT);
  
  delay(1000); // 3 second delay for recovery
  
  // tell FastLED about the LED strip configuration
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);
  

  
//********************************CUSTOM********************************
  thisPalette = RainbowColors_p;
  thatPalette = RainbowColors_p;
  currentBlending = LINEARBLEND;
//********************************CUSTOM********************************


resetvars();   
}


// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm, sin1, sin2, sin3, sin4, sin5, sin6, sin7, sin8, sin11 };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
  
void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();
  
  // send the 'leds' array out to the actual LED strip
  //FastLED.show();
  
  FastLED.delay(1000/FRAMES_PER_SECOND);
  // insert a delay to keep the framerate modest
   
  ButtonValue = digitalRead(Button);
  if(ButtonValue == HIGH)
  {
    FastLED.show();
    //ChangeMe();
    
    EVERY_N_SECONDS( 1 ) { nextPattern(); } // change patterns periodically
    ButtonValue = LOW;
    FastLED.delay(1000/FRAMES_PER_SECOND);
    
  }
  else
  {
    ButtonValue = LOW;
  }
  // do some periodic updates
  EVERY_N_MILLISECONDS( 1 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  //EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
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
  fadeToBlackBy( leds, NUM_LEDS, 300);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(50, 0, NUM_LEDS);
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 1000;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 10, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 300);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16(i+50,0,NUM_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void two_sin() 

{
    FastLED.show();
    thisdir ? thisphase += beatsin8(thisspeed, 100, 100) : thisphase -= beatsin8(thisspeed, 100, 100);
    thatdir ? thatphase += beatsin8(thisspeed, 100, 100) : thatphase -= beatsin8(thatspeed, 100, 100);
    thishue += thisrot;                                        // Hue rotation is fun for thiswave.
    thathue += thatrot;                                        // It's also fun for thatwave.

    for (int k=0; k<NUM_LEDS-1; k++) {
      if(ButtonValue != HIGH)
      {
        
        int thisbright = qsuba(cubicwave8((k*allfreq)+thisphase), thiscutoff);      // qsub sets a minimum value called thiscutoff. If < thiscutoff, then bright = 0. Otherwise, bright = 128 (as defined in qsub)..
        int thatbright = qsuba(cubicwave8((k*allfreq)+128+thatphase), thatcutoff);  // This wave is 180 degrees out of phase (with the value of 128).

        leds[k] = ColorFromPalette(thisPalette, thishue, thisbright, currentBlending);
        leds[k] += ColorFromPalette(thatPalette, thathue, thatbright, currentBlending);
      }
  }
     nscale8(leds,NUM_LEDS,fadeval);

} // two_sin()

void sin1()
{
  thisrot = 1; thatrot = 1; thisPalette=PartyColors_p; thatPalette=PartyColors_p; 
  two_sin();
  //resetvars();
}

void sin2()
{
  thisrot = 0; thatdir = 1; thatspeed = -100; thisPalette=ForestColors_p; thatPalette=OceanColors_p; 
  two_sin();
  //resetvars();
}

void sin3()
{
  thatrot = 0; thisPalette=PartyColors_p; thatPalette=RainbowColors_p;
  two_sin();
  //resetvars();
}

void sin4()
{
  allfreq = 16; thisdir = 1; thathue = 128;
  two_sin();
  //resetvars();
}

void sin5()
{
  thiscutoff = 96; thatcutoff = 240;
  two_sin();
  //resetvars();
}

void sin6()
{
  thiscutoff = 96; thatdir = 0; thatcutoff = 96; thisrot = 1;
  two_sin();
  //resetvars();
}

void sin7()
{
  thisspeed= -1000; thisdir = 0; thatspeed= -1000;
  two_sin();
  //resetvars();
}

void sin8()
{
  thiscutoff = 128; thatcutoff = 128;
  two_sin();
  //resetvars();
}



void sin11()
{
  thisspeed = 1000; thatcutoff = 96; thiscutoff = 224; thatspeed = 1000;
  two_sin();
  //resetvars();
}








void ChangeMe() {
  
  uint8_t secondHand = (millis() / 1000) % 60;                // Increase this if you want a longer demo.
  static uint8_t lastSecond = 99;                             // Static variable, means it's only defined once. This is our 'debounce' variable.
  
  if( lastSecond != secondHand) {
    lastSecond = secondHand;
    switch (secondHand) {
      case  0: thisrot = 1; thatrot = 1; thisPalette=PartyColors_p; thatPalette=PartyColors_p;
      case  5: thisrot = 0; thatdir = 1; thatspeed = -100; thisPalette=ForestColors_p; thatPalette=OceanColors_p; break;
      case 10: thatrot = 0; thisPalette=PartyColors_p; thatPalette=RainbowColors_p; break;
      case 15: allfreq = 16; thisdir = 1; thathue = 128; break;
      case 20: thiscutoff = 96; thatcutoff = 240; break;
      case 25: thiscutoff = 96; thatdir = 0; thatcutoff = 96; thisrot = 1; break;
      case 30: thisspeed= -1000; thisdir = 0; thatspeed= -1000; break;
      case 35: thiscutoff = 128; thatcutoff = 128; break;
      case 40: thisspeed = 1000; break;
      case 45: thisspeed = 1000; thatspeed = -1000; break;
      case 50: thisspeed = 1000; thatcutoff = 96; thiscutoff = 224; thatspeed = 1000; break;
      case 55: resetvars(); break;
      case 60: break;
    }
  }
}

void resetvars() {                       // Reset the variable back to the beginning.

  thishue = 0;                          // You can change the starting hue value for the first wave.
  thathue = 140;                        // You can change the starting hue for other wave.
  thisrot = 1;                          // You can change how quickly the hue rotates for this wave. Currently 0.
  thatrot = 1;                          // You can change how quickly the hue rotates for the other wave. Currently 0.
  allsat = 255;                         // I like 'em fully saturated with colour.
  thisdir = 0;                          // Change the direction of the first wave.
  thatdir = 0;                          // Change the direction of the other wave.
  alldir = 0;                           // You can change direction.
  thisspeed = 1000;                        // You can change the speed, and use negative values.
  thatspeed = 1000;                        // You can change the speed, and use negative values.
  allfreq = 32;                         // You can change the frequency, thus overall width of bars.
  thisphase = 0;                        // Phase change value gets calculated.
  thatphase = 0;                        // Phase change value gets calculated.
  thiscutoff = 192;                     // You can change the cutoff value to display this wave. Lower value = longer wave.
  thatcutoff = 192;                     // You can change the cutoff value to display that wave. Lower value = longer wave.
  thisdelay = 10;                       // You can change the delay. Also you can change the allspeed variable above.
  fadeval = 192;                        // How quickly we fade.
  
} // resetvars()


