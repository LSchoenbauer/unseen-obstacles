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

#include <web/http/AbstractHtmlController.h>

namespace Web {
namespace Http {

class HttpServer;

/**
 * The abstract base class for REST controllers.
 *
 * Because REST controllers are typically able to serve the web page
 * to communicate with the REST API, this class provides also basic
 * HTML controller functionality.
 */
class AbstractRestController : public AbstractHtmlController {
		typedef AbstractHtmlController Base;
	public:

	protected:

		/** Constructor, child classes shall provide a public 'Create' method */
		AbstractRestController();
		/** Destructor */
		virtual ~AbstractRestController();

	private:
		friend class HttpServerImpl;
};

} // namespace Http
} // namespace Web
