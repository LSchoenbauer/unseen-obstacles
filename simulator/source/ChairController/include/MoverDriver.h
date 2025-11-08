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

class MoverDriver;
typedef ::std::shared_ptr<MoverDriver> MoverDriverPtr;

class MoverDriver {
	public:
        enum class Direction {
            FORWARD,
            BACKWARD
        };

        static MoverDriverPtr Create(
            uint8_t pulsePin,
            uint8_t dirPin,
            uint8_t startPin,
            uint8_t centerPin,
            uint8_t endPin
        );

		virtual ~MoverDriver();
        
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

        

    protected:
        MoverDriver(
            uint8_t pulsePin,
            uint8_t dirPin,
            uint8_t startPin,
            uint8_t centerPin,
            uint8_t endPin
        );

    private:
		void Init();
        uint32_t CalcPulseDurationUs();
        void ProcessDirection();
        void Step(uint32_t pulseDurationUs);

        static const uint32_t DEFAULT_RAMPING_STEPS;
        static const uint32_t FULL_REVOLUTION_STEP_COUNT;
        static const uint32_t PULSE_DUTY_CYCLE_PC;
        static const uint32_t MIN_PULSE_DURATION_US;

        uint8_t mPulsePin;
        uint8_t mDirPin;
        uint8_t mStartPin;
        uint8_t mCenterPin;
        uint8_t mEndPin;

        uint32_t mCurrentStep;
        uint32_t mTargetSpeed;
        uint32_t mSetTargetSpeed;
        uint32_t mCurrentSpeed;
        Direction mTargetDirection;
        Direction mCurrentDirection;

        bool mIsRamping;
        uint32_t mRampingSteps;
        uint32_t mMicrostepFactor;
};