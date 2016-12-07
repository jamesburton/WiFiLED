#include <ESP8266WiFi.h>
#ifdef USE_BLYNK
#define BLYNK_PRINT Serial
#define BLYNK_MAX_SENDBYTES 256  // NB: Default is 128
#include <BlynkSimpleEsp8266.h>

#define BLYNK_AUTH_TOKEN "ac2c04d72f3c4d89a2d1485ba422b6dd"
#define OFF_PIN V0
#define TERMINAL_PIN V1
#define RGB_PIN V2
#define MENU_PIN V3
#define RESTART_PIN V4
#define LED_PIN V5
#define BRIGHTNESS_PIN V6
#define LIGHT_SENSOR_PIN V7
#else  // If not USE_BLYNK
#endif // USE_BLYNK


