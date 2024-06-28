import math
from array import array
from config import *

sampleRate = const(44100)  # 44.1 kHz - CD quality
bitsPerSample = const(16)
num_channels = const(1)
bufferSize = const(4096)

constant = const((2**15) - 1)
frequency_C = 261.63  # Frequency for middle C (C4)
frequency_E = 329.63  # Frequency for E4
frequency_A = 440.00  # Frequency for A4

def generate_sine_wave(voltage: float, frequency: float, duration_ms: int, sample_rate: int = sampleRate) -> Generator:
    """
    Generate a 16-bit sine wave array based on a voltage input.

    Args:
    - voltage (float): Voltage input (0.0 to 3.3V).
    - duration_ms (int): Duration of the sine wave in milliseconds.
    - sample_rate (int): Sample rate of the output waveform. Default is 44100 Hz.

    Returns:
    - array.array: The generated sine wave array.
    """
    # Map voltage range (0.0 to 3.3V) to amplitude range (0.0 to 1.0)

    index = (voltage / 3.3)

    num_samples = int(sample_rate / frequency)
    x = (2 * math.pi * frequency) / sample_rate
    buffer = bytearray()
    for i in range(num_samples):
        amplitude = int(((constant * math.sin(i * x)) % 10000 + 1) * index)
        sample = int(amplitude * math.sin(2 * math.pi * frequency * i / sample_rate))
        buffer.extend(sample.to_bytes(2, 'little'))
    return buffer