// Version History
// v1.5 - Adding ArduinoOTA, to allow remote deployment to active devices
// v1.4 - Adding Blynk, for remote control app
// v1.3 - Added WebServer functionality for control
// v1.2 - Added WiFiManager for wifi setup
// v1.1 - Added FastLED and initial LED control
// v1.0 - Initial Project


// NB: For WifiManager info, see https://github.com/tzapu/WiFiManager
#define BLYNK_PRINT Serial
#define BLYNK_MAX_SENDBYTES 256	// NB: Default is 128
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#define DEFAULT_PASSWORD "12345"

#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

//#define FASTLED_ESP8266_RAW_PIN_ORDER
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
//#define FASTLED_ESP8266_D1_PIN_ORDER

#include <FastLED.h>

#include <ArduinoOTA.h>

#include <EEPROM.h>

//#define NUM_LEDS 30   // NB: Works, but flickers with low light for blue if all active
#define NUM_LEDS 32   // NB: Works, but flickers with low light for blue
//#define NUM_LEDS 32
//#define NUM_LEDS 16
//#define NUM_LEDS 15
//#define NUM_LEDS 12 // NB: Works, but flickers slightly for blue
//#define NUM_LEDS 10 // Still flickering on blue
//#define NUM_LEDS 6  // Flickering blue, but good enough
//#define NUM_LEDS 8  // Flickering blue, but good enough
//#define NUM_LEDS 6  // Solid colours
#define ACTIVE_LEDS NUM_LEDS
//#define ACTIVE_LEDS 24  // NB: Good solid output with mix!
//#define ACTIVE_LEDS 28  // NB: Slightly weak blue output!
//#define ACTIVE_LEDS 32  // NB: Pretty good output!
//#define ACTIVE_LEDS 48
//#define ACTIVE_LEDS 6
//#define ACTIVE_LEDS 16
#define NUM_COLOURS ACTIVE_LEDS // NB: This is now a maximum mix size, and a variable will set the active size
#define DATA_PIN 1

#define TERMINAL_PIN V1
#define RGB_PIN V2

#define SETTINGS_VALID 123
#define SETTINGS_INVALID 0xFE
#define SETTINGS_EEPROM_OFFSET 512
struct Settings {
  int valid;
  CRGB colours_custom[NUM_COLOURS];
  int colours_count;
  int activeLEDs;
  bool cycling;
  bool flashing;
  //char password;  // NB: Should consider encryption before adding this
} settings;

char blynkAuthToken[] = "ac2c04d72f3c4d89a2d1485ba422b6dd";
WidgetTerminal terminal(TERMINAL_PIN);

int activeLEDs = ACTIVE_LEDS;

int numColours = NUM_COLOURS;
//CRGB leds[NUM_LEDS];
CRGBArray<NUM_LEDS> leds;
CRGB colours[NUM_COLOURS];
CRGB colours_mixed[3];
CRGB colours_redgreen[1];
CRGB *remote_colour;

WiFiServer server(80);

/*
  enum LEDCommands {
	None,
	Off,
	On,
	Clear,
	Red,
	Green,
	Blue,
	Purple,
	RedGreen,
	Mixed,
	CustomSingle,
	CustomMixed,
	FlashOn,
	FlashOff,
	CycleOn,
	CycleOff
  };
*/
const int COMMAND_NONE = 0;
const int COMMAND_OFF = 1;
//const int COMMAND_ON = 2;
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

void setLEDs() {
  for (int i = 0; i < activeLEDs; i++) {
    leds[i] = colours[i % numColours];
  }
  FastLED.show();
}

void addLED() {
  leds[activeLEDs] = colours[activeLEDs % numColours];
  activeLEDs++;
  FastLED.show();
}

void removeLED() {
  if (activeLEDs > 0) {
    activeLEDs--;
    leds[activeLEDs] = CRGB::Black;
  }
  FastLED.show();
}

char buffer[100];
void setLEDs(int newColourCount, struct CRGB newColours[])
{
  for (int i = 0; i < newColourCount; i++) {
    sprintf(buffer, "newColours[%d]={%d,%d,%d}", i, newColours[i].r, newColours[i].g, newColours[i].b);
    Serial.println(buffer);
    colours[i] = newColours[i];
  }
  numColours = newColourCount;
  setLEDs();
}

void setLEDs(struct CRGB colour) {
  fill_solid(leds, activeLEDs, colour);
  FastLED.show();
}

void setup_ArduinoOTA()
{
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword((const char *)"123");

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed");
  });
  ArduinoOTA.begin();
}

String bsLink(String link) {
  return "<a class=\"btn btn-success\" href=\"" + link + "\">" + link + "</a>";
}

String homepage;

void buildHomepage() {
  homepage = "HTTP/1.1 200 OK";
  homepage += "\r\nContent-Type: text/html";
  homepage += "\r\n";
  homepage += "\r\n<!DOCTYPE HTML>";
  homepage += "\r\n<html>";
  homepage += "\r\n<head>";
  // homepage += "\r\n<link rel=\"icon\" href=\"data:;base64,=">"; // NB: Shortest version, but doesn't validate against HTML5, use this instead:-
  // homepage += "\r\n<link rel=\"icon\" href=\"data:;base64,iVBORw0KGgo=\">";
  homepage += "\r\n<link rel=\"icon\" href=\"data:,\">"; // NB: Apparently valid HTML5 and no IE <= 8 browser quirks!
  homepage += "\r\n<meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\">";
  homepage += "\r\n<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0-alpha.5/css/bootstrap.min.css\" integrity=\"sha384-AysaV+vQoT3kOAXZkl02PThvDr8HYKPZhNT5h/CXfBThSRXQ6jW5DO2ekP5ViFdi\" crossorigin=\"anonymous\">";
  homepage += "\r\n</head>";
  homepage += "\r\n<body>";
  homepage += "\r\n<div class=\"container\">";
  homepage += "\r\n<div class=\"jumbotron\">";
  homepage += "\r\n<div class=\"lead\">WiFi LED</div>";
  homepage += "\r\n<div>Try these links: <br/>";
  homepage += "\r\n" + bsLink("/red");
  homepage += "\r\n" + bsLink("/green");
  homepage += "\r\n" + bsLink("/blue");
  homepage += "\r\n" + bsLink("/black");
  homepage += "\r\n" + bsLink("/white");
  homepage += "\r\n" + bsLink("/purple");
  homepage += "\r\n" + bsLink("/mixed");
  homepage += "\r\n" + bsLink("/redgreen");
  homepage += "\r\n" + bsLink("/rainbow");
  homepage += "\r\n</div>";
  homepage += "\r\n<div>";
  homepage += "\r\n" + bsLink("/addled");
  homepage += "\r\n" + bsLink("/removeled");
  homepage += "\r\n" + bsLink("/restart");
  homepage += "\r\n" + bsLink("/reset");
  homepage += "\r\n" + bsLink("/clearwifi");
  homepage += "\r\n</div>";
  // Add Bootstrap JS
  homepage += "\r\n</div>";
  homepage += "\r\n<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.1.1/jquery.min.js\" integrity=\"sha384-3ceskX3iaEnIogmQchP8opvBy3Mi7Ce34nWjpBIwVTHfGYWQS9jwHDVRnpKKHJg7\" crossorigin=\"anonymous\"></script>";
  homepage += "\r\n<script src=\"https://cdnjs.cloudflare.com/ajax/libs/tether/1.3.7/js/tether.min.js\" integrity=\"sha384-XTs3FgkjiBgo8qjEjBk0tGmf3wPrWtA6coPfQDfFEY8AnYJwjalXCiosYRBIBZX8\" crossorigin=\"anonymous\"></script>";
  homepage += "\r\n<script src=\"https://maxcdn.bootstrapcdn.com/bootstrap/4.0.0-alpha.5/js/bootstrap.min.js\" integrity=\"sha384-BLiI7JTZm+JWlgKa0M0kGRpJbF2J8q+qreVrKBC47e3K6BW78kGLrCkeRX6I9RoK\" crossorigin=\"anonymous\"></script>";
  // End of Bootstrap JS
  homepage += "\r\n</body>";
  homepage += "\r\n</html>";
}

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

void setup() {
  Serial.begin(115200);

  Serial.println("\r\n\r\n**********************************\r\n\r\n");

  // Setup LED strip
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  // Setup EEPROM access
  /*
    EEPROM.begin(4096);
    byte data[4096];
    EEPROM.get(0,data);
    Serial.println("EEPROM Data:");
    //Serial.println(data);
    for(int i = 0; i < 4096; i++) {
    Serial.print(data[i]);
    }
    Serial.println();
    char hex[3];
    for(int i = 0; i < 4096; i++) {
    sprintf(hex, "%02X ", data[i]);
    Serial.print(hex);
    }
    Serial.println();
  */
  int flashChipSize = ESP.getFlashChipSize();
  Serial.print("flashChipSize=");
  Serial.println(flashChipSize);
  int eepromSize = SETTINGS_EEPROM_OFFSET + sizeof(Settings);
  EEPROM.begin(eepromSize);
  byte firstByte;
  EEPROM.get(SETTINGS_EEPROM_OFFSET, firstByte);
  if (firstByte != 0x00 && firstByte != 0xFE) {
    EEPROM.get(SETTINGS_EEPROM_OFFSET, settings);
    if (settings.valid == SETTINGS_VALID) {
      Serial.println("Loaded settings were valid");
      flash(1, 500, CRGB::Green);
    } else {
      Serial.println("Loaded settings were not valid");
      flash(3, 250, CRGB::Red);
    }
  } else {
    Serial.println("No settings loaded");
    flash(5, 200, CRGB::Orange);
  }
  if (settings.valid != SETTINGS_VALID) {
    // Set default values
    Serial.println("Setting default values");
    settings.valid = SETTINGS_VALID;
    settings.colours_count = NUM_LEDS;
    settings.activeLEDs = NUM_LEDS;
    settings.cycling = false;
    settings.flashing = false;
    //settings.password = ...;  // NB: Should consider encryption before adding this
    fill_rainbow(settings.colours_custom, settings.colours_count, CRGB::Red, CRGB::Blue);
    // NB: This should be switched to a Save routine
    EEPROM.put(SETTINGS_EEPROM_OFFSET, settings);
    EEPROM.commit();
    Serial.println("Settings saved to EEPROM");
  }

  //WiFiManager (Local intialization - Once its business is done, there is no need to keep it around)
  WiFiManager wiFiManager;
  // Reset settings - for testing
  // wiFiManager.resetSettings();

  //fetches ssid and pass from eeprom and tries to connect
  //if it does not connect it starts an access point with the specified name
  //and goes into a blocking loop awaiting configuration
  wiFiManager.autoConnect("WiFiLEDs");
  //or use this for auto generated name ESP + ChipID
  //wiFiManager.autoConnect();

  //if you get here you have connected to the WiFi
  Serial.print("Connected to wifi, please browse to http://");
  Serial.println(WiFi.localIP());

  flash(3, 100);

  // Set up colour ranges
  colours_mixed[0] = CRGB::Blue;
  //colours_mixed[0] = CRGB::DeepSkyBlue;
  colours_mixed[1] = CRGB::Green;
  colours_mixed[2] = CRGB::Red;
  //colours_blue[0] = CRGB::Blue;
  //colours_red[0] = CRGB::Red;
  //colours_green[0] = CRGB::Green;
  //colours_white[0] = CRGB::White;
  //colours_black[0] = CRGB::Black;
  //colours_purple[0] = CRGB::Purple;
  //colours_purple[0] = CRGB::BlueViolet;
  colours_redgreen[0] = CRGB::Red;
  colours_redgreen[1] = CRGB::Green;

  Serial.println("Setting to LightYellow to show it is live");
  setLEDs(CRGB::LightYellow);

  setup_ArduinoOTA();

  buildHomepage();

  server.begin();
  Serial.print("HTTP server started - http://");
  Serial.println(WiFi.localIP());

  // Starting Blynk
  //Blynk.begin(blynkAuthToken, WiFi.SSID(),
  Blynk.config(blynkAuthToken);
  //Blynk.begin(blynkAuthToken, WiFi.SSID().c_str(), WiFi.psk().c_str());
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

const int STATE_OFF = 0;
const int STATE_ON = 1;
bool started = false;
int state = STATE_OFF;
int customLength = 1;
CRGB colours_custom[NUM_COLOURS];
bool cycling = false;
bool flashing = false;

void update(int command) {
  Serial.print("In update: command=");
  Serial.println(command);
  switch (command) {
    case COMMAND_NONE:
      break;
    case COMMAND_OFF:
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
    case COMMAND_MIXED:
      setLEDs(3, colours_mixed);
      break;
    case COMMAND_CUSTOM_SINGLE:
      setLEDs(colours_custom[0]);
      break;
    case COMMAND_CUSTOM_MIXED:
      setLEDs(customLength, colours_custom);
      break;
    case COMMAND_FLASH_ON:
      flashing = true;
      break;
    case COMMAND_FLASH_OFF:
      flashing = false;
      break;
    case COMMAND_CYCLE_ON:
      cycling = true;
      break;
    case COMMAND_CYCLE_OFF:
      cycling = false;
      break;
    case COMMAND_REMOVE_LED:
      removeLED();
      break;
    case COMMAND_ADD_LED:
      addLED();
      break;
    case COMMAND_SAVE:
      Serial.println("COMMAND_SAVE has not been implemented yet");
      terminal.println("COMMAND_SAVE has not been implemented yet");
      break;
    case COMMAND_LOAD:
      Serial.println("COMMAND_LOAD has not been implemented yet");
      terminal.println("COMMAND_LOAD has not been implemented yet");
      break;
    case COMMAND_RAINBOW:
      //Serial.println("COMMAND_RAINBOW has not been implemented yet");
      Serial.println("Setting a rainbow");
      //fill_rainbow(settings.colours_custom, settings.colours_count, CRGB::Red, CRGB::Blue);
      /*
      fill_rainbow(settings.colours_custom, settings.colours_count / 2, CRGB::Red, CRGB::Cyan);
      fill_rainbow(settings.colours_custom[startIndex], settings.colours_count - startIndex, CRGB::Cyan, CRGB::Red);
      //*/
      //static int midIndex = settings.colours_count / 2;
      //fill_gradient_RGB(settings.colours_custom, 0, CRGB::Red, midIndex - 1, CRGB::Cyan);
      //fill_gradient_RGB(settings.colours_custom, midIndex, CRGB::Cyan, settings.colours_count - 1, CRGB::Red);
      static int index[2];
      index[0] = settings.colours_count / 3;
      index[1] = (settings.colours_count * 2) / 3;
      fill_gradient_RGB(settings.colours_custom, 0, CRGB::Red, index[0]-1, CRGB::Green);
      fill_gradient_RGB(settings.colours_custom, index[0], CRGB::Green, index[1]-1, CRGB::Blue);
      fill_gradient_RGB(settings.colours_custom, index[1], CRGB::Blue, settings.colours_count-1, CRGB::Red);
      setLEDs(settings.colours_count, settings.colours_custom);
      break;
    case COMMAND_CUSTOM_RAINBOW:
      Serial.println("Received custom rainbow command, but not currently implemented");
      break;
  }
}

void handleWiFiClient()
{
  WiFiClient client = server.available();

  bool available = false;
  if (client) {
    Serial.print("New client: ");
    Serial.println(client.remoteIP());

    // Wait until client is ready
    const int clientDelay = 10;
    int remainingWaits = 100;
    while (!client.available() && remainingWaits-- > 0) {
      delay(clientDelay);
    }
    if (client.available()) {
      Serial.println("client available, continuing");
      available = true;
    } else {
      client.stop();
    }
  }
  if (available)
  {
    // Read the first line of the request
    String request = client.readStringUntil('\r');
    String url;  // null represents no match
    bool doNotReturnHomepage = false;
    Serial.println(request);
    client.flush();

    // NB: Request should be of this following format
    // GET /somepath HTTP/1.1
    if (request.startsWith("GET ")) {
      url = request.substring(4);
      int spacePos = url.indexOf(' ');
      url = url.substring(0, spacePos);
      Serial.print("Handling GET request: ");
      Serial.println(url);
    } else if (request.startsWith("POST ")) {
      Serial.print("Unhandled POST request: ");
      Serial.println(request.substring(5));
    }

    if (url) {
      // Match the request
      if (url == "/") {
        Serial.println("Returning homepage only");
      } else if (url == "/black") {
        Serial.println("Setting to black");
        //setLEDs(1, colours_black);
        setLEDs(CRGB::Black);
      } else if (url == "/white") {
        Serial.println("Setting to white");
        //setLEDs(1, colours_white);
        setLEDs(CRGB::White);
      } else if (url == "/blue") {
        Serial.println("Setting to blue");
        //setLEDs(1, colours_blue);
        setLEDs(CRGB::Blue);
      } else if (url == "/red") {
        Serial.println("Setting to red");
        //setLEDs(CRGB::Red);
        //update(LEDCommands::Red);
        update(COMMAND_RED);
      } else if (url == "/green") {
        Serial.println("Setting to green");
        //setLEDs(1, colours_green);
        setLEDs(CRGB::Green);
      } else if (url == "/redgreen") {
        Serial.println("Setting to red-green");
        setLEDs(2, colours_redgreen);
      } else if (url == "/mixed") {
        Serial.println("Setting to mixed");
        setLEDs(3, colours_mixed);
      } else if (url == "/purple") {
        Serial.println("Setting to purple");
        //setLEDs(1, colours_purple);
        setLEDs(CRGB::Purple);
        //setLEDs(CRGB::MediumSlateBlue);
      } else if (url == "/rainbow") {
        Serial.println("Setting to rainbox");
        update(COMMAND_RAINBOW);
      } else if (url == "/favicon.ico") {
        Serial.println("ignored request for favicon.ico");
        doNotReturnHomepage = true;
      } else if (url == "/addled") {
        Serial.println("Adding LED");
        addLED();
      } else if (url == "/removeled") {
        Serial.println("Removing LED");
        removeLED();
      } else if (url == "/clearwifi") {
        Serial.println("Clearing WiFi");
        WiFiManager wiFiManager;
        wiFiManager.resetSettings();
      } else if (url == "/restart") {
        Serial.println("Restarting (soft-reboot)");
        ESP.restart();
      } else if (url == "/reset") {
        Serial.println("Resetting (hard-reboot)");
        ESP.reset();
      } else {
        Serial.print("Unhandled path: ");
        Serial.println(url);
        //client.stop();
      }
    }
    if (!doNotReturnHomepage) {
      returnHomepage(client);
    }
    client.flush();
    client.stop();
  }
}

void loop() {
  ArduinoOTA.handle();
  Blynk.run();

  if (Blynk.connected()) {
    if (!started) {
      // First connected set-up goes here
      Serial.println("First connection to Blynk");
      started = true;

      // Clear Blynk terminal (which only stores last 25 messages)
      for (int i = 0; i < 25; i++) {
        terminal.println();
      }

      // Send connection info to Blynk terminal
      //terminal.clear();	// NB: Currently, no such controls appears to exist
      terminal.println("Device connected, and can be accessed here:");
      terminal.print("http://");
      terminal.println(WiFi.localIP());
      terminal.flush();
    }
  }

  handleWiFiClient();
}

/*
  BLYNK_CONNECTED()
  {
	// Refetch/set all Blynk pin values
	Blynk.syncAll();
  }
  //*/

BLYNK_DISCONNECTED()
{
  started = false;
}

BLYNK_WRITE(0)
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

