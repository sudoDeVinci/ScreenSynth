import math
from array import array
from config import *

sampleRate = const(44100)  # 44.1 kHz - CD quality
bitsPerSample = const(16)
num_channels = const(1)  # 1 - MONO, 2 - STEREO
duration = const(100)  # in milliseconds
bufferSize = const(1000)

def generate_sine_wave(voltage: float, frequency: float, duration_ms: int, sample_rate: int = 44100) -> array:
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
    min_voltage = 0.0
    max_voltage = 3.3
    min_amplitude = 0.0
    max_amplitude = 1.0

    # Map different finger placements to different amplitude ranges
    bridge_min_voltage = 2.0  # Example voltage range for near the bridge
    bridge_max_voltage = 2.5
    nut_min_voltage = 1.0  # Example voltage range for near the nut
    nut_max_voltage = 1.5

    if voltage >= bridge_min_voltage and voltage <= bridge_max_voltage:
        amplitude = min_amplitude + (max_amplitude - min_amplitude) * (voltage - bridge_min_voltage) / (bridge_max_voltage - bridge_min_voltage)
    elif voltage >= nut_min_voltage and voltage <= nut_max_voltage:
        amplitude = min_amplitude + (max_amplitude - min_amplitude) * (nut_max_voltage - voltage) / (nut_max_voltage - nut_min_voltage)
    else:
        amplitude = min_amplitude + (max_amplitude - min_amplitude) * (voltage - min_voltage) / (max_voltage - min_voltage)

    sine_wave = array('H', [0] * int(duration_ms * sample_rate / 1000))
    for i in range(len(sine_wave)):
        duty_cycle = int(2**15 * (math.sin(2 * math.pi * frequency * i / sample_rate) + 1) / 2)
        sine_wave[i] = int(duty_cycle * amplitude)

    return sine_wave