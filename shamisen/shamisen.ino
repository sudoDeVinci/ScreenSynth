#include "pitches.h"
#include "softpot.h"

/**
 * This program acts as a synthesizer, allowing the user to play tones from the ESP32S3 as you would a Shamisen.
 * This is inspired by the CRT instruments created by the band Electronicos Fantasticos - specifically the Telesen.
 * The "strings" are expected to be softpots, though technically any potentiometer can work.
 * 
 * Tones and volume are read into a queue for each string in one process, and are outputted as pwm signals
 * on pins pinned to individual pwm chanels. While this doesn't give the best audio quality, we only need a 
 * consistent tone output. 
 */

// 12 bit precision for the output frequency
#define RESOLUTION 12

// Speaker pins - left and right.
#define AUDIO_PIN_LEFT 2
#define AUDIO_PIN_MIDDLE 42
#define AUDIO_PIN_RIGHT 41

// String pins - 4,5 & 6
#define STRING_PIN_ICHI 4
#define STRING_PIN_NI 5
#define STRING_PIN_SAN 6
#define VOLUME_PIN 7

// Queues to hold frequencies.
QueueHandle_t soundQueue;
QueueHandle_t strumQueue;

// Volume value changed with softpot slider.
uint16_t volume = 8000;

/**
 * Producer task for audio pipeline.
 * Read in the audio from the strings, as well as our volume into cross-process queues.
 * @param parameters: Nothing - We don't actually pass anything in here.
 */
void readStrings(void * parameters);

/**
 * Consumer task for audio pipeline.
 * Attach audio pins to pwm outputs, and play sounds from the queue w/ given volume.
 * @param parameters: Nothing - We don't actually pass anything in here.
 */
void playAudio(void * parameters);

// Our Strings and volume slider.
SoftPot ichi = SoftPot(NOTE_C2, NOTE_C5, STRING_PIN_ICHI);
SoftPot ni = SoftPot(NOTE_F2, NOTE_F5, STRING_PIN_NI);
SoftPot san = SoftPot(NOTE_F2, NOTE_F5, STRING_PIN_SAN);
SoftPot volumeSlider = SoftPot(0, volume, VOLUME_PIN, 0);

void setup() {
  Serial.begin(115200);

  // Configure ADC resolution
  analogReadResolution(RESOLUTION);
  
  // Queue to hold 3 frequency values and volume.
  soundQueue = xQueueCreate(10, (sizeof(uint16_t)*4));

  // Queue to hold whether or not we actuall play the sound.
  strumQueue = xQueueCreate(10, (sizeof(bool)));
  
  // Consumer and producer tasks.
  xTaskCreate(
      readStrings,   // Function to implement the task
      "ReadStringsTask", // Name of the task
      10000,            // Stack size in words
      NULL,             // Task input parameter
      1,                // Priority of the task
      NULL             // Task handle);
  );
  
  xTaskCreate(
      playAudio,    // Function to implement the task
      "PlayAudioTask",  // Name of the task
      10000,            // Stack size in words
      NULL,             // Task input parameter
      1,                // Priority of the task
      NULL             // Task handle
  );      
}


void readStrings(void * parameters) {
  uint16_t soundData[4];

  bool strum = true;

  while (true) {
    soundData[0] = ichi.freq();
    soundData[1] = ni.freq();
    soundData[2] = san.freq();
    soundData[3] = volumeSlider.freq(); 

    // We read from touch sensor here but for testing just say it's true.
    // TODO: Read from touch sensor.
    strum = true;

    xQueueSend(soundQueue, &soundData, portMAX_DELAY);
    xQueueSend(strumQueue, &strum, portMAX_DELAY);

    delay(10);
  }
}

void detachSound() {
  ledcDetach(AUDIO_PIN_LEFT);
  ledcDetach(AUDIO_PIN_MIDDLE);
  ledcDetach(AUDIO_PIN_RIGHT);
}


/*
  * Attach a dedicated pwm channel to each "string", outputting to our audio pins.
  * There are 8 channels, 0-7. It's good to have some space between channels used - 
  * - in my experience, so we take every 3rd channel.
  */
void attachSound() {
  ledcAttachChannel(AUDIO_PIN_LEFT, volume, RESOLUTION, 0);
  ledcAttachChannel(AUDIO_PIN_MIDDLE, volume, RESOLUTION, 3);
  ledcAttachChannel(AUDIO_PIN_RIGHT, volume, RESOLUTION, 6);
}


/**
 * Set the audio output to the current global volume.
 */
void setVolume() {
  detachSound();
  attachSound();
}

void playAudio(void * parameters) {

  uint8_t threshold = 50;
  uint16_t soundDataBuffer[4];

  bool strumBuffer = true;
  bool strumBufferState[2] = {true, true};
  constexpr uint8_t strumStateMask[4] = {
    0b00,  // both off
    0b11,  // both on
    0b01,  // off->on
    0b10   // on->off
  };

  attachSound();

  while(true) {

    /**
     * Read the current state of whether we strum or not.
     * We define transitions between sound being turned on or off using this
     * boolean mask.
     */
    if (xQueueReceive(strumQueue, &strumBuffer, 0) == pdPASS) {
      strumBufferState[1] = strumBuffer;

      uint8_t transition = (strumBufferState[0] << 1) | strumBuffer;

      switch (transition) {
        
        case strumStateMask[2]:
          attachSound();
          break;

        case strumStateMask[3]:
          detachSound();
          break;

        case strumStateMask[0]:
        case strumStateMask[1]:
        default:
          break;
      }

      strumBufferState[0] = strumBuffer;
    }

    if (xQueueReceive(soundQueue, &soundDataBuffer, 0) == pdPASS) {
      if (soundDataBuffer[3] != volume) {
        volume = soundDataBuffer[3];
        setVolume();
        Serial.println(volume);
      }

        ledcWriteTone(AUDIO_PIN_LEFT, soundDataBuffer[0]);
        ledcWriteTone(AUDIO_PIN_MIDDLE, soundDataBuffer[1]);
        ledcWriteTone(AUDIO_PIN_RIGHT, soundDataBuffer[2]);
    }

    delay(5);
  }
}

void loop () {
  while (true) {
    delay(2000000000);
  }
}