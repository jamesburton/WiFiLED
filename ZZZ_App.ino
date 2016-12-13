#include <arduino.h>

void zzz_loop()
{
  //Serial.println("zzz_loop");
  
  #ifdef USE_ARDUINO_OTA
    ArduinoOTA.handle();
  #endif  // USE_ARDUINO_OTA

  AppWebServer::handle();
}

void zzz_setup()
{
  Serial.println("zzz_setup");

  setupLEDs();

  WiFiConfig::setup();
  AppWebServer::setupWebServer();
}

