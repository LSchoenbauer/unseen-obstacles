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

#include <web/http/AbstractWebSocketJsonController.h>
#include <utils/Log.h>

#include <WString.h>

namespace Web {
namespace Http {

AbstractWebSocketJsonController::AbstractWebSocketJsonController(const String& wsUri) :
        Base(wsUri), mRxJsonDoc() {
}

AbstractWebSocketJsonController::~AbstractWebSocketJsonController() {
}

void AbstractWebSocketJsonController::NotifyClient(uint32_t clientId, const JsonObject& jsonObj) {
	if (!jsonObj.isNull()) {
		String jsonStr;
		serializeJson(jsonObj, jsonStr);
		Base::NotifyClient(clientId, jsonStr);
	}
}

void AbstractWebSocketJsonController::NotifyClients(const JsonObject& jsonObj) {
	if (!jsonObj.isNull()) {
		String jsonStr;
		serializeJson(jsonObj, jsonStr);
		Base::NotifyClients(jsonStr);
	}
}

void AbstractWebSocketJsonController::OnData(uint32_t clientId, uint8_t* data, size_t length, void* arg) {
	if (data != 0 && length > 0) {
		LogDbg("Retrieved data: '%s' of size %d", data, length);
		mRxJsonDoc.clear();
		DeserializationError rc = deserializeJson(mRxJsonDoc, data, length);
		if (rc == DeserializationError::Ok) {
			OnJsonData(clientId, mRxJsonDoc.to<JsonObject>(), arg);
		} else {
			LogWarn("Deserialization of retrieved JSON document failed with error '%s'", rc.c_str());
		}
	}
}

void AbstractWebSocketJsonController::OnJsonData(uint32_t clientId, const JsonObject& jsonObj, void* arg) {
	// does nothing by intension
}

}
// namespace Http
}// namespace Web
