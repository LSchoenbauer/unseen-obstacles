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

#include <web/http/HttpResponse.h>
#include <utils/Log.h>
#include "impl/HttpResponseImpl.h"

namespace Web {
namespace Http {

::std::shared_ptr<HttpResponse> HttpResponse::Create(int status, const String& contentType, size_t contentLength) {
	return HttpResponseImpl::Create(status, contentType, contentLength);
}

HttpResponse::HttpResponse() {
}

HttpResponse::~HttpResponse() {
}

}
} // namespace Web::Http
