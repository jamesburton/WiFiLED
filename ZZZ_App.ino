#include <arduino.h>

void zzz_loop()
{
  //Serial.println("zzz_loop");
  
#ifdef USE_ARDUINO_OTA
  ArduinoOTA.handle();
#endif  // USE_ARDUINO_OTA

#ifdef USE_BLYNK
  //loop_blynk();
#endif  // USE_BLYNK

#ifdef USE_MQTT
#endif  // USE_MQTT

  AppWebServer::handle();
}

void zzz_setup()
{
  Serial.println("zzz_setup");

  setupLEDs();  
  setup_colours();
  
  Serial.println("Setting to Coral to show it is loading");
  setLEDs(CRGB::Coral);
  loadSettings();

  Serial.println("Setting to Orange to show it is connecting to WiFi ");
  setLEDs(CRGB::Orange);
  WiFiConfig::setup();  // This presents a WiFi AP to configure wireless network access, if no stored details are found

  Serial.println("Setting to Orange to show it is loading");
  setLEDs(CRGB::Orange);
  AppWebServer::setupWebServer(); // This sets up the web server to provide control via HTTP

#ifdef USE_ARDUINO_OTA
  Serial.println("Setting to Purple to show it is setting up Arduino OTA");
  setLEDs(CRGB::Purple);
  setup_ArduinoOTA();
#endif  // USE_ARDUINO_OTA

#ifdef USE_BLYNK
  setup_blynk();
#endif // USE_BLYNK
  
  Serial.println("Setting to Blue to show it is live");
  setLEDs(CRGB::Blue);
  Serial.println(" *** Setup Complete *** ");
}

