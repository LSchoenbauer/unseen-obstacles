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
#include <utils/Log.h>

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
        LogDbg("UDP Server: Packet with %d bytes received and %d bytes read", dataSize, bytesRead);
        dataSize = bytesRead;
    }
    return dataSize;
}

}
} // namespace Web::Udp