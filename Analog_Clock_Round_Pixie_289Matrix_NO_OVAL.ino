#include <FastLED.h>
#include <WiFi.h>
#include "time.h"
#include <math.h>

#define DATA_PIN   6
#define NUM_LEDS   289
#define BRIGHTNESS 120

// Wi-Fi / NTP
const char* ssid       = "NOS-EA84-Guest";
const char* password   = "43211234";
const char* ntpServer  = "europe.pool.ntp.org";
const long  gmtOffset_sec     = 0;
const int   daylightOffset_sec = 0;
const uint32_t NTP_TIMEOUT_MS = 15000;

// Matrix mapping (17×21)
#define NUM_ROWS 21
const uint8_t rowCount[NUM_ROWS]  = {5,9,11,13,15,15,17,17,17,17,17,17,17,17,17,15,15,13,11,9,5};
const uint8_t rowOffset[NUM_ROWS] = {6,4,3,2,1,1,0,0,0,0,0,0,0,0,0,1,1,2,3,4,6};
const uint16_t prefix[NUM_ROWS] PROGMEM = {
  0,5,14,25,38,53,68,85,102,119,136,153,170,187,204,221,236,251,264,275,284
};
#define INVALID 0xFFFF

CRGB leds[NUM_LEDS];

uint16_t XY(uint8_t x, uint8_t y) {
  if (y >= NUM_ROWS || x < rowOffset[y] || x >= rowOffset[y] + rowCount[y]) return INVALID;
  return pgm_read_word_near(prefix + y) + (x - rowOffset[y]);
}

const int cx  = 8;
const int cy  = 10;
const int maxR = 10;

// ---------- SECOND HAND ----------
void drawSecondHandle(float angle, int endRadius, const CRGB &color, float t) {
  float dist;
  if (t < 0.5f) dist = t * 2.0f * endRadius;
  else          dist = (1.0f - (t - 0.5f) * 2.0f) * endRadius;

  float ovalX = 17.0f / 21.0f;
  int x = round(cx + cos(angle) * dist * ovalX);
  int y = round(cy + sin(angle) * dist);

  uint16_t idx = XY(x, y);
  if (idx != INVALID) leds[idx] = color;

  if (t < 0.5f) {
    for (float f = 0.4f; f < 1.0f; f += 0.2f) {
      int xt = round(cx + cos(angle) * dist * f * ovalX);
      int yt = round(cy + sin(angle) * dist * f);
      uint16_t idt = XY(xt, yt);
      if (idt != INVALID) leds[idt] = color / 10;
    }
  }
}

// ---------- PULSE HAND ----------
void drawPulseHandle(float angle, int endRadius, const CRGB &color, float pulseFactor) {
  float ovalX = 17.0f / 21.0f;

  int x = round(cx + cos(angle) * endRadius * ovalX);
  int y = round(cy + sin(angle) * endRadius);

  uint16_t idx = XY(x, y);
  if (idx != INVALID) {
    CRGB c = color;
    uint8_t scale = 40 + 215 * pulseFactor;
    c.nscale8_video(scale);
    leds[idx] = c;
  }
}

// ---------- 12 HOUR DOTS (DIMMED FILAMENT YELLOW) ----------
void drawHourPoints() {

  CRGB filamentYellow = CRGB(153, 255, 0);   // filament style 10503
  filamentYellow.nscale8_video(4);           // dim so they do not dominate

  for (int i = 0; i < 12; i++) {
    float angle = i * (TWO_PI / 12.0) - HALF_PI;
    float r = maxR;

    float ovalX = 17.0f / 21.0f;
    int x = round(cx + cos(angle) * r * ovalX);
    int y = round(cy + sin(angle) * r);

    uint16_t idx = XY(x, y);

    if (idx != INVALID) {
      leds[idx] = filamentYellow;
    }
  }
}

float getSecondsFraction(struct tm &tmnow,uint32_t nowMs) {
  return tmnow.tm_sec + (nowMs%1000)/1000.0f;
}

void waitForNtpSync(uint32_t timeoutMs) {
  uint32_t start = millis();
  Serial.print("Waiting for NTP sync");
  while (millis()-start < timeoutMs) {
    struct tm ti;
    if (getLocalTime(&ti)) {
      Serial.printf("\nNTP synced: %02d:%02d:%02d\n",ti.tm_hour,ti.tm_min,ti.tm_sec);
      return;
    }
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nNTP sync timed out.");
}

void setup() {
  Serial.begin(115200);
  delay(50);
  Serial.println("\n--- CLOCK WITH FILAMENT DOTS ---");

  FastLED.addLeds<WS2812B,DATA_PIN,GRB>(leds,NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  FastLED.clear(); FastLED.show();

  WiFi.begin(ssid,password);
  uint32_t wifiStart=millis();
  while(WiFi.status()!=WL_CONNECTED && millis()-wifiStart<20000){
    delay(300);
  }

  if (WiFi.status()==WL_CONNECTED) {
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    waitForNtpSync(NTP_TIMEOUT_MS);
  }
}

void loop() {
  uint32_t nowMs = millis();
  struct tm tmnow;
  bool hasTime = getLocalTime(&tmnow);

  fadeToBlackBy(leds, NUM_LEDS, 8);
  FastLED.clear(false);

  // 12 FILAMENT DOTS FIRST
  drawHourPoints();

  if (hasTime) {
    float secF  = getSecondsFraction(tmnow, nowMs);
    float minF  = tmnow.tm_min + secF/60.0f;
    float hourF = (tmnow.tm_hour % 12) + minF/60.0f;

    float angleSec  = (secF/60.0f) * TWO_PI - HALF_PI;
    float angleMin  = (minF/60.0f) * TWO_PI - HALF_PI;
    float angleHour = (hourF/12.0f) * TWO_PI - HALF_PI;

    float tSec = fmodf((float)nowMs/1000.0f, 1.0f);

    float tMinPulse  = (sin(nowMs / 1500.0f * TWO_PI) + 1.0f) * 0.5f;
    float tHourPulse = (sin(nowMs / 2500.0f * TWO_PI) + 1.0f) * 0.5f;

    // SECOND HAND: orange
    drawSecondHandle(angleSec,  maxR - 1, CRGB(255,180,0), tSec);

    // MINUTE HAND: PINK
    drawPulseHandle(angleMin,   maxR - 3, CRGB(255, 90, 150), tMinPulse);

    // HOUR HAND: blue (unchanged)
    drawPulseHandle(angleHour,  maxR - 5, CRGB(60,80,255), tHourPulse);
  }

  FastLED.show();
  delay(33);
}
