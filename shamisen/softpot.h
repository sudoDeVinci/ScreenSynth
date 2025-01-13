#include <Arduino.h>
#define ADC_MAX 4095

struct SoftPot {
  uint16_t LOW_NOTE;
  uint16_t HIGH_NOTE;
  uint8_t PIN;
  uint8_t THRESH;

  SoftPot () {
    ;
  }

  SoftPot (uint16_t low_note,
           uint16_t high_note,
           uint8_t pin,
           uint8_t thresh=10
          ) {
    this -> LOW_NOTE = low_note;
    this -> HIGH_NOTE = high_note;
    this -> PIN = pin;
    this -> THRESH = thresh;
  }

  uint16_t read () {
    uint16_t s = analogRead(this -> PIN);
    if (s >= ADC_MAX) s = ADC_MAX;
    if (s <  0 ) s = this -> THRESH;
    return s;
  }

  uint16_t freq () {
    uint16_t val = this -> read();
    uint16_t freq = map(val, this->THRESH, ADC_MAX, this->LOW_NOTE, this->HIGH_NOTE);
    return freq;
  }
};