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

#include "HttpResponseImpl.h"
#include <utils/Log.h>
#include <utils/SharedGlobals.h>

namespace Web {
namespace Http {

::std::shared_ptr<HttpResponse> HttpResponseImpl::Create(AsyncWebServerResponse* nativeResponse) {
	return ::std::make_shared < HttpResponseImpl > (nativeResponse, true);
}

::std::shared_ptr<HttpResponse> HttpResponseImpl::Create(int status, const String& contentType, size_t contentLength) {
	AsyncWebServerResponse* nativeRsp = new AsyncWebServerResponse();
	if (nativeRsp != 0) {
		nativeRsp->setCode(status);
		if (contentType) {
			nativeRsp->setContentType(contentType);
			nativeRsp->setContentLength(contentLength);
		}
	}
	return ::std::make_shared < HttpResponseImpl > (nativeRsp, true);
}

HttpResponseImpl::HttpResponseImpl(AsyncWebServerResponse* nativeResponse, bool destructNative) :
        Base(), Wrapper(nativeResponse), mDeleteNativeOnDestruction(destructNative) {
}

HttpResponseImpl::~HttpResponseImpl() {
}

void HttpResponseImpl::AddHeader(const String& name, const String& value) {
	if (HasWrapped()) {
		GetWrapped()->addHeader(name, value);
	}
}

void HttpResponseImpl::AddHeader(const HttpHeader& header) {
	if (HasWrapped()) {
		GetWrapped()->addHeader(header.GetName(), header.GetValue());
	}
}

void HttpResponseImpl::DumpDiagnostics() const {
	LogInfo("HTTP Response");
}

}
} // namespace Web::Http
