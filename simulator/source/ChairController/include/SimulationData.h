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


class SimulationData {
    public:
        enum class Mode {
            NORMAL,
            BUMPING
        };
        
        ~SimulationData();

        static SimulationData* Acquire(
            uint32_t timestampMs,
            SimulationData::Mode mode,
            uint32_t pitch,
            uint32_t yaw,
            uint32_t roll,
            uint32_t posX,
            uint32_t posY,
            uint32_t posZ);

        void Release();

        //void SetTimestamp(uint32_t timestampMs);
        uint32_t GetTimestampMs() const {return mTimestampMs;}

        //void SetMode(Mode mode);
        Mode GetMode() const {return mMode;}

        //void SetPitch(uint32_t pitch);
        uint32_t GetPitch() const {return mPitch;}

        //void SetYaw(uint32_t yaw);
        uint32_t GetYaw() const {return mYaw;}

        //void SetRoll(uint32_t roll);
        uint32_t GetRoll() const {return mRoll;}

        //void SetPosX(uint32_t posX);
        uint32_t GetPosX() const {return mPosX;}
        
        //void SetPosY(uint32_t posY);
        uint32_t GetPosY() const {return mPosY;}

        //void SetPosY(uint32_t posY);
        uint32_t GetPosZ() const {return mPosZ;}

    private:
        static const uint8_t POOL_SIZE;
        static SimulationData mPool[];
        static SemaphoreHandle_t mPoolSemaphore;

        SimulationData();

        // if timestamp is 0, the instance is free
        uint32_t mTimestampMs = 0;
        Mode mMode;
        uint32_t mPitch; // Y-Drehung
        uint32_t mYaw; // Z-Drehung
        uint32_t mRoll; // X-Drehung
        uint32_t mPosX;
        uint32_t mPosY;
        uint32_t mPosZ;
};