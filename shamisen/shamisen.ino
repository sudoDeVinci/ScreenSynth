#include "pitches.h"
// 12 bit precision for the output frequency
#define RESOLUTION  12

// Speaker pins - left and right.
#define AUDIO_PIN_LEFT 47
#define AUDIO_PIN_RIGHT 15

// String pins - 4,5 & 6
#define STRING_PIN_ICHI 4
#define STRING_PIN_NI 5
#define STRING_PIN_SAN 6


/*
 * Use 8000 Hz as the base output frequency
 * This acts as the volume control for the speaker.
 */
uint32_t volume = 8000;

// Queues to hold frequencies 
QueueHandle_t ichi_Queue;
QueueHandle_t ni_Queue;
QueueHandle_t san_Queue;

// Producer and Consumer tasks.
void readStrings(void * parameter);
void playAudio(void * parameter);

void setup() {
    Serial.begin(115200);

    // Configure ADC resolution
    analogReadResolution(RESOLUTION);
    
    // Create Queues to hold frequency values
    ichi_Queue = xQueueCreate(10, sizeof(uint32_t));
    ni_Queue = xQueueCreate(10, sizeof(uint32_t));
    san_Queue = xQueueCreate(10, sizeof(uint32_t));
    
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