#define USE_BLYNK
#define USE_MMQT
#define USE_ARDUINO_OTA

#define NUM_LEDS 30
#define LED_DATA_PIN 1

// Override, to allow playing with part of a larger strip ... otherwise, simply match the NUM_LEDS setting to your strip instead
#define ACTIVE_LEDS NUM_LEDS
// Override, to set an initial reduced active LED count (default should be all)
#define NUM_COLOURS ACTIVE_LEDS

#ifdef USE_BLYNK
//#define BLYNK_AUTH_TOKEN ""
#define BLYNK_AUTH_TOKEN "ac2c04d72f3c4d89a2d1485ba422b6dd"
#endif  // USE_BLYNK

//#define FASTLED_ESP8266_RAW_PIN_ORDER
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
//#define FASTLED_ESP8266_D1_PIN_ORDER


