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

// Volume value changed with softpot slider.
uint16_t volume = 8000;

// Queues to hold frequencies.
QueueHandle_t ichiQueue;
QueueHandle_t niQueue;
QueueHandle_t sanQueue;
QueueHandle_t volumeQueue;

/**
 * Producer task for audio pipeline.
 * Read in the audio from the strings, as well as our volume into cross-process queues.
 * @param parameters: Nothing - We don't actually pass anything in here.
 */
void readStrings(void * parameters);

/**
 * Consumer task for audio pipeline.
 * Attach audio pins to pwm outputs, and play sounds from the queue w/ given volume.\
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
  
  // Create Queues to hold frequency values
  ichiQueue = xQueueCreate(10, sizeof(uint16_t));
  niQueue = xQueueCreate(10, sizeof(uint16_t));
  sanQueue = xQueueCreate(10, sizeof(uint16_t));
  volumeQueue = xQueueCreate(10, sizeof(uint16_t));
  
  // Create tasks
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
  while (true) {
    uint16_t ichiFreq = ichi.freq();
    uint16_t niFreq = ni.freq();
    uint16_t sanFreq = san.freq();
    uint16_t volumeFreq = volumeSlider.freq(); 


    xQueueSend(ichiQueue, &ichiFreq, portMAX_DELAY);
    xQueueSend(niQueue, &niFreq, portMAX_DELAY);
    xQueueSend(sanQueue, &sanFreq, portMAX_DELAY);
    xQueueSend(volumeQueue, &volumeFreq, portMAX_DELAY);

    delay(10);
  }
}

void playAudio(void * parameters) {
  /*
   * We attach a dedicated pwm channel to each "string", outputting to our audio pins.
   * There are 8 channels, 0-7. It's good to have some space between channels used - 
   * - in my experience, so we take every 3rd channel.
   */
  ledcAttachChannel(AUDIO_PIN_LEFT, volume, RESOLUTION, 0);
  ledcAttachChannel(AUDIO_PIN_MIDDLE, volume, RESOLUTION, 3);
  ledcAttachChannel(AUDIO_PIN_RIGHT, volume, RESOLUTION, 6);
  bool attached = true;

  // Frequency buffers
  uint16_t ichiFreq;
  uint16_t niFreq;
  uint16_t sanFreq;
  uint16_t volumeFeq;

  /*
   * Technically we don't exactly need these - but they give us slightly smoother audio.
   * These are the last successful sound made by the strings (the queue returns 'pdPASS').
   *
   * We fetch the sounds in bulk, and play them one after the other.
   * Fetching then playing each within their own if statement makes the audio hitch slightly. 
   */
  uint16_t lastIchiFreq = 0;
  uint16_t lastNiFreq = 0;
  uint16_t lastSanFreq = 0;
  uint16_t lastVolumeFreq = 0;

  /**
  * We use this to tell that a string is being touched.
  */
  uint8_t threshold = 50;

  while(true) {
    // Update last known frequency 
    if (xQueueReceive(ichiQueue, &ichiFreq, 0) == pdPASS) lastIchiFreq = ichiFreq;
    if (xQueueReceive(niQueue, &niFreq, 0) == pdPASS) lastNiFreq = niFreq;
    if (xQueueReceive(sanQueue, &sanFreq, 0) == pdPASS) lastSanFreq = sanFreq;

    ledcWriteTone(AUDIO_PIN_LEFT, lastIchiFreq);
    ledcWriteTone(AUDIO_PIN_MIDDLE, lastNiFreq);
    ledcWriteTone(AUDIO_PIN_RIGHT, lastSanFreq);

    Serial.println(lastSanFreq);

    delay(5);
  }
}

void loop () {
  while (true) {
    delay(2000000000);
  }
}