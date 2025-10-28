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

#include <hal/rfs/SdMmcPartition.h>
#include <utils/Log.h>

namespace Hal {
using namespace Rfs;

bool SdMmcPartition::Mount(const SdMmcMountCfg& cfg) {
	LogError("SD-MMC FS is not supported");
	return false;
}

void SdMmcPartition::Unmount() {
	LogError("SD-MMC FS is not supported");
}

bool SdMmcPartition::Format() {
	LogError("SD-MMC FS is not supported");
	return false;
}

SdMmcPartition::CardType::Enum SdMmcPartition::GetCardType() const {
	LogError("SD-MMC FS is not supported");
	return SdMmcPartition::CardType::Unknown;
}

uint64_t SdMmcPartition::GetCardSize() const {
	LogError("SD-MMC FS is not supported");
	return 0;
}

uint64_t SdMmcPartition::GetCapacity() const {
	LogError("SD-MMC FS is not supported");
	return 0;
}

uint64_t SdMmcPartition::GetUsedSize() const {
	LogError("SD-MMC FS is not supported");
	return 0;
}

} // namespace Hal
