#include "sound.h"
#include <driver/i2s.h>
#include <cstring>

void init_i2s() {
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
        .bck_io_num = 18,   // Serial Clock
        .ws_io_num = 19,    // Word Select (LRCLK)
        .data_out_num = 20, // Serial Data
        .data_in_num = I2S_PIN_NO_CHANGE
    };

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
}

void send_sine_wave(SineWaveGenerator& generator) {
    size_t bytes_written;
    for (auto sample : generator) {
        i2s_write(I2S_NUM_0, &sample, sizeof(sample), &bytes_written, portMAX_DELAY);
    }
}