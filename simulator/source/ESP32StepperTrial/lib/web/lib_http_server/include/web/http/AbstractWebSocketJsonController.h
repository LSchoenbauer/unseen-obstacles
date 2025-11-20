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

#include <web/http/AbstractWebSocketController.h>

#include <ArduinoJson.h>
#include <inttypes.h>

namespace Web {
namespace Http {

/**
 * Adds JSON capabilities to the WebSocket controller.
 * Derive from this class if the complete WebSocket communication
 * is JSON based.
 */
class AbstractWebSocketJsonController : public AbstractWebSocketController {
		typedef AbstractWebSocketController Base;
	public:

	protected:
		/**
		 * Constructor, child classes shall provide a public 'Create' method.
		 *
		 * @param wsUri The URI for accessing the WebSocket.
		 */
		AbstractWebSocketJsonController(const String& wsUri);

		/**
		 * Destructor
		 */
		virtual ~AbstractWebSocketJsonController();

		/**
		 * Sends the given JSON object to the client with the given ID.
		 *
		 * @param clientId The ID of the addressed client.
		 * @param message The JSON object to send.
		 */
		void NotifyClient(uint32_t clientId, const JsonObject& jsonObj);

		/**
		 * Sends the given JSON object to all connected clients.
		 *
		 * @param message The JSON object to send.
		 */
		void NotifyClients(const JsonObject& jsonObj);

		/**
		 * Implements conversion of received binary data into a JSON object.
		 */
		virtual void OnData(uint32_t clientId, uint8_t* data, size_t length, void* arg) override;

		/**
		 * This method is invoked if a JSON string is received and could be
		 * successfully parsed AND sufficient capacity is reserved
		 * via method 'PrepareJsonObjectBuffer' for storing the received
		 * JSON object.
		 *
		 * @param clientId The ID of the client that sent the data.
		 * @param jsonObj The received JSON object.
		 * @param arg The application specific argument that was routed through the server.
		 */
		virtual void OnJsonData(uint32_t clientId, const JsonObject& jsonObj, void* arg);

	private:

		JsonDocument  mRxJsonDoc;
};

} // namespace Http
} // namespace Web
