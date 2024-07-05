#include <toneAC.h>

#define NOTE_C4 262
#define NOTE_C5 523

#define NOTE_E4 330
#define NOTE_E5 659

#define NOTE_A4 440
#define NOTE_A5 880

#define NOTE_D4 294
#define NOTE_D5 587

#define NOTE_G2 98
#define NOTE_G3 196

#define NOTE_G4 392
#define NOTE_G5 784

int threshold = 29;

void setup() {
  Serial.begin(115200);
  pinMode(8, OUTPUT);
}

void loop() {
  int pot0 = analogRead(A0);
  int pot1 = analogRead(A1);

  int freq0 = map(pot0, 1300, 30, NOTE_G2, NOTE_G3);
  int freq1 = map(pot1, 1300, 30, NOTE_D4, NOTE_D5);

  if (pot0 > threshold && pot1 > threshold) {
    toneAC(freq0);
    toneAC(freq1);
  } else {
    if (pot0 > threshold) toneAC(freq0);
    else if (pot1 > threshold) toneAC(freq1);
    else noToneAC();
  }
}