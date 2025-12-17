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

#pragma message("DUMMY: System time HAL")

#include <hal/system/SystemTimeHal.h>
#include <Arduino.h>


namespace Hal {

uint32_t SystemTimeHal::GetMilliseconds() {
	return millis();
}

uint64_t SystemTimeHal::GetMilliseconds64() {
	return SystemTimeHal::GetMilliseconds();
}

uint32_t SystemTimeHal::GetMicroseconds() {
	return millis() * 1000;
}

uint64_t SystemTimeHal::GetMicroseconds64() {
	return SystemTimeHal::GetMicroseconds();
}

} // namespace Hal
