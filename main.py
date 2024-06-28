from sine import *
from machine import Pin, ADC, I2S

sine_wave_generator = generate_sine_wave()
for data in sine_wave_generator: pass

# Initialize I2S
i2s = I2S(
    0,
    sck=Pin(18),  # Serial Clock
    ws=Pin(19),   # Word Select (LRCLK)
    sd=Pin(20),   # Serial Data
    mode=I2S.TX,
    bits=bitsPerSample,
    format=I2S.MONO,
    rate=sampleRate,
    ibuf=bufferSize
)


# Generate sine wave and write to I2S
sine_wave_generator = generate_sine_wave()
for data in sine_wave_generator:
    i2s.write(data)