void setup_EEPROM() {
  int eepromSize = SETTINGS_EEPROM_OFFSET + sizeof(Settings);
  EEPROM.begin(eepromSize);
  // Check for obvious junk data in settings location, to handle as "No Settings"
  byte firstByte;
  EEPROM.get(SETTINGS_EEPROM_OFFSET, firstByte);
  if (firstByte != 0x00 && firstByte != 0xFE) {
    // The most obvious junk was not found, so attempt to load as settings
    EEPROM.get(SETTINGS_EEPROM_OFFSET, settings);
    // A specific value is set to show if this is valid, so read this to verify
    // NB: If the structure changes, this SETTINGS_VALID constant should change to invalidate previous entries
    if (settings.valid == SETTINGS_VALID) {
      Serial.println("Loaded settings were valid");
      flash(1, 500, CRGB::Green);
    }
    else {
      Serial.println("Loaded settings were not valid");
      flash(3, 250, CRGB::Red);
    }
  }
  else {
    Serial.println("No settings loaded");
    flash(5, 200, CRGB::Orange);
  }
  if (settings.valid != SETTINGS_VALID) {
    setDefaultSettings();
  }
}

