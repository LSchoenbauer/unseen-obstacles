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

#include <appfw/AppEvent.h>

namespace AppFw {

AppEvent::AppEvent() {
}

AppEvent::~AppEvent() {
}

bool AppEvent::IsOf(EventType type) const {
	return type == AppEvent::Type;
}

EventType AppEvent::GetType() const {
	return AppEvent::Type;
}

AppExecEvent::AppExecEvent() :
        Base() {
}

AppExecEvent::~AppExecEvent() {
}

bool AppExecEvent::IsOf(EventType type) const {
	return (type == AppExecEvent::Type || Base::IsOf(type));
}

EventType AppExecEvent::GetType() const {
	return AppExecEvent::Type;
}

} // namespace AppFw
