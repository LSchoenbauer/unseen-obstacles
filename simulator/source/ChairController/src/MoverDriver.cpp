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
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <TaskMgmt.h>
#include <utils/Log.h>

const uint32_t MoverDriver::MIN_PULSE_DURATION_US = 50; // > 1 / the specified max frequency of the stepper (200kHz -> 5µs)
const uint32_t MoverDriver::MAX_PULSE_DURATION_US = 1 * 1000 * 1000; // slower than 1 Hz -> 60 RPM 
const uint32_t MoverDriver::DEBOUNCE_TIME_MS = 20;
// const uint64_t MoverDriver::COASTING_TIME_US = 2000 * 1000; // TODO Replace with the line below - this for debugging only
const uint64_t MoverDriver::COASTING_TIME_US = 100 * 1000; // 3 frames at 30 fps = 100 ms

// ISR Debugging, comment out to disable
#define ISR_DBG_ENABLED

#ifdef ISR_DBG_ENABLED
    #define IsrDbgReset() digitalWrite(BUILTIN_LED, LOW)
    #define IsrDbgBlink(state, divider) IsrDbgBlinkFn(state, divider)
    #define IsrDbg(state) IsrDbgLedFn(state)
#else
    #define IsrDbgReset()
    #define IsrDbgBlink(state, divider)
    #define IsrDbg(state)
#endif

MoverDriver::MoverDriver(MoverDriverCfgPtr moverDriverCfg) : 
            mDriverTask(nullptr),
            mDriverQueue(nullptr),
            mDriverMutex(nullptr),
            mPulseTimer(nullptr),
            mPulseIsrMutex(portMUX_INITIALIZER_UNLOCKED),
            mCfg(moverDriverCfg), 
            mCurrentStep(0), 
            mTargetSpeed(0), 
            mSetTargetSpeed(0), 
            mCurrentSpeed(0), 
            mTargetDirection(Direction::FORWARD), 
            mCurrentDirection(Direction::FORWARD), 
            mIsAtTop(false), 
            mIsAtCenter(false), 
            mIsAtBottom(false), 
            mTopPosition(0), 
            mCenterPosition(0), 
            mBottomPosition(0),
            mIsRamping(false),
            mLastMoverTriggerTimeUs(0),
            mLastPulseDurationUs(0),
            mTransitionStep(0),
            mRampingStartSpeed(0.),
            mRampingStepSpeedDif(0.),
            mTopSwitchData(),
            mCenterSwitchData(),
            mBottomSwitchData()
            
{
    Init();
}

MoverDriver::~MoverDriver() {
    // task will stop and delete itself
    LogDbg("%s: destructing", mCfg != nullptr ? mCfg->GetLabel(): "NULL");
    mDriverTask = nullptr;
    if (mPulseTimer != nullptr) {
        timerEnd(mPulseTimer);
        mPulseTimer = nullptr;
    }
    // cleanup task environment
    if (mDriverQueue != nullptr) {
        vQueueDelete(mDriverQueue);
        mDriverQueue = nullptr;
    }
    if (mDriverMutex != nullptr) {
        vSemaphoreDelete(mDriverMutex);
        mDriverMutex = nullptr;
    }
}

MoverDriverPtr MoverDriver::Create(MoverDriverCfgPtr moverDriverCfg) {
    return ::std::shared_ptr<MoverDriver>(new MoverDriver(
        moverDriverCfg
    ));
}

void MoverDriver::Init() {
    IsrDbgReset();
    LogDbg("%s: Initializing MoverDriver", mCfg->GetLabel() );

    pinMode(mCfg->GetPulsePin(), OUTPUT);
    pinMode(mCfg->GetDirPin(), OUTPUT);
    digitalWrite(mCfg->GetPulsePin(), LOW);
    digitalWrite(mCfg->GetDirPin(), LOW);

    pinMode(mCfg->GetTopSwitchPin(), INPUT_PULLUP);
    pinMode(mCfg->GetCenterSwitchPin(), INPUT_PULLUP);
    pinMode(mCfg->GetBottomSwitchPin(), INPUT_PULLUP);
    static uint8_t timerId = 0;
    
    mTopSwitchData = {
        this, mCfg->GetTopSwitchPin(), false, &mIsAtTop
    };
    attachInterruptArg(mCfg->GetTopSwitchPin(), MoverDriver::OnPinChangeStatic, &mTopSwitchData, CHANGE);


    mCenterSwitchData = {
        this, mCfg->GetCenterSwitchPin(), false, &mIsAtCenter
    };
    attachInterruptArg(mCfg->GetCenterSwitchPin(), MoverDriver::OnPinChangeStatic, &mCenterSwitchData, CHANGE);

    mBottomSwitchData = {
        this, mCfg->GetBottomSwitchPin(), false, &mIsAtBottom
    };
    attachInterruptArg(mCfg->GetBottomSwitchPin(), MoverDriver::OnPinChangeStatic, &mBottomSwitchData, CHANGE);

    mPulseTimer = timerBegin(timerId++, 80, true); // 80 MHz / 80 = 1 MHz -> 1 tick = 1 µs
    timerAlarmDisable(mPulseTimer);
    AttachTimerIsr(mPulseTimer, MoverDriver::OnPulseTimerStatic, this);

    LogDbg("%s: Switchpins: %d %d %d", mCfg->GetLabel(), mCfg->GetTopSwitchPin(), mCfg->GetCenterSwitchPin(), mCfg->GetBottomSwitchPin());

    CreateDriverTask();
}

void MoverDriver::CreateDriverTask() {
    if (mDriverMutex == nullptr) {
        mDriverMutex = xSemaphoreCreateMutex();
    }

    if (mDriverQueue == nullptr) {
        mDriverQueue = xQueueCreate(8, sizeof(DriveParams));
    }

    // create the controller task on core 0
    const char* label = mCfg->GetLabel();
    if (mDriverTask == nullptr) {
        mDriverTask = TaskMgmt::CreateTask(
            TaskMgmt::TaskId::STEPPER_DRIVER,
            [](void* pvParameters) { // Task function - lambda
                MoverDriver* driver = static_cast<MoverDriver*>(pvParameters);
                LogDbg("%s: Driver task started", driver->mCfg->GetLabel());
                driver->Drive();
            },
            this,
            label
        );
        // TaskMgmt::TaskConfig cfg = TaskMgmt::GetConfig(TaskMgmt::TaskId::STEPPER_DRIVER);
        // BaseType_t res = xTaskCreatePinnedToCore(
        //     [](void* pvParameters) { // Task function - lambda
        //         MoverDriver* driver = static_cast<MoverDriver*>(pvParameters);
        //         LogDbg("%s: Driver task started", driver->mCfg->GetLabel());
        //         driver->Drive();
        //     },
        //     label,             // Task name
        //     cfg.stackDepth,    // Stacksize
        //     this,              // Parameter - reference to the instance
        //     cfg.priority,      // Priority
        //     &mDriverTask,      // Task handle
        //     cfg.core           // pin to core 0
        // );
        // if (res != pdPASS) {
        //     LogError("%s: Failed to create MoverDriver command task", label);
        // }
    } else {
        LogWarn("%s: Skipped recreation of MoverDriver command task", label);
    }
}

void MoverDriver::Drive() {
    while (mDriverTask != nullptr) {
        LogDbg("%s driving", mCfg->GetLabel());
        RetrieveDriveParams();
        CalcStepperValues();
        vTaskDelay(pdMS_TO_TICKS(50)); // recalculate every 5 ms
    }
    vTaskDelete(NULL);
    LogDbg("in drive task over");
}

void MoverDriver::RetrieveDriveParams() {
    DriveParams driveParams = {};
    if (xQueueReceive(mDriverQueue, &driveParams, 0) == pdTRUE) {
        // don't wait for params
        if (driveParams.mIsSpeedValid) {
            mIsRamping = (mSetTargetSpeed == driveParams.mSpeedRpm ? mIsRamping : false);
            mSetTargetSpeed = driveParams.mSpeedRpm;
        }
        if (driveParams.mDirection != Direction::NONE) {
            mIsRamping = (mTargetDirection == driveParams.mDirection ? mIsRamping : false);
            mTargetDirection = driveParams.mDirection;
        }
        LogDbg("%s: applied direction: %s, speed: %d rpm", mCfg->GetLabel(), 
            MoverDriver::DirectionToString(driveParams.mDirection),
            (driveParams.mSpeedRpm < UINT32_MAX ? driveParams.mSpeedRpm : -1));
    } else {
        LogDbg("%s: in retrievedriveparams else", mCfg->GetLabel());
    }
}

void MoverDriver::SetSpeedRpm(uint32_t speedRpm) {
    SetSpeedAndDirection(speedRpm, Direction::NONE);
}

void MoverDriver::SetDirection(Direction direction) {
    SetSpeedAndDirection(UINT32_MAX, direction);
}

void MoverDriver::SetSpeedAndDirection(uint32_t speedRpm, Direction direction) {
    if (xSemaphoreTake(mDriverMutex, pdMS_TO_TICKS(10)) == pdTRUE) {
        DriveParams driveParams;
        if (speedRpm < UINT32_MAX) {
            driveParams.SetSpeed(speedRpm);
        }
        driveParams.SetDirection(direction);
        xQueueSend(mDriverQueue, &driveParams, portMAX_DELAY);
        LogDbg("%s: queued  direction: %s, speed: %d rpm", mCfg->GetLabel(), 
            MoverDriver::DirectionToString(driveParams.mDirection),
            (driveParams.mSpeedRpm < UINT32_MAX ? driveParams.mSpeedRpm : -1));

        // reset the stepper running period
        mLastMoverTriggerTimeUs = esp_timer_get_time(); // ISR safe time in µs

        xSemaphoreGive(mDriverMutex);
    }
}

uint32_t MoverDriver::GetCurrentSpeedRpm() {
    return mCurrentSpeed;
}

MoverDriver::Direction MoverDriver::GetCurrentDirection() {
    return mCurrentDirection;
}

uint32_t MoverDriver::GetCurrentPosition() {
    return mCurrentStep;
}

bool MoverDriver::IsAtTop() {
    bool isAtTop = false;
    portENTER_CRITICAL(&mPulseIsrMutex);
    isAtTop = mIsAtTop;
    portEXIT_CRITICAL(&mPulseIsrMutex);
    return isAtTop;
}

bool MoverDriver::IsAtCenter() {
    bool isAtCenter = false;
    portENTER_CRITICAL(&mPulseIsrMutex);
    isAtCenter = mIsAtCenter;
    portEXIT_CRITICAL(&mPulseIsrMutex);
    return isAtCenter;
}

bool MoverDriver::IsAtBottom() {
    bool isAtBottom = false;
    portENTER_CRITICAL(&mPulseIsrMutex);
    isAtBottom = mIsAtBottom;
    portEXIT_CRITICAL(&mPulseIsrMutex);
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

void MoverDriver::CalibratePositions() {

}

void MoverDriver::CalcStepperValues() {
    ProcessDirection();
    //TODO Remove - 1000 just for testing
    // static uint32_t mLastPulseDurationUs = 0;
    // uint32_t pulseDurationUs = 1000; 
    
    uint32_t durationSinceLastTriggerUs = esp_timer_get_time() - mLastMoverTriggerTimeUs;
    if (((durationSinceLastTriggerUs >= COASTING_TIME_US)
            || (mCurrentDirection == Direction::FORWARD && IsAtTop())
            || (mCurrentDirection == Direction::BACKWARD && IsAtBottom())))
  {
        if (mSetTargetSpeed != 0) {
            mIsRamping = false;
            LogDbg("%s: Limit switch reached, stopping stepper (dir: %s, atTop: %d, atBottom: %d, timeout: %s)", 
                mCfg->GetLabel(),
                MoverDriver::DirectionToString(mCurrentDirection),
                IsAtTop(), IsAtBottom(),
                (durationSinceLastTriggerUs >= COASTING_TIME_US) ? "YES" : "NO"
            );
        }
        mSetTargetSpeed = 0;
    }

    uint32_t pulseDurationUs = CalcPulseDurationUs();
    
    if (pulseDurationUs <= MIN_PULSE_DURATION_US) {
        pulseDurationUs = MIN_PULSE_DURATION_US;
    }

    if (pulseDurationUs > MAX_PULSE_DURATION_US) {
        // too slow -> stop it completely
        timerAlarmDisable(mPulseTimer);
        IsrDbgReset();

        // apply direction
        uint8_t pin = mCfg->GetDirPin();
        digitalWrite(pin, mCurrentDirection == Direction::FORWARD ? HIGH : LOW); // might be vice versa

        if (mLastPulseDurationUs <= MAX_PULSE_DURATION_US) {
            LogDbg("%s: Stepper stopped (last %d, MAX: %d)", mCfg->GetLabel()
                , mLastPulseDurationUs, MAX_PULSE_DURATION_US);
            mLastPulseDurationUs = MAX_PULSE_DURATION_US + 1;
        }
    } else {
        uint32_t dutyDurationUs = pulseDurationUs / 2;
        timerAlarmWrite(mPulseTimer, dutyDurationUs, true);
        timerAlarmEnable(mPulseTimer);
        if (mLastPulseDurationUs != pulseDurationUs) {
            LogDbg("%s: Pulse timer period: %d µs (last %d)", mCfg->GetLabel(), pulseDurationUs
            , mLastPulseDurationUs);
            mLastPulseDurationUs = pulseDurationUs;
        }
    }
}

uint32_t MoverDriver::CalcPulseDurationUs() {
    // TODO Cleanup
    // static uint32_t mTransitionStep = 0;
    // static double mRampingStartSpeed = 0;
    // static double mRampingStepSpeedDif = 0;
    bool hasChanged = false;

    //  apply linear ramping if speed changed
    if (mTargetSpeed != mCurrentSpeed) {
        if (!mIsRamping) {
            mRampingStartSpeed = mCurrentSpeed;
            mIsRamping = true;
            mTransitionStep = 0;
            mRampingStepSpeedDif = (mTargetSpeed - mRampingStartSpeed) / mCfg->GetRampingSteps();
        }

        mCurrentSpeed = mTransitionStep == mCfg->GetRampingSteps() 
            ? mTargetSpeed
            : (uint32_t) ((mRampingStepSpeedDif * mTransitionStep) + mRampingStartSpeed);
        mTransitionStep++;
        hasChanged = true;
    }
    
    // Calculate the duration of a complete pulse in µs
    uint32_t stepsPerMinute = (mCurrentSpeed * mCfg->GetFullRevolutionStepCount() * mCfg->GetMicrostepFactor());
    uint32_t pulseDurationUs = stepsPerMinute != 0 ? (60e6) / stepsPerMinute : INT_MAX;

    if (hasChanged) {
        LogDbg("%s: Target speed: %d rpm, current speed: %d rpm (step: %d, f: %d Hz, tPulse: %d µs, spm: %d)", 
            mCfg->GetLabel(), mTargetSpeed, mCurrentSpeed, mTransitionStep, pulseDurationUs != 0 ? (1000000 / pulseDurationUs) : 0, pulseDurationUs, stepsPerMinute);
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

void IRAM_ATTR MoverDriver::Step() {
    // Increment the counter and set the time of ISR
    portENTER_CRITICAL_ISR(&mPulseIsrMutex);

    // TODO: Cleanup - moved to CalcStepperValues()
    // uint32_t durationSinceLastTriggerUs = esp_timer_get_time() - mLastMoverTriggerTimeUs;
    // if (((mCurrentDirection == Direction::FORWARD && !IsAtTop())
    //         || (mCurrentDirection == Direction::BACKWARD && !IsAtBottom()))
    //         && (durationSinceLastTriggerUs < COASTING_TIME_US)) {

        uint8_t pin = mCfg->GetPulsePin();
        uint8_t pinState = digitalRead(pin);
        // TODO Remove fake implementation
        // static unit8_t fakeState = LOW;
        // pinState = fakeState;
        // fakeState = fakeState == LOW ? HIGH : LOW;
        IsrDbgBlink(pinState, 25);

        digitalWrite(pin, pinState == LOW ? HIGH : LOW);
        mCurrentStep += pinState == HIGH ?
                mCurrentDirection == Direction::FORWARD ? 1 : -1
                : 0;
    
    // TODO: Cleanup - moved to CalcStepperValues()           
    // } else {
    //     timerAlarmDisable(mPulseTimer);
    //     IsrDbgReset();
    // }

    portEXIT_CRITICAL_ISR(&mPulseIsrMutex);
    // TODO: Remove: trigger task execution - not needed
    // xSemaphoreGiveFromISR(mTimerSemaphore, NULL);
}

bool IRAM_ATTR MoverDriver::OnPulseTimerStatic(void* userData) {
    MoverDriver* self = static_cast<MoverDriver*>(userData);
    self->Step();
    return false;
}

void IRAM_ATTR MoverDriver::OnPinChangeStatic(void* userData) {
    if (userData != nullptr) {
            PinIsrData* data = static_cast<PinIsrData*>(userData);
            data->moverDriver->OnPinChange(data);
    }
}

void MoverDriver::OnPinChange(PinIsrData* data) {
    portENTER_CRITICAL_ISR(&mPulseIsrMutex);
    if (data != nullptr) {
        data->lastState = digitalRead(data->pin) == LOW;
        *(data->state) = data->lastState;
        //timerAlarmWrite(mDeBounceTimer, DEBOUNCE_TIME_MS, false);
        //timerAlarmEnable(mDeBounceTimer);
    }
    IsrDbg(data->lastState);
    portEXIT_CRITICAL_ISR(&mPulseIsrMutex);
}

bool IRAM_ATTR MoverDriver::OnPinDebounceStatic(void* userData) {
    if (userData != nullptr) {
            PinIsrData* data = static_cast<PinIsrData*>(userData);
            data->moverDriver->OnPinDebounce(data);
    }
    return false;
}

void MoverDriver::OnPinDebounce(PinIsrData* data) {
    portENTER_CRITICAL_ISR(&mPulseIsrMutex);
    if (data != nullptr) {
        bool currentState = digitalRead(data->pin);
        if ( data->lastState == currentState) {
            *(data->state) = currentState;
        }
        //timerAlarmDisable(mDeBounceTimer);
    }
    portEXIT_CRITICAL_ISR(&mPulseIsrMutex);
}

void MoverDriver::AttachTimerIsr(hw_timer_t* timer, bool(*fn)(void*), void* fnArgs) {
    HwTimer* hwTimer = reinterpret_cast<HwTimer*>(timer);
    timer_isr_callback_add((timer_group_t)hwTimer->group, (timer_idx_t)hwTimer->num, fn, fnArgs, 0);
}

const char* MoverDriver::DirectionToString(Direction dir) {
    switch (dir) {
        case Direction::FORWARD:  return "FORWARD";
        case Direction::BACKWARD: return "BACKWARD";
        case Direction::NONE:     return "NONE";
        default:                  return "INVALID";
    }
}

void IRAM_ATTR MoverDriver::IsrDbgBlinkFn(bool state, uint32_t divider) {
    // only one instance!
    static bool ledState = state;
    static uint32_t counter = 0;
    if (ledState == state) {
        counter++; // to change LED state after 'divider' occurrences
    }

    if (counter >= divider && ledState != state) {
        // toggles LED upon the next different state after 'divider' is reached
        digitalWrite(BUILTIN_LED, state ? HIGH : LOW);
        counter = 0;
        ledState = state;
    }
}


void IRAM_ATTR MoverDriver::IsrDbgLedFn(bool state) {
    digitalWrite(BUILTIN_LED, state ? HIGH : LOW);
}
