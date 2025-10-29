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

#include <utils/Singleton.h>

#include <memory>


class MoverDriver : public Singleton<MoverDriver> {
		typedef Singleton<MoverDriver> Base;
	public:
		/** Destructor */
		virtual ~MoverDriver();

		/** Initializes the application. */
		void Init();
        
        uint32_t getTargetSpeedRpm();

        uint32_t getPulseDurationsUs(uint32_t speedRpm);

        bool toggleDirection(uint32_t maxStepsPerDirection);

        void stdPrintfLn(const char* format, ...);
        
        void setup();

        void loop();

	private:
		friend class Singleton<MoverDriver> ;
		/** Singleton constructor */
		MoverDriver();

};