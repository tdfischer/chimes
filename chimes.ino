#include <FastLED.h>

#define LED_NUM 30
#define CHIME_COUNT 3

class Chime {
  int m_pos;
  int m_speed;
  int m_hue;
  const int m_fullLength;
  
public:
  
  Chime()
    : m_pos(0),
      m_hue(0),
      m_speed(1),
      m_fullLength(10) {}
      
  int length() const {
    return m_fullLength;
  }
  
  void setSpeed(int i) {
    m_speed = i % 255;
  }
  
  void setPos(int i) {
    m_pos = i;
  }
  
  void setHue(int i) {
    m_hue = i % 255;
  }
  
  void step() {
    m_pos += 1;
    
    if (random(255) > m_speed) {
      m_pos += 2;
    }
    
    if (m_pos > m_fullLength * 40) {
      m_pos = 0;
      m_hue += 3;
      m_hue %= 255;
    }
  }
  
  void setLEDs(CRGB* leds) {
    for(int i = 0; i < m_fullLength; i++) {
      if (i > m_pos) {
        leds[i] = CHSV(0, 0, 0);
      } else {
        double distance = m_pos - i;
        double brightness = sin((m_fullLength / distance) * (M_PI/2));
        if (brightness <= 0.2)
          brightness = 0;
        leds[i] = CHSV(m_hue, max(200, brightness * 255), brightness * 255);
      }
    }
  }
};

Chime chimes[3];

CRGB leds[LED_NUM];

void setup() {
  FastLED.addLeds<LPD8806>(leds, LED_NUM);
  for(int i = 0; i < CHIME_COUNT;i++) {
    chimes[i].setPos(random(chimes[i].length() * 5));
    chimes[i].setHue(random(255));
    chimes[i].setSpeed(random(255));
  }
}

void loop() {
  int offset = 0;
  for(int i = 0; i < CHIME_COUNT;i++) {
    chimes[i].step();
    chimes[i].setLEDs(&leds[offset]);
    offset += chimes[i].length();
  }
  FastLED.show();
  delay(10);
}
