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

#include <web/http/impl/WebSocketClientImpl.h>

namespace Web {
namespace Http {

::std::shared_ptr<WebSocketClient> WebSocketClientImpl::Create(AsyncWebSocketClient* nativeClient) {
	return std::make_shared < WebSocketClientImpl > (nativeClient);
}

WebSocketClientImpl::WebSocketClientImpl(AsyncWebSocketClient* nativeClient) :
        Base(), Wrapper(nativeClient) {
}

WebSocketClientImpl::~WebSocketClientImpl() {
}

uint32_t WebSocketClientImpl::GetId() {
	return (HasWrapped() ? GetWrapped()->id() : 0);
}

Ip4Address WebSocketClientImpl::GetIpAddress() {
	return (HasWrapped() ? GetWrapped()->remoteIP() : Ip4Address());
}

uint16_t WebSocketClientImpl::GetPort() {
	return (HasWrapped() ? GetWrapped()->remotePort() : 0);
}

uint16_t WebSocketClientImpl::GetKeepAlivePeriod() {
	return (HasWrapped() ? GetWrapped()->keepAlivePeriod() : 0);
}

void WebSocketClientImpl::SetKeepAlivePeriod(uint16_t value) {
	if (HasWrapped()) {
		GetWrapped()->keepAlivePeriod(value);
	}
}

bool WebSocketClientImpl::IsConnected() {
	return (HasWrapped() ? GetWrapped()->status() == WS_CONNECTED : false);
}

bool WebSocketClientImpl::IsDisconnecting() {
	return (HasWrapped() ? GetWrapped()->status() == WS_DISCONNECTING : false);
}

bool WebSocketClientImpl::CanSend() {
	return (HasWrapped() ? GetWrapped()->canSend() : false);
}

void WebSocketClientImpl::Disconnect(uint16_t code, const String& message) {
	if (HasWrapped()) {
		GetWrapped()->close(code, message.c_str());
	}
}

void WebSocketClientImpl::Ping(uint8_t* data, size_t length) {
	if (HasWrapped()) {
		GetWrapped()->ping(data, length);
	}
}

void WebSocketClientImpl::Notify(uint8_t* data, size_t length) {
	if (HasWrapped()) {
		GetWrapped()->binary(data, length);
	}
}

void WebSocketClientImpl::Notify(const String& message) {
	if (HasWrapped()) {
		GetWrapped()->text(message);
	}
}

} // namespace Http
} // namespace Web
