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

#include <web/http/HttpServer.h>
#include <web/http/AbstractRestController.h>
#include "impl/HttpServerImpl.h"
#include <utils/Log.h>

namespace Web {
namespace Http {

HttpServerPtr HttpServer::Create(uint16_t port, const String& fsDocRoot) {
	return HttpServerImpl::Create(port, fsDocRoot);
}

HttpServer::HttpServer() {
}

HttpServer::~HttpServer() {
}

}
} // namespace Web::Http
