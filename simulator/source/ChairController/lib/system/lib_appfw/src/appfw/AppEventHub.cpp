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

#include <appfw/AppEventHub.h>
#include <utils/Log.h>
#include <appfw/AppLogger.h>

namespace AppFw {

AppEventHub::AppEventHub() :
        mEventTargets() {
}

AppEventHub::~AppEventHub() {
}

void AppEventHub::AttachTarget(::std::weak_ptr<AppEventTarget> eventTarget) {
	AppLogDbg("-* TRYING to attach event target, event target cnt: %d", mEventTargets.size());
	if (!eventTarget.expired()) {
		mEventTargets.push_back(eventTarget);
		AppLogDbg("-* attached event target, event target cnt: %d", mEventTargets.size());
	}
}

void AppEventHub::OnEvent(AppEventPtr ev) {
	if (ev) {
		OnMyEvent(ev);
		DispatchEvent(ev);
	}
}

void AppEventHub::OnMyEvent(AppEventPtr ev) {
	// does nothing by default.
}

void AppEventHub::DispatchEvent(AppEventPtr ev) {
	::std::list<::std::weak_ptr<AppEventTarget>>::iterator etIt = mEventTargets.begin();
	AppOtherEvLogDbg(ev, AppExecEvent::Type, "-* dispatching event #0x%02X to %d targets", ev->GetType(), mEventTargets.size());
	while (etIt != mEventTargets.end()) {
		if ((*etIt).expired()) {
			AppLogDbg("-* event target is expired and removed");
			etIt = mEventTargets.erase(etIt);
		} else {
			AppOtherEvLogDbg(ev, AppExecEvent::Type, "-* dispatching event #0x%02X to target", ev->GetType());
			(*etIt).lock()->OnEvent(ev);
			etIt++;
		}
	}
}

} // namespace AppFw
