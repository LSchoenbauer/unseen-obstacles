
#include "WifiService.h"

#include <utils/Log.h>
#include <net/MacAddress.h>
#include <net/Ip4Address.h>

#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

using namespace Net;

const uint8_t WifiService::mChannel = 1;
const uint8_t WifiService::mMaxClients = 4;
const char* WifiService::mSsid =  "Unseen-Obstacles";
const char* WifiService::mWifiPw = "unseenobstaclesEKLS";

WifiService::WifiService() :
        Base() {
}

WifiService::~WifiService() {
}

void WifiService::OnWifiEvent(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data) {
	LogInfo("On WiFi event: %d", event_id);
	if (event_data != 0) {
		switch (event_id) {
			case WIFI_EVENT_AP_STACONNECTED: {
				wifi_event_ap_staconnected_t* src = (wifi_event_ap_staconnected_t*) event_data;
				MacAddress mac(src->mac);
				LogInfo("WiFi AP: Client connected: %s", mac.ToString());
				break;
			}
			case WIFI_EVENT_AP_STADISCONNECTED: {
				wifi_event_ap_stadisconnected_t* src = (wifi_event_ap_stadisconnected_t*) event_data;
				MacAddress mac(src->mac);
				LogInfo("WiFi AP: Client disconnected: %s", mac.ToString());
				break;
			}
			default:
				break;
		}
	}
}

void WifiService::StartWifi(void) {
    Rc esp_rc = esp_netif_init();
	LogDbgRc("Netif init", esp_rc);
    esp_rc = esp_event_loop_create_default();
	LogDbgRc("Created default event loop", esp_rc);
    esp_rc = esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, WifiService::OnWifiEvent, NULL,NULL);
	LogDbgRc("Wifi event handler registration", esp_rc);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_rc = esp_wifi_init(&cfg);
	LogDbgRc("Wifi init", esp_rc);

    esp_netif_t* esp_netif_ap = esp_netif_create_default_wifi_ap();
	LogDbg("Created default AP");

    wifi_config_t wifi_config = {
		.ap = {
			.ssid = {0}, // will be set later
			.password = {0}, // will be set later
			.ssid_len = (uint8_t) strlen(mSsid),
			.channel = mChannel,
			.authmode = WIFI_AUTH_WPA2_PSK,
			.ssid_hidden = 0,
			.max_connection = mMaxClients,
			.beacon_interval = 100,
			.pairwise_cipher = WIFI_CIPHER_TYPE_CCMP,
			.ftm_responder = false,
		},
	};
	strcpy((char*)wifi_config.ap.ssid, mSsid);
	strcpy((char*)wifi_config.ap.password, mWifiPw);

    if (strlen(mWifiPw) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }

	esp_netif_ip_info_t ip_config;
	IP4_ADDR(&ip_config.ip,      192, 168, 7, 3);    // AP IP
	IP4_ADDR(&ip_config.gw,      192, 168, 7, 3);    // Gateway (same as AP IP)
	IP4_ADDR(&ip_config.netmask, 255, 255, 255, 0);  // Subnet mask

    esp_rc = esp_wifi_set_mode(WIFI_MODE_AP);
	LogDbgRc("Wifi set mode to AP", esp_rc);
    esp_rc = esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
	LogDbgRc("Wifi set config", esp_rc);
	esp_rc = esp_netif_dhcps_stop(esp_netif_ap);
	LogDbgRc("DHCP server stopped", esp_rc);
	esp_rc = esp_netif_set_ip_info(esp_netif_ap, &ip_config);
	LogDbgRc("IP address applied", esp_rc);
	esp_rc = esp_netif_dhcps_start(esp_netif_ap);
	LogDbgRc("DHCP server started again", esp_rc);
    esp_rc = esp_wifi_start();
	LogDbgRc("Wifi start", esp_rc);

	esp_netif_ip_info_t ip_info;
	ip_info.ip = {
		.addr = 0,
	};

	esp_rc = esp_netif_get_ip_info(esp_netif_ap, &ip_info);
	LogDbgRc("Retrieved WiFi info", esp_rc);

	Ip4Address ipAddr(ip_info.ip.addr);
    LogDbg("WiFi AP started: SSID: '%s', PW: '%s' IP: '%s'",
             mSsid, mWifiPw, ipAddr.toString());
}

