/*
 * ===============================================================================
 * c) HTL Leonding
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

#include <WString.h>
#include <memory>

namespace Web {
namespace Http {

class HttpHeader {
	public:
		/** Creates a new HTTP Header with the given values. */
		static ::std::shared_ptr<HttpHeader> Create(const String& name, const String& value);
		/** Creates a new HTTP Header with the given value in format <name>:<value>. */
		static ::std::shared_ptr<HttpHeader> Create(const String& header);

		/** Provides the name / key of the header. */
		virtual const String& GetName() const = 0;
		/** Provides the value of the header. */
		virtual const String& GetValue() const = 0;
		/** Provides the header as string. */
		virtual String ToString() const = 0;

	protected:
		HttpHeader();
		virtual ~HttpHeader();
};

}
} // namespace Web::Http
