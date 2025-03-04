#include <Arduino.h>
#define ADC_MAX 4095
#define BUFFER_SIZE 512
#define SAMPLE_RATE 44100
#define DECAY_FACTOR 0.996f

struct SoftPot {
  uint16_t LOW;
  uint16_t HIGH;
  uint8_t PIN;
  uint8_t THRESH;

  SoftPot () {
    ;
  }

  SoftPot (uint16_t low,
           uint16_t high,
           uint8_t pin,
           uint8_t thresh=0
          ) {
    this -> LOW = low;
    this -> HIGH = high;
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
    uint16_t freq = map(val, this->THRESH, ADC_MAX, this->LOW, this->HIGH);
    return freq;
  }
};


struct KarplusStrong {
  uint16_t* buffer
  uint16_t bufferIndex;
  uint16_t delaySamples:

  KarplusStrong () {
    ;
  }

  /**
   * @param freq: Frequency of the note to play.
   */
  void setFrequency(float freq) {
    delaySamples = (uint16_t)(SAMPLE_RATE / freq);
    delaySamples = min(delaySamples, BUFFER_SIZE - 1);
    delaySamples = max(delaySamples, 1);
  }
}: