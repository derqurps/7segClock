// start clock specific config/parameters
/* Segment order, seen from the front:

   <  A  >
 /\       /\
 F        B
 \/       \/
   <  G  >
 /\       /\
 E        C
 \/       \/
   <  D  >

digit positions, seen from the front:
 _   _   _   _   _   _
|_| |_| |_| |_| |_| |_|
|_| |_| |_| |_| |_| |_|

 5   4   3   2   1   0
*/

uint8_t numbers[10][7] = {
  {   1,   1,   1,   1,   1,   1,   0 },  // 0
  {   0,   1,   1,   0,   0,   0,   0 },  // 1
  {   1,   1,   0,   1,   1,   0,   1 },  // 2
  {   1,   1,   1,   1,   0,   0,   1 },  // 3
  {   0,   1,   1,   0,   0,   1,   1 },  // 4
  {   1,   0,   1,   1,   0,   1,   1 },  // 5
  {   1,   0,   1,   1,   1,   1,   1 },  // 6
  {   1,   1,   1,   0,   0,   0,   0 },  // 7
  {   1,   1,   1,   1,   1,   1,   1 },  // 8
  {   1,   1,   1,   1,   0,   1,   1 }   // 9
};

struct segment {
  uint16_t sStart;
  uint16_t sEnd;
};

struct digit {
  segment seg[7];
};

const uint8_t digitPositions[4] = { 0, 1, 2, 3 };
const uint16_t digitStartPositions[4] = {0,22,49,71};
digit digits[4];


const uint16_t upperDots[2] PROGMEM = {  44,  45 };
const uint16_t lowerDots[2] PROGMEM = {  47,  48 };

void firstType(uint16_t i, struct digit &returnDigit) {
  returnDigit = {
    segment{/*A*/(i+6),(i+8)},
    segment{/*B*/(i+3),(i+5)},
    segment{/*C*/(i+19),(i+21)},
    segment{/*D*/(i+16),(i+18)},
    segment{/*E*/(i+13),(i+15)},
    segment{/*F*/(i+9),(i+11)},
    segment{/*G*/(i+0),(i+2)}
   };
}
void secondType(uint16_t i, struct digit &returnDigit) {
  returnDigit = {
    segment{/*A*/(i+13),(i+15)},
    segment{/*B*/(i+10),(i+12)},
    segment{/*C*/(i+6),(i+8)},
    segment{/*D*/(i+3),(i+5)},
    segment{/*E*/(i+0),(i+2)},
    segment{/*F*/(i+16),(i+18)},
    segment{/*G*/(i+19),(i+21)}
   };
}

void setupDisplay() {
  uint16_t i = digitStartPositions[0];
  firstType(i, digits[0]);

  i = digitStartPositions[1];
  secondType(i, digits[1]);
  
  i = digitStartPositions[2];
  firstType(i, digits[2]);
   
  i = digitStartPositions[3];
  secondType(i, digits[3]);

  #ifdef AUTOBRIGHTNESS
    #ifdef DEBUG
      Serial.print(F("autoBrightness enabled, LDR using pin: ")); Serial.println(LDR_PIN);
    #endif
    //pinMode(LDR_PIN, INPUT);
  #endif

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, LED_COUNT).setCorrection(TypicalSMD5050).setTemperature(DirectSunlight).setDither(1);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, LED_PWR_LIMIT);
  FastLED.clear();
  FastLED.show();
  #ifdef DEBUGGING
    Serial.println(F("setup(): Lighting up some leds..."));
  #endif
  /*for ( uint8_t i = 0; i < LED_DIGITS; i++ ) {
    showDigit(i, 1);
  }*/
  showDots();
  FastLED.show();
}



void displayLoop() {

  static uint8_t refreshDelay = 5;                // refresh leds every 5ms
  static long lastRefresh = millis();             // Keeps track of the last led update/FastLED.show() inside the loop

  if ( lastSecondDisplayed != second(now()) ) {
    doDisplayLoop();
  }
  
  #ifdef AUTOBRIGHTNESS
    if ( millis() - lastReadLDR >= intervalLDR ) {     // if LDR is enabled and sample interval has been reached...
      readLDR();                                       // ...call readLDR();
      if ( abs(avgLDR - lastAvgLDR) >= 5 ) {           // if avgLDR has changed for more than +/- 5 update lastAvgLDR
        lastAvgLDR = avgLDR;
        #ifdef DEBUGGING
          Serial.print("Setting brightness to: ");
          Serial.println(avgLDR);
        #endif
        FastLED.setBrightness(avgLDR);
      }
      lastReadLDR = millis();
    }
  #endif
  if ( millis() - lastRefresh >= refreshDelay ) {
    FastLED.show();
    lastRefresh = millis();
  }
}

void doDisplayLoop() {
  /*if (timeNotSet){
    Serial.println("time not set");
  }
  if (timeNeedsSync){
    Serial.println("time needs sync");
  }
  if (timeSet){
    Serial.println("time set");
  }
  Serial.print("timeNotSet ");
  Serial.println(timeNotSet);
  Serial.print("timeNeedsSync ");
  Serial.println(timeNeedsSync);
  Serial.print("timeSet ");
  Serial.println(timeSet);*/
  #ifdef USERTC
    setTime(rtcTime);                                  // sync system time to rtc every second
    FastLED.clear();                                 // 1A - clear all leds...
    displayTime(rtcTime);                            // 2A - output rtcTime to the led array..
    lastSecondDisplayed = second(rtcTime);
  #else
    FastLED.clear();                                 // 1B - clear all leds...
    displayTime(now());                            // 2B - output sysTime to the led array...
    lastSecondDisplayed = second(); 
  #endif
}

void displayTime(time_t t) {

  time_t local = myTimeZone.toLocal(t, &tcr);                          // convert display time to local time zone according to rules on top of the sketch
  #ifdef DEBUGGING
    /*Serial.print(hour(local));
    Serial.print(":");
    Serial.print(minute(local));
    Serial.print(":");
    Serial.println(second(local));
    Serial.print(minute());
    Serial.print(":");
    Serial.println(second());*/
  #endif
  if ( clockStatus >= 90 ) {
    FastLED.clear();
  }
  /* hours */
  if ( displayMode == 0 ) {
    if ( hour(local) < 10 ) {
      if ( leadingZero ) {
        showDigit(digitPositions[0], 0);
      }
    } else {
      showDigit(digitPositions[0], hour(local) / 10);
    }
    showDigit(digitPositions[1], hour(local) % 10);
  } else if ( displayMode == 1 ) {
    if ( hourFormat12(local) < 10 ) {
      if ( leadingZero ) {
        showDigit(digitPositions[0], 0);
      }
    } else {
      showDigit(digitPositions[0], hourFormat12(local) / 10);
    }
    showDigit(digitPositions[1], hourFormat12(local) % 10);
  }
  /* minutes */
  showDigit(digitPositions[2], minute(local) / 10);
  showDigit(digitPositions[3], minute(local) % 10);
  if ( LED_DIGITS == 6 ) {
    /* seconds */
    showDigit(digitPositions[4], second(local) / 10);
    showDigit(digitPositions[5], second(local) % 10);
  }

  /* dots */
  if ( dotsBlinking ) {
    if ( second(local) % 2 == 0 ) {
      showDots();
    }
  } else {
    showDots();
  }
}


void showSegment(uint8_t digitN, uint8_t segmentN) {
  
  uint16_t startLed = digits[digitPositions[digitN]].seg[segmentN].sStart;
  uint16_t endLed = digits[digitPositions[digitN]].seg[segmentN].sEnd;

  for ( uint16_t i = startLed; i <= endLed; i++ ) {
    setLED(i);
  }
}


void showDots() {
  for ( uint16_t i = upperDots[0]; i <= upperDots[1]; i++ ) {
    setLED(i);
  }
  for ( uint16_t i = lowerDots[0]; i <= lowerDots[1]; i++ ) {
    setLED(i);
  }
}

void setLED(uint16_t i) {
  //leds[i].setRGB( 255, 68, 221);
  leds[i] = ColorFromPalette(currentPalette, i * 32, avgLDR, LINEARBLEND);
}

void showDigit(uint8_t pos, uint8_t digit) {
  if (pos >= LED_DIGITS ||
    digit >= sizeof(numbers)) {
    Serial.print("POS=");
    Serial.print(pos);
    Serial.print("/DIGIT=");
    Serial.print(digit);
    Serial.println();
  } else {
    // This draws numbers using the according segments as defined on top of the sketch
    for (uint8_t i = 0; i < 7; i++) {
      if (numbers[digit][i]) {
        showSegment(pos, i);
      }
    }
  }
}

void switchColor(uint16_t r, uint16_t g, uint16_t b) {
  currentPalette = CRGBPalette16(CRGB( r, g, b ));
}

void switchPalette(uint8_t paletteIndex) {
  switch ( paletteIndex ) {
    default:
    case 0: currentPalette = CRGBPalette16( CRGB( 224,   0,  32 ),
                                            CRGB(   0,   0, 244 ),
                                            CRGB( 128,   0, 128 ),
                                            CRGB( 224,   0,  64 ) ); break;
    case 1: currentPalette = CRGBPalette16( CRGB( 224,  16,   0 ),
                                            CRGB( 192,  64,   0 ),
                                            CRGB( 192, 128,   0 ),
                                            CRGB( 240,  40,   0 ) ); break;
    case 2: currentPalette = CRGBPalette16( CRGB::Aquamarine,
                                            CRGB::Turquoise,
                                            CRGB::Blue,
                                            CRGB::DeepSkyBlue   ); break;
    case 3: currentPalette = RainbowColors_p; break;
    case 4: currentPalette = PartyColors_p; break;
    case 5: currentPalette = CRGBPalette16( CRGB::LawnGreen ); break;
  }
}


#ifdef AUTOBRIGHTNESS
void readLDR() {                                                                                            // read LDR value 5 times and write average to avgLDR
  static uint8_t runCounter = 1;
  static uint16_t tmp = 0;
  int LDRval = analogRead(LDR_PIN);

  uint8_t readOut = map(LDRval, 0, 1023, minBrightness, maxBrightness);
  tmp += readOut;
  if (runCounter == 5) {
    avgLDR = ( tmp / 5 )  * factorLDR;
    tmp = 0;
    runCounter = 0;
    #ifdef DEBUGGING
      Serial.print(F("readLDR(): avgLDR value: "));
      Serial.print(avgLDR);
    #endif
    if ( avgLDR >= upperLimitLDR ) {
      avgLDR = maxBrightness; 
    } else if ( avgLDR <= lowerLimitLDR ) {
      avgLDR = minBrightness;
    }
    #ifdef DEBUGGING
      Serial.print(F(" - adjusted to: "));
      Serial.println(avgLDR);
    #endif
  }
  runCounter++;
}
#endif
