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

#include <appfw/AppEventHub.h>
#include <appfw/AppEvent.h>
#include <memory>
#include <list>

namespace AppFw {

class AppEventPosterMementoImpl;

/**
 * Dispatcher for application events.
 */
class AppEventPoster : public AppEventHub {
		typedef AppEventHub Base;

	public:
		/** Dispatches an application event. */
		static void Post(AppEventPtr ev);

	protected:
		template<typename > friend class Application;
		/** Constructor */
		AppEventPoster();
		/** Destructor */
		~AppEventPoster();
		/** Dispatches all events inside the event queue*/
		void ProcessEvents();

	private:
		// poor mans singleton - intended for being instantiated by app only
		static AppEventPoster* mMe;
		::std::list<AppEventPtr> mEventQueue;

};

} // namespace AppFw
