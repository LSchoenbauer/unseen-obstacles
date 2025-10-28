# SNTP Client for ESP32 Arduino

## Overview

The SNTP Client library provides a simple way to synchronize time on ESP32-based projects using the Simple Network Time Protocol (SNTP). Designed for the Arduino environment, this library allows your device to obtain accurate time from an NTP server, manage local timezones, and provide real-time diagnostics. The implementation follows a Singleton pattern to ensure a single, consistent instance throughout your application.

## Features

- **Time Synchronization**: Update the system time using data provided by an NTP server.
- **Singleton Design**: Ensures that only one instance of the SNTP client exists, simplifying global time management.
- **Configurable NTP Server**: Easily set or change the NTP server (defaults to `pool.ntp.org` if unspecified).
- **Adjustable Update Interval**: Configure the interval (in seconds) between successive time synchronizations. Setting it to 0 disables automatic updates.
- **Timezone Management**: Set the local timezone either by specifying an official time zone name/ISO code or by providing a POSIX timezone string.
- **Dynamic Configuration Updates**: Listen for configuration changes (such as changes in the NTP server, update interval, or timezone settings) using the built-in configuration change listener interface.
- **Diagnostic Capabilities**: Process internal events and dump diagnostic information to support debugging and system monitoring.

## Getting Started

### Installation

This library is designed being used in PlatformIO driven project.

#### PlatformIO

Include the library in attribute `lib_deps` of your PlatformIO project.

#### Manual Installation

1. **Download the Library**  
   Clone or download the repository as well as local libraries this library depends on (see section _Dependencies_).

2. **Include in Your Project**  
   Add the library files to your project. Make sure your development environment is set up accordingly.

### Initializing the SNTP Client

You can initialize the SNTP client with either the default (or persisted) configuration or a custom configuration. Below is an example demonstrating both approaches:

```cpp
#include <net/sntp/SntpClient.h>
#include <net/sntp/SntpClientConfig.h>
#include <utils/Singleton.h>

// Retrieve the singleton instance of the SNTP client
Net::Sntp::SntpClient* sntpClient = Singleton<Net::Sntp::SntpClient>::GetInstance();

// Option 1: Initialize with the default or previously saved configuration
sntpClient->Init();

// Option 2: Initialize with a custom configuration
Net::Sntp::SntpClientConfig config("pool.ntp.org");  // Specify the NTP server name/IP
config.SetUpdateInterval(3600);                      // Set synchronization interval (3600 seconds = 1 hour)
config.SetTimezoneLocation("Europe/Vienna");         // Configure the timezone by location
sntpClient->Init(&config);
```

### Time Synchronization

To update the system time immediately or query the NTP server without updating the clock, use the following functions:

```cpp
// Perform an immediate update of the device time
sntpClient->UpdateTime();

// Alternatively, query the NTP server without updating the clock:
time_t currentTime;
unsigned long measureTimestamp;
Net::Sntp::SntpClient::QueryResult result = sntpClient->QueryNtp(currentTime, measureTimestamp);

if(result == Net::Sntp::SntpClient::Ok) {
    // Process the obtained time value (currentTime) as needed
}
```

### Handling Configuration Changes

To respond to any changes in the SNTP client's configuration (such as a new NTP server name, update interval, or timezone settings), implement the `SntpClientConfigChangedListener` interface:

```cpp
#include <net/sntp/SntpClientConfigChangedListener.h>
#include <WString.h>

class MyConfigListener : public Net::Sntp::SntpClientConfigChangedListener {
public:
    void OnNtpServerNameChanged(const String& newNtpServerName) override {
        // Handle update of the NTP server name
    }

    void OnUpdateIntervalChanged(uint16_t newIntervalSeconds) override {
        // Handle update of the synchronization interval
    }

    void OnTimezoneLocationChanged(const String& newLocation, bool isValid) override {
        // Handle changes in timezone location
    }

    void OnTimezoneInfoChanged(const String& tzInfo, bool isValid) override {
        // Handle changes in POSIX timezone info
    }
};

// Register the configuration change listener
std::shared_ptr<MyConfigListener> listener = std::make_shared<MyConfigListener>();
sntpClient->AddConfigurationChangedListener(listener);
```

## API Overview

For more detailed usage, here is a brief overview of the primary API functions:

- **Initialization and Configuration**
  - `void Init()`: Initialize the SNTP client with default/persisted settings.
  - `void Init(const SntpClientConfig* cfg)`: Initialize the client with custom configuration.
  - `void SetConfiguration(const SntpClientConfig& cfg)`: Apply and store a new configuration.
  - `const SntpClientConfig& GetConfiguration()`: Retrieve the current configuration.

- **Time and Timezone Handling**
  - `void UpdateTime()`: Update the system clock from the NTP server.
  - `QueryResult QueryNtp(time_t& time, unsigned long& measure_ts) const`: Query the server without updating the clock.
  - `time_t GetLastUpdateTime() const`: Get the time of the last successful update.
  - `String GetTimeZoneAbbr()`, `int16_t GetTimeZoneOffset()`: Get local timezone details.
  - `bool IsDst()`, `bool IsDst(time_t ts)`: Check for daylight saving time.

- **Configuration Change Notifications**
  - See the `SntpClientConfigChangedListener` interface for details on how to react to configuration changes.

- **Diagnostics**
  - `void ProcessEvents()`: Handle internal events.
  - `void DumpDiagnostics()`: Output diagnostic information for debugging.

## Dependencies

This library depends on `lib_utils`, `lib_settings` and the third party library `ropg/ezTime`. 

## Contributing

Contributions to improve features, documentation, and bug fixes are welcome. Please adhere to the repository’s contribution guidelines when submitting pull requests or reporting issues.

## Support

If you encounter any problems or have feature requests, please consult the issue tracker in the repository.

## License

This library is licensed under the [MIT License](LICENSE).  
(c) HTL Leonding. All trademarks used in this document are property of their respective owners.

---

This SNTP Client library for ESP32 Arduino is designed to streamline time synchronization tasks by providing an easy-to-use interface along with robust configuration and diagnostic capabilities. For additional technical details, please refer to the source header files.