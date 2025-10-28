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

#pragma once

#include <web/http/WebSocketClient.h>
#include <utils/ObjectWrapper.h>
#include <AsyncWebSocket.h>

#include <WString.h>
#include <inttypes.h>
#include <memory>

namespace Web {
namespace Http {

/**
 * Implementation of WebSocket Client using AsyncWebSocketClient.
 */
class WebSocketClientImpl : public WebSocketClient, public ObjectWrapper<AsyncWebSocketClient> {
		typedef WebSocketClient Base;
		typedef ObjectWrapper<AsyncWebSocketClient> Wrapper;
	public:
		static ::std::shared_ptr<WebSocketClient> Create(AsyncWebSocketClient* nativeClient);

		WebSocketClientImpl(AsyncWebSocketClient* nativeClient);
		virtual ~WebSocketClientImpl();

		inline virtual operator bool() const final {
			return HasWrapped();
		}

		/**
		 * Provides the ID of the client.
		 *
		 * @return The client ID.
		 */
		virtual uint32_t GetId() override;

		/**
		 * Provides the IP address of the remove client.
		 *
		 * @return The remote IP address.
		 */
		virtual Ip4Address GetIpAddress() override;

		/**
		 * Provides the port of the remove client.
		 *
		 * @return The remote port.
		 */
		virtual uint16_t GetPort() override;

		/**
		 * Provides the period for keeping the connection established.
		 *
		 * @return The keep alive period in seconds.
		 */
		virtual uint16_t GetKeepAlivePeriod() override;

		/**
		 * Applies the period for keeping the connection established.
		 *
		 * @param value The keep alive period in seconds.
		 */
		virtual void SetKeepAlivePeriod(uint16_t value) override;

		/**
		 * Determines whether or not the client is connected.
		 *
		 * @return True if the client is connected, false otherwise.
		 */
		virtual bool IsConnected() override;

		/**
		 * Determines whether or not disconnecting the client is in progress.
		 *
		 * @return True if the client is disconnecting, false otherwise.
		 */
		virtual bool IsDisconnecting() override;

		/**
		 * Determines whether or not this client is able to send data.
		 *
		 * @return True if data can be sent, false otherwise.
		 */
		virtual bool CanSend() override;

		/**
		 * Disconnects the client using the provided code and optional message.
		 *
		 * @param code The code to send to the client.
		 * @param message The optional message to send to the client.
		 */
		virtual void Disconnect(uint16_t code, const String& message = String("")) override;

		/**
		 * Sends a ping to the client.
		 *
		 * @param data The data to send with the ping.
		 * @param length The size of the data package.
		 */
		virtual void Ping(uint8_t* data = 0, size_t length = 0) override;

		/**
		 * Sends the given binary data to the client.
		 *
		 * @param data The data to send.
		 * @param length The size of the data package.
		 */
		virtual void Notify(uint8_t* data, size_t length) override;

		/**
		 * Sends the given textual message to the client.
		 *
		 * @param message The message to send.
		 */
		virtual void Notify(const String& message) override;

	private:
};

} // namespace Http
} // namespace Web
