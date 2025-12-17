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

#include "HttpHeaderImpl.h"
#include <utils/Log.h>
#include <utils/SharedGlobals.h>

namespace Web {
namespace Http {

::std::shared_ptr<HttpHeader> HttpHeaderImpl::Create(AsyncWebHeader* nativeHdr) {
	return ::std::make_shared<HttpHeaderImpl>(nativeHdr, false);
}

::std::shared_ptr<HttpHeader> HttpHeaderImpl::Create(const String& name, const String& value) {
	AsyncWebHeader* nativeHdr = new AsyncWebHeader(name, value);
	return ::std::make_shared<HttpHeaderImpl>(nativeHdr, true);
}

::std::shared_ptr<HttpHeader> HttpHeaderImpl::Create(const String& header) {
	AsyncWebHeader* nativeHdr = new AsyncWebHeader(header);
	return ::std::make_shared<HttpHeaderImpl>(nativeHdr, true);
}

HttpHeaderImpl::HttpHeaderImpl(AsyncWebHeader* nativeHdr, bool destructNative) :
        Base(), Wrapper(nativeHdr), mDeleteNativeOnDestruction(destructNative) {
}

HttpHeaderImpl::~HttpHeaderImpl() {
	if (mDeleteNativeOnDestruction) {
		DeleteWrapped();
	}
}

const String& HttpHeaderImpl::GetName() const {
	return (HasWrapped() ? GetWrapped()->name() : Gbl::invalidString);
}

const String& HttpHeaderImpl::GetValue() const {
	return (HasWrapped() ? GetWrapped()->value() : Gbl::invalidString);
}

String HttpHeaderImpl::ToString() const {
	return (HasWrapped() ? GetWrapped()->toString() : Gbl::invalidString);
}

void HttpHeaderImpl::DumpDiagnostics() const {
	LogInfo("HTTP Header: %s: %s", Logger::Dump(GetName()), Logger::Dump(GetValue()));
}

}
} // namespace Web::Http
