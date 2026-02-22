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

#include <inttypes.h>
#include <memory>

namespace AppFw {

typedef uint32_t EventType;

class AppEvent {
	public:

		/** The concrete type of the event. */
		static const EventType Type = 0x01;

		virtual bool IsOf(EventType type) const;
		virtual EventType GetType() const;

	protected:

		/** Constructor */
		AppEvent();
		/** Destructor */
		virtual ~AppEvent();

	private:
};

typedef std::shared_ptr<AppEvent> AppEventPtr;

class AppExecEvent : public AppEvent {
		typedef AppEvent Base;
	public:
		static const EventType Type = 0x02;

		/** Constructor */
		AppExecEvent();
		/** Destructor */
		virtual ~AppExecEvent();

		virtual bool IsOf(EventType type) const override;
		virtual EventType GetType() const override;
};

// other events:
//AppTimerEvent:  0x04
//WifiReadyEvent: 0x10

}// namespace AppFw
