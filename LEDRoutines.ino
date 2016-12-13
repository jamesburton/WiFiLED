void setupLEDs() {
  Serial.println("Setting up LEDs");
  // Setup LED strip
  FastLED.addLeds<NEOPIXEL, LED_DATA_PIN>(leds, NUM_LEDS);
}

void addLED() {
  leds[settings.activeLEDs] = colours[settings.activeLEDs % numColours];
  settings.activeLEDs++;
  FastLED.show();
}

static int i;
CRGB backup_colours[NUM_LEDS];
void flash(int cycles, int delayLength, struct CRGB colour) {
  /*
  Serial.print("In flash(cycles=");
  Serial.print(cycles);
  Serial.print(", delayLength=");
  Serial.print(delayLength);
  Serial.print(", colour=(");
  Serial.print(colour.red);
  Serial.print(", ");
  Serial.print(colour.green);
  Serial.print(", ");
  Serial.print(colour.blue);
  Serial.println(")");
  
  Serial.print("NUM_LEDS=");
  Serial.println(NUM_LEDS);
  Serial.print("settings.colours_count=");
  Serial.println(settings.colours_count);
  delay(2000);
  */

  Serial.println("... Copying colours");
  int count = settings.colours_count;
  // Check for bad values and clean up (as this has been managed with loading settings across versions)
  if(count < 0 || count > NUM_LEDS) count = NUM_LEDS;
  
  //backup_colours = settings.colours_custom;
  for(i = 0; i < count; i++) 
    backup_colours[i] = settings.colours_custom[i];
  
  Serial.println("... Flashing");
  for (int i = 0; i < cycles; i++) {
    setLEDs(colour);
    delay(delayLength);
    setLEDs(CRGB::Black);
    delay(delayLength);
  }

  Serial.println("... Restoring colours");
  settings.colours_count = count;
  //settings.colours_custom = backup_colours;
  for(i = 0; i < settings.colours_count; i++) 
    settings.colours_custom[i] = backup_colours[i];
  setLEDs();
}
#define DEFAULT_FLASH_COUNT 1
#define DEFAULT_FLASH_DELAY 100
void flash(int cycles, int delayLength) 
  { flash(cycles, delayLength, CRGB::White); }
void flash() 
  { flash(DEFAULT_FLASH_COUNT, DEFAULT_FLASH_DELAY); }
void flash(struct CRGB colour) 
  { flash(DEFAULT_FLASH_COUNT, DEFAULT_FLASH_DELAY, colour); }

void removeLED() {
  if (settings.activeLEDs > 0) {
    settings.activeLEDs--;
    leds[settings.activeLEDs] = CRGB::Black;
  }
  FastLED.show();
}

void setLEDs() {
  for (int i = 0; i < settings.activeLEDs; i++) {
    leds[i] = colours[cycleIndex + i % numColours];
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
  //fill_solid(leds, activeLEDs, colour);
  //FastLED.show();
  // NB: Have switched to setting via array routines, to ensure persistence and brightness is used
  settings.colours_count = 1;
  settings.colours_custom[0] = colour;
  //setLEDs();
  setLEDs(settings.colours_count, settings.colours_custom);
}

void set_rainbow(CRGB colourArray[], int numberToFill) {
  Serial.println("In set_rainbow()...");
  if(numberToFill < 0) numberToFill = 0;
  if(numberToFill > NUM_LEDS) numberToFill = NUM_LEDS;
  static int index[2];
  index[0] = numberToFill / 3;
  index[1] = (numberToFill * 2) / 3;
  Serial.print("numberToFill=");
  Serial.print(numberToFill);
  Serial.print(", index[0]=");
  Serial.print(index[0]);
  Serial.print(", index[1]=");
  Serial.println(index[1]);
  if(index[0] > 0)
    fill_gradient_RGB(colourArray, 0, CRGB::Red, index[0] - 1, CRGB::Green);
  if(index[1] > index[0])
    fill_gradient_RGB(colourArray, index[0], CRGB::Green, index[1] - 1, CRGB::Blue);
  if(numberToFill > 0)
    fill_gradient_RGB(colourArray, index[1], CRGB::Blue, numberToFill - 1, CRGB::Red);
}

void setup_colours() {
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

}


