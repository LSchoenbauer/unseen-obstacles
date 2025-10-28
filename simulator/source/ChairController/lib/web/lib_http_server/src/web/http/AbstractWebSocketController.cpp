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

#include <web/http/AbstractWebSocketController.h>
#include <web/http/impl/WebSocketClientImpl.h>
#include <web/http/HttpServer.h>
#include <utils/Log.h>

#include <AsyncWebSocket.h>

namespace Web {
namespace Http {

class WebSocketClientIteratorImpl : public AbstractWebSocketController::ClientIterator {
	public:

		static std::shared_ptr<AbstractWebSocketController::ClientIterator> Create(
		        AsyncWebSocket::AsyncWebSocketClientLinkedList::ConstIterator& nativeClientIt) {
			return std::make_shared<WebSocketClientIteratorImpl>(nativeClientIt);
		}

		WebSocketClientIteratorImpl(AsyncWebSocket::AsyncWebSocketClientLinkedList::ConstIterator& nativeClientIt) {
			GetPlus(false, &nativeClientIt);
		}

		virtual ~WebSocketClientIteratorImpl() {
		}

		virtual bool HasNext() override {
			return GetPlus() != AsyncWebSocket::AsyncWebSocketClientLinkedList::ConstIterator(nullptr);
		}

		virtual std::shared_ptr<WebSocketClient> GetNext() override {
			std::shared_ptr<WebSocketClient> client = (
			        (*GetPlus()) ? WebSocketClientImpl::Create(*GetPlus()) : std::shared_ptr<WebSocketClient>());
			GetPlus(true);
			return client;
		}

	private:
		// work-around for immutable iterator proceeding - not sure if this works.
		AsyncWebSocket::AsyncWebSocketClientLinkedList::ConstIterator& GetPlus(bool inc = false,
		        AsyncWebSocket::AsyncWebSocketClientLinkedList::ConstIterator* initIt = 0) {
			static auto clientIt = (*initIt);
			if (inc) {
				++clientIt;
			}
			return clientIt;
		}

};

AbstractWebSocketController::AbstractWebSocketController(const String& wsUri) :
        Base(), mWsUri(wsUri) {
}

AbstractWebSocketController::~AbstractWebSocketController() {
}

void AbstractWebSocketController::SetWsUri(const String& uri) {
	if (uri && !uri.isEmpty()) {
		mWsUri = uri;
	}
}

const String& AbstractWebSocketController::GetWsUri() const {
	return mWsUri;
}

std::shared_ptr<WebSocketClient> AbstractWebSocketController::GetClient(uint32_t clientId) {
	AsyncWebSocketClient* client = GetNativeWebSocket().client(clientId);
	return WebSocketClientImpl::Create(client);
}

std::shared_ptr<AbstractWebSocketController::ClientIterator> AbstractWebSocketController::GetClientIterator() {
	return WebSocketClientIteratorImpl::Create(GetNativeWebSocket().getClients().begin());
}

size_t AbstractWebSocketController::GetClientCount() {
	return GetNativeWebSocket().count();
}

void AbstractWebSocketController::DisconnectClient(uint32_t clientId, uint16_t code, const String& message) {
	GetNativeWebSocket().close(clientId, code, message.c_str());
}

void AbstractWebSocketController::DisconnectClients(uint16_t code, const String& message) {
	GetNativeWebSocket().closeAll(code, message.c_str());
}

void AbstractWebSocketController::PingClient(uint32_t clientId, uint8_t* data, size_t length) {
	GetNativeWebSocket().ping(clientId, data, length);
}

void AbstractWebSocketController::PingClients(uint8_t* data, size_t length) {
	GetNativeWebSocket().pingAll(data, length);
}

void AbstractWebSocketController::NotifyClient(uint32_t clientId, uint8_t* data, size_t length) {
	GetNativeWebSocket().binary(clientId, data, length);
}

void AbstractWebSocketController::NotifyClients(uint8_t* data, size_t length) {
	GetNativeWebSocket().binaryAll(data, length);
}

void AbstractWebSocketController::NotifyClient(uint32_t clientId, const String& message) {
	GetNativeWebSocket().text(clientId, message);
}

void AbstractWebSocketController::NotifyClients(const String& message) {
	GetNativeWebSocket().textAll(message);
}

void AbstractWebSocketController::OnClientConnected(uint32_t clientId, void* arg) {
// does nothing by intention
}

void AbstractWebSocketController::OnData(uint32_t clientId, uint8_t* data, size_t length, void* arg) {
// does nothing by intention
}

void AbstractWebSocketController::OnPing(uint32_t clientId, void* arg) {
// does nothing by intention
}

void AbstractWebSocketController::OnError(uint32_t clientId, void* arg) {
// does nothing by intention
}

void AbstractWebSocketController::OnClientDisconnected(uint32_t clientId, void* arg) {
// does nothing by intention
}

AsyncWebSocket& AbstractWebSocketController::GetNativeWebSocket() {
	static AsyncWebSocket ws(mWsUri);
	static bool initialized = false;
	if (!initialized) {
		ws.onEvent(
		        [this](AsyncWebSocket* server, AsyncWebSocketClient* client, AwsEventType type, void* arg, uint8_t* data,
		                size_t len) {
			        if (client != 0) {
				        uint32_t clientId = client->id();
				        switch (type) {
					        case WS_EVT_CONNECT:
						        LogDbg("Client [%5d] connected", clientId);
						        OnClientConnected(clientId, arg);
						        break;
					        case WS_EVT_DATA:
						        LogDbg("Received data from client [%5d]", clientId);
						        OnData(clientId, data, len, arg);
						        break;
					        case WS_EVT_DISCONNECT:
						        LogDbg("Client [%5d] disconnected", clientId);
						        OnClientDisconnected(clientId, arg);
						        break;
					        case WS_EVT_PONG:
						        LogDbg("Pong to client [%5d]", clientId);
						        OnPing(clientId, arg);
						        break;
					        case WS_EVT_ERROR:
						        LogDbg("Error at client [%5d]", clientId);
						        OnError(clientId, arg);
						        break;
				        }
			        }
		        });
	}
	initialized = true;
	return ws;
}

void AbstractWebSocketController::DumpDiagnostics() {
	Base::DumpDiagnostics();
	LogInfo("GET URI for WebSocket:   %s", GetWsUri() ? GetWsUri().c_str() : "INVALID");
}

} // namespace Http
} // namespace Web
