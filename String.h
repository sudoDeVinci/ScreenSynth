#include <Arduino.h>

// use 12 bit precission for LEDC timer
#define LEDC_TIMER_12_BIT  12

// use 5000 Hz as a LEDC base frequency
#define LEDC_BASE_FREQ     5000

// fade LED PIN (replace with LED_BUILTIN constant for built-in LED)
#define LED_PIN_1 47
#define LED_PIN_2 15

// define starting duty, target duty and maximum fade time
#define LEDC_START_DUTY   (0)
#define LEDC_TARGET_DUTY  (4095)
#define LEDC_FADE_TIME    (3000)


struct String {
    uint8_t PIN;
    uint8_t CHANNEL;
    uint16_t LOW_NOTE;
    uint16_t HIGH_NOTE;
    uint32_t BASE_FREQ;
    uint8_t PRECISSION;
    int THRESH = 50;

    TwoWire *BUS; 

    Potentiometer ( uint8_t pin,
                    uint8_t channel,    
                    uint16_t low_note,
                    uint16_t high_note,
                    uint32_t base_freq = LEDC_BASE_FREQ,
                    uint8_t precission = LEDC_TIMER_12_BIT,
                  ) {
      this -> PIN = pin;
      this -> CHANNEL = channel;
      this -> LOW_NOTE = low_note;
      this -> HIGH_NOTE = high_note;
      this -> BASE_FREQ = base_freq;
      this -> PRECISSION = precission;
    }

    int getValue () {
        return analogRead(this -> PIN);
    }

    int getFreq () {
      int value = this -> getValue();
      int freq = map(value, 1300, THRESH, this -> LOW_NOTE, this -> HIGH_NOTE); 
      return freq;
    }

    bool isOn () {
      return this -> getValue() > THRESH;
    }

    bool operator& (Potentiometer const& pot) {
      return this -> isOn() & pot.isOn();
    }
};