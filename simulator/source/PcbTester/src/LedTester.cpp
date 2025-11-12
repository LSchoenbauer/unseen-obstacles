#include <Arduino.h>

// Note: GPIO 34 to 39 are read-only pins!
static const uint8_t usedPins[] = {
// Dir, Pls, SwT, SwC, SwB
   14,   33,  36,  39,  35, // Mover 1
   26,   18,   5,  23,  19, // Mover 2
   22,   21,   2,  16,  17, // Mover 3
    4,   12,  27,  25,  32, // Mover 4
};
static const char* pinNames[] = {
   "Dir-1", "Pls-1", "SwT-1", "SwC-1", "SwB-1", // Mover 1
   "Dir-2", "Pls-2", "SwT-2", "SwC-2", "SwB-2", // Mover 2
   "Dir-3", "Pls-3", "SwT-3", "SwC-3", "SwB-3", // Mover 3
   "Dir-4", "Pls-4", "SwT-4", "SwC-4", "SwB-4", // Mover 4
};
static const int pinCount = sizeof(usedPins) / sizeof(usedPins[0]);
static bool g_writeMode = true;

void setWriteModeEnabled(bool writeMode) {
  g_writeMode = writeMode;
  for (int i = 0; i < pinCount; i++) {
    if (g_writeMode && usedPins[i] < 34) { // GPIO 34 to 39 are read-only pins!
      pinMode(usedPins[i], OUTPUT);
    } else {
      pinMode(usedPins[i], INPUT_PULLUP);
    }
  }
  Serial.printf("%smodus aktiviert!\n", g_writeMode ? "Schreib": "Lese");
}

bool isWriteModeEnabled() {
  return g_writeMode;
}

void writePin(uint8_t pinIdx, bool high, uint32_t delayMs = 0) {
  if (isWriteModeEnabled()) {
    if (usedPins[pinIdx] < 34) { // GPIO 34 to 39 are read-only pins!
      digitalWrite(usedPins[pinIdx], high ? HIGH : LOW);
      Serial.printf("Pin '%s' GPIO %d -> %s\n", pinNames[pinIdx], usedPins[pinIdx], high ? "HIGH": "LOW");
      if (delayMs > 0) {
        delay(delayMs);
      }
    }
  } else {
    Serial.println("Lesemodus aktiv, Pin Pegel wird nicht geändert.");
  }
}

int readPin(uint8_t pinIdx, uint32_t delayMs = 0) {
    int val = digitalRead(usedPins[pinIdx]);
    Serial.printf("Lese Pin '%s' GPIO %d -> %s\n", pinNames[pinIdx], usedPins[pinIdx], val == HIGH ? "HIGH": "LOW");
    if (delayMs > 0) {
      delay(delayMs);
    }
    return val;
}

void togglePin(uint8_t pinIdx) {
  int val = digitalRead(usedPins[pinIdx]);
  writePin(pinIdx, val != HIGH);
}

void setAllPins(bool high, uint32_t delayMs = 0) {
  setWriteModeEnabled(true);
  for (int i = 0; i < pinCount; i++) {
    writePin(i, high, delayMs);
  }
}

void runAutoMode() {
  Serial.printf("%smodus aktiv!\n;", isWriteModeEnabled() ? "Schreib": "Lese");
  for (int i = 0; i < pinCount; i++) {
    if (isWriteModeEnabled() && usedPins[i] < 34) { // GPIO 34 to 39 are read-only pins!
      writePin(i, HIGH, 500);
      writePin(i, LOW, 500);
    } else {
      readPin(i, 500);
    }
  }
}

void processPinCommand(String pinStr) {
  uint8_t pinIdx = (uint8_t)pinStr.toInt();
  if (pinIdx < pinCount) {
    if (isWriteModeEnabled() && usedPins[pinIdx] < 34) {
      togglePin(pinIdx);
    } else {
      readPin(pinIdx);
    }
  } else {
      Serial.printf("Pin mit Index '%d' ist nicht verfügbar\n", pinIdx);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("=== ESP32 LED Test (alle verwendeten Pins) ===");

  setAllPins(true);

  Serial.println("Alle verfügbaren Ausgangspins sind nun HIGH.");
  Serial.println("Kommandos:");
  Serial.println("  a: Automatischer Durchlauf aller Ausgabepins");
  Serial.println("  h: Setzt alle Ausgabepins auf HIGH");
  Serial.println("  l: Setzt alle Ausgabepins auf LOW");
  Serial.println("  r: Setzt alle Pins in den Lesemodus, Pin per Index lesen");
  Serial.println("  w: Setzt alle Ausgabepins in den Schreibmodus, Pin per Index toggeln");
  Serial.println("  Pin Index (0 .. 19):");
  Serial.println("    Mover 1 - 0: Dir, 1: Pls, 2: SwT, 3: SwC, 4: SwB");
  Serial.println("    Mover 2 - 5: Dir, 6: Pls, 7: SwT, 8: SwC, 9: SwB");
  Serial.println("    Mover 3 - 10: Dir, 11: Pls, 12: SwT, 13: SwC, 14: SwB");
  Serial.println("    Mover 4 - 15: Dir, 16: Pls, 17: SwT, 18: SwC, 19: SwB");
  Serial.println(" ");
  Serial.printf("Kommando: ");
}

void loop() {
  String cmd = Serial.readString();
  if (cmd.length() > 0) {
    Serial.printf("[%s]\n", cmd);
    if (cmd.compareTo("a") == 0) {
      runAutoMode();
    } else if (cmd.compareTo("h") == 0) {
      setAllPins(true);
    } else if (cmd.compareTo("l") == 0) {
      setAllPins(false);
    } else if (cmd.compareTo("r") == 0) {
      setWriteModeEnabled(false);
    } else if (cmd.compareTo("w") == 0) {
      setWriteModeEnabled(true);
    } else {
      processPinCommand(cmd);
    }
    Serial.printf("Kommando: ");
  }
}