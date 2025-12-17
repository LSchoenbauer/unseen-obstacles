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

#include <inttypes.h>

namespace Hal {

/**
 * Abstracts platform dependent system time source.
 */
class SystemTimeHal {
	public:

		/**
		 * Provides the milliseconds since system start.
		 * @return The system time in milliseconds.
		 */
		static uint32_t GetMilliseconds();

		/**
		 * Provides the milliseconds since system start with 64bit precision.
		 * @return The system time in milliseconds.
		 */
		static uint64_t GetMilliseconds64();

		/**
		 * Provides the microseconds since system start.
		 * @return The system time in microseconds.
		 */
		static uint32_t GetMicroseconds();

		/**
		 * Provides the microseconds since system start with 64bit precision.
		 * @return The system time in microseconds.
		 */
		static uint64_t GetMicroseconds64();
};

} // namespace Hal
