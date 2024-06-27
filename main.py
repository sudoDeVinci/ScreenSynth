from micropython import const
from machine import Pin, ADC, I2S
import time
import math  # for sine wave
import ustruct
import utime
from gc import collect

# micPin = ADC(Pin(26))
FILE_NAME = const('sine_5s.wav')
DEBUG = const(True)

sampleRate = const(22050)  # 44.1 kHz - CD quality
bitsPerSample = const(16)
num_channels = const(1)  # 1 - MONO, 2 - STEREO
duration = const(0.5)  # in seconds
bufferSize = const(1000)
num_samples = duration * sampleRate

recordSize = duration * sampleRate
constant = const((2**16 - 1 // 2) + ((2**16 - 1) // 2))
x = (2 * math.pi) / 8 # divide by an arbitrary number to create steps




def getsizeof(obj: object) -> int:
    """
    Calculate the approximate memory size of an object in bytes.

    This function uses the garbage collector to clean up memory before
    calculating the size. It handles various types of objects including
    primitive types, collections, and custom objects.

    :param obj: The object whose size is to be calculated.
    :return: The size of the object in bytes.
    """
    collect()
    if isinstance(obj, (int, float, bool)):
        return ustruct.calcsize('P')  # Size of a pointer
    elif isinstance(obj, (str, bytes, bytearray)):
        return len(obj)
    elif isinstance(obj, (list, tuple, set, frozenset)):
        return sum(getsizeof(item) for item in obj) + ustruct.calcsize('P') * len(obj)
    elif isinstance(obj, dict):
        return sum(getsizeof(k) + getsizeof(v) for k, v in obj.items()) + ustruct.calcsize('P') * len(obj)
    else:
        return ustruct.calcsize('P') + getsizeof(obj.__dict__)


def create_wav_header(sampleRate, bitsPerSample, num_channels, num_samples):
    """
    This function generates a WAV file header based on the given parameters.
    
    Got this from tinker.toadie's answer @ https://stackoverflow.com/questions/77783261/white-noise-but-no-sound-recording-audio-from-electret-microphone-preamp-on-r

    Parameters:
    sampleRate (int): The sample rate of the audio data in Hz.
    bitsPerSample (int): The number of bits per sample.
    num_channels (int): The number of audio channels (1 for mono, 2 for stereo).
    num_samples (int): The total number of samples in the audio data.

    Returns:
    bytes: A byte string representing the WAV file header.

    The WAV file header contains information about the audio format, such as sample rate,
    bits per sample, and number of channels. This function constructs the header based on the
    provided parameters and returns it as a byte string.
    """
    datasize = num_samples * num_channels * bitsPerSample // 8
    o = bytes("RIFF", "ascii")  # 1-4 (4byte) Marks file as RIFF
    o += (datasize + 36).to_bytes(
        4, "little"
    )  # 5-8 (4byte) File size in bytes excluding this and RIFF marker
    o += bytes("WAVE", "ascii")  # 9-12 (4bytes) File type
    o += bytes("fmt ", "ascii")  # 13-16 (4bytes) Format Chunk Marker, trailing 0
    o += (16).to_bytes(4, "little")  # 17-20 (4bytes) Length of above format data
    o += (1).to_bytes(2, "little")  # 21-22 (2bytes) Format type (1 - PCM)
    o += (num_channels).to_bytes(2, "little")  # 23-24 (2bytes)
    o += (sampleRate).to_bytes(4, "little")  # 25-28 (4bytes)
    o += (sampleRate * num_channels * bitsPerSample // 8).to_bytes(4, "little")  # 29-32 (4bytes)
    o += (num_channels * bitsPerSample // 8).to_bytes(2, "little")  # 33-34 (2bytes)
    o += (bitsPerSample).to_bytes(2, "little")  # 35-36 (2bytes)
    o += bytes("data", "ascii")  # 37-40 (4bytes) Data Chunk Marker
    o += (datasize).to_bytes(4, "little")  # 41-44 (4bytes) Data size in bytes
    return o


def generate_sine_wave():
    """
    Generate a sine wave signal.

    This function generates a sine wave signal based on the global variables `recordSize` and `x`.
    It calculates the sine value for each step, scales it to the desired amplitude, and yields
    the result as a 2-byte little-endian integer.

    The sine wave is generated using the formula:
    y = sin(i * x)
    where `i` is the current step and `x` is the step size in radians.

    Yields:
        bytes: The amplitude of the sine wave at each step, represented as a 2-byte little-endian integer.
    """
    global recordSize
    global x
    for i in range(recordSize):
        y = math.sin(i * x)
        amplitude = math.floor(constant * y)
        yield amplitude.to_bytes(2, 'little')


def write_wav_file(file_name, sampleRate, bitsPerSample, num_channels, num_samples, sine_wave_generator, bufferSize):
    try:
        with open('/' + file_name, 'wb') as recFile:
            wav_header = create_wav_header(
                sampleRate,
                bitsPerSample,
                num_channels,
                num_samples
            )
            recFile.write(wav_header)
            print('Header written')

        with open('/' + file_name, 'ab') as recFile:
            buffer = bytearray()
            for data in sine_wave_generator:
                buffer.extend(data)
                if len(buffer) >= bufferSize:
                    recFile.write(buffer)
                    buffer = bytearray()
            if buffer:
                recFile.write(buffer)
            print("Data written")

    except (KeyboardInterrupt, Exception) as e:
        print(f"Caught: {type(e).__name__}: {e}")





sine_wave_generator = generate_sine_wave()
for data in sine_wave_generator: pass
#write_wav_file(FILE_NAME, sampleRate, bitsPerSample, num_channels, num_samples, sine_wave_generator, bufferSize)


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