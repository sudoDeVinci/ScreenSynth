#include "laser.h"
#include "pitches.h"

// 12 bit precision for LEDC timer
#define LEDC_TIMER_12_BIT  12
// use 5000 Hz as a LEDC base frequency
#define LEDC_BASE_FREQ 8000

// Speaker pins - left and right.
#define LED_PIN_1 47
#define LED_PIN_2 15

// Pin to reset sensors.
#define resetPin 45

// Queues to hold frequencies 
QueueHandle_t freqAQueue;
QueueHandle_t freqBQueue;

// Producer and Consumer tasks.
void readSensorTask(void * parameter);
void playAudioTask(void * parameter);

// Two I2C busses on the ESP32S3
TwoWire wireA = TwoWire(1);
TwoWire wireB = TwoWire(0);

void setup() {
  Serial.begin(115200);

  wireA.begin(41, 42);
  wireB.begin(20, 21);

  // Create Queues to hold frequency values
  freqAQueue = xQueueCreate(10, sizeof(int));
  freqBQueue = xQueueCreate(10, sizeof(int));

  // Reset the laser sensors
  Laser.reset(resetPin);

  // Create tasks
  xTaskCreate(
    readSensorTask,   // Function to implement the task
    "ReadSensorTask", // Name of the task
    10000,            // Stack size in words
    NULL,             // Task input parameter
    1,                // Priority of the task
    NULL             // Task handle);
  );

  xTaskCreate(
    playAudioTask,    // Function to implement the task
    "PlayAudioTask",  // Name of the task
    10000,            // Stack size in words
    NULL,             // Task input parameter
    1,                // Priority of the task
    NULL             // Task handle
  );
}

void loop () {
  delay(2_000_000_000);
}

void readSensorTask(void * parameter) {
  Laser sensorA = Laser(NOTE_A3, NOTE_A6, resetPin, &wireA);
  Laser sensorB = Laser(NOTE_G3, NOTE_G6, resetPin, &wireB);

  while (true) {
    int freqA = sensorA.getFreq();
    int freqB = sensorB.getFreq();

    xQueueSend(freqAQueue, &freqA, portMAX_DELAY);
    xQueueSend(freqBQueue, &freqB, portMAX_DELAY);

    Serial.print(freqA);
    Serial.print("--");
    Serial.println(freqB);
    delay(10);
  }
}

void playAudioTask(void * parameter) {

  // Setup timer and attach timer to a led pins
  ledcAttachChannel(LED_PIN_1, 8000, LEDC_TIMER_12_BIT, 1);
  ledcAttachChannel(LED_PIN_2, 3000, LEDC_TIMER_12_BIT, 6);

  // Intermediate buffer for frequency
  int freqA;
  int freqB;

  // Initialize last known frequency
  int lastFreqA = 0;
  int lastFreqB = 0;

  while (true) {
    // Update last known frequency
    if (xQueueReceive(freqAQueue, &freqA, 0) == pdPASS) lastFreqA = freqA;
    if (xQueueReceive(freqBQueue, &freqB, 0) == pdPASS) lastFreqB = freqB;

    // Use last known frequency
    ledcWriteTone(LED_PIN_1, lastFreqA);
    ledcWriteTone(LED_PIN_2, lastFreqB); 
    delay(5);
  }
}