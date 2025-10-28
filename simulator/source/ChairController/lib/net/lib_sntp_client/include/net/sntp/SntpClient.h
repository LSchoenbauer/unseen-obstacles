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
#include <net/sntp/SntpClientConfig.h>
#include <net/sntp/SntpClientConfigChangedListener.h>
#include <WString.h>
#include <time.h>
#include <memory>

namespace Net {
namespace Sntp {

class SntpClient : public Singleton<SntpClient> {
		typedef Singleton<SntpClient> Base;

	public:
		typedef enum {
			/** The time was neither set via NTP nor manually. */
			NotSet,
			/** The time was set and is expected being in sync */
			Set,
			/** The time was not synchronized for a while (> 1h) and should be updated */
			SyncNeeded,
			/** Status is unknown */
			Unknown,
		} Status;

		typedef enum {
			/** The query was executed successfully. */
			Ok,
			/** The NTP server could not be reached due to network issues. */
			NoNetwork,
			/** No response was received in time */
			Timeout,
			/** Failed for another reason. */
			OtherFailure,
		} QueryResult;

		/**
		 * Initializes the SNTP client with the default or persisted configuration.
		 */
		virtual void Init() = 0;

		/**
		 * Initializes the SNTP client with the given configuration.
		 */
		virtual void Init(const SntpClientConfig* cfg) = 0;

		/**
		 * Applies and persists the configuration of the SNTP client.
		 */
		virtual void SetConfiguration(const SntpClientConfig& cfg) = 0;

		/**
		 * Provides the configuration of the SNTP client.
		 */
		virtual const SntpClientConfig& GetConfiguration() = 0;

		/**
		 * Registers the given configuration changed listener.
		 */
		virtual void AddConfigurationChangedListener(std::weak_ptr<SntpClientConfigChangedListener> listener) = 0;
		
		/**
		 * Provides the status of the client.
		 */
		virtual Status GetStatus() const = 0;

		/** Provides the abbreviation of the currently set local time zone. */
		virtual String GetTimeZoneAbbr() = 0;
		/** Provides the offset of the currently set local time zone. */
		virtual int16_t GetTimeZoneOffset() = 0;
		/** Determines whether or not the daylight saving time is in effect right now for the current time zone. */
		virtual bool IsDst() = 0;
		/** Determines whether or not the daylight saving time is or was in effect for the given time and time zone. */
		virtual bool IsDst(time_t ts) = 0;

		/** Updates the device time with the time provided by NTP immediately. */
		virtual void UpdateTime() = 0;

		/** Queries the NTP server without actually updating the clock */
		virtual QueryResult QueryNtp(time_t& time, unsigned long& measure_ts) const = 0;

		/** Provides the timestamp of the last NTP update. */
		virtual time_t GetLastUpdateTime() const = 0;

		virtual void ProcessEvents() = 0;

		virtual void DumpDiagnostics() = 0;

	protected:
		/** Destructor */
		virtual ~SntpClient() {
		}

	private:
		friend class Singleton<SntpClient>;
};

} // namespace Sntp
} // namespace Net

template<> Net::Sntp::SntpClient* ::Singleton<Net::Sntp::SntpClient>::GetInstance(); // @suppress("Member declaration not found")
