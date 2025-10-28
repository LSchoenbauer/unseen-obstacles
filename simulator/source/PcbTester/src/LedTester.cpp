#include <Arduino.h>

const int usedPins[] = {
  14, 33, 36, 39, 35,
  26, 18, 5, 23, 19,
  22, 21, 2, 16, 17,
  4, 12, 27, 25, 32
};

const int pinCount = sizeof(usedPins) / sizeof(usedPins[0]);

void setup() {
  Serial.begin(115200);
  Serial.println("=== ESP32 LED Dauer-An Test (alle verwendeten Pins) ===");
  Serial.println("Alle verfügbaren Ausgangspins sind nun HIGH.");
}

void loop() {
  for (int i = 0; i < pinCount; i++) {
    pinMode(usedPins[i], OUTPUT);
    digitalWrite(usedPins[i], HIGH);
    Serial.printf("Pin %d -> HIGH\n", usedPins[i]);
    delay(500);
    digitalWrite(usedPins[i], LOW);
    Serial.printf("Pin %d -> LOW\n", usedPins[i]);
    delay(500);
  }
}
