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

#include <web/http/AbstractHtmlController.h>
#include <web/http/WebSocketClient.h>

#include <WString.h>
#include <inttypes.h>
#include <memory>

class AsyncWebSocket;

namespace Web {
namespace Http {

class HttpServer;

/**
 * Specifies the interface and basic implementation for
 * HTTP controllers that operate on WebSockets.
 *
 * Because WebSocket controllers are typically able to serve the web page
 * to communicate with the WebSocket API, this class provides also basic
 * HTML controller functionality.
 */
class AbstractWebSocketController : public AbstractHtmlController {
		typedef AbstractHtmlController Base;
	public:

		/** Implements the iterator over all registered WebSocket clients. */
		class ClientIterator {
			public:

				/**
				 * Determines whether or not this iterator can provide at least one further client.
				 * @return True if there is another client, false otherwise.
				 */
				virtual bool HasNext() = 0;

				/**
				 * Provides the next client or an invalid shared pointer, if there is no further client.
				 *
				 * @return The next client.
				 */
				virtual std::shared_ptr<WebSocketClient> GetNext() = 0;

			protected:
				ClientIterator() {
				}
				virtual ~ClientIterator() {
				}
		};

		/**
		 * Applies the URI relative to HTTP root for connecting to the Web Socket.
		 * The URI must be applied BEFORE attaching the controller to the server.
		 * @param uri The WebSocket URI.
		 */
		virtual void SetWsUri(const String& uri);

		/**
		 * Provides the URI of this WebSocket server.
		 *
		 * @return The URI.
		 */
		const String& GetWsUri() const;

		/**
		 * Provides the client with the given ID or an invalid pointer
		 * if no such client exists.
		 *
		 * @param clientId The ID of the client to retrieve.
		 * @return The shared pointer to the client.
		 */
		std::shared_ptr<WebSocketClient> GetClient(uint32_t clientId);

		/**
		 * Provides the iterator for iterating through all registered clients.
		 * Initially the iterator provides the first client via its 'GetNext()' method.
		 *
		 * @return The shared pointer to the client iterator.
		 */
		std::shared_ptr<ClientIterator> GetClientIterator();

		/**
		 * Provides the number of currently connected clients.
		 *
		 * @return The number of clients.
		 */
		size_t GetClientCount();

		/**
		 * Disconnects the client with the given ID using the provided code and optional message.
		 *
		 * @param clientId The ID of the client to disconnect.
		 * @param code The code to send to the client.
		 * @param message The optional message to send to the client.
		 */
		void DisconnectClient(uint32_t clientId, uint16_t code, const String& message = String(""));

		/**
		 * Disconnects all connected client using the provided code and optional message.
		 *
		 * @param code The code to send to all the clients.
		 * @param message The optional message to send to all the clients.
		 */
		void DisconnectClients(uint16_t code, const String& message = String(""));

		/**
		 * Sends a ping to the client with the given ID.
		 *
		 * @param clientId The ID of the addressed client.
		 * @param data The data to send with the ping.
		 * @param length The size of the data package.
		 */
		void PingClient(uint32_t clientId, uint8_t* data = 0, size_t length = 0);

		/**
		 * Sends a ping to all connected clients.
		 *
		 * @param data The data to send with the ping.
		 * @param length The size of the data package.
		 */
		void PingClients(uint8_t* data = 0, size_t length = 0);

		/**
		 * Sends the given binary data to the client with the given ID.
		 *
		 * @param clientId The ID of the addressed client.
		 * @param data The data to send.
		 * @param length The size of the data package.
		 */
		void NotifyClient(uint32_t clientId, uint8_t* data, size_t length);

		/**
		 * Sends the given binary data to all connected clients.
		 *
		 * @param data The data to send.
		 * @param length The size of the data package.
		 */
		void NotifyClients(uint8_t* data, size_t length);

		/**
		 * Sends the given textual message to the client with the given ID.
		 *
		 * @param clientId The ID of the addressed client.
		 * @param message The message to send.
		 */
		void NotifyClient(uint32_t clientId, const String& message);

		/**
		 * Sends the given textual message to all connected clients.
		 *
		 * @param message The message to send.
		 */
		void NotifyClients(const String& message);

		/** Prints information about this REST Controller. */
		virtual void DumpDiagnostics();

	protected:

		/**
		 * Constructor, child classes shall provide a public 'Create' method.
		 *
		 * @param wsUri The URI for accessing the WebSocket.
		 */
		AbstractWebSocketController(const String& wsUri);
		/** Destructor */
		virtual ~AbstractWebSocketController();

		/**
		 * This method is invoked when a client got connected to the server.
		 *
		 * @param clientId The ID of the connected client.
		 * @param arg The application specific argument that was routed through the server.
		 */
		virtual void OnClientConnected(uint32_t clientId, void* arg);

		/**
		 * This method is invoked if a data event was received AND if no more specific data
		 * handler methods could be successfully invoked, e.g. due to data validation.
		 *
		 * @param clientId The ID of the client that sent the data.
		 * @param data The data carried by the event in binary format.
		 * @param length The size of the data.
		 * @param arg The application specific argument that was routed through the server.
		 */
		virtual void OnData(uint32_t clientId, uint8_t* data, size_t length, void* arg);

		/**
		 * This method is invoked when a ping request was received.
		 *
		 * @param clientId The ID of the pinging client.
		 * @param arg The application specific argument that was routed through the server.
		 */
		virtual void OnPing(uint32_t clientId, void* arg);

		/**
		 * This method is invoked when an error occurred.
		 *
		 * @param clientId The ID of the client that threw an error.
		 * @param arg The application specific argument that was routed through the server.
		 */
		virtual void OnError(uint32_t clientId, void* arg);

		/**
		 * This method is invoked when a client got disconnected from the server.
		 *
		 * @param clientId The ID of the disconnected client.
		 * @param arg The application specific argument that was routed through the server.
		 */
		virtual void OnClientDisconnected(uint32_t clientId, void* arg);

	private:
		friend class HttpServerImpl;

		AsyncWebSocket& GetNativeWebSocket();

		String mWsUri;
};

} // namespace Http
} // namespace Web
