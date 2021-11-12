#include "arduino_secrets.h"
#include <WiFiNINA.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#define NO_OTA_PORT
#include <ArduinoOTA.h>
#include <ArduinoJson.h>
#include <FastLED.h>
#include <TimeLib.h>
#include <Timezone.h>

//#define DEBUGGING //uncomment for serial debug messages, no serial messages if this whole line is a comment!

#define BAUDRATE 115200

#define LED_PWR_LIMIT 500
#define LED_COUNT 93
#define LED_DIGITS 4
#define LED_PIN 6

#define AUTOBRIGHTNESS

bool statusToggle = false;

void setup() {
  #ifdef DEBUGGING
    Serial.begin(BAUDRATE);
    //while(!Serial);
    Serial.println("Serial start");
    Serial.print(F("Total LED count: ")); Serial.println(LED_COUNT);
    Serial.print(F("LED digits: ")); Serial.println(LED_DIGITS);
    
  #endif
  initWiFi();     //initializes WiFi
  setupTime();    //initializes time functions
  build_registration();
  mqttSetup();    // setup mqtt
  webServerSetup(); // setup webserver
  switchPalette(1);
  setupDisplay();
  
  #ifdef DEBUGGING
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.println("setup ready");
  #endif
}

void loop() {
  EVERY_N_MILLISECONDS( 50 ) {
    updateTime();
    displayLoop();
  }
  EVERY_N_MILLISECONDS( 500 ) {
    #ifdef DEBUGGING
      if (statusToggle) {
        digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
        statusToggle = false;
      } else {
        digitalWrite(LED_BUILTIN, LOW);
        statusToggle = true;
      }
    #endif
  }
  EVERY_N_SECONDS( 10 ) {
    checkForRestart();
    mqttCheckIn();
  }
  // put your main code here, to run repeatedly:
  mqttLoopOps();
}
