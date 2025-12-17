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

#include <utils/Log.h>
#include <esp_heap_caps.h>
#include <esp_log.h>

void Logger::LogHeapStatistix() {
	if (IsLogInfoEnabled()) {
		size_t free = heap_caps_get_free_size(MALLOC_CAP_8BIT);
		size_t freeMin = heap_caps_get_minimum_free_size(MALLOC_CAP_8BIT);
		size_t largestFreeBlock = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
		LogNewLine();
		LogInfo("=======================");
		LogInfo("Heap Statistics:");
		LogInfo("  free total:         %d B", free);
		LogInfo("  free minimum:       %d B", freeMin);
		LogInfo("  largest free block: %d B", largestFreeBlock);
		LogInfo("=======================");
	}
}

