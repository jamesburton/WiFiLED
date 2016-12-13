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

char blynkAuthToken[] = BLYNK_AUTH_TOKEN;
WidgetTerminal terminal(TERMINAL_PIN);

void print_blynk(char *msg) { terminal.print(msg); }
void print_blynk(String msg) { terminal.print(msg); }
void print_blynk(int val) { terminal.print(val); }
void print_blynk(byte val) { terminal.print(val); }
void print_blynk(long val) { terminal.print(val); }
void print_blynk(char val) { terminal.print(val); }
void println_blynk(char *msg) { terminal.println(msg); }
void println_blynk(String msg) { terminal.println(msg); }
void println_blynk(int val) { terminal.println(val); }
void println_blynk(byte val) { terminal.println(val); }
void println_blynk(long val) { terminal.println(val); }
void println_blynk(char val) { terminal.println(val); }

void setup_blynk()
{
  Serial.println("Setting to Yellow to show it is connecting to Blynk");
  setLEDs(CRGB::Yellow);
  // Starting Blynk
  //Blynk.begin(blynkAuthToken, WiFi.SSID(),
  Blynk.config(blynkAuthToken);
  //Blynk.begin(blynkAuthToken, WiFi.SSID().c_str(), WiFi.psk().c_str());
  Serial.println("Blynk setup complete");
}

void loop_blynk()
{
#ifdef USE_BLYNK
  Blynk.run();

  if (Blynk.connected()) {
    if (!started) {
      // First connected set-up goes here
      Serial.println("First connection to Blynk");
      started = true;

      /*
      // Clear Blynk terminal (which only stores last 25 messages)
      for (int i = 0; i < 25; i++) {
        terminal.println();
      }

      // Send connection info to Blynk terminal
      //terminal.clear();  // NB: Currently, no such controls appears to exist
      terminal.println("Device connected, and can be accessed here:");
      terminal.print("http://");
      terminal.println(WiFi.localIP());
      terminal.flush();
      */
    }
  }
#endif  // USE_BLYNK
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
  Serial.println("BLYNK_DISCONNECTED");
  started = false;
}

BLYNK_WRITE(OFF_PIN)
{
  Serial.println("BLYNK_WRITE(OFF_PIN)");
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
  Serial.println("BLYNK_WRITE(TERMINAL_PIN)");
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
  Serial.println("BLYNK_WRITE(RGB_PIN)");
  int r = param[0].asInt();
  int g = param[1].asInt();
  int b = param[2].asInt();
  CRGB newColour = CRGB(r, g, b);
  setLEDs(newColour);
}

BLYNK_WRITE(MENU_PIN)
{
  Serial.println("BLYNK_WRITE(MENU_PIN)");
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
  Serial.println("BLYNK_WRITE(RESTART_PIN)");
  Serial.println("BLYNK: RESTART_PIN pressed");
  update(COMMAND_RESTART);
}

// NB: LED_PIN is an output pin
//BLYNK_WRITE(LED_PIN) { }

BLYNK_WRITE(BRIGHTNESS_PIN)
{
  Serial.println("BLYNK_WRITE(BRIGHTNESS_PIN)");
  int brightness = param.asInt();
  Serial.print("BLYNK: Brightness received: ");
  Serial.println(brightness);
  settings.brightness = brightness;
  update(COMMAND_SET_BRIGHTNESS);
}

BLYNK_WRITE(LIGHT_SENSOR_PIN)
{
  Serial.println("BLYNK_WRITE(LIGHT_SENSOR_PIN)");
  int lux = param.asInt();
  Serial.print("BLYNK: Light sensor value received: ");
  Serial.println(lux);
}
//*/

/*
#else   // Defined USE_BLYNK
void print_blynk(char *msg) { }
void print_blynk(String msg) { }
void print_blynk(int val) { }
void print_blynk(byte val) { }
void print_blynk(long val) { }
void print_blynk(char val) { }
void println_blynk(char *msg) { }
void println_blynk(String msg) { }
void println_blynk(int val) { }
void println_blynk(byte val) { }
void println_blynk(long val) { }
void println_blynk(char val) { }
*/
#endif  // USE_BLYNK

