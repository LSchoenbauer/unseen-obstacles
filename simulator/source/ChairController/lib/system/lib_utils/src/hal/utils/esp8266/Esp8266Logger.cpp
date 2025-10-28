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

#ifdef ESP8266
#pragma note ("Caution: ESP8266 variant is not finally migrated to library!")

#include <utils/Log.h>
#include <user_interface.h>

void Logger::LogHeapStatistix() {
	if (IsLogInfoEnabled()) {
		LogNewLine();
		LogInfo("=======================");
		LogInfo("Heap Statistics:");
		size_t freeHeap = system_get_free_heap_size();
		LogInfo("  Free heap: %d B", freeHeap);
		LogInfo("=======================");
	}
}

#endif

