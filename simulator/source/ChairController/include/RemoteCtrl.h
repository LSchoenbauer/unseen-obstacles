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
#include <rfs/RootFileSystem.h>
#include <web/http/HttpServer.h>

#include <memory>

// TODO: Remove after switching to WifiService
#include <esp_event.h>

using namespace Web;
using namespace Web::Http;

class RemoteCtrl : public Singleton<RemoteCtrl> {
		typedef Singleton<RemoteCtrl> Base;
	public:
		/** Destructor */
		virtual ~RemoteCtrl();

		/** Initializes the application. */
		void Init();

	private:
		friend class Singleton<RemoteCtrl> ;
		/** Singleton constructor */
		RemoteCtrl();

		/** Starts the WiFi feature */
		void StartWifi();
		static void OnWifiEvent(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);

		/** Provides information about the installed RFS */
		void CheckRfs() const;

		/** Traverses a directory structure */
		void TraverseDirectory(File dir, size_t level) const;

		/** The HTTP server instance */
		HttpServerPtr mHttpServer;

		static const char* mSsid;
		static const char* mWifiPw;
};