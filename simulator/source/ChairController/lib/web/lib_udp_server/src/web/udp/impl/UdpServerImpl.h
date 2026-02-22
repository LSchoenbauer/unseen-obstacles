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

#include <web/udp/UdpServer.h>
#include <Arduino.h>
#include <appfw/AppComponent.h>
#include <os/TaskMgmt.h>
#include <WiFiUdp.h>
#include <memory>

namespace Web {
namespace Udp {

class UdpServerImpl : public UdpServer {
		typedef UdpServer Base;
    public:
        static UdpServerPtr Create(uint16_t port, size_t packetBufferSize);
        UdpServerImpl(uint16_t port, size_t packetBufferSize);
        virtual ~UdpServerImpl();

    protected:
        virtual bool StartListening(uint16_t port) override;
        virtual size_t RetrieveNextPacket(uint8_t* packetBuffer, size_t packetBufferSize) override;

    private:
        WiFiUDP mUdp;
};

}
} // namespace Web::Udp