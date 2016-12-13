void setupLEDs() {
  Serial.println("Setting up LEDs");
  // Setup LED strip
  FastLED.addLeds<NEOPIXEL, LED_DATA_PIN>(leds, NUM_LEDS);
}

void addLED() {
  leds[activeLEDs] = colours[activeLEDs % numColours];
  activeLEDs++;
  FastLED.show();
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

void removeLED() {
  if (activeLEDs > 0) {
    activeLEDs--;
    leds[activeLEDs] = CRGB::Black;
  }
  FastLED.show();
}

void setLEDs() {
  for (int i = 0; i < activeLEDs; i++) {
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
  static int index[2];
  index[0] = numberToFill / 3;
  index[1] = (numberToFill * 2) / 3;
  fill_gradient_RGB(colourArray, 0, CRGB::Red, index[0] - 1, CRGB::Green);
  fill_gradient_RGB(colourArray, index[0], CRGB::Green, index[1] - 1, CRGB::Blue);
  fill_gradient_RGB(colourArray, index[1], CRGB::Blue, settings.colours_count - 1, CRGB::Red);
}

