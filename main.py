from sine import *
from machine import Pin, ADC, I2S
from time import sleep_ms
import uasyncio as asyncio

adcpin = 1
pot = ADC(adcpin)

async def send(audio_writer, sine_wave: array) -> None:
    audio_writer.write(sine_wave)
    await audio_writer.drain()

# Initialize I2S
i2s = I2S(
    0,
    sck=Pin(42),  # Serial Clock
    ws=Pin(2),   # Word Select (LRCLK)
    sd=Pin(41),   # Serial Data
    mode=I2S.TX,
    bits=bitsPerSample,
    format=I2S.MONO,
    rate=sampleRate,
    ibuf=bufferSize
)


# Read voltage from potentiometer (replace with your actual code)
#voltage_A = 3.3  # Example voltage for A note
#voltage_E = 3.3  # Example voltage for E note
#voltage_C = 3.3  # Example voltage for C note

# Generate sine waves for A, E, and C notes based on voltage inputs
#NOTE_A = generate_sine_wave(voltage_A, 440, 100)   # Generate a 1 second A note
#NOTE_E = generate_sine_wave(voltage_E, 329.63, 100)  # Generate a 1 second E note
#NOTE_C = generate_sine_wave(voltage_C, 261.63, 100)  # Generate a 1 second C note

# Continuously write the sine wave buffer to the I2S interface
try:
    audio_writer = asyncio.StreamWriter(i2s)
    while True: 
        adc_value = pot.read_u16()
        print(adc_value)
        volt = (3.3/65535)*adc_value
        #print(f"Voltage is: {volt:.2f} V")
        NOTE_C = generate_sine_wave(volt, 261.63)
        for i in range(100): asyncio.run(send(audio_writer, NOTE_C))

except KeyboardInterrupt:
    print("Stopped by user.")