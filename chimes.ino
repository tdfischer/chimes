#include <FastLED.h>

#define CHIME_COUNT 7
#define CHIME_LENGTH 8
#define BLOB_COUNT 3
#define LED_NUM (CHIME_COUNT * CHIME_LENGTH)

CRGB leds[LED_NUM];

class Blob {
  uint16_t m_pos;
  uint16_t m_hue;
  int16_t m_brightness;
  int8_t m_fadeDir;
  int8_t m_velocity;
public:
  Blob()
    : m_pos(0),
      m_velocity(1),
      m_hue(0),
      m_brightness(1),
      m_fadeDir(1) {}
      
  void setPos(uint16_t p) {
    m_pos = p;
    m_brightness = p % 120 + 1;
  }

  void setHue(uint8_t p) {
    m_hue = p;
  }

  void setBrightness(uint8_t p) {
    m_brightness = p;
  }

  void setVelocity(int8_t v) {
    m_velocity = v;
  }

  void step() {
      m_pos += m_velocity;
      m_pos %= LED_NUM;
      m_hue += 1;
      m_brightness += m_fadeDir;
      if (m_brightness >= 255 || m_brightness <= 0) {
        m_fadeDir *= -1;
      }
  }
  
  void setLEDs(CRGB* leds) {
      const uint8_t width = 25;
      for(uint8_t i = 0;i < width; i++) {
        int absPos = LED_NUM - (m_pos + i - (width / 2));
        if (absPos < 0 || absPos >= LED_NUM)
          continue;
        leds[absPos] = blend(leds[absPos], CHSV(m_hue, 200, quadwave8((i / (double)width) * m_brightness)), 128);
      }
  }
};

template<int ChimeLength> class Chime {
  uint16_t m_pos;
  uint8_t m_speed;
  uint16_t m_hue;
  uint16_t m_brightness;
    
public:

  static const int Length = ChimeLength;
  
  Chime()
    : m_pos(0),
      m_speed(1),
      m_brightness(255),
      m_hue(0) {}
  
  void setSpeed(uint8_t i) {
    m_speed = i % 255;
  }
  
  void setPos(uint8_t i) {
    m_pos = i;
  }
  
  void setHue(uint8_t i) {
    m_hue = i % 255;
  }

  void setBrightness(uint8_t i) {
    m_brightness = i % 255;
  }
  
  void step() {
    m_pos += 1;
    
    if (random(255) > m_speed) {
      m_pos += 2;
    }
    
    if (m_pos > ChimeLength * 20) {
      m_pos = 0;
      m_hue += 3;
      m_hue %= 255;
    }
  }
  
  void setLEDs(CRGB* leds) {
    for(int i = 0; i < ChimeLength; i++) {
      if (i > m_pos) {
        leds[i] = CHSV(0, 0, 0);
      } else {
        uint8_t distance = m_pos - i;
        uint8_t brightness = scale8(quadwave8((ChimeLength / (double)distance) * 255), m_brightness);
        if (brightness <= 0.2)
          brightness = 0;
        leds[i] = CHSV(m_hue, max(200, brightness), brightness);
      }
    }
  }
};

Chime<CHIME_LENGTH> chimes[CHIME_COUNT];
Blob blobs[BLOB_COUNT];

void setup() {
  FastLED.addLeds<WS2812,11, GBR>(leds, LED_NUM);
  for(int i = 0; i < CHIME_COUNT;i++) {
    chimes[i].setPos(random(Chime<CHIME_LENGTH>::Length * 5));
    chimes[i].setHue(random(255));
    chimes[i].setSpeed(random(90) + 138);
    chimes[i].setBrightness(70);
  }
  for(int i = 0; i < BLOB_COUNT;i++) {
    blobs[i].setPos(random(LED_NUM));
    blobs[i].setHue(random(255));
    blobs[i].setBrightness(random(255));
    if (i % 2) {
      blobs[i].setVelocity(-1);
    } else {
      blobs[i].setVelocity(1);
    }
  }
}

void loop() {
  int offset = 0;
  for(int i = 0; i < CHIME_COUNT;i++) {
    chimes[i].step();
    chimes[i].setLEDs(&leds[offset]);
    offset += Chime<CHIME_LENGTH>::Length;
  }
  for(int i = 0; i < BLOB_COUNT;i++) {
    blobs[i].step();
    blobs[i].setLEDs(leds);
  }
  FastLED.show();
  delay(20);
}
