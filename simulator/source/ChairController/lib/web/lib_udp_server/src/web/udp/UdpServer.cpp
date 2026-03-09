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

#include <web/udp/UdpServer.h>
#include <web/udp/impl/UdpServerImpl.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace Web {
namespace Udp {

using namespace Os;

const Logger::Tag UdpServer::LTAG_CFG = CreateLogTag("UDP_CFG");
const Logger::Tag UdpServer::LTAG_RX = CreateLogTag("UDP_RX");
const Logger::Tag UdpServer::LTAG_TX = CreateLogTag("UDP_TX");

UdpServerPtr UdpServer::Create(uint16_t port, size_t packetBufferSize) {
    return UdpServerImpl::Create(port, packetBufferSize);
}

UdpServer::UdpServer(uint16_t port, size_t packetBufferSize) : 
        mPort(port), mPacketBufferSize(packetBufferSize), 
        mPacketHandler(nullptr), mServerTask(nullptr) 
{
    LogDbgT(LTAG_CFG, "UDP Server: Created with port '%d' and buffer size %d", port, mPacketBufferSize);
}

UdpServer::~UdpServer() {
    // task will stop and delete itself
    mServerTask = nullptr;
}

void UdpServer::OnPacket(UdpPacketHandler packetHandler) {
    mPacketHandler = packetHandler;
    LogDbgT(LTAG_CFG, "UDP Server: Packet handler attached");
}

void UdpServer::Start() {
    TaskMgmt::TaskConfig cfg = { 
        4242,
        "UdpServer",
        TaskMgmt::CoreId::CORE_1, 
        4096, 
        configMAX_PRIORITIES - 13 
    };    
    Start(cfg);
}

void UdpServer::Start(const TaskMgmt::TaskConfig& taskCfg) {
    if (mServerTask == nullptr) {
        if (mPacketHandler != nullptr) {
            mServerTask = TaskMgmt::CreateTask(
                taskCfg,
                [](void* params) { // Task function - lambda
                    UdpServer* server = static_cast<UdpServer*>(params);
                    LogDbgT(LTAG_CFG, "UDP Server: Task created, waiting to start ...");
                    ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(50)); // wait for the signal that task creation is complete
                    LogDbgT(LTAG_CFG, "UDP Server: Started");
                    server->Listen();
                },
                this
            );
            if (mServerTask != nullptr) {
                LogDbgT(LTAG_CFG, "UDP Server: Server task creation completed, starting it now");
                xTaskNotifyGive(mServerTask); // signal the task that creation is complete
            }
        } else {
            LogErrorT(LTAG_CFG, "UDP Server: Denied starting server: No packet handler attached!");
        }
    } else {
        LogWarnT(LTAG_CFG, "UDP Server: Skipped recreation of server task");
    }
}

void UdpServer::Stop() {
    if (mServerTask != nullptr) {
        LogDbgT(LTAG_CFG, "UDP Server: Stopping server task");
        vTaskDelete(mServerTask);
        mServerTask = nullptr;
    }
}

void UdpServer::Listen() {
    uint8_t buffer[mPacketBufferSize];

    if (StartListening(mPort)) {
        LogDbgT(LTAG_CFG, "UDP Server: Listening on port %d...", mPort);
        while (mServerTask != nullptr) {
            size_t dataSize = RetrieveNextPacket(buffer, mPacketBufferSize);
            if (dataSize > 0) {
                OnDataReceived(buffer, dataSize);
            } else {
                // RetrieveNextPacket(...) is supposed to block, but in some environments it doesn't
                // LogDbgT(LTAG_RX, "UDP Server: No data received");
                vTaskDelay(pdMS_TO_TICKS(5)); // wait a bit before checking for new packets to avoid busy waiting
            }
        }
        LogDbgT(LTAG_CFG, "UDP Server: Stopped listening");
        vTaskDelay(pdMS_TO_TICKS(100));
        vTaskDelete(NULL);
    } else  {
        LogErrorT(LTAG_CFG, "UDP Server: Failed to start server on port %d", mPort);
    }
}

void UdpServer::OnDataReceived(const uint8_t* data, size_t dataLength) {
    if (mPacketHandler != nullptr) {
        mPacketHandler(data, dataLength);
    }
}

}
} // namespace Web::Udp