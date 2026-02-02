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
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

class CommandData {
    public:
        enum class Command {
            UP,
            DOWN,
            TO_TOP,
            TO_CENTER,
            TO_BOTTOM
        };

        enum class Mover {
            REAR_LEFT,
            REAR_RIGHT,
            FRONT,
            ROTATION
        };
        
        ~CommandData();

        static CommandData* Acquire(
            Command command,
            Mover mover);

        void Release();

        Command GetCommand() const {return mCommand;}
        Mover GetMover() const {return mMover;}
        
    private:
        static const uint8_t POOL_SIZE;
        static CommandData mPool[];
        static SemaphoreHandle_t mPoolSemaphore;
        
        CommandData();

        bool mAllocated;
        Command mCommand;
        Mover mMover;
};