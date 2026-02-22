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

#include <appfw/Application.h>
#include <RemoteCtrl.h>
#include <web/udp/UdpServer.h>
#include "ChairController.h"

using namespace AppFw;
using namespace Web::Udp;

class ChairControllerApp : public Application<ChairControllerApp> {
		typedef Application<ChairControllerApp> Base;
	public:
		/** Destructor */
		virtual ~ChairControllerApp();

		/** Initializes the application. */
		void Init();

		virtual void ProcessEvents();

		void InitializeChairController();


	private:
		friend class Singleton<ChairControllerApp> ;
		/** Singleton constructor */
		ChairControllerApp();

		/** Starts the WiFi feature */
		void StartRemoteCtrl();
		
		void StartVrConnection();
		
		void OnVrDataReceived(const uint8_t* data, size_t dataLength);
		
		/** The HTTP server instance */
		RemoteCtrl* remoteCtrl;

		// TODO: Design flaw - better to move the UDP server and chair controller instances to a separate adapter class, similar to "RemoteCtrl"
		UdpServerPtr mUdpServer;
		static const uint16_t VR_SERVER_PORT = 4210;
		
		ChairControllerPtr mChairController;

		MoverDriverCfgPtr mRearLeftCfg;
		MoverDriverCfgPtr mRearRightCfg;
		MoverDriverCfgPtr mFrontCfg;
		MoverDriverCfgPtr mRotationCfg;
};