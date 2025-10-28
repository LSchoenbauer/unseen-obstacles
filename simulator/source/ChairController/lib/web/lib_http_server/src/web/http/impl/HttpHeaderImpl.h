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

#include <web/http/HttpHeader.h>
#include <utils/ObjectWrapper.h>

#include <ESPAsyncWebServer.h>
#include <memory>

namespace Web {
namespace Http {

class HttpHeaderImpl : public HttpHeader, public ObjectWrapper<AsyncWebHeader> {
		typedef HttpHeader Base;
		typedef ObjectWrapper<AsyncWebHeader> Wrapper;

	public:
		static ::std::shared_ptr<HttpHeader> Create(AsyncWebHeader* nativeHdr);
		static ::std::shared_ptr<HttpHeader> Create(const String& name, const String& value);
		static ::std::shared_ptr<HttpHeader> Create(const String& header);

		HttpHeaderImpl(AsyncWebHeader* nativeHdr, bool destructNative);
		virtual ~HttpHeaderImpl();

		inline virtual operator bool() const final {
			return HasWrapped();
		}

		virtual const String& GetName() const;
		virtual const String& GetValue() const;
		virtual String ToString() const;

		virtual void DumpDiagnostics() const;

	protected:

	private:
		bool mDeleteNativeOnDestruction;
};

}
} // namespace Web::Http
