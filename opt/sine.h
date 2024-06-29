#ifndef SINE_H
#define SINE_H

#include <cmath>
#include <iterator>
#include <cstdint>

#define FREQUENCY_C 261.63
#define FREQUENCY_D 293.66
#define FREQUENCY_E 329.63
#define FREQUENCY_F 349.23
#define FREQUENCY_G 392.00
#define FREQUENCY_A 440.00
#define FREQUENCY_B 493.88

const int sampleRate = 44100;  // 44.1 kHz - CD quality
const int bitsPerSample = 16;
const int channels = 1;
const int bufferSize = 4096;

const int16_t constant = (1 << 15) - 1;  // 2^15 - 1

class SineWaveIterator {
public:
    using iterator_category = std::input_iterator_tag;
    using value_type = int16_t;
    using difference_type = std::ptrdiff_t;
    using pointer = const int16_t*;
    using reference = const int16_t&;

    SineWaveIterator(float voltage, float frequency, int sample_rate, int position = 0)
        : voltage_(voltage), frequency_(frequency), sample_rate_(sample_rate), position_(position) {
        index_ = voltage_ / 3.3f;
        num_samples_ = static_cast<int>(sample_rate_ / frequency_);
        x_ = (2 * M_PI * frequency_) / sample_rate_;
    }

    value_type operator*() const {
        float amplitude = constant * index_;
        return static_cast<value_type>(amplitude * std::sin(2 * M_PI * frequency_ * position_ / sample_rate_));
    }

    SineWaveIterator& operator++() {
        ++position_;
        return *this;
    }

    SineWaveIterator operator++(int) {
        SineWaveIterator tmp = *this;
        ++(*this);
        return tmp;
    }

    bool operator==(const SineWaveIterator& other) const {
        return position_ == other.position_;
    }

    bool operator!=(const SineWaveIterator& other) const {
        return !(*this == other);
    }

private:
    float voltage_;
    float frequency_;
    int sample_rate_;
    int position_;
    float index_;
    int num_samples_;
    float x_;
};

class SineWaveGenerator {
public:
    SineWaveGenerator(float voltage, float frequency, int sample_rate = sampleRate)
        : voltage_(voltage), frequency_(frequency), sample_rate_(sample_rate) {}

    SineWaveIterator begin() const {
        return SineWaveIterator(voltage_, frequency_, sample_rate_);
    }

    SineWaveIterator end() const {
        return SineWaveIterator(voltage_, frequency_, sample_rate_, static_cast<int>(sample_rate_ / frequency_));
    }

private:
    float voltage_;
    float frequency_;
    int sample_rate_;
};

#endif