/*
 * ===============================================================================
 * c) HTL Leonding
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Licensed under MIT License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the license.
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * All trademarks used in this document are property of their respective owners.
 * ===============================================================================
 */

#pragma once

#include <memory>
#include <MoverDriverCfg.h>
#include <Arduino.h>

class MoverDriver;
typedef ::std::shared_ptr<MoverDriver> MoverDriverPtr;

class MoverDriver {
    public:
        enum class Direction {
            NONE,
            FORWARD,
            BACKWARD
        };

        /**
         * Creates a shared pointer of a MoverDriver instance.
         * @param moverDriverCfg The shared pointer.
         */
        static MoverDriverPtr Create(MoverDriverCfgPtr moverDriverCfg);

        /**
         * Destructor
         */
        ~MoverDriver();
        
        /**
         * Enqueues a new target speed in revolutions per minute (rpm).
         * Note that this method is asynchronous.
         * @param speedRpm The target speed in revolutions per minute.
         */
        void SetSpeedRpm(uint32_t speedRpm);

        /**
         * Enqueues a new target direction.
         * Note that this method is asynchronous.
         * @param direction The target direction.
         */
        void SetDirection(Direction direction);

        /**
         * Enqueues a new target speed and direction.
         * Note that this method is asynchronous.
         * @param speedRpm The target speed in revolutions per minute.
         * @param direction The target direction.
         */
        void SetSpeedAndDirection(uint32_t speedRpm, Direction direction);

        /**
         * Provides the current speed of the stepper in revolutions per minute (rpm).
         * The current speed may deviate from the target speed
         * while the stepper is ramping up or down.
         * @return The current speed in rpm.
         */
        uint32_t GetCurrentSpeedRpm();

        /**
         * Provides the current direction of the stepper movement.
         * The current direction may deviate from the target direction 
         * while the stepper is ramping up or down to zero speed
         * before changing direction.
         * @return The current direction.
         */
        Direction GetCurrentDirection();

        /**
         * Determines whether the stepper is at the 'top' limit switch.
         * @return true if the stepper is at the top position, false otherwise.
         */
        bool IsAtTop();

        /** 
         * Determines whether the stepper is at the 'center' limit switch.
         * @return true if the stepper is at the center position, false otherwise.
         */
        bool IsAtCenter();

        /**
         * Determines whether the stepper is at the 'bottom' limit switch.
         * @return true if the stepper is at the bottom position, false otherwise.
         */
        bool IsAtBottom();

        /**
         * Provides the absolute step count for the 'top' position.
         * @return The step count for the top position.
         */
        uint32_t GetTopPosition();

        /**
         * Provides the absolute step count for the 'center' position.
         * @return The step count for the center position.
         */
        uint32_t GetCenterPosition();

        /**
         * Provides the absolute step count for the 'bottom' position.
         * @return The step count for the bottom position.
         */
        uint32_t GetBottomPosition();

        /**
         * Provides the current absolute position in steps.
         * @return The current position in steps.
         */
        uint32_t GetCurrentPosition();

        /**
         * Performs a calibration run to top position, 
         * then to bottom position and eventually to center position.
         */
        void CalibratePositions();

    protected:
        MoverDriver(MoverDriverCfgPtr moverDriverCfg);

    private:
        typedef struct DriveParamData {
            bool mIsSpeedValid;
            Direction mDirection;
            uint32_t mSpeedRpm;

            DriveParamData() : 
                mIsSpeedValid(false),
                mDirection(Direction::NONE),
                mSpeedRpm(false)
            {}

            void SetSpeed(uint32_t speedRpm) {
                mIsSpeedValid = true;
                mSpeedRpm = speedRpm;
            }

            void SetDirection(Direction direction) {
                mDirection = direction;
            }

        } DriveParams;

        typedef struct { 
            MoverDriver* moverDriver;
            uint8_t pin;
            bool lastState;
            bool* state;
        } PinIsrData;

        // required to workaround limitations of Arduino timer API
        typedef struct {
            uint8_t group;
            uint8_t num;
        } HwTimer;

        void Init();
        void CreateDriverTask();
        // runs the loop for driving the stepper
        void Drive();
        // retrieves speed and direction from the queue
        void RetrieveDriveParams();
        // calculates timing and pin values for stepper
        void CalcStepperValues();
        // calculates the step puls duration in microseconds
        uint32_t CalcPulseDurationUs();
        void ProcessDirection();
        void OnPinDebounce(PinIsrData* data);
        void OnPinChange(PinIsrData* data);
        void AttachTimerIsr(hw_timer_t* timer, bool(*fn)(void*), void* fnArgs);

        void IRAM_ATTR Step();
        static bool IRAM_ATTR OnPulseTimerStatic(void* userData);
        static void IRAM_ATTR OnPinChangeStatic(void* userData);
        static bool IRAM_ATTR OnPinDebounceStatic(void* userData);

        // switches the internal LED of the ESP on or off. The 'divider' decreases the frequency by 1/divider.
        void IRAM_ATTR IsrDbgBlink(bool state, uint32_t divider);

        // utility method for debugging purposes
        static const char* DirectionToString(Direction dir);

        static const uint32_t MIN_PULSE_DURATION_US;
        static const uint32_t MAX_PULSE_DURATION_US;
        static const uint32_t DEBOUNCE_TIME_MS;
        static const uint64_t COASTING_TIME_US;
    
        // the task to calculate speed and direction for the next steps
        TaskHandle_t mDriverTask;
        // the queue for configuring to the stepper task
        QueueHandle_t mDriverQueue;
        // the mutex to protect shared data
        SemaphoreHandle_t mDriverMutex;

        // the timer for generating stepper pulses
        hw_timer_t* mPulseTimer;
        // The mutex to protect shared data in timer ISRs globally
        portMUX_TYPE mPulseIsrMutex;

        // the configuration for this mover driver
        MoverDriverCfgPtr mCfg;

        uint32_t mCurrentStep;
        uint32_t mTargetSpeed;
        uint32_t mSetTargetSpeed;
        uint32_t mCurrentSpeed;
        Direction mTargetDirection;
        Direction mCurrentDirection;

        bool mIsAtTop;
        bool mIsAtCenter;
        bool mIsAtBottom;
        uint32_t mTopPosition;
        uint32_t mCenterPosition;
        uint32_t mBottomPosition;

        volatile bool mIsRamping;
        volatile uint64_t mLastMoverTriggerTimeUs;

        uint32_t mLastPulseDurationUs;
        uint32_t mTransitionStep;
        double mRampingStartSpeed;
        double mRampingStepSpeedDif;
};