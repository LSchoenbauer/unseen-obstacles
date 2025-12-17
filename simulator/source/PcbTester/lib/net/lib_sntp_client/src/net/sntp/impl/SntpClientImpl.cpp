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

#include "SntpClientImpl.h"
#include <system/Settings.h>
#include <utils/Log.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace Net {
namespace Sntp {
	
	const char* SntpClientImpl::mSettingsPath      = "sntpc.cfg";
	const char* SntpClientImpl::mKeyServerName     = "sntpc_host";
	const char* SntpClientImpl::mKeyUpdateInterval = "sntpc_ui";
	const char* SntpClientImpl::mKeyLocation       = "sntpc_tz";
	const char* SntpClientImpl::mKeyTimeZoneInfo   = "sntpc_tzinfo";

SntpClientImpl::SntpClientImpl() :
        Base(), mConfig(), mTimeZone(), mCfgChangedListeners() {
	// Uncomment the line below to see what it does behind the scenes
	setDebug(INFO);
}

SntpClientImpl::~SntpClientImpl() {
}

void SntpClientImpl::Init() {
	Init(0);
}

void SntpClientImpl::Init(const SntpClientConfig* cfg) {
	LogDbg("Initializing SNTP client ...");
	RestoreConfiguration();
	if (cfg != 0) {
		SetConfiguration((*cfg));
	}
	
	TaskHandle_t sntpClientTask;
	xTaskCreate(
		SntpClientImpl::NtpSyncTask,       /* Task function. */
		"SNTP sync",           /* name of task. */
		30000,                 /* Stack size of task */
		NULL,                  /* parameter of the task */
		3,                     /* priority of the task */
		&sntpClientTask        /* Task handle to keep track of created task */
	);
	LogDbg("... init done");
}

void SntpClientImpl::NtpSyncTask(void* taskParams) {
	LogDbg("Synchronizing SNTP client ...");
	waitForSync();
	LogDbg("... sync done");
	SntpClient::GetInstance()->DumpDiagnostics();
	vTaskDelete(NULL);
}

void SntpClientImpl::RestoreConfiguration() {
	LogInfo("Restoring SNTP client configuration ...");
	
	System::Settings settings;
	if (settings.Begin(SntpClientImpl::mSettingsPath)) {
			mConfig.SetNtpServerName(settings.GetString(mKeyServerName, mConfig.GetNtpServerName()));
			mConfig.SetUpdateInterval(settings.GetUInt(mKeyUpdateInterval, mConfig.GetUpdateInterval()));
			mConfig.SetTimezoneLocation(settings.GetString(mKeyLocation, mConfig.GetTimezoneLocation()));
			mConfig.SetTimezoneInfo(settings.GetString(mKeyTimeZoneInfo, mConfig.GetTimezoneInfo()));
		settings.End();
		LogDbg("... restored SNTP client configuration");
	} else {
		LogError("Failed to restore SNTP client config: Failed to open 'settings'");
	}
	ApplyConfiguration(mConfig, true);
}

void SntpClientImpl::ApplyConfiguration(const SntpClientConfig& cfg, bool force) {
	LogInfo("Configuring SNTP client ...");

	SetServer(cfg.GetNtpServerName(), force);
	SetUpdateInterval(cfg.GetUpdateInterval(), force);
	if ((!cfg.GetTimezoneLocation() 
			|| cfg.GetTimezoneLocation().isEmpty() 
			|| !SetLocation(cfg.GetTimezoneLocation(), force)) 
				&& cfg.GetTimezoneInfo()
				&& !cfg.GetTimezoneInfo().isEmpty()
	) {
		SetTimeZoneInfo(cfg.GetTimezoneInfo(), force);
	}
	LogDbg("Applied SNTP client config ...");
}

void SntpClientImpl::SetConfiguration(const SntpClientConfig& cfg) {
	ApplyConfiguration(cfg);

	LogInfo("Persisting SNTP client configuration ...");
	System::Settings settings;
	if (settings.Begin(SntpClientImpl::mSettingsPath)) {
		settings.PutString(mKeyServerName, mConfig.GetNtpServerName());
		settings.PutUInt(mKeyUpdateInterval, mConfig.GetUpdateInterval());
		settings.PutString(mKeyLocation, mConfig.GetTimezoneLocation());
		settings.PutString(mKeyTimeZoneInfo, mConfig.GetTimezoneInfo());
		settings.End();
		LogDbg("... persisted SNTP client configuration");
	} else {
		LogError("Failed to persist SNTP client config: Failed to open 'settings'");
	}
}

const SntpClientConfig& SntpClientImpl::GetConfiguration() {
	return mConfig;
}

void SntpClientImpl::AddConfigurationChangedListener(std::weak_ptr<SntpClientConfigChangedListener> listener) {
	if (!listener.expired()) {
		mCfgChangedListeners.push_back(listener);
	}
}

void SntpClientImpl::SetServer(const String& ntpServerName, bool force) {
	if (force || !mConfig.GetNtpServerName().equals(ntpServerName)) {
		mConfig.SetNtpServerName(ntpServerName);
		setServer(mConfig.GetNtpServerName());

		FireChangeEvent([this](::std::shared_ptr<SntpClientConfigChangedListener> listener) {
			if (listener) {
				listener->OnNtpServerNameChanged(mConfig.GetNtpServerName());
			}
		});
	}
}

void SntpClientImpl::SetUpdateInterval(uint16_t seconds, bool force) {
	if (force || !mConfig.GetUpdateInterval() != seconds) {
		mConfig.SetUpdateInterval(seconds);
		setInterval(mConfig.GetUpdateInterval());

		FireChangeEvent([this](::std::shared_ptr<SntpClientConfigChangedListener> listener) {
			if (listener) {
				listener->OnUpdateIntervalChanged(mConfig.GetUpdateInterval());
			}
		});
	}
}

bool SntpClientImpl::SetLocation(const String& location, bool force) {
	bool rc = true;
	if (force || !mConfig.GetTimezoneLocation().equals(location)) {
		rc = (location.isEmpty() ? mTimeZone.setLocation() : mTimeZone.setLocation(location));
		mConfig.SetTimezoneLocation(location, rc);

		FireChangeEvent([this](::std::shared_ptr<SntpClientConfigChangedListener> listener) {
			if (listener) {
				listener->OnTimezoneLocationChanged(mConfig.GetTimezoneLocation(), mConfig.IsTimezoneLocationValid());
			}
		});
	}
	return rc;
}

bool SntpClientImpl::SetTimeZoneInfo(const String& tzInfo, bool force) {
	bool rc = true;
	if (force || !mConfig.GetTimezoneInfo().equals(tzInfo)) {
		rc = mTimeZone.setPosix(tzInfo);
		mConfig.SetTimezoneInfo(tzInfo, rc);

		FireChangeEvent([this](::std::shared_ptr<SntpClientConfigChangedListener> listener) {
			if (listener) {
				listener->OnTimezoneInfoChanged(mConfig.GetTimezoneInfo(), mConfig.IsTimezoneInfoValid());
			}
		});
	}
	return rc;
}

SntpClientImpl::Status SntpClientImpl::GetStatus() const {
	timeStatus_t status = timeStatus();
	switch (status) {
		case timeNotSet:
			return SyncNeeded;
		case timeSet:
			return Set;
		case timeNeedsSync:
			return SyncNeeded;
		default:
			break;
	}
	return Unknown;
}

String SntpClientImpl::GetTimeZoneAbbr() {
	return mTimeZone.getTimezoneName();
}

int16_t SntpClientImpl::GetTimeZoneOffset() {
	return mTimeZone.getOffset();
}

bool SntpClientImpl::IsDst() {
	return mTimeZone.isDST();
}

bool SntpClientImpl::IsDst(time_t ts) {
	return mTimeZone.isDST(ts);
}

void SntpClientImpl::UpdateTime() {
	updateNTP();
}

SntpClientImpl::QueryResult SntpClientImpl::QueryNtp(time_t& time, unsigned long& measure_ts) const {
	QueryResult result = Ok;
	bool rc = queryNTP(mConfig.GetNtpServerName(), time, measure_ts);
	if (!rc) {
		ezError_t err = error();
		result = err == NO_NETWORK ? NoNetwork : (err == TIMEOUT ? Timeout : OtherFailure);
	}
	return result;
}

time_t SntpClientImpl::GetLastUpdateTime() const {
	return lastNtpUpdateTime();
}

void SntpClientImpl::ProcessEvents() {
	events();
}

void SntpClientImpl::DumpDiagnostics() {
#ifdef LOG_INFO_ENABLED
	Status status = GetStatus();
	time_t now = mTimeZone.now();
	String lastUpdateTsFormatted = mTimeZone.dateTime(GetLastUpdateTime(), RFC3339_EXT);

	LogInfo("");
	LogInfo("============================");
	LogInfo("Time info");
	LogInfo("Local time :     %s [%d], %04d/%02d/%02d, CW %02d, doy: %03d",
	        dayShortStr(weekday(now)).c_str(), weekday(now), year(now), month(now), day(now), weekISO(now), dayOfYear(now));
	LogInfo("                 %02d:%02d %s - %02d:%02d:%02d.%06d %s",
	        hourFormat12(now), minute(now), isAM(now) ? "am" : "pm", hour(now), minute(now),
	        second(now), ms(now), militaryTZ(now).c_str());
	LogInfo("----------------------------");
	LogInfo("Status:          %s",
	        status == NotSet ?
	                "NOT SET" : (status == Set ? "SET" : (status == SyncNeeded ? "SYNC NEEDED" : "UNKNOWN")));
	LogInfo("NTP Servers:     %s", mConfig.GetNtpServerName().c_str());
	LogInfo("Update Interval: %d s", mConfig.GetUpdateInterval());
	LogInfo("Last Update:     %s", lastUpdateTsFormatted ? lastUpdateTsFormatted.c_str() : "---");
	LogInfo("UTC:             %s", UTC.dateTime().c_str());
	LogInfo("Local time (RFC) %s", mTimeZone.dateTime(RFC3339_EXT).c_str());
	LogInfo("Compile time:    %s", mTimeZone.dateTime(compileTime(), RFC3339_EXT).c_str());
	LogInfo("TZ Location:     %s", mConfig.GetTimezoneLocation() ? mConfig.GetTimezoneLocation().c_str() : "--");
	LogInfo("TZ Info:         %s", mConfig.GetTimezoneInfo() ? mConfig.GetTimezoneInfo().c_str() : "--");
	LogInfo("TZ Abbr:         %s", GetTimeZoneAbbr().c_str());
	LogInfo("TZ Offset:       %d", GetTimeZoneOffset());
	LogInfo("DST:             %s", IsDst() ? "ON" : "OFF");
	LogInfo("----------------------------");
	LogInfo("");
#endif
}

void SntpClientImpl::FireChangeEvent(std::function<void(std::shared_ptr<SntpClientConfigChangedListener> listener)> handler) {
	::std::list<::std::weak_ptr<SntpClientConfigChangedListener>>::iterator it = mCfgChangedListeners.begin();
	while (it != mCfgChangedListeners.end()) {
		if (it->expired()) {
			it = mCfgChangedListeners.erase(it);
		} else {
			auto lstn = it->lock();
			if (lstn) {
				handler(lstn);
			}
			it++;
		}
	}
}

} // namespace Sntp
} // namespace Net
