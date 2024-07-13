#include "potentiometer.h"
#include "pitches.h"
/* This example shows how to use continuous mode to take
range measurements with the VL53L0X. It is based on
vl53l0x_ContinuousRanging_Example.c from the VL53L0X API.

The range readings are in units of mm. */

#include <Wire.h>
#include <VL53L0X.h>
#define resetPin 45
#define SAMPLES 10

TwoWire wire = TwoWire(0);
TwoWire wire2 = TwoWire(1);

VL53L0X sensor;
VL53L0X sensor2;


void setup()
{
  Serial.begin(9600);
  wire.begin(41, 42);
  wire2.begin(8, 9);

  // Setup timer and attach timer to a led pins
  ledcAttachChannel(LED_PIN_1, LEDC_BASE_FREQ, LEDC_TIMER_12_BIT, 0);
  ledcAttachChannel(LED_PIN_2, 5000, LEDC_TIMER_12_BIT, 5);
  
  pinMode(resetPin, OUTPUT);
  digitalWrite(resetPin, HIGH);  // Keep this HIGH as we need to pull it low when we reset the sensor

  sensor = VL53L0X();
  sensor.setBus(&wire);
  if (!sensor.init())
  {
    Serial.println("Failed to detect and initialize sensor!");
    while (1) {}
  }

  sensor2 = VL53L0X();
  sensor2.setBus(&wire2);
  if (!sensor2.init())
  {
    Serial.println("Failed to detect and initialize sensor2!");
    while (1) {}
  }


  sensor.setTimeout(0);
  sensor2.setTimeout(0);

  #if defined LONG_RANGE
  // lower the return signal rate limit (default is 0.25 MCPS)
  sensor.setSignalRateLimit(0.1);
  sensor2.setSignalRateLimit(0.1);
  // increase laser pulse periods (defaults are 14 and 10 PCLKs)
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
  sensor.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
  sensor2.setVcselPulsePeriod(VL53L0X::VcselPeriodPreRange, 18);
  sensor2.setVcselPulsePeriod(VL53L0X::VcselPeriodFinalRange, 14);
  #endif

  #if defined HIGH_SPEED
    // reduce timing budget to 20 ms (default is about 33 ms)
    sensor.setMeasurementTimingBudget(20000);  // minimum timing budget 20 ms
    sensor2.setMeasurementTimingBudget(20000);  // minimum timing budget 20 ms
  #elif defined HIGH_ACCURACY
    // increase timing budget to 200 ms
    sensor.setMeasurementTimingBudget(200000);
    sensor2.setMeasurementTimingBudget(200000);
  #endif

    // Start continuous back-to-back mode (take readings as
    // fast as possible).  To use continuous timed mode
    // instead, provide a desired inter-measurement period in
    // ms (e.g. sensor.startContinuous(100)).
    sensor.startContinuous();
    sensor2.startContinuous();
}

void loop()
{
  uint16_t s1 = sensor.readRangeContinuousMillimeters();
  uint16_t s2 = sensor2.readRangeContinuousMillimeters();
  if (s1 >= 600) s1 = 600;
  if (s1 < 40) s1 = 40;
  if (s2 >= 600) s2 = 600;
  if (s2 < 40) s2 = 40;

  int freq1 = map(s1, 600, 40, NOTE_C3, NOTE_C5);
  int freq2 = map(s2, 600, 40, NOTE_D2, NOTE_D4);
  Serial.print(freq1);
  Serial.print(" \t--\t ");
  Serial.println(freq2);

  ledcWriteTone(LED_PIN_1, freq1);
  ledcWriteTone(LED_PIN_2, freq2);
}