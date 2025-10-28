/*
 * ===============================================================================
 * c) HTL Leonding
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

// ESP32 INLINE implementation of ResultCode
// this implementation variant is used of HAL layer
// !! keep in sync with non-inline variant !!

#pragma once

#include <utils/ResultCode.h>
#include <esp_err.h>

/** 
 * Intended for hardware specific implementations.
 * Do not use in applications or generic libraries
 */
inline bool Rc::IsOk() const {
	return static_cast<esp_err_t>(mCode) == ESP_OK;
}

/** 
 * Intended for hardware specific implementations.
 * Do not use in applications or generic libraries
 */
inline const char* Rc::GetName() const {
	return esp_err_to_name(static_cast<esp_err_t>(mCode));
}

/** 
 * Intended for hardware specific implementations.
 * Do not use in applications or generic libraries
 */
inline const char* Rc::CopyName(char* target, size_t targetLength) const {
	return esp_err_to_name_r(static_cast<esp_err_t>(mCode), target, targetLength);
}
