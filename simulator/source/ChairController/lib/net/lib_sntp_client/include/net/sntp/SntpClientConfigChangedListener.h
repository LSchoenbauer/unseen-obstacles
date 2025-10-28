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

#include <WString.h>
#include <inttypes.h>

namespace Net {
namespace Sntp {

/**
 * The interface for listeners to client configuration changes.
 */
class SntpClientConfigChangedListener {

	public:

		/** 
		 * Notifies clients that the NTP server name has changed.
		 */
		virtual void OnNtpServerNameChanged(const String& newNtpServerName) = 0;

		/**
		 * Notifies clients that the NTP update interval has changed.
		 */
		virtual void OnUpdateIntervalChanged(uint16_t newIntervalSeconds) = 0;

		/**
		 * Notifies clients that the timezone location has changed.
		 */
		virtual void OnTimezoneLocationChanged(const String& newLocation, bool isValid) = 0;

		/**
		 * Notifies clients that the timezone POSIX information has changed.
		 */
		virtual void OnTimezoneInfoChanged(const String& tzInfo, bool isValid) = 0;

};

} // namespace Sntp
} // namespace Net
