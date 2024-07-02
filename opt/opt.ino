#include <driver/i2s.h>
#include <math.h>

#define PIN_BCLK 42
#define PIN_LRCK 2
#define PIN_DIN 41

const int sampleRate = 44100;
const int bitsPerSample = 16;
const int num_channels = 1;
const int bufferSize = 1024;

const int16_t constant = (1 << 15) - 1;
const float frequency_C = 261.63;

int16_t* buffer = new int16_t[bufferSize];

void generate_sine_wave(float voltage, float frequency, int sample_rate = sampleRate, int16_t* audioBuf = buffer) {
  //int16_t* buffer = new int16_t[bufferSize];
  float index = voltage / 3.3;
  int num_samples = (int)(sample_rate / frequency);
  float phase_increment = 2 * PI * frequency;
  float x = phase_increment / sample_rate;

  for (int i = 0; i < num_samples; i++) {
    int amplitude = (int)(constant * sin(x*i) * index);
    audioBuf[i] = (int16_t)(amplitude * sin(i * x));
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Setting up.");

  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = sampleRate,
    .bits_per_sample = (i2s_bits_per_sample_t)bitsPerSample,
    .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
    .communication_format = I2S_COMM_FORMAT_I2S_MSB,
    .intr_alloc_flags = 0,
    .dma_buf_count = 8,
    .dma_buf_len = bufferSize,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = PIN_BCLK,
    .ws_io_num = PIN_LRCK,
    .data_out_num = PIN_DIN,
    .data_in_num = I2S_PIN_NO_CHANGE
  };

  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &pin_config);
  generate_sine_wave(1.2, frequency_C, sampleRate);
}

void loop() {
  size_t bytes_written;
  i2s_write(I2S_NUM_0, buffer, bufferSize * sizeof(int16_t), &bytes_written, portMAX_DELAY);
}