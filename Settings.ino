void setDefaultSettings() {
  // Set default values
  Serial.println("Setting default values");
  settings.valid = SETTINGS_VALID;
  settings.colours_count = NUM_LEDS;
  settings.activeLEDs = NUM_LEDS;
  settings.cycling = false;
  settings.flashing = false;
  //settings.password = ...;  // NB: Should consider encryption before adding this
  settings.autoSave = true;
  settings.brightness = 255;
  set_rainbow(settings.colours_custom, settings.colours_count);
}

void saveSettings() {
  // Ensure that EEPROM is initialised
  setup_EEPROM();

  EEPROM.put(SETTINGS_EEPROM_OFFSET, settings);
  EEPROM.commit();
  Serial.println("Settings saved to EEPROM");
}

void loadSettings() {
  // Ensure that EEPROM is initialised
  setup_EEPROM();

  flash(CRGB::Blue);  // Flash Blue to showing start of loading settings
  // Check for obvious junk data in settings location, to handle as "No Settings"
  byte firstByte;
  EEPROM.get(SETTINGS_EEPROM_OFFSET, firstByte);
  if (firstByte != 0x00 && firstByte != 0xFE) {
    // The most obvious junk was not found, so attempt to load as settings
    EEPROM.get(SETTINGS_EEPROM_OFFSET, settings);
    // A specific value is set to show if this is valid, so read this to verify
    // NB: If the structure changes, this SETTINGS_VALID constant should change to invalidate previous entries
    //if (settings.valid == SETTINGS_VALID) {
    if (settings.valid == SETTINGS_VALID && settings.colours_count >= 0 && settings.colours_count <= NUM_LEDS && settings.activeLEDs >= 0 && settings.activeLEDs <= NUM_LEDS) {
      Serial.println("Loaded settings were valid");
      delay(2000);
      flash(1, 500, CRGB::Green); // Flash Green to show loaded settings were valid
      Serial.println("Finished flashing");
      delay(10000);
    }
    else {
      Serial.println("Loaded settings were not valid");
      flash(3, 250, CRGB::Red); // Flash Redx3 to show loaded settings were not valid
    }
  }
  else {
    Serial.println("No settings loaded");
    flash(5, 200, CRGB::Orange);  // Flash Orange if no settings were loaded
  }
  if (settings.valid != SETTINGS_VALID) {
    flash(CRGB::Crimson);
    setDefaultSettings();
  }
  Serial.println("End of loading settings");
}

