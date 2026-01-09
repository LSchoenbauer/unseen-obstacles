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
#include "driver/timer.h"

#include <utils/Log.h>

const uint32_t MoverDriver::DEFAULT_RAMPING_STEPS = 50;
const uint32_t MoverDriver::FULL_REVOLUTION_STEP_COUNT = 200;
const uint32_t MoverDriver::PULSE_DUTY_CYCLE_PC = 50;
const uint32_t MoverDriver::MIN_PULSE_DURATION_US = 10;
const uint32_t MoverDriver::DEBOUNCE_TIME_MS = 20;
const uint32_t MoverDriver::COASTING_TIME_MS = 100; // 3 frames at 30 fps

MoverDriver::MoverDriver(MoverDriverCfgPtr moverDriverCfg) : mMoverDriverCfg(moverDriverCfg), mCurrentStep(0), mTargetSpeed(0), mSetTargetSpeed(0), mCurrentSpeed(0), mTargetDirection(Direction::FORWARD), mCurrentDirection(Direction::FORWARD), mIsRamping(false), mRampingSteps(DEFAULT_RAMPING_STEPS), mMicrostepFactor(1), mIsAtTop(false), mIsAtCenter(true), mIsAtBottom(false), mTopPosition(0), mCenterPosition(0), mBottomPosition(1), mStepperTimer(NULL), mTimerMux(portMUX_INITIALIZER_UNLOCKED), mLastMoverTriggerTime(0), mTimerSemaphore(NULL) {
   Init();
}

MoverDriver::~MoverDriver(){}

MoverDriverPtr MoverDriver::Create(MoverDriverCfgPtr moverDriverCfg) {
	return ::std::shared_ptr<MoverDriver>(new MoverDriver(
    moverDriverCfg
  ));
}

void MoverDriver::Init() {
  mTimerSemaphore = xSemaphoreCreateBinary(); // TODO: semaphore usage
  pinMode(mMoverDriverCfg->GetPulsePin(), OUTPUT);
  pinMode(mMoverDriverCfg->GetDirPin(), OUTPUT);
  digitalWrite(mMoverDriverCfg->GetPulsePin(), LOW);
  digitalWrite(mMoverDriverCfg->GetDirPin(), LOW);

  pinMode(mMoverDriverCfg->GetTopSwitchPin(), INPUT_PULLUP);
  pinMode(mMoverDriverCfg->GetCenterSwitchPin(), INPUT_PULLUP);
  pinMode(mMoverDriverCfg->GetBottomSwitchPin(), INPUT_PULLUP);

  // Timer 0 auf Core 0
  mStepperTimer = timerBegin(0, 80, true); // 80 MHz / 80 = 1 MHz -> 1 tick = 1 µs
  timerAlarmDisable(mStepperTimer);
  AttachTimerIsr(mStepperTimer, MoverDriver::OnPulseTimerStatic, this);

  mDeBounceTimer = timerBegin(1, 80 * 1000, true);
  timerAlarmDisable(mDeBounceTimer);
  static PinIsrData topSwitchData = {
    this, mMoverDriverCfg->GetTopSwitchPin(), false, &mIsAtTop
  };
  attachInterruptArg(mMoverDriverCfg->GetTopSwitchPin(), MoverDriver::OnPinChangeStatic, &topSwitchData, CHANGE);
  AttachTimerIsr(mDeBounceTimer, MoverDriver::OnPinDebounceStatic, &topSwitchData);

  static PinIsrData centerSwitchData = {
    this, mMoverDriverCfg->GetCenterSwitchPin(), false, &mIsAtCenter
  };
  attachInterruptArg(mMoverDriverCfg->GetCenterSwitchPin(), MoverDriver::OnPinChangeStatic, &centerSwitchData, CHANGE);
  AttachTimerIsr(mDeBounceTimer, MoverDriver::OnPinDebounceStatic, &centerSwitchData);

  static PinIsrData bottomSwitchData = {
    this, mMoverDriverCfg->GetBottomSwitchPin(), false, &mIsAtBottom
  };
  attachInterruptArg(mMoverDriverCfg->GetBottomSwitchPin(), MoverDriver::OnPinChangeStatic, &bottomSwitchData, CHANGE);
  AttachTimerIsr(mDeBounceTimer, MoverDriver::OnPinDebounceStatic, &bottomSwitchData);
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
  bool isAtTop = false;
  portENTER_CRITICAL(&mTimerMux);
  isAtTop = mIsAtTop;
  portEXIT_CRITICAL(&mTimerMux);
  return isAtTop;
 }

bool MoverDriver::IsAtCenter() {
  bool isAtCenter = false;
  portENTER_CRITICAL(&mTimerMux);
  isAtCenter = mIsAtCenter;
  portEXIT_CRITICAL(&mTimerMux);
  return isAtCenter;
}

bool MoverDriver::IsAtBottom() {
  bool isAtBottom = false;
  portENTER_CRITICAL(&mTimerMux);
  isAtBottom = mIsAtBottom;
  portEXIT_CRITICAL(&mTimerMux);
  return isAtBottom;
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
  mLastMoverTriggerTime = millis();
  timerAlarmWrite(mStepperTimer, pulseDurationUs, true);
  timerAlarmEnable(mStepperTimer);
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
  uint32_t durationSinceLastTrigger = millis() - mLastMoverTriggerTime;
  if (((mCurrentDirection == Direction::FORWARD && !IsAtTop())
      || (mCurrentDirection == Direction::BACKWARD && !IsAtBottom()))
      && (durationSinceLastTrigger < COASTING_TIME_MS)) {

    uint8_t pin = mMoverDriverCfg->GetPulsePin();
    uint8_t readPin = digitalRead(pin);

    digitalWrite(pin, readPin == LOW ? HIGH : LOW);
    mCurrentStep += readPin == HIGH ?
          mCurrentDirection == Direction::FORWARD ? 1 : -1
          : 0;
  } else {
    timerAlarmDisable(mStepperTimer);
  }
  portEXIT_CRITICAL_ISR(&mTimerMux);
  
  // Give a semaphore that we can check in the loop
  xSemaphoreGiveFromISR(mTimerSemaphore, NULL);
  // It is safe to use digitalRead/Write here if you want to toggle an output

}

bool IRAM_ATTR MoverDriver::OnPulseTimerStatic(void* userData) {
  MoverDriver* self = static_cast<MoverDriver*>(userData);
  self->Step();
  return false;
}

void ARDUINO_ISR_ATTR MoverDriver::OnPinChangeStatic(void* userData) {
  if (userData != nullptr) {
      PinIsrData* data = static_cast<PinIsrData*>(userData);
      data->moverDriver->OnPinChange(data);
  }
}

void MoverDriver::OnPinChange(PinIsrData* data) {
  portENTER_CRITICAL_ISR(&mTimerMux);
  if (data != nullptr) {
    data->lastState = digitalRead(data->pin) == HIGH;
    timerAlarmWrite(mDeBounceTimer, DEBOUNCE_TIME_MS, false);
    timerAlarmEnable(mDeBounceTimer);
  }
  portEXIT_CRITICAL_ISR(&mTimerMux);
}

bool IRAM_ATTR MoverDriver::OnPinDebounceStatic(void* userData) {
  if (userData != nullptr) {
      PinIsrData* data = static_cast<PinIsrData*>(userData);
      data->moverDriver->OnPinDebounce(data);
  }
  return false;
}

void MoverDriver::OnPinDebounce(PinIsrData* data) {
  portENTER_CRITICAL_ISR(&mTimerMux);
  if (data != nullptr) {
    bool currentState = digitalRead(data->pin);
    if ( data->lastState == currentState) {
      *(data->state) = currentState;
    }
    timerAlarmDisable(mDeBounceTimer);
  }
  portEXIT_CRITICAL_ISR(&mTimerMux);

}

void MoverDriver::AttachTimerIsr(hw_timer_t* timer, bool(*fn)(void*), void* fnArgs) {
  HwTimer* hwTimer = reinterpret_cast<HwTimer*>(timer);
  timer_isr_callback_add((timer_group_t)hwTimer->group, (timer_idx_t)hwTimer->num, fn, fnArgs, 0);
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

