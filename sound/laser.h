#include <Arduino.h>
#include <Wire.h>
#include <VL53L0X.h>


struct Laser {
  uint16_t LOW_NOTE;
  uint16_t HIGH_NOTE;
  uint8_t RESET_PIN;
  int THRESH = 50;

  TwoWire *BUS; 
  VL53L0X SENSOR;

  Laser() {// fade LED PIN (replace with LED_BUILTIN constant for built-in LED)
    ;
  }

  Laser ( uint16_t low_note,
          uint16_t high_note,
          uint8_t resetPin,
          TwoWire *bus
        ) {
    this -> LOW_NOTE = low_note;
    this -> HIGH_NOTE = high_note;
    this -> RESET_PIN = resetPin;
    this -> BUS = bus;

    digitalWrite(resetPin, HIGH);  // Keep this HIGH as we need to pull it low when we reset the sensor
    
    VL53L0X sensor = VL53L0X();
    sensor.setBus(bus);
    if (!sensor.init())
    {
      Serial.println("Failed to detect and initialize sensor!");
      this -> SENSOR = sensor;
      return;
    }

    sensor.setTimeout(0);

    #if defined LONG_RANGE
    // lower the return signal rate limit (default is 0.25 MCPS)
    sensor.setSignalRateLimit(0.1);
    // increase laser pulse periods (defaults are 14 and 10 PCLKs)
    sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
    sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
    #endif

    #if defined HIGH_SPEED
      // reduce timing budget to 20 ms (default is about 33 ms)
      sensor.setMeasurementTimingBudget(20000);  // minimum timing budget 20 ms
    #elif defined HIGH_ACCURACY
      // increase timing budget to 200 ms
      sensor.setMeasurementTimingBudget(200000);
    #endif
      // Start continuous back-to-back mode (take readings as
      // fast as possible).  To use continuous timed mode
      // instead, provide a desired inter-measurement period in
      // ms (e.g. sensor.startContinuous(100)).
      sensor.startContinuous();
  }

  uint16_t getValue () {
    uint16_t s = this -> SENSOR.readRangeContinuousMillimeters();
    if (s >= 600) s = 600;
    if (s < 40) s = 40;
    return s;
  }

  int getFreq () {
    uint16_t val = this -> getValue();
    int freq = map(val, 40, 600, this->LOW_NOTE, this->HIGH_NOTE);
    return freq;
  }

  static void reset (uint8_t pin) {
    digitalWrite(pin, LOW);
    delay(100);
    digitalWrite(pin, HIGH);
    delay(100);
    digitalWrite(pin, LOW);
    delay(100);
    digitalWrite(pin, HIGH);
  }
};