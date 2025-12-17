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

#include <hal/rfs/SpiFfsPartition.h>
#include <utils/Log.h>

namespace Hal {
using namespace Rfs;

bool SpiFfsPartition::Mount(const SpiFfsMountCfg& cfg) {
	LogError("SPIFFS is not supported");
	return false;
}

void SpiFfsPartition::Unmount() {
	LogError("SPIFFS is not supported");
}

bool SpiFfsPartition::Format() {
	LogError("SPIFFS is not supported");
	return false;
}

uint64_t SpiFfsPartition::GetCapacity() const {
	LogError("SPIFFS is not supported");
	return 0;
}

uint64_t SpiFfsPartition::GetUsedSize() const {
	LogError("SPIFFS is not supported");
	return 0;
}

} // namespace Hal
