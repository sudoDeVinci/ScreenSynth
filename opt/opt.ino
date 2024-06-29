#include "sound.h"
#include <driver/adc.h>
#include <Arduino.h>

void setup() {
    Serial.begin(115200);
    init_i2s();

    // Configure ADC
    analogReadResolution(12); // 12-bit resolution
    analogSetAttenuation(ADC_11db); // 11dB attenuation for full-scale voltage range
}

void loop() {
    int adc_value = analogRead(36); // Read from GPIO36 (ADC1_CHANNEL_0)
    float voltage = (3.3f / 4095) * adc_value;

    SineWaveGenerator generator(voltage, 261.63f);
    send_sine_wave(generator);

    delay(5); // Small delay to avoid overwhelming the I2S interface
}