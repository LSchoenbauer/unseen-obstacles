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

#include <utils/Singleton.h>
#include <appfw/AppEventHub.h>
#include <appfw/AppEventTarget.h>
#include <appfw/AppEventPoster.h>
#include <appfw/AppComponent.h>
#include <appfw/AppEvent.h>
#include <memory>

namespace AppFw {

template<typename A>
class Application : public Singleton<A> {
		typedef Singleton<A> Base;

	public:
		/** Initializes the application. */
		virtual void Init() = 0;

		virtual void ProcessEvents() {
			mEventPoster.ProcessEvents();
		}

	protected:
		/** Constructor */
		Application() :
		        Base(), mEventPoster() {
		}

		/** Destructor */
		virtual ~Application() {
		}

		/** Attaches the given application component. */
		void Attach(AppCmpPtr cmp) {
			mEventPoster.AttachTarget(cmp);
		}

	private:
		AppEventPoster mEventPoster;
};

} // namespace AppFw
