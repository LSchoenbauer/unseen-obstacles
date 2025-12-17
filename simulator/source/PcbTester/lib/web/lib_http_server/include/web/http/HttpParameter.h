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

#include <WString.h>

namespace Web {
namespace Http {

class HttpParameter {
	public:
		/** Determines whether or not this parameter is valid. */
		virtual operator bool() const = 0;
		/** Provides the name / key of the parameter. */
		virtual const String& GetName() const = 0;
		/** Provides the value of the parameter. */
		virtual const String& GetValue() const = 0;
		/** Provides the size of the parameter value. */
		virtual size_t GetSize() const = 0;
		/** Determines whether or not this parameter is a post parameter. */
		virtual bool IsPost() const = 0;
		/** Determines whether or not this parameter is a file. */
		virtual bool IsFile() const = 0;

	protected:
		HttpParameter() {
		}
		virtual ~HttpParameter() {
		}
};

}
} // namespace Web::Http
