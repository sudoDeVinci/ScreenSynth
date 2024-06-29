#ifndef SOUND_H
#define SOUND_H

#include <driver/i2s.h>
#include "sine.h"

void init_i2s();
void send_sine_wave(SineWaveGenerator& generator);

#endif // SOUND_H