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
  EEPROM.put(SETTINGS_EEPROM_OFFSET, settings);
  EEPROM.commit();
  Serial.println("Settings saved to EEPROM");
}

