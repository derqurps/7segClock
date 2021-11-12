
char ssid[] = SECRET_WIFI1_SSID;
char password[] = SECRET_WIFI1_PASS;

const char mdns_hostname[] = SECRET_HOST;

const char* mqtt_server = SECRET_MQTT_SERVER;
const int mqtt_port = 1883;

const char* mqtt_listen = SECRET_MQTT_LISTEN;
const char* mqtt_registration = SECRET_MQTT_REGISTRATION;
const char* mqtt_debug = "/debug";

const char* MQTT_USER = SECRET_MQTT_USER;
const char* MQTT_PASSWORD = SECRET_MQTT_PASS;

const String localTimezoneString = SECRET_TIMEZONE_POSIX;

WiFiClient client;
PubSubClient mqttClient(client);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, SECRET_NTP_SERVER);
const int sync_time = 120;     // Time in seconds to Sync NTP Time
bool syncIntervalSet = false;

// Function prototypes
time_t syncNTPTime();

//-----------------------------------------------
/* Europe */
TimeChangeRule tcr1 = {"tcr1", Last, Sun, Oct, 3, 60};         // standard/winter time, valid from last sunday of october at 3am, UTC + 1 hour (+60 minutes) (negative value like -300 for utc -5h)
TimeChangeRule tcr2 = {"tcr2", Last, Sun, Mar, 2, 120};        // daylight/summer time, valid from last sunday of march at 2am, UTC + 2 hours (+120 minutes)
//-----------------------------------------------
Timezone myTimeZone(tcr1, tcr2);
TimeChangeRule *tcr;

static int restartNow = false;

uint8_t clockStatus = 1;                  // Used for various things, don't mess around with it! 1 = startup

String mqttRegistrationStr;

uint8_t markerHSV[3] = { 0, 127, 20 };                         // this color will be used to "flag" leds for coloring later on while updating the leds
CRGB leds[LED_COUNT];
CRGBPalette16 currentPalette;

static uint8_t lastSecondDisplayed = 0;           // This keeps track of the last second when the display was updated (HH:MM and HH:MM:SS)



/* Start autoBrightness config/parameters -------------------------------------------------------------- */
#define LDR_PIN A1
uint8_t upperLimitLDR = 254;                      // everything above this value will cause max brightness (according to current level) to be used (if it's higher than this)
uint8_t lowerLimitLDR = 50;                       // everything below this value will cause minBrightness to be used
uint8_t minBrightness = 22;                       // anything below this avgLDR value will be ignored
uint8_t maxBrightness = 254;                       // anything below this avgLDR value will be ignored

const bool nightMode = false;                     // nightmode true -> if minBrightness is used, colorizeOutput() will use a single color for everything, using HSV
const uint8_t nightColor[2] = { 0, 70 };          // hue 0 = red, fixed brightness of 70, https://github.com/FastLED/FastLED/wiki/FastLED-HSV-Colors
float factorLDR = 1.0;                            // try 0.5 - 2.0, compensation value for avgLDR. Set dbgLDR true & define DEBUG and watch the serial monitor. Looking...
const bool dbgLDR = false;                        // ...for values roughly in the range of 120-160 (medium room light), 40-80 (low light) and 0 - 20 in the dark
uint8_t intervalLDR = 75;                         // read value from LDR every 75ms (most LDRs have a minimum of about 30ms - 50ms)
uint16_t avgLDR = 0;                              // we will average this value somehow somewhere in readLDR();
uint16_t lastAvgLDR = 0;                          // last average LDR value we got
static long lastReadLDR = millis();
/* End autoBrightness config/parameters ---------------------------------------------------------------- */


/* Start basic appearance config------------------------------------------------------------------------ */
const bool dotsBlinking = true;                                  // true = only light up dots on even seconds, false = always on
const bool leadingZero = true;                                  // true = enable a leading zero, 9:00 -> 09:00, 1:30 -> 01:30...
uint8_t displayMode = 0;                                         // 0 = 24h mode, 1 = 12h mode ("1" will also override setting that might be written to EEPROM!)
uint8_t colorMode = 0;                                           // different color modes, setting this to anything else than zero will overwrite values written to eeprom, as above
uint16_t colorSpeed = 750;                                       // controls how fast colors change, smaller = faster (interval in ms at which color moves inside colorizeOutput();)
const bool colorPreview = true;                                  // true = preview selected palette/colorMode using "8" on all positions for 3 seconds
const uint8_t colorPreviewDuration = 3;                          // duration in seconds for previewing palettes/colorModes if colorPreview is enabled/true
const bool reverseColorCycling = false;                          // true = reverse color movements
/* End basic appearance config-------------------------------------------------------------------------- */
