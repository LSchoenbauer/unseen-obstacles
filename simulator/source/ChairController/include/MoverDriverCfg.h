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

#include <stdint.h>
#include <memory>

class MoverDriverCfg;
typedef ::std::shared_ptr<MoverDriverCfg> MoverDriverCfgPtr;

/*
    Pure configuration class for a single MoverDriver instance.

    Contains only the hardware-specific configuration parameters:

        - Step/dir pins
        - Limit switches (top, center, bottom)
        - Ramping steps
        - Microstepping factor
        
    This class is a dumb data capsule, just like CommandData.
*/

class MoverDriverCfg {
public:
    ~MoverDriverCfg() {}

    static MoverDriverCfgPtr Create (
            const char* label,
            uint8_t pulsePin,
            uint8_t dirPin,
            uint8_t startPin,
            uint8_t centerPin,
            uint8_t endPin,
            uint32_t microstepFactor
        );

    static MoverDriverCfgPtr Create (
            const char* label,
            uint8_t pulsePin,
            uint8_t dirPin,
            uint8_t startPin,
            uint8_t centerPin,
            uint8_t endPin,
            uint32_t microstepFactor,
            uint32_t rampingSteps
        );

    // --- Getters -------------------------------------------------------------
    const char* GetLabel() const       { return mLabel; }

    uint8_t GetPulsePin() const        { return mPulsePin; }
    uint8_t GetDirPin() const          { return mDirPin; }
    uint8_t GetTopSwitchPin() const    { return mTopSwitchPin; }
    uint8_t GetCenterSwitchPin() const { return mCenterSwitchPin; }
    uint8_t GetBottomSwitchPin() const { return mBottomSwitchPin; }

    uint32_t GetRampingSteps() const    { return mRampingSteps; }
    uint32_t GetMicrostepFactor() const { return mMicrostepFactor; }
    uint32_t GetFullRevolutionStepCount() const { return FULL_REVOLUTION_STEP_COUNT; }

protected:
    MoverDriverCfg (
        const char* label,
        uint8_t pulsePin,
        uint8_t dirPin,
        uint8_t topSwitchPin,
        uint8_t centerSwitchPin,
        uint8_t bottomSwitchPin,
        uint32_t microstepFactor,
        uint32_t rampingSteps
    );

private:
    static const uint32_t DEFAULT_RAMPING_STEPS;
    static const uint32_t FULL_REVOLUTION_STEP_COUNT;

    // The label for debugging/logging purposes
    const char* mLabel;

    // --- Pins ---
    uint8_t mPulsePin;
    uint8_t mDirPin;
    uint8_t mTopSwitchPin;
    uint8_t mCenterSwitchPin;
    uint8_t mBottomSwitchPin;

    // --- Additional tuning ---
    uint32_t mMicrostepFactor;
    uint32_t mRampingSteps;
};

