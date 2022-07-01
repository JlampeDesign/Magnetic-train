#include <Arduino.h>
// --------- esp now library ----------
#include <esp_now.h>
#include <WiFi.h>
// --------- RGB library --------------
#include <FastLED.h>
#include <OneButton.h>
// --------- RGB define ----------------
#define LED_TYPE          WS2812
#define COLOR_ORDER       GRB
#define DATA_PIN          26
#define NUM_LEDS          144
#define BRIGHTNESS        10
#define FRAMES_PER_SECOND 60
#define COOLING  55
#define SPARKING 120

CRGB leds[NUM_LEDS];
CRGBPalette16 gPal;
bool gReverseDirection =  false;
// --------- patern enable -------------
bool patternStart = true;
int patternColor;
// --------- espNow structure ----------
typedef struct struct_message {int y;} 
struct_message;
struct_message message;
// --------- void callback -------------
void Turned_Off();
void Fire2012WithPalette();
void RedPattern2();
void RedPattern3();
// -------- received position ----------
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  memcpy(&message, incomingData, sizeof(message));
  Serial.print("received pattern: ");
  Serial.println(message.y);

  switch(message.y){
    case 1: patternColor = 1;   patternStart = true;      break;
    case 2: patternColor = 2;                             break;
    case 3: patternColor = 3;                             break;
    case 0: patternStart = false;                         break;
  }
}
// -------- setup ------------
void setup() {
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  gPal = HeatColors_p;

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  esp_now_register_recv_cb(OnDataRecv);

  
}
// -------- loop ------------
void loop() {

  if (patternStart == true) {
    switch(patternColor){
      case 1: Fire2012WithPalette(); Serial.println("p1"); break;
      case 2: RedPattern2(); Serial.println("p2"); break;
      case 3: RedPattern3(); Serial.println("p3"); break;
    }
  } else {
    Turned_Off();
  }
  FastLED.show();
  random16_add_entropy( random(3));
}

// -------- led patterns --------
void Turned_Off(){
  fadeToBlackBy(leds, NUM_LEDS, 10);
}

void Fire2012WithPalette()
{
  static uint8_t heat[NUM_LEDS];

    for( int i = 0; i < NUM_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_LEDS) + 2));
    }

    for( int k= NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    for( int j = 0; j < NUM_LEDS; j++) {

      uint8_t colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( gPal, colorindex);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
    }
}

void RedPattern2() {

  uint16_t SinBeat = beatsin16 (25, 0, 96, 0, 0);

  leds[SinBeat] = CRGB::Red;

  fadeToBlackBy(leds, NUM_LEDS, 10);
}

void RedPattern3() {

  uint16_t SinBeat = beatsin16 (25, 0, NUM_LEDS - 1 , 0, 0);

  leds[SinBeat] = CRGB::Red;

  fadeToBlackBy(leds, NUM_LEDS, 10);

}