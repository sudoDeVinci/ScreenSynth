from sine import *

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


def write_wav_file(file_name: str, sampleRate: int, bitsPerSample: int, num_channels: int, num_samples: int, sine_wave_generator: Generator[bytes, None, None], bufferSize: int) -> None:
    """
    This function writes the generated sine wave data to a WAV file.

    Parameters:
    file_name (str): The name of the WAV file to be created.
    sampleRate (int): The sample rate of the audio data in Hz.
    bitsPerSample (int): The number of bits per sample.
    num_channels (int): The number of audio channels (1 for mono, 2 for stereo).
    num_samples (int): The total number of samples in the audio data.
    sine_wave_generator (Generator[bytes, None, None]): A generator that yields the sine wave data as bytes.
    bufferSize (int): The size of the buffer used for writing data to the file.

    Returns:
    None

    The function writes the WAV file header and the sine wave data to the specified file.
    It handles exceptions such as KeyboardInterrupt and general exceptions, printing an error message.
    """
    
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
