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

#include "CommandData.h"
#include "SimulationData.h"
#include "MoverDriver.h"
#include <appfw/AppComponent.h>
#include <stdint.h>

using namespace AppFw;

class ChairController;
typedef ::std::shared_ptr<ChairController> ChairControllerPtr;

class ChairController : public AppComponent {
        typedef AppComponent Base;
    public:
        static ChairControllerPtr Create(MoverDriverPtr rearLeft, MoverDriverPtr rearRight, MoverDriverPtr front, MoverDriverPtr rotation);

        virtual ~ChairController();

        void SetCommandModeEnabled(bool enabled);

        void ApplyCommand(const CommandData& commandData);

        void AdjustToSimulation(const SimulationData& simulationData);

        virtual void OnEvent(AppEventPtr ev);

    protected:
        ChairController(
                MoverDriverPtr rearLeft,
                MoverDriverPtr rearRight,
                MoverDriverPtr front,
                MoverDriverPtr rotation
            );

    private:
        enum class CtrlMode {
            NONE,
            SIMULATION,
            COMMAND
        };
        
        typedef struct CtrlCommandData {
            CommandData::Command mCommand;
            CommandData::Mover mMover;

            CtrlCommandData() : 
                mCommand(CommandData::Command::DOWN),
                mMover(CommandData::Mover::ROTATION)
            {}

            CtrlCommandData(const CommandData& commandData) : 
                mCommand(commandData.GetCommand()),
                mMover(commandData.GetMover())
            {}
        } CtrlCommandData;

        typedef struct CtrlSimulationData {
            uint32_t mTimestampMs;
            SimulationData::Mode mMode;
            uint32_t mPitch; // Y-Drehung
            uint32_t mYaw; // Z-Drehung
            uint32_t mRoll; // X-Drehung
            uint32_t mPosX;
            uint32_t mPosY;
            uint32_t mPosZ;

            CtrlSimulationData() : 
                mTimestampMs(0),
                mMode(SimulationData::Mode::NORMAL),
                mPitch(0), mYaw(0), mRoll(0),
                mPosX(0), mPosY(0), mPosZ(0)
            {}

            CtrlSimulationData(const SimulationData& simData) : 
                mTimestampMs(simData.GetTimestampMs()),
                mMode(simData.GetMode()),
                mPitch(simData.GetPitch()), mYaw(simData.GetYaw()), mRoll(simData.GetRoll()),
                mPosX(simData.GetPosX()), mPosY(simData.GetPosY()), mPosZ(simData.GetPosZ())
            {}

        } CtrlSimulationData;

        typedef struct CtrlData {
            CtrlMode mMode;
            CtrlCommandData mCommandData;
            CtrlSimulationData mSimulationData;

            CtrlData() : mMode(CtrlMode::NONE) {}
            CtrlData(const CtrlCommandData& data) : 
                mMode(CtrlMode::COMMAND), mCommandData(data) {}
            CtrlData(const CtrlSimulationData& data) : 
                mMode(CtrlMode::SIMULATION), mSimulationData(data) {}
        } CtrlData;

        void CmdUp(MoverDriverPtr mvr);
        void CmdDown(MoverDriverPtr mvr);
        void CmdToTop(MoverDriverPtr mvr);
        void CmdToBottom(MoverDriverPtr mvr);
        void CmdToCenter(MoverDriverPtr mvr);
        
        void OnCommand(const CtrlCommandData& cmdData);
        void OnSimulationData(const CtrlSimulationData& simData);

        double DegToRad(double deg);

        MoverDriverPtr GetMoverDriver(CommandData::Mover mover);

        void ApplyRotation(uint32_t deltaTimestamp, int32_t deltaYaw);
        void ApplyFrontMover(uint32_t deltaTimestamp, int32_t deltaPitch, int32_t deltaZ);
        void ApplyBackMover(uint32_t deltaTimestamp, int32_t deltaRoll, int32_t deltaZ);
        void ApplyShakeMode(uint32_t deltaTimestamp, uint32_t intensity, uint32_t wheelchairSpeed);

        static const uint32_t NORMAL_MOVEMENT_SPEED;
        static const uint32_t DISTANCE_ROTATOR;
        static const uint32_t CIRCUMFERENCE_ROTATOR_WHEEL;
        static const uint32_t DISTANCE_FRONT;
        static const uint32_t STROKE_PER_TURN;
        static const uint32_t DISTANCE_SIDE_BACK;
        static const uint32_t INTENSITY_SHAKING;

        // the queue for triggering the execution task
        QueueHandle_t mCtrlQueue;
        // the mutex to protect shared data
        SemaphoreHandle_t mCtrlMutex;

        //bei jedem adjust aufruf triggern, jeden 3. frame ca. Richtung und
        //Geschwindigkeit random ändern, bis stop von Modus, Delta winkel für rütteln,

        MoverDriverPtr mRearLeft;
        MoverDriverPtr mRearRight;
        MoverDriverPtr mFront;
        MoverDriverPtr mRotation;

        bool mCommandModeEnabled;
        CtrlSimulationData mLastSimData;
        uint32_t mLastSpeedZ;
};