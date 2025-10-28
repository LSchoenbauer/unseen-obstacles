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

#include <appfw/AppEventPoster.h>
#include <utils/Log.h>
#include <appfw/AppLogger.h>

namespace AppFw {

AppEventPoster* AppEventPoster::mMe = 0;

AppEventPoster::AppEventPoster() :
        Base() {
	mMe = this;
}

AppEventPoster::~AppEventPoster() {
	mMe = 0;
}

void AppEventPoster::Post(AppEventPtr ev) {
	if (mMe != 0 && ev) {
		AppOtherEvLogDbg(ev, AppExecEvent::Type, "-* posting event #0x%02X", ev->GetType());
		mMe->mEventQueue.push_back(ev);
	}
}

void AppEventPoster::ProcessEvents() {
	if (mMe != 0) {
		while (!mEventQueue.empty()) {
			AppEventPtr ev = mEventQueue.front();
			mEventQueue.pop_front();
			AppOtherEvLogDbg(ev, AppExecEvent::Type, "-* dispatching event #0x%02X", ev->GetType());
			mMe->DispatchEvent(ev);
		}
	}
}

} // namespace AppFw
