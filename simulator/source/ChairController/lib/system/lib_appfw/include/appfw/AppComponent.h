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

#include <appfw/AppEvent.h>
#include <appfw/AppEventTarget.h>
#include <memory>

namespace AppFw {

class AppComponent : public AppEventTarget {
		typedef AppEventTarget Base;
	public:

	protected:
		/** Constructor */
		AppComponent();
		/** Destructor */
		virtual ~AppComponent();

	private:

};

typedef ::std::shared_ptr<AppComponent> AppCmpPtr;

} // namespace AppFw
