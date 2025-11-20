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

#include <web/http/HttpRequestMethods.h>

namespace Web {
namespace Http {

const char* HttpRequestMethod::GetName(Enum method) {
	switch (method) {
		case GET:
			return "GET";
		case POST:
			return "POST";
		case DELETE:
			return "DELETE";
		case PUT:
			return "PUT";
		case PATCH:
			return "PATCH";
		case HEAD:
			return "HEAD";
		case OPTIONS:
			return "OPTIONS";
		case ANY:
			return "ANY";
	}
	return "UNKNOWN";
}

}
} // namespace Web::Http
