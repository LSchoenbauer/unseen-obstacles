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

// Cleanup
//#include <memory>

#include <esp_event.h>

class WifiService : public Singleton<WifiService> {
		typedef Singleton<WifiService> Base;
	public:
		/** Destructor */
		virtual ~WifiService();
		
		/** Starts the WiFi feature */
		void StartWifi();

	private:
		friend class Singleton<WifiService> ;
		/** Singleton constructor */
		WifiService();

		static void OnWifiEvent(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

		static const char* mSsid;
		static const char* mWifiPw;
		static const uint8_t mChannel;
		static const uint8_t mMaxClients;
};