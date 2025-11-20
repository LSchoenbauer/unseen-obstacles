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
 * The configuration of the SNTP Client.
 * Instances of this class hold all configuration settings
 * of SNTP clients.
 */
class SntpClientConfig {

	public:
		/**
		 * Constructs a configuration with default values.
		 */
		SntpClientConfig();

		/**
		 * Constructs a configuration with the given NTP server.
		 */
		SntpClientConfig(const char* ntpServerName);

		/**
		 * Constructs a configuration with the given NTP server.
		 */
		SntpClientConfig(String ntpServerName);

		/** 
		 * Defines the name (IP address or host) of the SNTP server.
		 * Defaults to 'pool.ntp.org'.
		 */
		void SetNtpServerName(const char* ntpServerName) {
			SetNtpServerName(String(ntpServerName));
		}

		/**
		 * Defines the host name or IP address of the NTP server to use.
		 * Defaults to 'pool.ntp.org'.
		 */
		void SetNtpServerName(const String& ntpServerName) {
			mNtpServerName = ntpServerName;
		}

		/**
		 * Provides the name or IP address of the configured NTP server.
		 */
		String GetNtpServerName() const {
			return mNtpServerName && !mNtpServerName.isEmpty() ? mNtpServerName : mDefaultServerName;
		}
		
		/** 
		 * Defines the interval in seconds for synchronizing the time.
		 * Set to 0 to disable updates.
		 * Defaults to 1 hour.
		 */
		void SetUpdateInterval(uint16_t seconds) {
			mUpdateInterval = seconds;
		}

		/** 
		 * Provides the interval in seconds for synchronizing the time
		 * or 0, if the synchronization is disabled.
		 */
		uint16_t GetUpdateInterval() const {
			return mUpdateInterval;
		}
		
		/** 
		 * Configures the location for determining the local timezone.
		 * It overrides timezone information specified by `SetTimeZoneInfo`.
		 * 
		 * It can either be:
		 * - the official name of the time zone (https://en.wikipedia.org/wiki/List_of_tz_database_time_zones)
		 * - the ISO 2 letter country code for countries spanning only one timezone
		 * - empty for trying to derive the time zone from GEO IP. This works only for countries spanning one timezone.
		 * @param location The location of the time zone (time area).
		 */
		void SetTimezoneLocation(const char* location) {
			SetTimezoneLocation(String(location));
		}

		/** 
		 * Configures the location for determining the local timezone.
		 * It overrides timezone information specified by `SetTimeZoneInfo`.
		 * 
		 * It can either be:
		 * - the official name of the time zone (https://en.wikipedia.org/wiki/List_of_tz_database_time_zones)
		 * - the ISO 2 letter country code for countries spanning only one timezone
		 * - empty for trying to derive the time zone from GEO IP. This works only for countries spanning one timezone.
		 * @param location The location of the time zone (time area).
		 */
		void SetTimezoneLocation(const String& location) {
			mLocation = location;
		}

		/**
		 * Provides the name of the configured timezone or an invalid string.
		 */
		String GetTimezoneLocation() const {
			return mLocation;
		}

		/**
		 * Determines whether or not the configured timezone location is valid
		 * and could be applied. Please note that the validity is proved at the
		 * time of applying the configuration, hence it may change even if
		 * the location was not modified.
		 */
		bool IsTimezoneLocationValid() const {
			return mLocationValid && mLocation && !mLocation.isEmpty();
		}

		/**
		 * Configures the specification of the timezone to use in Posix format.
		 * Note that this value is only used if no timezone location is defined.
		 * e.g.: CET-1CEST,M3.5.0/2,M10.5.0/3
		 * CET: Abbr. of the time zone.
		 * -1: UTC offset
		 * CEST: Abbr. of the corresponding DST time zone
		 * ,M3: DST starts in March
		 * .5 on the last occurrence of
		 * .0 a Sunday
		 * /2 at 02:00 o'clock local time
		 * ,M10 and ends in October
		 * .5 on the last occurrence of
		 * .0 a Sunday
		 * /3 at 03:00 o'clock local time
		 * @param tzInfo The configuration string.
		 */
		void SetTimezoneInfo(const char* tzInfo) {
			SetTimezoneInfo(String(tzInfo));
		}

		/**
		 * Configures the specification of the timezone to use in Posix format.
		 * Note that this value is only used if no timezone location is defined.
		 * e.g.: CET-1CEST,M3.5.0/2,M10.5.0/3
		 * CET: Abbr. of the time zone.
		 * -1: UTC offset
		 * CEST: Abbr. of the corresponding DST time zone
		 * ,M3: DST starts in March
		 * .5 on the last occurrence of
		 * .0 a Sunday
		 * /2 at 02:00 o'clock local time
		 * ,M10 and ends in October
		 * .5 on the last occurrence of
		 * .0 a Sunday
		 * /3 at 03:00 o'clock local time
		 * @param tzInfo The configuration string.
		 */
		void SetTimezoneInfo(const String& tzInfo) {
			mTzInfo = tzInfo;
		}

		/**
		 * Provides the timezone info in POSIX format an invalid string.
		 */
		String GetTimezoneInfo() const {
			return mTzInfo;
		}

		/**
		 * Determines whether or not the configured timezone information is valid
		 * and could be applied. Please note that the validity is proved at the
		 * time of applying the configuration, hence it may change even if
		 * the location was not modified.
		 */
		bool IsTimezoneInfoValid() const {
			return mTzInfoValid && mLocation && !mLocation.isEmpty();
		}

	private:
		friend class SntpClientImpl;

		static const char* mDefaultServerName;
		static uint16_t mDefaultUpdateIntervalS;

		void SetTimezoneLocation(const String& location, bool isValid) {
			SetTimezoneLocation(location);
			mLocationValid = isValid;
		}

		void SetTimezoneInfo(const String& tzInfo, bool isValid) {
			SetTimezoneInfo(tzInfo);
			mLocationValid = isValid;
		}

		bool     mLocationValid;
		bool     mTzInfoValid;
		uint16_t mUpdateInterval;
		String   mNtpServerName;
		String   mLocation;
		String   mTzInfo;

};

} // namespace Sntp
} // namespace Net
