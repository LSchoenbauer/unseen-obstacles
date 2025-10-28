/*
 * ===============================================================================
 * (c) HTL Leonding
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

#include <net/sntp/SntpClient.h>

#include <Print.h>  // required by ezTime
#include <ezTime.h> // https://github.com/ropg/ezTime
#include <list>
#include <functional>

namespace Net {
namespace Sntp {

class SntpClientImpl : public SntpClient {
		typedef SntpClient Base;

	public:
		/**Singleton constructor */
		SntpClientImpl();
		/** Destructor */
		virtual ~SntpClientImpl();

		virtual void Init();
		virtual void Init(const SntpClientConfig* cfg);
		virtual void SetConfiguration(const SntpClientConfig& cfg);
		virtual const SntpClientConfig& GetConfiguration();
		virtual void AddConfigurationChangedListener(std::weak_ptr<SntpClientConfigChangedListener> listener);
		virtual Status GetStatus() const;
		virtual String GetTimeZoneAbbr();
		virtual int16_t GetTimeZoneOffset();
		virtual bool IsDst();
		virtual bool IsDst(time_t time);
		virtual void UpdateTime();
		virtual QueryResult QueryNtp(time_t& time, unsigned long& measure_ts) const;
		virtual time_t GetLastUpdateTime() const;
		virtual void ProcessEvents();
		virtual void DumpDiagnostics();

	private:
		static const char* mSettingsPath;
		static const char* mKeyServerName;
		static const char* mKeyUpdateInterval;
		static const char* mKeyLocation;
		static const char* mKeyTimeZoneInfo;

		void RestoreConfiguration();
		void ApplyConfiguration(const SntpClientConfig& cfg, bool force = false);
		void FireChangeEvent(std::function<void(std::shared_ptr<SntpClientConfigChangedListener> listener)> handler);
		static void NtpSyncTask(void* taskParams);
		
		void SetServer(const String& ntpServerName, bool force = false);
		void SetUpdateInterval(uint16_t seconds, bool force = false);
		bool SetLocation(const String& location, bool force = false);
		bool SetTimeZoneInfo(const String& tzInfo, bool force = false);

		SntpClientConfig mConfig;
		Timezone mTimeZone;
		::std::list<::std::weak_ptr<SntpClientConfigChangedListener>> mCfgChangedListeners;
};

} // namespace Sntp
} // namespace Net
