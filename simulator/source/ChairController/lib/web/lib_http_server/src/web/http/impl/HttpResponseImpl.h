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

#include <web/http/HttpResponse.h>
#include <utils/ObjectWrapper.h>

#include <ESPAsyncWebServer.h>
#include <memory>

namespace Web {
namespace Http {

class HttpResponseImpl : public HttpResponse, public ObjectWrapper<AsyncWebServerResponse> {
		typedef HttpResponse Base;
		typedef ObjectWrapper<AsyncWebServerResponse> Wrapper;

	public:
		static ::std::shared_ptr<HttpResponse> Create(AsyncWebServerResponse* nativeResponse);
		static ::std::shared_ptr<HttpResponse> Create(int status, const String& contentType = String(),
		        size_t contentLength = 0);

		HttpResponseImpl(AsyncWebServerResponse* nativeResponse, bool destructNative);
		virtual ~HttpResponseImpl();

		inline virtual operator bool() const final {
			return HasWrapped();
		}

		virtual void AddHeader(const String& name, const String& value);
		virtual void AddHeader(const HttpHeader& header);

		virtual void DumpDiagnostics() const;

	protected:

	private:
		bool mDeleteNativeOnDestruction;
};

}
} // namespace Web::Http
