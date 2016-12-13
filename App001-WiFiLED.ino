// Version History
// v1.6 - Added switches to toggle between Blynk and MQTT, with initial MQTT functionality
// v1.5 - Adding ArduinoOTA, to allow remote deployment to active devices
// v1.4 - Adding Blynk, for remote control app
// v1.3 - Added WebServer functionality for control
// v1.2 - Added WiFiManager for wifi setup
// v1.1 - Added FastLED and initial LED control
// v1.0 - Initial Project

///////////////// Helper, to allow organisation of .ino files by alphabet:- ///////////////////////
void zzz_setup();
void zzz_loop();
///////////// End of Helper, to allow organisation of .ino files by alphabet:- ////////////////////


//#include "Options.h"	// NB: Change defines here to enable/disable features/modes, vary LED count, etc.
// NB: For WifiManager info, see https://github.com/tzapu/WiFiManager

//const int aNumber = NUM_LEDS;

///////////// Constants ///////////////
const int COMMAND_NONE = 0;
const int COMMAND_OFF = 1;
const int COMMAND_ON = 2;
const int COMMAND_CLEAR = COMMAND_OFF;
const int COMMAND_WHITE = 3;
const int COMMAND_RED = 4;
const int COMMAND_GREEN = 5;
const int COMMAND_BLUE = 6;
const int COMMAND_PURPLE = 7;
const int COMMAND_REDGREEN = 8;
const int COMMAND_BLACK = COMMAND_OFF;
const int COMMAND_MIXED = 9;
const int COMMAND_CUSTOM_SINGLE = 10;
const int COMMAND_CUSTOM_MIXED = 11;
const int COMMAND_FLASH_ON = 12;
const int COMMAND_FLASH_OFF = 13;
const int COMMAND_CYCLE_ON = 14;
const int COMMAND_CYCLE_OFF = 15;
const int COMMAND_REMOVE_LED = 16;
const int COMMAND_ADD_LED = 17;
const int COMMAND_SAVE = 18;
const int COMMAND_LOAD = 19;
const int COMMAND_RAINBOW = 20;
const int COMMAND_CUSTOM_RAINBOW = 21;
const int COMMAND_RESTART = 22;
const int COMMAND_CLEAR_WIFI = 23;
const int COMMAND_SET_BRIGHTNESS = 24;
const int COMMAND_DARKEN = 25;
const int COMMAND_LIGHTEN = 26;
const int COMMAND_RESET = 27;
const int COMMAND_RRRGGG = 28;
const int COMMAND_RRRGGGBBB = 29;
///////////////////////////////////////

////////////// Defines ////////////////
//#define USE_BLYNK   // NB: Currently, this is crashing upon connect
#define USE_MMQT
#define USE_ARDUINO_OTA

#define NUM_LEDS 30
#define LED_DATA_PIN 1

// Override, to allow playing with part of a larger strip ... otherwise, simply match the NUM_LEDS setting to your strip instead
#define ACTIVE_LEDS NUM_LEDS
// Override, to set an initial reduced active LED count (default should be all)
#define NUM_COLOURS ACTIVE_LEDS

#ifdef USE_BLYNK
//#define BLYNK_AUTH_TOKEN ""
#define BLYNK_AUTH_TOKEN "ac2c04d72f3c4d89a2d1485ba422b6dd"
#endif  // USE_BLYNK

//#define FASTLED_ESP8266_RAW_PIN_ORDER
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
//#define FASTLED_ESP8266_D1_PIN_ORDER
/////////// End of Defines ////////////

/////////// Defines_Blynk /////////////
#include <ESP8266WiFi.h>
#ifdef USE_BLYNK
#define BLYNK_PRINT Serial
#define BLYNK_MAX_SENDBYTES 256  // NB: Default is 128
#include <BlynkSimpleEsp8266.h>

#define BLYNK_AUTH_TOKEN "ac2c04d72f3c4d89a2d1485ba422b6dd"
#define OFF_PIN V0
#define TERMINAL_PIN V1
#define RGB_PIN V2
#define MENU_PIN V3
#define RESTART_PIN V4
#define LED_PIN V5
#define BRIGHTNESS_PIN V6
#define LIGHT_SENSOR_PIN V7
#else  // If not USE_BLYNK
#endif // USE_BLYNK
//////// End of Defines_Blynk /////////

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#include <FastLED.h>

#ifdef USE_ARDUINO_OTA
#include <ArduinoOTA.h>
#endif // USE_ARDUINO_OTA

#include <EEPROM.h>
// NB: Change the SETTINGS_VALID definition to invalidate previous settings
#define SETTINGS_VALID 123
#define SETTINGS_INVALID 0xFE
#define SETTINGS_EEPROM_OFFSET 512

#define SOFT_AP_NAME "WiFiLEDs"

struct Settings {
  int valid;
  CRGB colours_custom[NUM_COLOURS];
  int colours_count;
  int activeLEDs;
  bool cycling;
  bool flashing;
  //char password;  // NB: Should consider encryption before adding this
  int lastCommand;
  byte brightness;
  bool on;
  bool autoSave;
};

/////////////////////////////////////
// Function Declarations

/////////////////////////////////////
// Globals
bool started = false;

int numColours = NUM_COLOURS;
//CRGB leds[NUM_LEDS];
CRGBArray<NUM_LEDS> leds;
CRGB colours[NUM_COLOURS];
CRGB colours_mixed[3];
CRGB colours_redgreen[2];
CRGB colours_rrrggg[6];
CRGB colours_rrrgggbbb[9];
CRGB *remote_colour;

int cycleIndex;
int cycleCount;

Settings settings;

///////// Function Definitions ///////////////
#ifdef USE_BLYNK
void setup_blynk();
void print_blynk(char *msg);
void print_blynk(String msg);
void print_blynk(int val);
void print_blynk(byte val);
void print_blynk(long val);
void print_blynk(char val);
void println_blynk(char *msg);
void println_blynk(String msg);
void println_blynk(int val);
void println_blynk(byte val);
void println_blynk(long val);
void println_blynk(char val);
#endif // USE_BLYNK

void setupLEDs();
void addLED();
void removeLED();
void setLEDs();
void setLEDs(int newColourCount, struct CRGB newColours[]);
void setLEDs(struct CRGB colour);

#ifdef USE_ARDUINO_OTA
void setup_ArduinoOTA();
#endif	// USE_ARDUINO_OTA

void flash(int cycles, int delayLength, struct CRGB colour);
void flash(int cycles, int delayLength);
void flash();
void set_rainbow(CRGB *colourArray, int numberToFill);

void setDefaultSettings();  // Moved to Settings.ino
void saveSettings();  // Moved to Settings.ino
void loadSettings();  // Moved to Settings.ino

void setup_EEPROM();  // Moved to EEPROM.ino

void setup_colours(); // Moved to LEDRoutines.ino

void setup() {
  // Connect Serial output
  Serial.begin(115200);
  
  Serial.println("\r\n\r\n**********************************\r\n\r\n");
  // Call Main application setup routine (in zzz_App.ino)
  zzz_setup();
  Serial.println("\r\n\r\n**********************************\r\n\r\n");
}

void update(int command) {
  Serial.print("In update: command=");
  Serial.println(command);
  switch (command) {
    case COMMAND_NONE:
      break;
    case COMMAND_OFF:
      //setLEDs(CRGB::Black);
      //settings.brightness = 0;
      //setLEDs();
      setLEDs(CRGB::Black);
      break;
    /* NB: Not currently storing last value, can be re-added when that is available
      case COMMAND_ON:
    	break;
    */
    /* NB: Handled via COMMAND_OFF
      case COMMAND_CLEAR:
    	break;
      case COMMAND_BLACK:
    	break;
    */
    case COMMAND_WHITE:
      setLEDs(CRGB::White);
      break;
    case COMMAND_RED:
      setLEDs(CRGB::Red);
      break;
    case COMMAND_GREEN:
      setLEDs(CRGB::Green);
      break;
    case COMMAND_BLUE:
      setLEDs(CRGB::Blue);
      break;
    case COMMAND_PURPLE:
      setLEDs(CRGB::Purple);
      break;
    case COMMAND_REDGREEN:
      setLEDs(2, colours_redgreen);
      break;
    case COMMAND_RRRGGG:
      setLEDs(6, colours_rrrggg);
      break;
    case COMMAND_RRRGGGBBB:
      setLEDs(9, colours_rrrgggbbb);
      break;
    case COMMAND_MIXED:
      setLEDs(3, colours_mixed);
      break;
    case COMMAND_CUSTOM_SINGLE:
      setLEDs(settings.colours_custom[0]);
      break;
    case COMMAND_CUSTOM_MIXED:
      setLEDs(settings.colours_count, settings.colours_custom);
      break;
    case COMMAND_FLASH_ON:
      settings.flashing = true;
      break;
    case COMMAND_FLASH_OFF:
      settings.flashing = false;
      break;
    case COMMAND_CYCLE_ON:
      settings.cycling = true;
      break;
    case COMMAND_CYCLE_OFF:
      settings.cycling = false;
      break;
    case COMMAND_REMOVE_LED:
      removeLED();
      break;
    case COMMAND_ADD_LED:
      addLED();
      break;
    case COMMAND_SET_BRIGHTNESS:
      FastLED.setBrightness(settings.brightness);
      FastLED.show();
      break;
    case COMMAND_DARKEN:
      settings.brightness = settings.brightness >= 8 ? settings.brightness - 8 : 0;
      update(COMMAND_SET_BRIGHTNESS);
      break;
    case COMMAND_LIGHTEN:
      settings.brightness = settings.brightness < 247 ? settings.brightness + 8 : 255;
      update(COMMAND_SET_BRIGHTNESS);
      break;
    case COMMAND_SAVE:
      Serial.println("COMMAND_SAVE: Saving settings to EEPROM");
      saveSettings();
#ifdef USE_BLYNK
      //terminal.println("COMMAND_SAVE has not been implemented yet");
      Serial.println("Printing to Blynk");
      println_blynk("COMMAND_SAVE has not been implemented yet");
#endif	// USE_BLYNK
      break;
    case COMMAND_LOAD:
      Serial.println("COMMAND_LOAD has not been implemented yet");
      //Serial.println("COMMAND_LOAD: Load settings from EEPROM");
#ifdef USE_BLYNK
      Serial.println("Printing to Blynk");
      //terminal.println("COMMAND_LOAD has not been implemented yet");
      println_blynk("COMMAND_LOAD has not been implemented yet");
#endif	// USE_BLYNK
      break;
    case COMMAND_RAINBOW:
      //Serial.println("Setting a rainbow");
      settings.colours_count = settings.activeLEDs;
      Serial.print("Setting a rainbow, settings.colours_count=");
      Serial.println(settings.colours_count);
      set_rainbow(settings.colours_custom, settings.colours_count);
      setLEDs(settings.colours_count, settings.colours_custom);
      break;
    case COMMAND_CUSTOM_RAINBOW:
      Serial.println("Received custom rainbow command, but not currently implemented");
      break;
    case COMMAND_RESET:
      Serial.println("COMMAND_RESET ... resetting");
      ESP.reset();
      break;
    case COMMAND_RESTART:
      Serial.println("COMMAND_RESTART ... restarting");
      ESP.restart();
      break;
    case COMMAND_CLEAR_WIFI:
      Serial.println("COMMAND_CLEAR_WIFI ... clearing wifi settings");
      WiFiManager wiFiManager;
      wiFiManager.resetSettings();
      break;
  }
}

void loop_blynk();

void loop() {
  zzz_loop();
}
