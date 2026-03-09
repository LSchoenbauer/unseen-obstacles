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

#include "UdpServerImpl.h"
#include <net/Ip4Address.h>
#include <utils/Log.h>

using namespace Net;

namespace Web {
namespace Udp {

UdpServerPtr UdpServerImpl::Create(uint16_t port, size_t packetBufferSize) {
	return ::std::make_shared<UdpServerImpl>(port, packetBufferSize);
}

UdpServerImpl::UdpServerImpl(uint16_t port, size_t packetBufferSize) : 
        Base(port, packetBufferSize), mUdp() {
}

UdpServerImpl::~UdpServerImpl() {
}

bool UdpServerImpl::StartListening(uint16_t port) {
    return mUdp.begin(port) != 0;
}

size_t UdpServerImpl::RetrieveNextPacket(uint8_t* packetBuffer, size_t packetBufferSize) {
    size_t dataSize = mUdp.parsePacket();
    if (dataSize > 0) {
        size_t bytesRead = mUdp.read(packetBuffer, packetBufferSize);
        LogDbgT(LTAG_RX, "UDP Server: Packet with %d bytes received and %d bytes read", dataSize, bytesRead);
        dataSize = bytesRead;
    }
    return dataSize;
}

size_t UdpServerImpl::Respond(const uint8_t* data, size_t dataLength) {
    size_t sentByteCnt = 0;
    Ip4Address clientIP = mUdp.remoteIP();
    Ip4Address clientPort = mUdp.remotePort();
    if (mUdp.beginPacket(mUdp.remoteIP(), mUdp.remotePort()) == 0) {
        LogWarnT(LTAG_TX, "UDP Server: Failed to send packet to %s:%d - check NW connection and call 'Start(...)' beforehand", clientIP.toString().c_str(), clientPort);
    } else {
        sentByteCnt = mUdp.write(data, dataLength);
        if (mUdp.endPacket() == 0) {
            LogWarnT(LTAG_TX, "UDP Server: Failed to send packet to %s:%d - endPacket() failed", clientIP.toString().c_str(), clientPort);
        } else {
            if (sentByteCnt != dataLength) {
                LogWarnT(LTAG_TX, "UDP Server: Failed to send complete packet to %s:%d - only %d of %d bytes sent", clientIP.toString().c_str(), clientPort, sentByteCnt, dataLength);
            } else {
                LogDbgT(LTAG_TX, "UDP Server: Sent packet to %s:%d, size: %d, content: %.*s", clientIP.toString().c_str(), clientPort, sentByteCnt, sentByteCnt, data);
            }
        }
    }
    return dataLength;
}

}
} // namespace Web::Udp