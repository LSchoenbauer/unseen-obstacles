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

#include <WString.h>
#include <memory>

namespace Web {
namespace Http {

class HttpResponse {
	public:
		/** Creates a new HTTP Response with the given values.
		 * @param status The HTTP status code of the response.
		 * @param contentType The optional content type of the response.
		 * @param contentLength The length of the responded content.
		 * @return The created HTTP Response instance.
		 */
		static ::std::shared_ptr<HttpResponse> Create(int status, const String& contentType = String(), size_t contentLength = 0);

		/** Determines whether or not this response is valid. */
		virtual operator bool() const = 0;

		/** Adds the HTTP header with the given name and value to the response.
		 * @param name The name of the HTTP header.
		 * @param value The value of the HTTP header.
		 */
		virtual void AddHeader(const String& name, const String& value) = 0;

		/** Adds the given HTTP header to the response.
		 * @param header The HTTP header to add.
		 */
		virtual void AddHeader(const HttpHeader& header) = 0;

	protected:
		HttpResponse();
		virtual ~HttpResponse();
};

}
} // namespace Web::Http
