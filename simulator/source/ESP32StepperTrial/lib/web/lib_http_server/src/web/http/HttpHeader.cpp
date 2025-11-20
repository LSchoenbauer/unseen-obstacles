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

#include <web/http/HttpHeader.h>
#include <utils/Log.h>
#include "impl/HttpHeaderImpl.h"

namespace Web {
namespace Http {

HttpHeader::HttpHeader() {
}

HttpHeader::~HttpHeader() {
}

::std::shared_ptr<HttpHeader> HttpHeader::Create(const String& name, const String& value) {
	return HttpHeaderImpl::Create(name, value);
}

::std::shared_ptr<HttpHeader> HttpHeader::Create(const String& header) {
	return HttpHeaderImpl::Create(header);
}

}
} // namespace Web::Http
