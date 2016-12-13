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
#define USE_BLYNK
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

//#include "Constants.cpp"
//#include "Constants.h"
#ifdef USE_BLYNK
const int BLYNK_MENU_OFF = 1;
const int BLYNK_MENU_WHITE = 2;
const int BLYNK_MENU_RAINBOW = 3;
const int BLYNK_MENU_RED = 4;
const int BLYNK_MENU_GREEN = 5;
const int BLYNK_MENU_BLUE = 6;
const int BLYNK_MENU_MIXED = 7;
const int BLYNK_MENU_RED_GREEN = 8;
const int BLYNK_MENU_RANDOM_RAINBOW = 9;
const int BLYNK_MENU_RANDOM_RGB = 10;
const int BLYNK_MENU_SAVE = 11;
const int BLYNK_MENU_LOAD = 12;
const int BLYNK_MENU_RESTART = 13;
const int BLYNK_MENU_RESET = 14;
const int BLYNK_MENU_CLEAR_WIFI = 15;
//const int BLYNK_MENU_ = ;
#endif  // #ifdef USE_BLYNK

//#include "Settings.h"
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

//void setLEDs();

/////////////////////////////////////
// Globals
String homepage;
bool started = false;

///////////////////////////////////

#ifdef USE_BLYNK
char blynkAuthToken[] = BLYNK_AUTH_TOKEN;
WidgetTerminal terminal(TERMINAL_PIN);
#endif	// USE_BLYNK

//struct Settings settings;

int activeLEDs = ACTIVE_LEDS;

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

//String homepage;

Settings settings;

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
/*
void flash(int cycles, int delayLength, struct CRGB colour) {
  for (int i = 0; i < cycles; i++) {
    setLEDs(colour);
    delay(delayLength);
    setLEDs(CRGB::Black);
    delay(delayLength);
  }
}
void flash(int cycles, int delayLength) {
  flash(cycles, delayLength, CRGB::White);
}
void flash() {
  flash(1, 100);
}

void set_rainbow(CRGB *colourArray, int numberToFill) {
  static int index[2];
  index[0] = numberToFill / 3;
  index[1] = (numberToFill * 2) / 3;
  fill_gradient_RGB(colourArray, 0, CRGB::Red, index[0] - 1, CRGB::Green);
  fill_gradient_RGB(colourArray, index[0], CRGB::Green, index[1] - 1, CRGB::Blue);
  fill_gradient_RGB(colourArray, index[1], CRGB::Blue, settings.colours_count - 1, CRGB::Red);
}
*/
void setDefaultSettings();  // Moved to Settings.ino
void saveSettings();  // Moved to Settings.ino

void setup_EEPROM();  // Moved to EEPROM.ino

void setup() {
  Serial.begin(115200);
  
  Serial.println("\r\n\r\n**********************************\r\n\r\n");
  zzz_setup();
  Serial.println("\r\n\r\n**********************************\r\n\r\n");
  
  // Set up colour ranges
  colours_mixed[0] = CRGB::Blue;
  colours_mixed[1] = CRGB::Green;
  colours_mixed[2] = CRGB::Red;
  colours_redgreen[0] = CRGB::Red;
  colours_redgreen[1] = CRGB::Green;
  colours_rrrggg[0] = colours_rrrggg[1] = colours_rrrggg[2] = CRGB::Red;
  colours_rrrggg[3] = colours_rrrggg[4] = colours_rrrggg[5] = CRGB::Green;
  colours_rrrgggbbb[0] = colours_rrrgggbbb[1] = colours_rrrgggbbb[2] = CRGB::Red;
  colours_rrrgggbbb[3] = colours_rrrgggbbb[4] = colours_rrrgggbbb[5] = CRGB::Green;
  colours_rrrgggbbb[6] = colours_rrrgggbbb[7] = colours_rrrgggbbb[8] = CRGB::Blue;

  Serial.println("Setting to LightYellow to show it is live");
  setLEDs(CRGB::LightYellow);

#ifdef USE_ARDUINO_OTA
  setup_ArduinoOTA();
#endif	// USE_ARDUINO_OTA

#ifdef USE_BLYNK
  // Starting Blynk
  //Blynk.begin(blynkAuthToken, WiFi.SSID(),
  Blynk.config(blynkAuthToken);
  //Blynk.begin(blynkAuthToken, WiFi.SSID().c_str(), WiFi.psk().c_str());
#endif // USE_BLYNK
}

void returnJSON(WiFiClient client, String json) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("");
  client.println(json);
  client.flush();
  delay(1);
}

void returnHomepage(WiFiClient client) {
  client.println(homepage);
  client.flush();
  delay(1);
}

//int myVal = testVal;

void update(int command) {
  Serial.print("In update: command=");
  Serial.println(command);
  switch (command) {
    case COMMAND_NONE:
      break;
    case COMMAND_OFF:
      //setLEDs(CRGB::Black);
      settings.brightness = 0;
      setLEDs();
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
      break;
    case COMMAND_DARKEN:
      settings.brightness = settings.brightness >= 8 ? settings.brightness - 8 : 0;
      update(COMMAND_SET_BRIGHTNESS);
    case COMMAND_LIGHTEN:
      settings.brightness = settings.brightness < 247 ? settings.brightness + 8 : 255;
      update(COMMAND_SET_BRIGHTNESS);
    case COMMAND_SAVE:
      Serial.println("COMMAND_SAVE: Saving settings to EEPROM");
      saveSettings();
#ifdef USE_BLYNK
      terminal.println("COMMAND_SAVE has not been implemented yet");
#endif	// USE_BLYNK
      break;
    case COMMAND_LOAD:
      //Serial.println("COMMAND_LOAD has not been implemented yet");
      Serial.println("COMMAND_LOAD: Load settings from EEPROM");
#ifdef USE_BLYNK
      terminal.println("COMMAND_LOAD has not been implemented yet");
#endif	// USE_BLYNK
      break;
    case COMMAND_RAINBOW:
      Serial.println("Setting a rainbow");
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

#ifdef USE_BLYNK
  loop_blynk();
#endif	// USE_BLYNK

#ifdef USE_MQTT
#endif	// USE_MQTT

  //handleWiFiClient(); // NB: Now in zzz_loop, and then calling AppWebServer.handle in WebServer.ino
}


/* NB: Moved to Blynk.ino
#ifdef USE_BLYNK
/*
BLYNK_CONNECTED()
{
	// Refetch/set all Blynk pin values
	Blynk.syncAll();
}
//* /

BLYNK_DISCONNECTED()
{
  started = false;
}

BLYNK_WRITE(OFF_PIN)
{
  Serial.print("Virtual Button 0 pressed");
  int value = param.asInt();
  bool bValue = value == 1;
  Serial.print("... value = ");
  Serial.println(bValue);
  if (!bValue) {
    update(COMMAND_OFF);
  }
}
BLYNK_WRITE(TERMINAL_PIN)
{
  String value = param.asStr();
  // NB: Can compared with String("Some Text") == param.asStr()
  // OR: Can us a buffer to read and output, like this:
  // terminal.write(param.getBuffer(), param.getLength());
  terminal.print("RCV: ");
  terminal.println(value);

  terminal.flush();
}

BLYNK_WRITE(RGB_PIN)
{
  int r = param[0].asInt();
  int g = param[1].asInt();
  int b = param[2].asInt();
  CRGB newColour = CRGB(r, g, b);
  setLEDs(newColour);
}

BLYNK_WRITE(MENU_PIN)
{
  switch (param.asInt()) {
    case BLYNK_MENU_OFF:
      update(COMMAND_OFF);
      break;
    case BLYNK_MENU_WHITE:
      update(COMMAND_WHITE);
      break;
    case BLYNK_MENU_RAINBOW:
      update(COMMAND_RAINBOW);
      break;
    case BLYNK_MENU_RED:
      update(COMMAND_RED);
      break;
    case BLYNK_MENU_GREEN:
      update(COMMAND_GREEN);
      break;
    case BLYNK_MENU_BLUE:
      update(COMMAND_BLUE);
      break;
    case BLYNK_MENU_MIXED:
      update(COMMAND_MIXED);
      break;
    case BLYNK_MENU_RED_GREEN:
      update(COMMAND_REDGREEN);
      break;
    case BLYNK_MENU_RANDOM_RAINBOW:
      //update(COMMAND_RANDOM_RAINBOW);
      Serial.println("BLYNK_MENU_RANDOM_RAINBOW received, but currently not implemented");
      break;
    case BLYNK_MENU_RANDOM_RGB:
      //update(COMMAND_RANDOM_RAINBOW);
      Serial.println("BLYNK_MENU_RANDOM_RGB received, but currently not implemented");
      break;
    case BLYNK_MENU_SAVE:
      update(COMMAND_SAVE);
      break;
    case BLYNK_MENU_LOAD:
      update(COMMAND_LOAD);
      break;
    case BLYNK_MENU_RESTART:
      update(COMMAND_RESTART);
      break;
    case BLYNK_MENU_RESET:
      update(COMMAND_RESET);
      break;
    case BLYNK_MENU_CLEAR_WIFI:
      update(COMMAND_CLEAR_WIFI);
      break;
    default:
      Serial.print("Unhandled Blynk menu item: ");
      Serial.println(param.asInt());
      break;
  }
}

BLYNK_WRITE(RESTART_PIN)
{
  Serial.println("BLYNK: RESTART_PIN pressed");
  update(COMMAND_RESTART);
}

// NB: LED_PIN is an output pin
//BLYNK_WRITE(LED_PIN) { }

BLYNK_WRITE(BRIGHTNESS_PIN)
{
  int brightness = param.asInt();
  Serial.print("BLYNK: Brightness received: ");
  Serial.println(brightness);
  settings.brightness = brightness;
  update(COMMAND_SET_BRIGHTNESS);
}

BLYNK_WRITE(LIGHT_SENSOR_PIN)
{
  int lux = param.asInt();
  Serial.print("BLYNK: Light sensor value received: ");
  Serial.println(lux);
}
#endif	// USE_BLYNK
*/

