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

#include <os/TaskMgmt.h>
#include <functional>
#include <memory>
#include <utils/Log.h>

namespace Web {
namespace Udp {

using namespace Os;

class UdpServer;
/** The type of the UDP server pointer */
typedef ::std::shared_ptr<UdpServer> UdpServerPtr;
/** The function pointer type for request handlers. */
typedef ::std::function<void(const uint8_t* data, size_t dataLength)> UdpPacketHandler;

class UdpServer {
    public:
        static const Logger::Tag LTAG_CFG; /// for logging related to configuration issues
        static const Logger::Tag LTAG_RX;  /// for logging related to received packets
        static const Logger::Tag LTAG_TX;  /// for logging related to transmitted packets

        /**
         * Creates a new UDP server instance with the given port.
         * The server will be initialized with a default packet buffer size of 128 bytes.
         * @param port The local port on which the server will listen for incoming UDP packets.
         * @return A shared pointer to the created UDP server instance.
         */
        static UdpServerPtr Create(uint16_t port) {
            return UdpServer::Create(port, 128);
        }

        /**
         * Creates a new UDP server instance with the given port and packet uffer size.
         */
        static UdpServerPtr Create(uint16_t port, size_t packetBufferSize);

        /**
         * Attaches the specified packet handler to the server.
         * The handler will be called for each received UDP packet.
         * 
         * The data, the packet handler receives, is only valid during the execution of the handler 
         * and must be copied if it needs to be stored or processed asynchronously.
         * 
         * @param packetHandler The function to call when a UDP packet is received. The function should accept a pointer to the packet data and the length of the packet.
         */
        virtual void OnPacket(UdpPacketHandler packetHandler);

        /**
         * Starts the asynchronous UDP server with the default task configuration.
         */
        void Start();

        /**
         * Starts the asynchronous UDP server task with the specified task configuration.
         * @param taskCfg The configuration to use for the UDP server task.
         */
        void Start(const TaskMgmt::TaskConfig& cfg);

        /**
         * Stops the UDP server and deletes the server task.
         */
        void Stop();

        /**
         * Sends an UDP packet with the specified data and length to the client from which the last packet was received.
         * Important: The method can only be called after a packet was received via "OnDataReceived(...)",
         * otherwise the client information is not available.
         * @param data The data to send in the UDP packet.
         * @param dataLength The length of the data to send in bytes.
         * @return The number of bytes successfully sent, or zero if the packet was not sent.
         */
        virtual size_t Respond(const uint8_t* data, size_t dataLength) = 0;

    protected:
        UdpServer(uint16_t port, size_t packetBufferSize);
        virtual ~UdpServer();
        
        // listens for incoming UDP packets in server task
        virtual void Listen();

        /**
         * Starts the underlying UDP server by binding to the specified port.
         * 
         * @param port The local port on which the server will listen for incoming UDP packets.
         * @return true if the server was successfully started, false otherwise.
         */
        virtual bool StartListening(uint16_t port) = 0;

        /** 
         * Wait for the next packet to be received, parse it if it is valid 
         * and call OnPacketReceived() for further processing.
         * 
         * @param packetBuffer The buffer to store the received packet data.
         * @param packetBufferLength The length of the packet buffer.
         * @return The size of the received packet or zero if no packet was received.
         */
        virtual size_t RetrieveNextPacket(uint8_t* packetBuffer, size_t packetBufferLength) = 0;

        /**
         * Processes a received UDP packet by calling the attached packet handler.
         * The packet data are expected being stored in the packet buffer provided by RetrieveNextPacket().
         * @param dataLength The length of the received packet data stored in the buffer in bytes.
         */
        virtual void OnDataReceived(const uint8_t* data, size_t dataLength);

    private:
        uint16_t mPort;
        size_t mPacketBufferSize;
        UdpPacketHandler mPacketHandler;
        TaskHandle_t mServerTask;
};

}
} // namespace Web::Udp