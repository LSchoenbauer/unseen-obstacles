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

const uint32_t MoverDriver::DEFAULT_RAMPING_STEPS = 50;
const uint32_t MoverDriver::FULL_REVOLUTION_STEP_COUNT = 200;
const uint32_t MoverDriver::PULSE_DUTY_CYCLE_PC = 50;
const uint32_t MoverDriver::MIN_PULSE_DURATION_US = 10;

MoverDriver::MoverDriver(
  uint8_t pulsePin,
  uint8_t dirPin,
  uint8_t startPin,
  uint8_t centerPin,
  uint8_t endPin) : mPulsePin(pulsePin), mDirPin(dirPin), mStartPin(startPin), mCenterPin(centerPin), mEndPin(endPin), mCurrentStep(0), mTargetSpeed(0), mSetTargetSpeed(0), mCurrentSpeed(0), mTargetDirection(Direction::FORWARD), mCurrentDirection(Direction::FORWARD), mIsRamping(false), mRampingSteps(DEFAULT_RAMPING_STEPS), mMicrostepFactor(1) {
   Init();
}

MoverDriver::~MoverDriver() {
}

void MoverDriver::Init() {
  pinMode(mPulsePin, OUTPUT);
  pinMode(mDirPin, OUTPUT);
  digitalWrite(mPulsePin, LOW);
  digitalWrite(mDirPin, LOW);

  pinMode(mStartPin, INPUT_PULLUP);
  pinMode(mCenterPin, INPUT_PULLUP);
  pinMode(mEndPin, INPUT_PULLUP);
}

void MoverDriver::SetSpeedRpm(uint32_t speedRpm) {
  mSetTargetSpeed = speedRpm;
  mIsRamping = false;
}

uint32_t MoverDriver::GetCurrentSpeedRpm() {
  return mCurrentSpeed;
}

void MoverDriver::SetDirection(Direction direction) {
  mTargetDirection = direction;
  mIsRamping = false;
}

MoverDriver::Direction MoverDriver::GetCurrentDirection() {
  return mCurrentDirection;
}

uint32_t MoverDriver::GetCurrentPosition() {
  return mCurrentStep;
}

void MoverDriver::SetRampingSteps(uint32_t rampingSteps) {
  mRampingSteps = rampingSteps;
}

uint32_t MoverDriver::GetRampingSteps() {
  return mRampingSteps;
}

void MoverDriver::SetMicrostepFactor(uint32_t microstepFactor) {
  mMicrostepFactor = microstepFactor;
}

uint32_t MoverDriver::GetMicrostepFactor() {
  return mMicrostepFactor;
}

void MoverDriver::Drive() {
  ProcessDirection();
  uint32_t pulseDurationUs = CalcPulseDurationUs();

  // proceed the stepper
  Step(pulseDurationUs);
}

uint32_t MoverDriver::CalcPulseDurationUs() {
  static uint32_t transitionStep = 0;
  static double rampingStartSpeed = 0;
  static double rampingStepSpeedDif = 0;
  bool hasChanged = false;

  //  apply linear ramping if speed changed
  if (mTargetSpeed != mCurrentSpeed) {
    if (!mIsRamping) {
      rampingStartSpeed = mCurrentSpeed;
      mIsRamping = true;
      transitionStep = 0;
      rampingStepSpeedDif = (mTargetSpeed - rampingStartSpeed) / mRampingSteps;
    }

    mCurrentSpeed = transitionStep == mRampingSteps 
     ? mTargetSpeed
     : (uint32_t) ((rampingStepSpeedDif * transitionStep) + rampingStartSpeed);
    transitionStep++;
    hasChanged = true;
  }
  
  // Calculate the duration of a complete pulse in µs
  uint32_t stepsPerMinute = (mCurrentSpeed * FULL_REVOLUTION_STEP_COUNT * mMicrostepFactor);
  uint32_t pulseDurationUs = stepsPerMinute != 0 ? (60e6) / stepsPerMinute : 0;

  if (hasChanged) {
    LogDbg("Target speed: %d rpm, current speed: %d rpm (step: %d, f: %d Hz, tPulse: %d µs, spm: %d)", 
      mTargetSpeed, mCurrentSpeed, transitionStep, pulseDurationUs != 0 ? (1000000 / pulseDurationUs) : 0, pulseDurationUs, stepsPerMinute);
  }

  return pulseDurationUs;
}

void MoverDriver::ProcessDirection() {
  if (mTargetDirection != mCurrentDirection) {
    mTargetSpeed = 0;
    if (mCurrentSpeed == 0) {
      mCurrentDirection = mTargetDirection;
      mTargetSpeed = mSetTargetSpeed;
    }
  } else {
    mTargetSpeed = mSetTargetSpeed;
  }
}

void MoverDriver::Step(uint32_t pulseDurationUs) {
  if (pulseDurationUs >= MIN_PULSE_DURATION_US) {
    uint32_t highDurationUs = (pulseDurationUs * PULSE_DUTY_CYCLE_PC) / 100;
    uint32_t lowDurationUs = pulseDurationUs - highDurationUs;

    digitalWrite(mPulsePin, HIGH);
    delayMicroseconds(highDurationUs);
    digitalWrite(mPulsePin, LOW);
    delayMicroseconds(lowDurationUs);
    mCurrentStep += mCurrentDirection == Direction::FORWARD ? 1 : -1;
  }
}
