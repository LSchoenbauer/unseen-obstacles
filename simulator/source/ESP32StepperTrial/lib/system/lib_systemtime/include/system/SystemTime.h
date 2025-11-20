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

#include <hal/system/SystemTimeHal.h>

/**
 * The interface for timers.
 * The interface is intended to make platform specific
 * software and / or hardware timers transparently available.
 */

namespace System {

using namespace Hal;

/**
 * The class providing system time.
 */
class SystemTime {
	public:

		/**
		 * Provides the milliseconds since system start.
		 * @return The system time in milliseconds.
		 */
		static uint32_t GetMilliseconds() {
			return ::Hal::SystemTimeHal::GetMilliseconds();
		}

		/**
		 * Provides the milliseconds since system start 
		 * in 64bit precision if available on the SoC.
		 * @return The system time in milliseconds.
		 */
		static uint64_t GetMilliseconds64() {
			return ::Hal::SystemTimeHal::GetMilliseconds64();
		}

		/**
		 * Provides the microseconds since system start.
		 * @return The system time in microseconds.
		 */
		static uint32_t GetMicroseconds() {
			return ::Hal::SystemTimeHal::GetMicroseconds();
		}

		/**
		 * Provides the microseconds since system start
		 * in 64bit precision if available on the SoC.
		 * @return The system time in microseconds.
		 */
		static uint64_t GetMicroseconds64() {
			return ::Hal::SystemTimeHal::GetMicroseconds64();
		}
};

} // namespace System
