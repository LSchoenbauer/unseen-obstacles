/* 

    Pin asignment for stepper and switches
    direction and speed for stepper
    position detection and reaction
    soft start
    triggerable movement

*/

#include "MoverDriver.h"
#include "Arduino.h"

#include <utils/Log.h>

/*
 *
 * Define the Pins
 * 
 * const ...
 * 
*/

const uint32_t DIR_CHANGE_LEAD_TIME_US = 10;

const uint32_t FULL_REVOLUTION_STEP_COUNT = 200;
const uint32_t MICROSTEP_FACTOR = 4;
const uint32_t PULSE_DUTY_CYCLE_PC = 50;
const uint32_t MIN_PULSE_DURATION_US = 10;

const uint32_t SPEED_TRANSITION_STEPS = 100;
const uint32_t STEPS_PER_DIR = 3200;

uint32_t currentSpeedRpm = 0;
uint32_t stepCount = 0;

struct SerialMessage {
  size_t GetCapacity() const {return mTextCapcity; }
  char* GetText() { return mText; };
  
  private:
    static const size_t mTextCapcity = 128;
    char mText[mTextCapcity];
};

MoverDriver::MoverDriver() :
        Base() {
}

MoverDriver::~MoverDriver() {
}

void MoverDriver::Init() {
    /*
        initialize Pins
    */
}

void MoverDriver::setup() {

}

uint32_t MoverDriver::getTargetSpeedRpm() {
    /*static uint32_t appliedValue = 0;
  // Read the potentiometer value
  uint32_t analogSpeedValue = analogRead(PIN_SPEED_IN);

  // calculate and print statistics
  static uint32_t minVal = 5000;
  static uint32_t maxVal = 0;
  static uint32_t sum = 0;
  static uint32_t count = 0;

  if (count > STEPS_PER_DIR) {
    minVal = 5000;
    maxVal = 0;
    sum = 0;
    count = 0;
  }

  minVal = analogSpeedValue < minVal ? analogSpeedValue : minVal;
  maxVal = analogSpeedValue > maxVal ? analogSpeedValue : maxVal;
  sum += analogSpeedValue;
  count++;
  double avg = ((double)sum) / count;
  uint32_t devFloor = 100 - ((minVal * 100) / avg);
  uint32_t devCeil = ((maxVal * 100) / avg) - 100;

  if (count % 100 == 0) {
    // stdPrintfLn("Analog speed value: min: %d, max: %d, avg: %.1f, dev: -%d%%/+%d%%", minVal, maxVal, avg, devFloor, devCeil);
  }

  // Convert the potentiometer reading to time activePulseDurationUs
  // Assuming the potentiometer returns a value between 0 and 4095
  if (analogSpeedValue > appliedValue * 1.05 || analogSpeedValue < appliedValue * 0.95) {
    appliedValue = analogSpeedValue;
  }
  return (uint32_t) map(appliedValue, 0, 4095, 6, 300);*/
}

uint32_t MoverDriver::getPulseDurationsUs(uint32_t speedRpm) {
  static uint32_t transitionStep = 0;
  static double transitionStartSpeedRpm = 0;
  static double transitionStepSpeedRpm = 0;
  static uint32_t targetSpeedRpm = 0;

  // Check if target speed has changed and apply linear transition
  if (targetSpeedRpm != speedRpm) {
    transitionStartSpeedRpm = currentSpeedRpm;
    targetSpeedRpm = speedRpm;
    transitionStepSpeedRpm = (targetSpeedRpm - transitionStartSpeedRpm) / SPEED_TRANSITION_STEPS;
    transitionStep = 0;
  }

  // Apply linear transition for pulse duration if not yet at target pulse duration
  bool printChange = false;
  if (transitionStep <= SPEED_TRANSITION_STEPS) {
    currentSpeedRpm = transitionStep == SPEED_TRANSITION_STEPS 
     ? targetSpeedRpm
     : (uint32_t)((transitionStepSpeedRpm * transitionStep) + transitionStartSpeedRpm);
    transitionStep++;
    printChange = true; //transitionStep % 10 == 0 || transitionStep == SPEED_TRANSITION_STEPS;
  }
  
  // Calculate the duration of a complete pulse in µs
  uint32_t stepsPerMinute = (currentSpeedRpm * FULL_REVOLUTION_STEP_COUNT * MICROSTEP_FACTOR);
  uint32_t pulseDuration = stepsPerMinute != 0 ? (60e6) / stepsPerMinute : 0;

  if (printChange) {
    stdPrintfLn("Target speed: %d rpm, current speed: %d rpm (step: %d, f: %d Hz, tPulse: %d µs, spm: %d)", 
      targetSpeedRpm, currentSpeedRpm, transitionStep, pulseDuration != 0 ? (1000000 / pulseDuration) : 0, pulseDuration, stepsPerMinute);
  }

  return pulseDuration;
}

bool MoverDriver::toggleDirection(uint32_t maxStepsPerDirection) {
  /*  // check if a direction change is required
  bool changeDir = stepCount >= maxStepsPerDirection;
  if (changeDir && currentSpeedRpm == 0) {
    stepCount = 0;

    // toggle the pin
    int toggleState = digitalRead(PIN_DIR);
    digitalWrite(PIN_DIR, !toggleState);

    // direction change must be ahead of the next pulse
    delayMicroseconds(DIR_CHANGE_LEAD_TIME_US);

    // print rotation change
    stdPrintfLn("Rotation changed to %s", (toggleState == LOW ? "RIGHT" : "LEFT"));
  }
  return changeDir;*/
}

void MoverDriver::stdPrintfLn(const char* format, ...) {
    /*SerialMessage msg;

    va_list args;
    va_start(args, format);
    vsnprintf(msg.GetText(), msg.GetCapacity(), format, args);
    va_end(args);
  
    xQueueSend(serialQueue, &msg, 0);*/
}

void MoverDriver::loop() {

}
