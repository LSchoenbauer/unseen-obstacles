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
#include <list>

namespace AppFw {

class AppEventHub : public AppEventTarget {
	public:
		/** Constructor */
		AppEventHub();

		/** Destructor */
		~AppEventHub();

		/**
		 * Attaches the given event target object for receiving events from this hub.
		 * An event target is automatically removed from the dispatcher list when it was destroyed.
		 */
		void AttachTarget(std::weak_ptr<AppEventTarget> eventTarget);

		/**
		 * Processes the received application event by forwarding it to the the method
		 * OnMyEvent() intended for being processed by the owned object
		 * and then to all attached AppEventTargets.
		 *
		 * Classes derived from this type shall therefore implement method 'OnMyEvent()'
		 * to process event addressed to the class itself.
		 *
		 * @param ev The event to process.
		 */
		virtual void OnEvent(AppEventPtr ev) final;

		/**
		 * Process events targeted for this class.
		 * @param ev The event to process.
		 */
		virtual void OnMyEvent(AppEventPtr ev);

		/** Dispatches the given event to all attached event targets. */
		void DispatchEvent(AppEventPtr ev);

	protected:

	private:
		/* prevent copies */
		AppEventHub(const AppEventHub& othr) = delete;
		/* prevent copies */
		AppEventHub operator =(AppEventHub src) = delete;

		::std::list<::std::weak_ptr<AppEventTarget>> mEventTargets;
};

} // namespace AppFw
