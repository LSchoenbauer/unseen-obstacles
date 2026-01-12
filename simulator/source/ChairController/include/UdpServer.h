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

#include <memory>
#include <Arduino.h>
#include <WiFiUdp.h>
#include <appfw/AppComponent.h>
#include "ChairController.h"

using namespace AppFw;

class UdpServer;
typedef ::std::shared_ptr<UdpServer> UdpServerPtr;

class UdpServer : public AppComponent {
		typedef AppComponent Base;
	public:
        static UdpServerPtr Create();

		~UdpServer();

        void Init(ChairControllerPtr chairController);

        virtual void OnEvent(std::shared_ptr<AppEvent> ev);

    protected:
        UdpServer();

    private:
        void ReceiveData();

        WiFiUDP mUdp;
        ChairControllerPtr mChairController;
};
