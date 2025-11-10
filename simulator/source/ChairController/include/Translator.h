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


class Translator {
	public:

		Translator(
            MoverDriverPtr rearLeft,
            MoverDriverPtr rearRight,
            MoverDriverPtr front,
            MoverDriverPtr rotation
        );

		virtual ~Translator();

        void SetCommandModeEnabled(bool enabled);

        void ApplyCommand(const CommandData& commandData);

        void AdjustToSimulation(const SimulationData& simulationData);

    private:
        MoverDriverPtr GetMoverDriver(CommandData::Mover mover);

        static const uint32_t NORMAL_MOVEMENT_SPEED;

        MoverDriverPtr mRearLeft;
        MoverDriverPtr mRearRight;
        MoverDriverPtr mFront;
        MoverDriverPtr mRotation;

        bool mCommandModeEnabled;
};