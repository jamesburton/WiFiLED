namespace WiFiConfig {
  void setup() {
    //WiFiManager (Local intialization - Once its business is done, there is no need to keep it around)
    WiFiManager wiFiManager;
    // Reset settings - for testing
    // wiFiManager.resetSettings();
  
    //fetches ssid and pass from eeprom and tries to connect
    //if it does not connect it starts an access point with the specified name
    //and goes into a blocking loop awaiting configuration
    wiFiManager.autoConnect(SOFT_AP_NAME);
    //or use this for auto generated name ESP + ChipID
    //wiFiManager.autoConnect();
  
    //if you get here you have connected to the WiFi
    Serial.print("Connected to wifi, please browse to http://");
    Serial.println(WiFi.localIP());
  
    flash(3, 100);
  }
}

