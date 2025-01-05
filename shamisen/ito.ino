#include <Arduino.h>
#define ADC_MAX 4095;

struct Ito {
  uint16_t LOW_NOTE;
  uint16_t HIGH_NOTE;
  uint8_t PIN;
  int THRESH = 50;

  Ito() {
      ;
  }

  Ito (uint16_t low_note,
      uint16_t high_note,
      uint8_t pin
    ) {
    this -> LOW_NOTE = low_note;
    this -> HIGH_NOTE = high_note;
    this -> PIN = pin;
  }

  uint16_t read () {
    uint16_t s = analogRead(this -> PIN);
    if (s >= ADC_MAX) s = ADC_MAX;
    if (s <  0 ) s = ADC_MAX;
    return s;
  }

  uint32_t getFreq () {

  }
};