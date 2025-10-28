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

#include <web/http/HttpParameter.h>
#include <utils/ObjectWrapper.h>

#include <ESPAsyncWebServer.h>
#include <memory>

namespace Web {
namespace Http {

class HttpParameterImpl : public HttpParameter, public ObjectWrapper<AsyncWebParameter> {
		typedef HttpParameter Base;
		typedef ObjectWrapper<AsyncWebParameter> Wrapper;

	public:
		static ::std::shared_ptr<HttpParameter> Create(AsyncWebParameter* nativeParam);

		HttpParameterImpl(AsyncWebParameter* nativeParam);
		virtual ~HttpParameterImpl();

		inline virtual operator bool() const final {
			return HasWrapped();
		}

		virtual const String& GetName() const;
		virtual const String& GetValue() const;
		virtual size_t GetSize() const;
		virtual bool IsPost() const;
		virtual bool IsFile() const;

		virtual void DumpDiagnostics() const;

	protected:

	private:
};

}
} // namespace Web::Http
