/* 

    Pin asignment for stepper and switches
    direction and speed for stepper
    position detection and reaction
    soft start
    triggerable movement

*/

#include "MoverDriver.h"
#include "Arduino.h"
#include "esp_timer.h"

#include <utils/Log.h>

const uint32_t MoverDriver::DEFAULT_RAMPING_STEPS = 50;
const uint32_t MoverDriver::FULL_REVOLUTION_STEP_COUNT = 200;
const uint32_t MoverDriver::PULSE_DUTY_CYCLE_PC = 50;
const uint32_t MoverDriver::MIN_PULSE_DURATION_US = 10;

MoverDriver::MoverDriver(MoverDriverCfgPtr moverDriverCfg) : mMoverDriverCfg(moverDriverCfg), mCurrentStep(0), mTargetSpeed(0), mSetTargetSpeed(0), mCurrentSpeed(0), mTargetDirection(Direction::FORWARD), mCurrentDirection(Direction::FORWARD), mIsRamping(false), mRampingSteps(DEFAULT_RAMPING_STEPS), mMicrostepFactor(1), mIsAtTop(false), mIsAtCenter(true), mIsAtBottom(false), mTopPosition(0), mCenterPosition(0), mBottomPosition(1), mTimer(NULL), mTimerMux(portMUX_INITIALIZER_UNLOCKED) {
   Init();
}

MoverDriverPtr MoverDriver::Create(MoverDriverCfgPtr moverDriverCfg) {
	return ::std::shared_ptr<MoverDriver>(new MoverDriver(
    moverDriverCfg
  ));
}

void MoverDriver::Init() {
  pinMode(mMoverDriverCfg->GetPulsePin(), OUTPUT);
  pinMode(mMoverDriverCfg->GetDirPin(), OUTPUT);
  digitalWrite(mMoverDriverCfg->GetPulsePin(), LOW);
  digitalWrite(mMoverDriverCfg->GetDirPin(), LOW);

  pinMode(mMoverDriverCfg->GetTopSwitchPin(), INPUT_PULLUP);
  pinMode(mMoverDriverCfg->GetCenterSwitchPin(), INPUT_PULLUP);
  pinMode(mMoverDriverCfg->GetBottomSwitchPin(), INPUT_PULLUP);

  // Timer 0 auf Core 0
  mTimer = timerBegin(0, 80, true); // 80 MHz / 80 = 1 MHz -> 1 tick = 1 µs
  timerAttachInterruptArg(mTimer, MoverDriver::OnPulseTimerStatic, this);
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

bool MoverDriver::IsAtTop() {
  // pin vom switch abfragen. Wenn low is dann is man an der position.
  return mIsAtTop;
  // wenn bottom switch zu geht setzen den step count 0
  // dann wenn isAtCenter zum ersten mal true liefert merken wir uns den step count weil dann simma center. dann bis er wieder aufgeht -1 bereich  für center und dann weiter bis top auch zu geht. diese werte liefern wir mit get center get top etc. position.
}

bool MoverDriver::IsAtCenter() {
  return mIsAtCenter;
}

bool MoverDriver::IsAtBottom() {
  mIsAtBottom = digitalRead(mMoverDriverCfg->GetBottomSwitchPin()) == HIGH;
  return mIsAtBottom;
}

uint32_t MoverDriver::GetTopPosition() {
  return mTopPosition;
}

uint32_t MoverDriver::GetCenterPosition() {
  return mCenterPosition;
}

uint32_t MoverDriver::GetBottomPosition() {
  return mBottomPosition;
}

void MoverDriver::CalibratePositionOfWheelchair() {

}

void MoverDriver::Drive() {
  ProcessDirection();
  uint32_t pulseDurationUs = CalcPulseDurationUs();

  if (pulseDurationUs >= MIN_PULSE_DURATION_US) {
    uint32_t highDurationUs = (pulseDurationUs * PULSE_DUTY_CYCLE_PC) / 100;
    uint32_t lowDurationUs = pulseDurationUs - highDurationUs;
  } else {
    pulseDurationUs = MIN_PULSE_DURATION_US;
  }
  timerAlarmWrite(mTimer, pulseDurationUs, true);

  // TODO
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

void ARDUINO_ISR_ATTR MoverDriver::Step() {
  // Increment the counter and set the time of ISR
  portENTER_CRITICAL_ISR(&mTimerMux);
  uint8_t pin = mMoverDriverCfg->GetPulsePin();
  uint8_t readPin = digitalRead(pin);

  digitalWrite(pin, readPin == LOW ? HIGH : LOW);
  mCurrentStep += readPin == HIGH ?
        mCurrentDirection == Direction::FORWARD ? 1 : -1
        : 0;
  portEXIT_CRITICAL_ISR(&mTimerMux);
  
  // Give a semaphore that we can check in the loop
  xSemaphoreGiveFromISR(timerSemaphore, NULL);
  // It is safe to use digitalRead/Write here if you want to toggle an output

}

void ARDUINO_ISR_ATTR MoverDriver::OnPulseTimerStatic(void* userData) {
  MoverDriver* self = static_cast<MoverDriver*>(userData);
  self->Step();
}

bool MoverDriver::ReadSwitchState() {
  return digitalRead(mMoverDriverCfg->GetBottomSwitchPin()) == HIGH;

}

volatile uint32_t toggle_interval_us = 10; // 100 kHz
volatile bool toggle_pin = false;

/*void loop() {
  // z.B. Frequenzänderung vom anderen Core
  delay(1000);
  portENTER_CRITICAL(&timerMux);
  toggle_interval_us = 5; // 200 kHz
  timerAlarmWrite(timer0, toggle_interval_us, true);
  portEXIT_CRITICAL(&timerMux);
}*/

