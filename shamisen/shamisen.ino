#include "pitches.h"
#include "ShamiString.h"
// 12 bit precision for the output frequency
#define RESOLUTION  12

// Speaker pins - left and right.
#define AUDIO_PIN_LEFT 47
#define AUDIO_PIN_RIGHT 15

// String pins - 4,5 & 6
#define STRING_PIN_ICHI 4
#define STRING_PIN_NI 5

/*
 * Use 8000 Hz as the base output frequency
 * This acts as the volume control for the speaker.
 * We may wanna change these later with dials.
 */
uint32_t volume_left = 8000;
uint32_t volume_right = 8000;

// Queues to hold frequencies 
QueueHandle_t ichiQueue;
QueueHandle_t niQueue;

// Producer and Consumer tasks.
void readStrings(void * parameters);
void playAudio(void * parameters);

// Our Strings
ShamiString ichi = ShamiString(NOTE_C2, NOTE_C5, STRING_PIN_ICHI);
ShamiString ni = ShamiString(NOTE_F2, NOTE_F5, STRING_PIN_NI);


void setup() {
    Serial.begin(115200);

    // Configure ADC resolution
    analogReadResolution(RESOLUTION);
    
    // Create Queues to hold frequency values
    ichiQueue = xQueueCreate(10, sizeof(uint32_t));
    niQueue = xQueueCreate(10, sizeof(uint32_t));
    
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

/**
 * Read and enqueue the current frequencies from our strings.
 */
void readStrings(void * parameters) {
  while (true) {
    uint32_t ichiFreq = ichi.freq();
    uint32_t niFreq = ni.freq();

    xQueueSend(ichiQueue, &ichiFreq, portMAX_DELAY);
    xQueueSend(niQueue, &niFreq, portMAX_DELAY);

    delay(10);
  }
}

void playAudio(void * parameters) {
  ledcAttachChannel(AUDIO_PIN_LEFT, volume_left, RESOLUTION, 1);
  ledcAttachChannel(AUDIO_PIN_RIGHT, volume_right, RESOLUTION, 1);
  bool attached = true;

  // Frequency buffers
  uint32_t ichiFreq;
  uint32_t niFreq;

  // Frequency buffers
  uint32_t lastIchiFreq = 0;
  uint32_t lastNiFreq = 0;

  while(true) {
    // Update last known frequency 
    if (xQueueReceive(ichiQueue, &ichiFreq, 0) == pdPASS) lastIchiFreq = ichiFreq;
    if (xQueueReceive(niQueue, &niFreq, 0) == pdPASS) lastNiFreq = niFreq;

    ledcWriteTone(AUDIO_PIN_LEFT, lastIchiFreq);
    ledcWriteTone(AUDIO_PIN_RIGHT, lastNiFreq);
    delay(5);
  }
}

void loop () {
  while (true) {
    delay(2000000000);
  }
}