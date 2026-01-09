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
//#include "esp_timer.h"
#include <Arduino.h>

class MoverDriver;
typedef ::std::shared_ptr<MoverDriver> MoverDriverPtr;

class MoverDriver {
	public:
        enum class Direction {
            FORWARD,
            BACKWARD
        };

        static MoverDriverPtr Create(MoverDriverCfgPtr moverDriverCfg);

		~MoverDriver();
        
        void SetSpeedRpm(uint32_t speedRpm);

        uint32_t GetCurrentSpeedRpm();

        void SetDirection(Direction direction);

        Direction GetCurrentDirection();

        uint32_t GetCurrentPosition();

        void Drive();

        void SetRampingSteps(uint32_t rampingSteps);

        uint32_t GetRampingSteps();

        void SetMicrostepFactor(uint32_t microstepFactor);

        uint32_t GetMicrostepFactor();

        bool IsAtTop();

        bool IsAtCenter();

        bool IsAtBottom();

        uint32_t GetTopPosition();

        uint32_t GetCenterPosition();

        uint32_t GetBottomPosition();

        void CalibratePositionOfWheelchair();
        

    protected:
        MoverDriver(MoverDriverCfgPtr moverDriverCfg);

    private:
        typedef struct { 
            MoverDriver* moverDriver;
            uint8_t pin;
            bool lastState;
            bool* state;
        } PinIsrData;
        typedef struct {
            uint8_t group;
            uint8_t num;
        } HwTimer;
		void Init();
        uint32_t CalcPulseDurationUs();
        void ProcessDirection();
        void ARDUINO_ISR_ATTR Step();
        void OnPinDebounce(PinIsrData* data);
        void OnPinChange(PinIsrData* data);
        void AttachTimerIsr(hw_timer_t* timer, bool(*fn)(void*), void* fnArgs);

        static bool IRAM_ATTR OnPulseTimerStatic(void* userData);
        static void ARDUINO_ISR_ATTR OnPinChangeStatic(void* userData);
        static bool IRAM_ATTR OnPinDebounceStatic(void* userData);

        static const uint32_t DEFAULT_RAMPING_STEPS;
        static const uint32_t FULL_REVOLUTION_STEP_COUNT;
        static const uint32_t PULSE_DUTY_CYCLE_PC;
        static const uint32_t MIN_PULSE_DURATION_US;
        static const uint32_t DEBOUNCE_TIME_MS;
        static const uint32_t COASTING_TIME_MS;
    
        hw_timer_t* mStepperTimer;
        hw_timer_t* mDeBounceTimer;

        SemaphoreHandle_t mTimerSemaphore;
        portMUX_TYPE mTimerMux;

        MoverDriverCfgPtr mMoverDriverCfg;

        uint32_t mCurrentStep;
        uint32_t mTargetSpeed;
        uint32_t mSetTargetSpeed;
        uint32_t mCurrentSpeed;
        Direction mTargetDirection;
        Direction mCurrentDirection;

        uint32_t mLastMoverTriggerTime;

        bool mIsRamping;
        uint32_t mRampingSteps;
        uint32_t mMicrostepFactor;

        bool mIsAtTop;
        bool mIsAtCenter;
        bool mIsAtBottom;
        uint32_t mTopPosition;
        uint32_t mCenterPosition;
        uint32_t mBottomPosition;
};