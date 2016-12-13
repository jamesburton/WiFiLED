static bool eeprom_initialised = false;

void setup_EEPROM() {
  if(!eeprom_initialised) {
    eeprom_initialised = true;
    int eepromSize = SETTINGS_EEPROM_OFFSET + sizeof(Settings);
    EEPROM.begin(eepromSize);
  }
}

