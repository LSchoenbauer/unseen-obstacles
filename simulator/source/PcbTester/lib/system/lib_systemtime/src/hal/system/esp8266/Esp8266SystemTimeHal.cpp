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

#pragma message("ESP8266: System time HAL")

#include <hal/system/SystemTimeHal.h>
#include <user_interface.h>

namespace Hal {

uint32_t SystemTimeHal::GetMilliseconds() {
	return system_get_time() / 1000;
}

uint32_t SystemTimeHal::GetMicroseconds() {
	return system_get_time();
}

} // namespace Hal

