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
#include <stdint.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/semphr.h>

class ChairController;
typedef ::std::shared_ptr<ChairController> ChairControllerPtr;

class ChairController {
    public:
        static ChairControllerPtr Create(MoverDriverPtr rearLeft, MoverDriverPtr rearRight, MoverDriverPtr front, MoverDriverPtr rotation);

        virtual ~ChairController();

        void SetCommandModeEnabled(bool enabled);

        void ApplyCommand(CommandData* commandData);

        void AdjustToSimulation(SimulationData* simulationData);

    protected:
        ChairController(
                MoverDriverPtr rearLeft,
                MoverDriverPtr rearRight,
                MoverDriverPtr front,
                MoverDriverPtr rotation
            );

    private:
        typedef enum {
            None,
            Command,
            Simulation,
        } EventType;

        typedef struct {
            const EventType mType;
            union {
                CommandData* mCommandData;
                SimulationData* mSimulationData;
            } mData;
        } Event;

        MoverDriverPtr GetMoverDriver(CommandData::Mover mover);

        void ProcessEvent();
        void ProcessSimulationEvent(const SimulationData* simulationData);
        void ProcessCommandEvent(const CommandData* commandData);

        void ApplyRotation(uint32_t deltaTimestamp, uint32_t deltaYaw);
        void ApplyFrontMover(uint32_t deltaTimestamp, uint32_t deltaPitch, uint32_t deltaZ);
        void ApplyBackMover(uint32_t deltaTimestamp, uint32_t deltaRoll, uint32_t deltaZ);
        void ApplyShakeMode(uint32_t deltaTimestamp, uint32_t intensity, uint32_t wheelchairSpeed);

        static const uint32_t NORMAL_MOVEMENT_SPEED;
        static const uint32_t DISTANCE_ROTATOR;
        static const uint32_t CIRCUMFERENCE_ROTATOR_WHEEL;
        static const uint32_t DISTANCE_FRONT;
        static const uint32_t STROKE_PER_TURN;
        static const uint32_t DISTANCE_SIDE_BACK;
        static const uint32_t INTENSITY_SHAKING;
        
        // one queue for all instances, although there is only one ChairController instance
        static SemaphoreHandle_t mMutex;
        static QueueHandle_t mEventQueue;

        //bei jedem adjust aufruf triggern, jeden 3. frame ca. Richtung und
        //Geschwindigkeit random ändern, bis stop von Modus, Delta winkel für rütteln,

        MoverDriverPtr mRearLeft;
        MoverDriverPtr mRearRight;
        MoverDriverPtr mFront;
        MoverDriverPtr mRotation;
        

        bool mCommandModeEnabled;
};